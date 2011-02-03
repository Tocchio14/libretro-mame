/***************************************************************************

    speaker.c

    Sound driver to emulate a simple speaker,
    driven by one or more output bits

    Original author: (unsigned)
    Filtering: Anders Hallstr?m
****************************************************************************/

/* Discussion of oversampling and anti-alias filtering: (Anders Hallstr?m)
 *
 * This driver is for machines that directly control
 * one or more simple digital-to-analog converters (DAC)
 * connected to one or more audio outputs (such as analog amp + speaker).
 * Currently only 1-bit DAC is supported via the interface to this module.
 *
 * Frequently such machines would oversample the DAC
 * in order to overcome the limited DAC resolution.
 * For faithful reproduction of the sound, this must be carefully handled
 * with anti-alias filtering when converting a high-rate low-resolution signal
 * to a moderate-rate high-resolution signal suitable for the DAC in the emulator's sound card.
 * (Originally, removal of any redundant high frequency content occured on the analog side
 *  with no aliasing effects.)
 *
 * The most straightforward, naive way to handle this is to use two streams;
 * stream 1 modeling the native audio, with a sampling rate that allows for
 * accurate representation of over-sampling, i.e. the sampling rate should match
 * the clock frequency of the audio generating device (such as the CPU).
 * Stream 1 is connected to stream 2, which is concerned with feeding the sound card.
 * The stream system has features to handle rate conversion from stream 1 to 2.
 *
 * I tried it out of curiosity; it works fine conceptually, but
 *  - it puts an unneccessary burden on system resources
 *  - sound quality is still not satisfactory, though better than without anti-alias
 *  - "stream 1" properties are machine specific and so should be configured
 *    individually in each machine driver using this approach.
 *    This can also be seen as an advantage for flexibility, though.
 *
 * Instead, dedicated filtering is implemented in this module,
 * in a machine-neutral way (based on machine time and external -samplerate only).
 *
 * The basic average filter has the advantage that it can be used without
 * explicitly generating all samples in "stream 1". However,
 * it is poor for anti-alias filtering.
 * Therefore, average filtering is combined with windowed sinc.
 *
 * Virtual stream 1: Samples in true machine time.
 * Any sampling rate up to attotime resolution is implicitly supported.
 * -> average filtering over each stream 2 sample ->
 * Virtual stream 2: Intermediate representation.
 * Sample rate = RATE_MULTIPLIER * stream 3 sample rate.
 * If effective rate of stream 1 exceeds rate of stream 2,
 * some aliasing distorsion is introduced in this step because the average filtering is a compromise.
 * The distorsion is however mostly in the higher frequencies.
 * -> low-pass anti-alias filtering with kernel ampl[] ->
 * -> down-sampling ->
 * Actual stream 3: channel output generated by speaker_sound_update().
 * Sample rate = device sample rate = configured "-samplerate".
 *
 * In the speaker_state data structure,
 *    "intermediate samples" refers to "stream 2"
 *    "channel samples" refers to "stream 3"
 */

/* IMPROVEMENTS POSSIBLE:
 * - Make filter length a run-time configurable parameter. min=1 max=1000 or something
 * - Optimize cutoff freq automatically after filter length, or configurable too
 * - Generalise this approach to other DAC-based sound types if susceptible to aliasing
 */

#include "emu.h"
#include "speaker.h"

static const INT16 default_levels[2] = {0, 32767};

/* Filter properties shared by all speaker devices:
 */
/* Length of anti-aliasing filter kernel, measured in number of intermediate samples */
enum {FILTER_LENGTH = 64};
/* Kernel (pulse response) for filtering across samples (while we avoid fancy filtering within samples) */
static double ampl[FILTER_LENGTH];
/* Internal oversampling factor (interm. samples vs stream samples) */
static const int RATE_MULTIPLIER = 4;

typedef struct _speaker_state speaker_state;
struct _speaker_state
{
	sound_stream *channel;
	const INT16 *levels;
	int num_levels;
	int level;

