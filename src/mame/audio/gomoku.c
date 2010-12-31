/***************************************************************************

    Gomoku sound driver (quick hack of the Wiping sound driver)

    used by wiping.c

***************************************************************************/

#include "emu.h"
#include "streams.h"
#include "includes/gomoku.h"


/* 4 voices max */
#define MAX_VOICES 4


static const int samplerate = 48000;
static const int defgain = 48;


/* this structure defines the parameters for a channel */
typedef struct
{
	int channel;
	int frequency;
	int counter;
	int volume;
	int oneshotplaying;
} sound_channel;


typedef struct _gomoku_sound_state gomoku_sound_state;
struct _gomoku_sound_state
{
	/* data about the sound system */
	sound_channel channel_list[MAX_VOICES];
	sound_channel *last_channel;

	/* global sound parameters */
	const UINT8 *sound_rom;
	int num_voices;
	int sound_enable;
	sound_stream *stream;

	/* mixer tables and internal buffers */
	INT16 *mixer_table;
	INT16 *mixer_lookup;
	short *mixer_buffer;
	short *mixer_buffer_2;

	UINT8 soundregs1[0x20];
	UINT8 soundregs2[0x20];
};

INLINE gomoku_sound_state *get_safe_token( device_t *device )
{
	assert(device != NULL);
	assert(device->type() == GOMOKU);

	return (gomoku_sound_state *)downcast<legacy_device_base *>(device)->token();
}


/* build a table to divide by the number of voices; gain is specified as gain*16 */
static void make_mixer_table(device_t *device, int voices, int gain)
{
	gomoku_sound_state *state = get_safe_token(device);
	int count = voices * 128;
	int i;

	/* allocate memory */
	state->mixer_table = auto_alloc_array(device->machine, INT16, 256 * voices);

	/* find the middle of the table */
	state->mixer_lookup = state->mixer_table + (128 * voices);

	/* fill in the table - 16 bit case */
	for (i = 0; i < count; i++)
	{
		int val = i * gain * 16 / voices;
		if (val > 32767) val = 32767;
		state->mixer_lookup[ i] = val;
		state->mixer_lookup[-i] = -val;
	}
}


/* generate sound to the mix buffer in mono */
static STREAM_UPDATE( gomoku_update_mono )
{
	gomoku_sound_state *state = get_safe_token(device);
	stream_sample_t *buffer = outputs[0];
	sound_channel *voice;
	short *mix;
	int i, ch;

	/* if no sound, we're done */
	if (state->sound_enable == 0)
	{
		memset(buffer, 0, samples * sizeof(*buffer));
		return;
	}

	/* zap the contents of the mixer buffer */
	memset(state->mixer_buffer, 0, samples * sizeof(short));

	/* loop over each voice and add its contribution */
	for (ch = 0, voice = state->channel_list; voice < state->last_channel; ch++, voice++)
	{
		int f = 16 * voice->frequency;
		int v = voice->volume;

		/* only update if we have non-zero volume and frequency */
		if (v && f)
		{
			int w_base;
			int c = voice->counter;

			if (ch < 3)
				w_base = 0x20 * (state->soundregs1[0x06 + (ch * 8)] & 0x0f);
			else
				w_base = 0x100 * (state->soundregs2[0x1d] & 0x0f);

			mix = state->mixer_buffer;

			/* add our contribution */
			for (i = 0; i < samples; i++)
			{
				c += f;

				if (ch < 3)
				{
					int offs = w_base | ((c >> 16) & 0x1f);

					/* use full byte, first the high 4 bits, then the low 4 bits */
					if (c & 0x8000)
						*mix++ += ((state->sound_rom[offs] & 0x0f) - 8) * v;
					else
						*mix++ += (((state->sound_rom[offs]>>4) & 0x0f) - 8) * v;
				}
				else
				{
					int offs = (w_base + (c >> 16)) & 0x0fff;

					if (state->sound_rom[offs] == 0xff)
					{
						voice->oneshotplaying = 0;
					}

					if (voice->oneshotplaying)
					{
						/* use full byte, first the high 4 bits, then the low 4 bits */
						if (c & 0x8000)
							*mix++ += ((state->sound_rom[offs] & 0x0f) - 8) * v;
						else
							*mix++ += (((state->sound_rom[offs]>>4) & 0x0f) - 8) * v;
					}
				}

				/* update the counter for this voice */
				voice->counter = c;
			}
		}
	}

	/* mix it down */
	mix = state->mixer_buffer;
	for (i = 0; i < samples; i++)
		*buffer++ = state->mixer_lookup[*mix++];
}