	/* The volume of a composed sample grows incrementally each time the speaker is over-sampled.
     * That is in effect a basic average filter.
     * Another filter can and will be applied to the array of composed samples.
     */
	double        composed_volume[FILTER_LENGTH];	/* integrator(s) */
	int           composed_sample_index;			/* array index for composed_volume */
	attoseconds_t channel_sample_period;			/* in as */
	double        channel_sample_period_secfrac;	/* in fraction of second */
	attotime      channel_last_sample_time;
	attotime      channel_next_sample_time;
	attoseconds_t interm_sample_period;
	double        interm_sample_period_secfrac;
	attotime      next_interm_sample_time;
	int           interm_sample_index;				/* counts interm. samples between stream samples */
	attotime      last_update_time;					/* internal timestamp */
};


static STREAM_UPDATE( speaker_sound_update );

/* Updates the composed volume array according to time */
static void update_interm_samples(speaker_state *sp, attotime time, int volume);

/* Updates the composed volume array and returns final filtered volume of next stream sample */
static double update_interm_samples_get_filtered_volume(speaker_state *sp, int volume);

/* Local helpers */
static void finalize_interm_sample(speaker_state *sp, int volume);
static void init_next_interm_sample(speaker_state *sp);
static double make_fraction(attotime a, attotime b, double timediv);
static double get_filtered_volume(speaker_state *sp);


INLINE speaker_state *get_safe_token(device_t *device)
{
	assert(device != NULL);
	assert(device->type() == SPEAKER_SOUND);
	return (speaker_state *)downcast<legacy_device_base *>(device)->token();
}


static DEVICE_START( speaker )
{
	speaker_state *sp = get_safe_token(device);
	const speaker_interface *intf = (const speaker_interface *) device->baseconfig().static_config();
	int i;
	double x;

	sp->channel = device->machine->sound().stream_alloc(*device, 0, 1, device->machine->sample_rate, sp, speaker_sound_update);

	if (intf != NULL)
	{
		assert(intf->num_level > 1);
		assert(intf->levels != NULL);
		sp->num_levels = intf->num_level;
		sp->levels = intf->levels;
	}
	else
	{
		sp->num_levels = 2;
		sp->levels = default_levels;
	}

	sp->level = 0;
	for (i = 0; i < FILTER_LENGTH; i++)
		sp->composed_volume[i] = 0;
	sp->composed_sample_index = 0;
	sp->last_update_time = timer_get_time(device->machine);
	sp->channel_sample_period = HZ_TO_ATTOSECONDS(device->machine->sample_rate);
	sp->channel_sample_period_secfrac = ATTOSECONDS_TO_DOUBLE(sp->channel_sample_period);
	sp->interm_sample_period = sp->channel_sample_period / RATE_MULTIPLIER;
	sp->interm_sample_period_secfrac = ATTOSECONDS_TO_DOUBLE(sp->interm_sample_period);
	sp->channel_last_sample_time = sp->channel->sample_time();
	sp->channel_next_sample_time = sp->channel_last_sample_time + attotime(0, sp->channel_sample_period);
	sp->next_interm_sample_time = sp->channel_last_sample_time + attotime(0, sp->interm_sample_period);
	sp->interm_sample_index = 0;
	/* Note: To avoid time drift due to floating point inaccuracies,
     * it is good if the speaker time synchronizes itself with the stream timing regularly.
     */

	/* Compute filter kernel; */
	/* (Done for each device though the data is shared...
     *  No problem really, but should be done as part of system init if I knew how)
     */
#if 1
	/* This is an approximated sinc (a perfect sinc makes an ideal low-pass filter).
     * FILTER_STEP determines the cutoff frequency,
     * which should be below the Nyquist freq, i.e. half the sample rate.
     * Smaller step => kernel extends in time domain => lower cutoff freq
     * In this case, with sinc, filter step PI corresponds to the Nyq. freq.
     * Since we do not get a perfect filter => must lower the cutoff freq some more.
     * For example, step PI/(2*RATE_MULTIPLIER) corresponds to cutoff freq = sample rate / 4;
     *    With -samplerate 48000, cutoff freq is ca 12kHz while the Nyq. freq is 24kHz.
     *    With -samplerate 96000, cutoff freq is ca 24kHz while the Nyq. freq is 48kHz.
     * For a steeper, more efficient filter, increase FILTER_LENGTH at the expense of CPU usage.
     */
	#define FILTER_STEP  (M_PI / 2 / RATE_MULTIPLIER)
	/* Distribute symmetrically on x axis; center has x=0 if length is odd */
	for (i = 0, 			x = (0.5 - FILTER_LENGTH / 2.) * FILTER_STEP;
	     i < FILTER_LENGTH;
		 i++,				x += FILTER_STEP)
	{
		if (x == 0)
			ampl[i] = 1;
		else
			ampl[i] = sin(x) / x;
	}
#else
	/* Trivial average filter with poor frequency cutoff properties;
     * First zero (frequency where amplification=0) = sample rate / filter length
     * Cutoff frequency approx <= first zero / 2
     */
	for (i = 0, i < FILTER_LENGTH; i++)
		ampl[i] = 1;
#endif
}