static DEVICE_START( gomoku_sound )
{
	gomoku_sound_state *state = get_safe_token(device);
	running_machine *machine = device->machine;
	sound_channel *voice;
	int ch;

	/* get stream channels */
	state->stream = stream_create(device, 0, 1, samplerate, NULL, gomoku_update_mono);

	/* allocate a pair of buffers to mix into - 1 second's worth should be more than enough */
	state->mixer_buffer = auto_alloc_array(machine, short, 2 * samplerate);
	state->mixer_buffer_2 = state->mixer_buffer + samplerate;

	/* build the mixer table */
	make_mixer_table(device, 8, defgain);

	/* extract globals from the interface */
	state->num_voices = MAX_VOICES;
	state->last_channel = state->channel_list + state->num_voices;

	state->sound_rom = memory_region(machine, "gomoku");

	/* start with sound enabled, many games don't have a sound enable register */
	state->sound_enable = 1;

	/* reset all the voices */
	for (ch = 0, voice = state->channel_list; voice < state->last_channel; ch++, voice++)
	{
		voice->channel = ch;
		voice->frequency = 0;
		voice->counter = 0;
		voice->volume = 0;
		voice->oneshotplaying = 0;
	}
}


DEVICE_GET_INFO( gomoku_sound )
{
	switch (state)
	{
		/* --- the following bits of info are returned as 64-bit signed integers --- */
		case DEVINFO_INT_TOKEN_BYTES:					info->i = sizeof(gomoku_sound_state);			break;

		/* --- the following bits of info are returned as pointers to data or functions --- */
		case DEVINFO_FCT_START:							info->start = DEVICE_START_NAME(gomoku_sound);	break;

		/* --- the following bits of info are returned as NULL-terminated strings --- */
		case DEVINFO_STR_NAME:							strcpy(info->s, "Gomoku Custom");				break;
		case DEVINFO_STR_SOURCE_FILE:						strcpy(info->s, __FILE__);						break;
	}
}


/********************************************************************************/

WRITE8_DEVICE_HANDLER( gomoku_sound1_w )
{
	gomoku_sound_state *state = get_safe_token(device);
	sound_channel *voice;
	int base;
	int ch;

	/* update the streams */
	stream_update(state->stream);

	/* set the register */
	state->soundregs1[offset] = data;

	/* recompute all the voice parameters */
	for (ch = 0, base = 0, voice = state->channel_list; voice < state->channel_list + 3; ch++, voice++, base += 8)
	{
		voice->channel = ch;
		voice->frequency = state->soundregs1[0x02 + base] & 0x0f;
		voice->frequency = voice->frequency * 16 + ((state->soundregs1[0x01 + base]) & 0x0f);
		voice->frequency = voice->frequency * 16 + ((state->soundregs1[0x00 + base]) & 0x0f);
	}
}

WRITE8_DEVICE_HANDLER( gomoku_sound2_w )
{
	gomoku_sound_state *state = get_safe_token(device);
	sound_channel *voice;
	int base;
	int ch;

	/* update the streams */
	stream_update(state->stream);

	/* set the register */
	state->soundregs2[offset] = data;

	/* recompute all the voice parameters */
	for (ch = 0, base = 0, voice = state->channel_list; voice < state->channel_list + 3; ch++, voice++, base += 8)
	{
		voice->channel = ch;
		voice->volume = state->soundregs2[0x06 + base] & 0x0f;
		voice->oneshotplaying = 0;
	}

	if (offset == 0x1d)
	{
		voice = &state->channel_list[3];
		voice->channel = 3;

		// oneshot frequency is hand tune...
		if ((state->soundregs2[0x1d] & 0x0f) < 0x0c)
			voice->frequency = 3000 / 16;			// ichi, ni, san, yon, go
		else
			voice->frequency = 8000 / 16;			// shoot

		voice->volume = 8;
		voice->counter = 0;

		if (state->soundregs2[0x1d] & 0x0f)
			voice->oneshotplaying = 1;
		else
			voice->oneshotplaying = 0;
	}
}


DEFINE_LEGACY_SOUND_DEVICE(GOMOKU, gomoku_sound);