/* Called via stream->update().
 * This can be triggered by the core (based on emulated time) or via speaker_level_w().
 */
static STREAM_UPDATE( speaker_sound_update )
{
	speaker_state *sp = (speaker_state *) param;
	stream_sample_t *buffer = outputs[0];
	int volume = sp->levels[sp->level];
	double filtered_volume;
	attotime sampled_time = attotime_zero;

	if (samples > 0)
	{
		/* Prepare to update time state */
		sampled_time = attotime(0, sp->channel_sample_period);
		if (samples > 1)
			sampled_time *= samples;

		/* Note: since the stream is in the process of being updated,
         * stream->sample_time() will return the time before the update! (MAME 0.130)
         * Avoid using it here in order to avoid a subtle dependence on the stream implementation.
         */
	}

	if (samples-- > 0)
	{
		/* Note that first interm. sample may be composed... */
		filtered_volume = update_interm_samples_get_filtered_volume(sp, volume);

		/* Composite volume is now quantized to the stream resolution */
		*buffer++ = (stream_sample_t)filtered_volume;

		/* Any additional samples will be homogeneous, however may need filtering across samples: */
		while (samples-- > 0)
		{
			filtered_volume = update_interm_samples_get_filtered_volume(sp, volume);
			*buffer++ = (stream_sample_t)filtered_volume;
		}

		/* Update the time state */
		sp->channel_last_sample_time += sampled_time;
		sp->channel_next_sample_time = sp->channel_last_sample_time + attotime(0, sp->channel_sample_period);
		sp->next_interm_sample_time = sp->channel_last_sample_time + attotime(0, sp->interm_sample_period);
		sp->last_update_time = sp->channel_last_sample_time;
	}

} /* speaker_sound_update */


void speaker_level_w(device_t *device, int new_level)
{
	speaker_state *sp = get_safe_token(device);
	int volume;
	attotime time;

	if (new_level == sp->level)
		return;

	if (new_level < 0)
		new_level = 0;
	else
	if (new_level >= sp->num_levels)
		new_level = sp->num_levels - 1;

	volume = sp->levels[sp->level];
	time = timer_get_time(device->machine);

	if (time < sp->channel_next_sample_time)
	{
		/* Stream sample is yet unfinished, but we may have one or more interm. samples */
		update_interm_samples(sp, time, volume);

		/* Do not forget to update speaker state before returning! */
		sp->level = new_level;
		return;
	}
	/* Reaching here means such time has passed since last stream update
     * that we can add at least one complete sample to the stream.
     * The details have to be handled by speaker_sound_update()
     */

	/* Force streams.c to update sound until this point in time now */
	sp->channel->update();

	/* This is redundant because time update has to be done within speaker_sound_update() anyway,
     * however this ensures synchronization between the speaker and stream timing:
     */
	sp->channel_last_sample_time = sp->channel->sample_time();
	sp->channel_next_sample_time = sp->channel_last_sample_time + attotime(0, sp->channel_sample_period);
	sp->next_interm_sample_time = sp->channel_last_sample_time + attotime(0, sp->interm_sample_period);
	sp->last_update_time = sp->channel_last_sample_time;

	/* Assertion: time - last_update_time < channel_sample_period, i.e. time < channel_next_sample_time */

	/* The overshooting fraction of time will make zero, one or more interm. samples: */
	update_interm_samples(sp, time, volume);

	/* Finally update speaker state before returning */
	sp->level = new_level;

} /* speaker_level_w */


static void update_interm_samples(speaker_state *sp, attotime time, int volume)
{
	double fraction;

	/* We may have completed zero, one or more interm. samples: */
	while (time >= sp->next_interm_sample_time)
	{
		/* First interm. sample may be composed, subsequent samples will be homogeneous. */
		/* Treat all the same general way. */
		finalize_interm_sample(sp, volume);
		init_next_interm_sample(sp);
	}
	/* Depending on status above:
     * a) Add latest fraction to unfinished composed sample
     * b) The overshooting fraction of time will start a new composed sample
     */
	fraction = make_fraction(time, sp->last_update_time, sp->interm_sample_period_secfrac);
	sp->composed_volume[sp->composed_sample_index] += volume * fraction;
	sp->last_update_time = time;
}


static double update_interm_samples_get_filtered_volume(speaker_state *sp, int volume)
{
	double filtered_volume;

	/* We may have one or more interm. samples to go */
	if (sp->interm_sample_index < RATE_MULTIPLIER)
	{
		/* First interm. sample may be composed. */
		finalize_interm_sample(sp, volume);

		/* Subsequent interm. samples will be homogeneous. */
		while (sp->interm_sample_index + 1 < RATE_MULTIPLIER)
		{
			init_next_interm_sample(sp);
			sp->composed_volume[sp->composed_sample_index] = volume;
		}
	}
	/* Important: next interm. sample not initialised yet, so that no data is destroyed before filtering... */
	filtered_volume = get_filtered_volume(sp);
	init_next_interm_sample(sp);
	/* Reset counter to next stream sample: */
	sp->interm_sample_index = 0;

	return filtered_volume;
}


static void finalize_interm_sample(speaker_state *sp, int volume)
{
	double fraction;

	/* Fill the composed sample up if it was incomplete */
	fraction = make_fraction(sp->next_interm_sample_time,
	                         sp->last_update_time,
	                         sp->interm_sample_period_secfrac);
	sp->composed_volume[sp->composed_sample_index] += volume * fraction;
	/* Update time state */
	sp->last_update_time = sp->next_interm_sample_time;
	sp->next_interm_sample_time += attotime(0, sp->interm_sample_period);

	/* For compatibility with filtering, do not incr. index and initialise next sample yet. */
}


static void init_next_interm_sample(speaker_state *sp)
{
	/* Move the index and initialize next composed sample */
	sp->composed_sample_index++;
	if (sp->composed_sample_index >= FILTER_LENGTH)
		sp->composed_sample_index = 0;
	sp->composed_volume[sp->composed_sample_index] = 0;

	sp->interm_sample_index++;
	/* No limit check on interm_sample_index here - to be handled by caller */
}


static double make_fraction(attotime a, attotime b, double timediv)
{
	/* fraction = (a - b) / timediv */
	return (a - b).as_double() / timediv;
}


static double get_filtered_volume(speaker_state *sp)
{
	double filtered_volume = 0;
	double ampsum = 0;
	int i, c;

	/* Filter over composed samples (each composed sample is already average filtered) */
	for (i = sp->composed_sample_index + 1, c = 0; c < FILTER_LENGTH; i++, c++)
	{
		if (i >= FILTER_LENGTH) i = 0;
		filtered_volume += sp->composed_volume[i] * ampl[c];
		ampsum += ampl[c];
	}
	filtered_volume /= ampsum;

	return filtered_volume;
}


/**************************************************************************
 * Generic get_info
 **************************************************************************/

DEVICE_GET_INFO( speaker_sound )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case DEVINFO_INT_TOKEN_BYTES:					info->i = sizeof(speaker_state);				break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME( speaker );		break;
		case DEVINFO_FCT_STOP:							/* nothing */									break;
		case DEVINFO_FCT_RESET:							/* nothing */									break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case DEVINFO_STR_NAME:							strcpy(info->s, "Speaker");						break;
		case DEVINFO_STR_FAMILY:					strcpy(info->s, "Speaker");						break;
		case DEVINFO_STR_VERSION:					strcpy(info->s, "1.0");							break;
		case DEVINFO_STR_SOURCE_FILE:						strcpy(info->s, __FILE__);						break;
		case DEVINFO_STR_CREDITS:					strcpy(info->s, "Copyright The MESS Team"); 	break;
	}
}


DEFINE_LEGACY_SOUND_DEVICE(SPEAKER_SOUND, speaker_sound);
