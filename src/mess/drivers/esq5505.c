/***************************************************************************

    esq5505.c - Ensoniq ES5505 + ES5510 based synthesizers and samplers

    Ensoniq VFX, VFX-SD, EPS, EPS-16 Plus, SD-1, SD-1 32, and SQ-1 (SQ-1 Plus,
    SQ-2, and KS-32 are known to also be this architecture).

    The Taito sound system in taito_en.c is directly derived from the SQ-1.

    Driver by R. Belmont with thanks to Parduz, Christian Brunschen, and Phil Bennett

    Memory map:

    0x000000-0x007fff   work RAM low (64k for SQ-1 and later)
    0x200000-0x20001f   OTIS (5505) regs
    0x240000-0x2400ff   DMAC (68450) regs (EPS/EPS-16)
    0x260000-0x2601ff   ESP (5510) regs
    0x280000-0x28001f   DUART (68681) regs
    0x2C0000-0x2C0003   Floppy (WD1772) regs (VFX-SD, SD-1, and EPS/EPS-16)
    0x2e0000-0x2fffff   Expansion cartridge (VFX, VFX-SD, SD-1, SD-1 32 voice)
    0x300000-0x300003   EPS/EPS-16 SCSI (WD33C93, register at 300001, data at 300003)
    0x330000-0x37ffff   VFX-SD / SD-1 sequencer RAM
    0x340000-0x3bffff   EPS/EPS-16 sample RAM
    0xc00000-0xc3ffff   OS ROM
    0xff8000-0xffffff   work RAM hi (64k for SQ-1 and later)

    Note from es5700.pdf PLA equations:
    RAM if (A23/22/21 = 000 and FC is not 6) or (A23/22/21 = 111 and FC is not 7)
    ROM if (A23/22/21 = 110) or (A23/22/21 = 000 & FC is 6)

    Interrupts:
    5505 interrupts are on normal autovector IRQ 1
    DMAC interrupts (EPS only) are on autovector IRQ 2
    68681 uses custom vector 0x40 (address 0x100) level 3

    VFX / VFX-SD / SD-1 / SD-1 32 panel button codes:
    2 = PROGRAM CONTROL
    3 = WRITE
    4 = WAVE
    5 = SELECT VOICE
    6 = MIXER/SHAPER
    7 = EFFECT
    8 = COMPARE
    9 = COPY EFFECTS PARAMETERS
    10 = LFO
    11 = PITCH
    12 = ENV1
    13 = PITCH MOD
    14 = ENV2
    15 = FILTER
    16 = ENV3
    17 = OUTPUT
    18 = ERROR 20 (VFX) / SEQ. CONTROL
    19 = RECORD
    20 = MASTER
    21 = STORAGE
    22 = STOP/CONT
    23 = PLAY
    24 = MIDI
    25 = BUTTON 9
    26 = PSEL
    27 = STAT
    28 = EFFECT
    29 = SEQ?  (toggles INT0 / TRAX display)
    30 = TRACKS 1-6
    31 = TRACKS 7-12
    32 = ERROR 20 (VFX) / CLICK-REC
    33 = ERROR 20 (VFX) / LOCATE
    34 = BUTTON 8
    35 = BUTTON 7
    36 = VOLUME
    37 = PAN
    38 = TIMBRE
    39 = KEY ZONE
    40 = TRANSPOSE
    41 = RELEASE
    42 = SOFT TOP CENTER
    43 = SOFT TOP RIGHT
    44 = SOFT BOTTOM CENTER
    45 = SOFT BOTTOM RIGHT
    46 = BUTTON 3
    47 = BUTTON 4
    48 = BUTTON 5
    49 = BUTTON 6
    50 = SOFT BOTTOM LEFT
    51 = ERROR 202 (VFX) / SEQ.
    52 = CART
    53 = SOUNDS
    54 = PRESETS
    55 = BUTTON 0
    56 = BUTTON 1
    57 = BUTTON 2
    58 = SOFT TOP LEFT
    59 = ERROR 20 (VFX) / EDIT SEQUENCE
    60 = ERROR 20 (VFX) / EDIT SONG
    61 = ERROR 20 (VFX) / EDIT TRACK
    62 = DATA INCREMENT
    63 = DATA DECREMENT

    VFX / VFX-SD / SD-1 analog values:
    0 = Pitch Bend
    1 = Patch Select
    2 = Mod Wheel
    3 = Value, aka Data Entry Slider
    4 = Pedal / CV
    5 = Volume Slider
    6 = Battery
    7 = Voltage Reference

***************************************************************************/

#include <cstdio>

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "sound/es5506.h"
#include "machine/n68681.h"
#include "cpu/es5510/es5510.h"
#include "machine/wd_fdc.h"
#include "machine/hd63450.h"    // compatible with MC68450, which is what these really have
#include "formats/esq16_dsk.h"
#include "machine/esqvfd.h"
#include "machine/esqpanel.h"
#include "machine/serial.h"
#include "machine/midiinport.h"
#include "machine/midioutport.h"

#define GENERIC (0)
#define EPS     (1)
#define SQ1     (2)

#define KEYBOARD_HACK (1)   // turn on to play the SQ-1, SD-1, and SD-1 32-voice: Z and X are program up/down, A/S/D/F/G/H/J/K/L and Q/W/E/R/T/Y/U play notes

#if KEYBOARD_HACK
static int shift = 32;
#endif

void print_to_stderr(const char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	vfprintf(stderr, format, arg);
	va_end(arg);
}

#define PUMP_DETECT_SILENCE 0
#define PUMP_TRACK_SAMPLES 0
#define PUMP_FAKE_ESP_PROCESSING 0
#define PUMP_REPLACE_ESP_PROGRAM 0

class esq_5505_5510_pump : public device_t,
	public device_sound_interface
{
public:
	esq_5505_5510_pump(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	void set_otis(es5505_device *otis) { m_otis = otis; }
	void set_esp(es5510_device *esp) { m_esp = esp; }
	void set_esp_halted(bool esp_halted) {
		m_esp_halted = esp_halted;
		logerror("ESP-halted -> %d\n", m_esp_halted);
		if (!esp_halted) {

#if PUMP_REPLACE_ESP_PROGRAM
			m_esp->write_reg(245, 0x1d0f << 8); // dlength = 0x3fff, 16-sample delay
			
			int pc = 0;
			for (pc = 0; pc < 0xc0; pc++) {
				m_esp->write_reg(pc, 0);
			}
			pc = 0;
			// replace the ESP program with a simple summing & single-sample delay
			m_esp->_instr(pc++) = 0xffffeaa09000; // MOV SER0R > grp_a0
			m_esp->_instr(pc++) = 0xffffeba00000; // ADD SER0L, gpr_a0 > gpr_a0
			m_esp->_instr(pc++) = 0xffffeca00000; // ADD SER1R, gpr_a0 > gpr_a0
			m_esp->_instr(pc++) = 0xffffeda00000; // ADD SER1L, gpr_a0 > gpr_a0
			m_esp->_instr(pc++) = 0xffffeea00000; // ADD SER2R, gpr_a0 > gpr_a0

			m_esp->_instr(pc  ) = 0xffffefa00000; // ADD SER2L, gpr_a0 > gpr_a0; prepare to read from delay 2 instructions from now, offset = 0
            m_esp->write_reg(pc++, 0); //offset into delay

			m_esp->_instr(pc  ) = 0xffffa0a09508; // MOV gpr_a0 > delay + offset
			m_esp->write_reg(pc++, 1 << 8); // offset into delay - -1 samples

			m_esp->_instr(pc++) = 0xffff00a19928; // MOV DIL > gpr_a1; read Delay and dump FIFO (so that the value gets written)

			m_esp->_instr(pc++) = 0xffffa1f09000; // MOV gpr_a1 > SER3R
			m_esp->_instr(pc++) = 0xffffa1f19000; // MOV gpr_a1 > SER3L

			m_esp->_instr(pc++) = 0xffffffff0000; // NO-OP
			m_esp->_instr(pc++) = 0xffffffff0000; // NO-OP
			m_esp->_instr(pc++) = 0xfffffffff000; // END

			while (pc < 160) {
				m_esp->_instr(pc++) = 0xffffffffffff; // no-op
			}
#endif

			// m_esp->list_program(print_to_stderr);
		}
	}
	bool get_esp_halted() {
		return m_esp_halted;
	}

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_stop();
	virtual void device_reset();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);

	// timer callback overrides
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:
	// internal state:
	// sound stream
	sound_stream *m_stream;

	// per-sample timer
	emu_timer *m_timer;

	// OTIS sound generator
	es5505_device *m_otis;

	// ESP signal processor
	es5510_device *m_esp;

	// Is the ESP halted by the CPU?
	bool m_esp_halted;

#if !PUMP_FAKE_ESP_PROCESSING
	osd_ticks_t ticks_spent_processing;
	int samples_processed;
#endif

#if PUMP_DETECT_SILENCE
	int silent_for;
	bool was_silence;
#endif

#if PUMP_TRACK_SAMPLES
	int last_samples;
	osd_ticks_t last_ticks;
	osd_ticks_t next_report_ticks;
#endif

#if !PUMP_FAKE_ESP_PROCESSING && PUMP_REPLACE_ESP_PROGRAM
	INT16 e[0x4000];
	int ei;
#endif
};

const device_type ESQ_5505_5510_PUMP = &device_creator<esq_5505_5510_pump>;

esq_5505_5510_pump::esq_5505_5510_pump(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, ESQ_5505_5510_PUMP, "ESQ_5505_5510_PUMP", tag, owner, clock, "esq_5505_5510_pump", __FILE__),
		device_sound_interface(mconfig, *this),
		m_esp_halted(true)
{
}

void esq_5505_5510_pump::device_start()
{
	logerror("Clock = %d\n", clock());

	m_stream = machine().sound().stream_alloc(*this, 8, 2, clock(), this);
	m_timer = timer_alloc(0);
	m_timer->enable(false);

#if PUMP_DETECT_SILENCE
	silent_for = 500;
	was_silence = 1;
#endif
#if !PUMP_FAKE_ESP_PROCESSING
	ticks_spent_processing = 0;
	samples_processed = 0;
#endif
#if PUMP_TRACK_SAMPLES
	last_samples = 0;
	last_ticks = osd_ticks();
	next_report_ticks = last_ticks + osd_ticks_per_second();
#endif

#if !PUMP_FAKE_ESP_PROCESSING && PUMP_REPLACE_ESP_PROGRAM
	memset(e, 0, 0x4000 * sizeof(e[0]));
	ei = 0;
#endif
}

void esq_5505_5510_pump::device_stop()
{
	m_timer->enable(false);
}

void esq_5505_5510_pump::device_reset()
{
	INT64 nsec_per_sample = 100 * 16 * 21;
	attotime sample_time(0, 1000000000 * nsec_per_sample);
	attotime initial_delay(0, 0);

	m_timer->adjust(initial_delay, 0, sample_time);
	m_timer->enable(true);
}

void esq_5505_5510_pump::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	if (samples != 1) {
		logerror("Pump: request for %d samples\n", samples);
	}

	stream_sample_t *left = outputs[0], *right = outputs[1];
	for (int i = 0; i < samples; i++)
	{
	    // anything for the 'aux' output?
		INT16 l = inputs[0][i] >> 4;
		INT16 r = inputs[1][i] >> 4;

		// push the samples into the ESP
		m_esp->ser_w(0, inputs[2][i] >> 4);
		m_esp->ser_w(1, inputs[3][i] >> 4);
		m_esp->ser_w(2, inputs[4][i] >> 4);
		m_esp->ser_w(3, inputs[5][i] >> 4);
		m_esp->ser_w(4, inputs[6][i] >> 4);
		m_esp->ser_w(5, inputs[7][i] >> 4);

#if PUMP_FAKE_ESP_PROCESSING
		m_esp->ser_w(6, m_esp->ser_r(0) + m_esp->ser_r(2) + m_esp->ser_r(4));
		m_esp->ser_w(7, m_esp->ser_r(1) + m_esp->ser_r(3) + m_esp->ser_r(5));
#else
		if (!m_esp_halted) {
			logerror("passing one sample through ESP\n");
			osd_ticks_t a = osd_ticks();
			m_esp->run_once();
			osd_ticks_t b = osd_ticks();
			ticks_spent_processing += (b - a);
			samples_processed++;
		}
#endif

		// read the processed result from the ESP and add to the saved AUX data
		INT16 ll = m_esp->ser_r(6);
		INT16 rr = m_esp->ser_r(7);
		l += ll;
		r += rr;

#if !PUMP_FAKE_ESP_PROCESSING && PUMP_REPLACE_ESP_PROGRAM
		// if we're processing the fake program through the ESP, the result should just be that of adding the inputs
		INT32 el = (inputs[2][i]) + (inputs[4][i]) + (inputs[6][i]);
		INT32 er = (inputs[3][i]) + (inputs[5][i]) + (inputs[7][i]);
		INT32 e_next = el + er;
		e[(ei + 0x1d0f) % 0x4000] = e_next;
		
		if (l != e[ei]) {
			fprintf(stderr, "expected (%d) but have (%d)\n", e[ei], l);
		}
		ei = (ei + 1) % 0x4000;
#endif

		// write the combined data to the output
		*left++  = l;
		*right++ = r;
	}

#if PUMP_DETECT_SILENCE
	for (int i = 0; i < samples; i++) {
		if (outputs[0][i] == 0 && outputs[1][i] == 0) {
			silent_for++;
		} else {
			silent_for = 0;
		}
	}
	bool silence = silent_for >= 500;
	if (was_silence != silence) {
		if (!silence) {
			fprintf(stderr, ".-*\n");
		} else {
			fprintf(stderr, "*-.\n");
		}
		was_silence = silence;
	}
#endif

#if PUMP_TRACK_SAMPLES
	last_samples += samples;
	osd_ticks_t now = osd_ticks();
	if (now >= next_report_ticks)
	{
		osd_ticks_t elapsed = now - last_ticks;
		osd_ticks_t tps = osd_ticks_per_second();
		fprintf(stderr, "Pump: %d samples in %" I64FMT "d ticks for %f Hz\n", last_samples, elapsed, last_samples * (double)tps / (double)elapsed);
		last_ticks = now;
		while (next_report_ticks <= now) {
			next_report_ticks += tps;
		}
		last_samples = 0;

#if !PUMP_FAKE_ESP_PROCESSING
		fprintf(stderr, "  ESP spent %" I64FMT "d ticks on %d samples, %f ticks per sample\n", ticks_spent_processing, samples_processed, (double)ticks_spent_processing / (double)samples_processed);
		ticks_spent_processing = 0;
		samples_processed = 0;
#endif
	}
#endif
}

void esq_5505_5510_pump::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) {
	// ecery time there's a new sample period, update the stream!
	m_stream->update();
}


class esq5505_state : public driver_device
{
public:
	esq5505_state(const machine_config &mconfig, device_type type, const char *tag)
	: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_duart(*this, "duart"),
		m_otis(*this, "otis"),
		m_esp(*this, "esp"),
		m_pump(*this, "pump"),
		m_fdc(*this, "wd1772"),
		m_panel(*this, "panel"),
		m_dmac(*this, "mc68450"),
		m_mdout(*this, "mdout")
	{ }

	required_device<m68000_device> m_maincpu;
	required_device<duartn68681_device> m_duart;
	required_device<es5505_device> m_otis;
	required_device<es5510_device> m_esp;
	required_device<esq_5505_5510_pump> m_pump;
	optional_device<wd1772_t> m_fdc;
	required_device<esqpanel_device> m_panel;
	optional_device<hd63450_device> m_dmac;
	required_device<serial_port_device> m_mdout;

	virtual void machine_start();
	virtual void machine_reset();

	DECLARE_READ16_MEMBER(es5510_dsp_r);
	DECLARE_WRITE16_MEMBER(es5510_dsp_w);
	DECLARE_READ16_MEMBER(lower_r);
	DECLARE_WRITE16_MEMBER(lower_w);

	DECLARE_READ16_MEMBER(analog_r);
	DECLARE_WRITE16_MEMBER(analog_w);

	DECLARE_WRITE_LINE_MEMBER(duart_irq_handler);
	DECLARE_WRITE_LINE_MEMBER(duart_tx_a);
	DECLARE_WRITE_LINE_MEMBER(duart_tx_b);
	DECLARE_READ8_MEMBER(duart_input);
	DECLARE_WRITE8_MEMBER(duart_output);

	int m_system_type;
	UINT8 m_duart_io;
	UINT8 otis_irq_state;
	UINT8 dmac_irq_state;
	int dmac_irq_vector;
	UINT8 duart_irq_state;
	int duart_irq_vector;

	void update_irq_to_maincpu();

	DECLARE_FLOPPY_FORMATS( floppy_formats );

private:
	UINT16  *m_rom, *m_ram;
	UINT16 m_analog_values[8];

public:
	DECLARE_DRIVER_INIT(eps);
	DECLARE_DRIVER_INIT(common);
	DECLARE_DRIVER_INIT(sq1);
	DECLARE_DRIVER_INIT(denib);
	DECLARE_INPUT_CHANGED_MEMBER(key_stroke);
	IRQ_CALLBACK_MEMBER(maincpu_irq_acknowledge_callback);
	DECLARE_WRITE_LINE_MEMBER(esq5505_otis_irq);
};

FLOPPY_FORMATS_MEMBER( esq5505_state::floppy_formats )
	FLOPPY_ESQIMG_FORMAT
FLOPPY_FORMATS_END

static SLOT_INTERFACE_START( ensoniq_floppies )
	SLOT_INTERFACE( "35dd", FLOPPY_35_DD )
SLOT_INTERFACE_END

IRQ_CALLBACK_MEMBER(esq5505_state::maincpu_irq_acknowledge_callback)
{
	// We immediately update the interrupt presented to the CPU, so that it doesn't
	// end up retrying the same interrupt over and over. We then return the appropriate vector.
	int vector = 0;
	switch(irqline) {
	case 1:
		otis_irq_state = 0;
		vector = M68K_INT_ACK_AUTOVECTOR;
		break;
	case 2:
		dmac_irq_state = 0;
		vector = dmac_irq_vector;
		break;
	case 3:
		duart_irq_state = 0;
		vector = duart_irq_vector;
		break;
	default:
		printf("\nUnexpected IRQ ACK Callback: IRQ %d\n", irqline);
		return 0;
	}
	update_irq_to_maincpu();
	return vector;
}

void esq5505_state::machine_start()
{
	driver_device::machine_start();
	// tell the pump about the OTIS & ESP chips
	m_pump->set_otis(m_otis);
	m_pump->set_esp(m_esp);
}

void esq5505_state::machine_reset()
{
	m_rom = (UINT16 *)(void *)memregion("osrom")->base();
	m_ram = (UINT16 *)(void *)memshare("osram")->ptr();
	m_maincpu->set_irq_acknowledge_callback(device_irq_acknowledge_delegate(FUNC(esq5505_state::maincpu_irq_acknowledge_callback),this));

	// Default analog values:
	m_analog_values[0] = 0x7fff; // pitch mod: start in the center
	m_analog_values[1] = 0x0000; // patch select: nothing pressed.
	m_analog_values[2] = 0x0000; // mod wheel: at the bottom, no modulation
	m_analog_values[3] = 0xcccc; // data entry: somewhere in the middle
	m_analog_values[4] = 0xffff; // control voltage / pedal: full on.
	m_analog_values[5] = 0xffff; // Volume control: full on.
	m_analog_values[6] = 0x7fc0; // Battery voltage: something reasonable.
	m_analog_values[7] = 0x5540; // vRef to check battery.
}

void esq5505_state::update_irq_to_maincpu() {
	//printf("\nupdating IRQ state: have OTIS=%d, DMAC=%d, DUART=%d\n", otis_irq_state, dmac_irq_state, duart_irq_state);
	if (duart_irq_state) {
		m_maincpu->set_input_line(M68K_IRQ_2, CLEAR_LINE);
		m_maincpu->set_input_line(M68K_IRQ_1, CLEAR_LINE);
		m_maincpu->set_input_line_and_vector(M68K_IRQ_3, ASSERT_LINE, duart_irq_vector);
	} else if (dmac_irq_state) {
		m_maincpu->set_input_line(M68K_IRQ_3, CLEAR_LINE);
		m_maincpu->set_input_line(M68K_IRQ_1, CLEAR_LINE);
		m_maincpu->set_input_line_and_vector(M68K_IRQ_2, ASSERT_LINE, dmac_irq_vector);
	} else if (otis_irq_state) {
		m_maincpu->set_input_line(M68K_IRQ_3, CLEAR_LINE);
		m_maincpu->set_input_line(M68K_IRQ_2, CLEAR_LINE);
		m_maincpu->set_input_line(M68K_IRQ_1, ASSERT_LINE);
	} else {
		m_maincpu->set_input_line(M68K_IRQ_3, CLEAR_LINE);
		m_maincpu->set_input_line(M68K_IRQ_2, CLEAR_LINE);
		m_maincpu->set_input_line(M68K_IRQ_1, CLEAR_LINE);
	}
}

READ16_MEMBER(esq5505_state::lower_r)
{
	offset &= 0x7fff;

	// get pointers when 68k resets
	if (!m_rom)
	{
		m_rom = (UINT16 *)(void *)memregion("osrom")->base();
		m_ram = (UINT16 *)(void *)memshare("osram")->ptr();
	}

	if (m68k_get_fc(m_maincpu) == 0x6)  // supervisor mode = ROM
	{
		return m_rom[offset];
	}
	else
	{
		return m_ram[offset];
	}
}

WRITE16_MEMBER(esq5505_state::lower_w)
{
	offset &= 0x7fff;

	if (offset < 0x4000)
	{
		if (m68k_get_fc(m_maincpu) != 0x6)  // if not supervisor mode, RAM
		{
			COMBINE_DATA(&m_ram[offset]);
		}
		else
		{
			logerror("Write to ROM: %x @ %x (fc=%x)\n", data, offset, m68k_get_fc(m_maincpu));
		}
	}
	else
	{
		COMBINE_DATA(&m_ram[offset]);
	}
}

static ADDRESS_MAP_START( vfx_map, AS_PROGRAM, 16, esq5505_state )
	AM_RANGE(0x000000, 0x007fff) AM_READWRITE(lower_r, lower_w)
	AM_RANGE(0x200000, 0x20001f) AM_DEVREADWRITE_LEGACY("otis", es5505_r, es5505_w)
	AM_RANGE(0x280000, 0x28001f) AM_DEVREADWRITE8("duart", duartn68681_device, read, write, 0x00ff)
	AM_RANGE(0x260000, 0x2601ff) AM_DEVREADWRITE8("esp", es5510_device, host_r, host_w, 0x00ff)
	AM_RANGE(0xc00000, 0xc1ffff) AM_ROM AM_REGION("osrom", 0)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM AM_SHARE("osram")
ADDRESS_MAP_END

static ADDRESS_MAP_START( vfxsd_map, AS_PROGRAM, 16, esq5505_state )
	AM_RANGE(0x000000, 0x00ffff) AM_READWRITE(lower_r, lower_w)
	AM_RANGE(0x200000, 0x20001f) AM_DEVREADWRITE_LEGACY("otis", es5505_r, es5505_w)
	AM_RANGE(0x280000, 0x28001f) AM_DEVREADWRITE8("duart", duartn68681_device, read, write, 0x00ff)
	AM_RANGE(0x260000, 0x2601ff) AM_DEVREADWRITE8("esp", es5510_device, host_r, host_w, 0x00ff)
	AM_RANGE(0x2c0000, 0x2c0007) AM_DEVREADWRITE8("wd1772", wd1772_t, read, write, 0x00ff)
	AM_RANGE(0x330000, 0x3bffff) AM_RAM // sequencer memory?
	AM_RANGE(0xc00000, 0xc3ffff) AM_ROM AM_REGION("osrom", 0)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM AM_SHARE("osram")
ADDRESS_MAP_END

static ADDRESS_MAP_START( eps_map, AS_PROGRAM, 16, esq5505_state )
	AM_RANGE(0x000000, 0x007fff) AM_READWRITE(lower_r, lower_w)
	AM_RANGE(0x200000, 0x20001f) AM_DEVREADWRITE_LEGACY("otis", es5505_r, es5505_w)
	AM_RANGE(0x240000, 0x2400ff) AM_DEVREADWRITE_LEGACY("mc68450", hd63450_r, hd63450_w)
	AM_RANGE(0x280000, 0x28001f) AM_DEVREADWRITE8("duart", duartn68681_device, read, write, 0x00ff)
	AM_RANGE(0x2c0000, 0x2c0007) AM_DEVREADWRITE8("wd1772", wd1772_t, read, write, 0x00ff)
	AM_RANGE(0x580000, 0x7fffff) AM_RAM         // sample RAM?
	AM_RANGE(0xc00000, 0xc0ffff) AM_ROM AM_REGION("osrom", 0)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM AM_SHARE("osram")
ADDRESS_MAP_END

static ADDRESS_MAP_START( sq1_map, AS_PROGRAM, 16, esq5505_state )
	AM_RANGE(0x000000, 0x03ffff) AM_READWRITE(lower_r, lower_w)
	AM_RANGE(0x200000, 0x20001f) AM_DEVREADWRITE_LEGACY("otis", es5505_r, es5505_w)
	AM_RANGE(0x260000, 0x2601ff) AM_DEVREADWRITE8("esp", es5510_device, host_r, host_w, 0x0ff)
	AM_RANGE(0x280000, 0x28001f) AM_DEVREADWRITE8("duart", duartn68681_device, read, write, 0x00ff)
	AM_RANGE(0x2c0000, 0x2c0007) AM_DEVREADWRITE8("wd1772", wd1772_t, read, write, 0x00ff)
	AM_RANGE(0x330000, 0x3bffff) AM_RAM // sequencer memory?
	AM_RANGE(0xc00000, 0xc3ffff) AM_ROM AM_REGION("osrom", 0)
	AM_RANGE(0xff0000, 0xffffff) AM_RAM AM_SHARE("osram")
ADDRESS_MAP_END

WRITE_LINE_MEMBER(esq5505_state::esq5505_otis_irq)
{
	otis_irq_state = (state != 0);
	update_irq_to_maincpu();
}

WRITE16_MEMBER(esq5505_state::analog_w)
{
	offset &= 0x7;
	m_analog_values[offset] = data;
}

READ16_MEMBER(esq5505_state::analog_r)
{
	return m_analog_values[m_duart_io & 7];
}

WRITE_LINE_MEMBER(esq5505_state::duart_irq_handler)
{
//    printf("\nDUART IRQ: state %d vector %d\n", state, vector);
	if (state == ASSERT_LINE)
	{
		duart_irq_vector = m_duart->get_irq_vector();
		duart_irq_state = 1;
	}
	else
	{
				duart_irq_state = 0;
	}
	update_irq_to_maincpu();
};

READ8_MEMBER(esq5505_state::duart_input)
{
	floppy_connector *con = machine().device<floppy_connector>("wd1772:0");
	floppy_image_device *floppy = con ? con->get_device() : 0;
	UINT8 result = 0;   // DUART input lines are separate from the output lines

	// on VFX, bit 0 is 1 for 'cartridge present'.
	// on VFX-SD and later, bit 0 is 1 for floppy present, bit 1 is 1 for cartridge present
	if (mame_stricmp(machine().system().name, "vfx") == 0)
	{
		// todo: handle VFX cart-in when we support cartridges
	}
	else
	{
		if (floppy)
		{
			// ready_r returns true if the drive is *not* ready, false if it is
//          if (!floppy->ready_r())
			{
				result |= 1;
			}
		}
	}

	return result;
}

WRITE8_MEMBER(esq5505_state::duart_output)
{
	floppy_connector *con = machine().device<floppy_connector>("wd1772:0");
	floppy_image_device *floppy = con ? con->get_device() : 0;

	m_duart_io = data;

	/*
	    EPS:
	    bit 2 = SSEL

	    VFX:
	    bits 0/1/2 = analog sel
	    bit 6 = ESPHALT
	    bit 7 = SACK (?)

	    VFX-SD & SD-1 (32):
	    bits 0/1/2 = analog sel
	    bit 3 = SSEL (disk side)
	    bit 4 = DSEL (drive select?)
	    bit 6 = ESPHALT
	    bit 7 = SACK (?)
	*/

	if (data & 0x40) {
		if (!m_pump->get_esp_halted()) {
			logerror("ESQ5505: Asserting ESPHALT\n");
			m_pump->set_esp_halted(true);
		}
	} else {
		if (m_pump->get_esp_halted()) {
			logerror("ESQ5505: Clearing ESPHALT\n");
			m_pump->set_esp_halted(false);
		}
	}

	if (floppy)
	{
		if (m_system_type == EPS)
		{
			floppy->ss_w((data & 2)>>1);
		}
		else
		{
			floppy->ss_w(((data & 8)>>3)^1);
		}
	}

//    printf("DUART output: %02x (PC=%x)\n", data, m_maincpu->pc());
}

// MIDI send
WRITE_LINE_MEMBER(esq5505_state::duart_tx_a)
{
	m_mdout->tx(state);
}

WRITE_LINE_MEMBER(esq5505_state::duart_tx_b)
{
	m_panel->rx_w(state);
}

static const duartn68681_config duart_config =
{
	DEVCB_DRIVER_LINE_MEMBER(esq5505_state, duart_irq_handler),
	DEVCB_DRIVER_LINE_MEMBER(esq5505_state, duart_tx_a),
	DEVCB_DRIVER_LINE_MEMBER(esq5505_state, duart_tx_b),
	DEVCB_DRIVER_MEMBER(esq5505_state, duart_input),
	DEVCB_DRIVER_MEMBER(esq5505_state, duart_output),

	500000, 500000, // IP3, IP4
	1000000, 1000000, // IP5, IP6
};

static void esq_dma_end(running_machine &machine, int channel, int irq)
{
	device_t *device = machine.device("mc68450");
	esq5505_state *state = machine.driver_data<esq5505_state>();

	if (irq != 0)
	{
		printf("DMAC IRQ, vector = %x\n", hd63450_get_vector(device, channel));
		state->dmac_irq_state = 1;
		state->dmac_irq_vector = hd63450_get_vector(device, channel);
	}
	else
	{
		state->dmac_irq_state = 0;
	}

	state->update_irq_to_maincpu();
}

static void esq_dma_error(running_machine &machine, int channel, int irq)
{
	device_t *device = machine.device("mc68450");
	esq5505_state *state = machine.driver_data<esq5505_state>();

	if(irq != 0)
	{
		printf("DMAC error, vector = %x\n", hd63450_get_error_vector(device, channel));
		state->dmac_irq_state = 1;
		state->dmac_irq_vector = hd63450_get_vector(device, channel);
	}
	else
	{
		state->dmac_irq_state = 0;
	}

	state->update_irq_to_maincpu();
}

static int esq_fdc_read_byte(running_machine &machine, int addr)
{
	esq5505_state *state = machine.driver_data<esq5505_state>();

	return state->m_fdc->data_r();
}

static void esq_fdc_write_byte(running_machine &machine, int addr, int data)
{
	esq5505_state *state = machine.driver_data<esq5505_state>();
	state->m_fdc->data_w(data & 0xff);
}

#if KEYBOARD_HACK
INPUT_CHANGED_MEMBER(esq5505_state::key_stroke)
{
	int val = (UINT8)(FPTR)param;

	if (val < 0x60)
	{
		if (oldval == 0 && newval == 1)
		{
			if (val == 0 && shift > 0)
			{
				shift -= 32;
				printf("New shift %d\n", shift);
			}
			else if (val == 1 && shift < 32)
			{
				shift += 32;
				printf("New shift %d\n", shift);
			}
			else if (val == 0x02)
			{
				printf("Analog tests!\n");
				m_panel->xmit_char(54 | 0x80); m_panel->xmit_char(0); // Preset down
				m_panel->xmit_char(8 | 0x80);  m_panel->xmit_char(0); // Compare down
				m_panel->xmit_char(8);         m_panel->xmit_char(0); // Compare up
				m_panel->xmit_char(54);        m_panel->xmit_char(0); // Preset up
			}
		}
	}
	else
	{
		val += shift;
		if (oldval == 0 && newval == 1)
		{
			printf("key pressed %d\n", val&0x7f);
			m_panel->xmit_char(val);
			m_panel->xmit_char(0x00);
		}
		else if (oldval == 1 && newval == 0)
		{
	//        printf("key off %x\n", (UINT8)(FPTR)param);
			m_panel->xmit_char(val&0x7f);
			m_panel->xmit_char(0x00);
		}
	}
}
#endif

static const hd63450_intf dmac_interface =
{
	"maincpu",  // CPU - 68000
	{attotime::from_usec(32),attotime::from_nsec(450),attotime::from_usec(4),attotime::from_hz(15625/2)},  // Cycle steal mode timing (guesstimate)
	{attotime::from_usec(32),attotime::from_nsec(450),attotime::from_nsec(50),attotime::from_nsec(50)}, // Burst mode timing (guesstimate)
	esq_dma_end,
	esq_dma_error,
	{ esq_fdc_read_byte, 0, 0, 0 },     // ch 0 = fdc, ch 1 = 340001 (ADC?)
	{ esq_fdc_write_byte, 0, 0, 0 }
};

static const es5505_interface es5505_config =
{
	"waverom",  /* Bank 0 */
	"waverom2", /* Bank 1 */
	4,          /* channels */
	DEVCB_DRIVER_LINE_MEMBER(esq5505_state, esq5505_otis_irq), /* irq */
	DEVCB_DRIVER_MEMBER16(esq5505_state, analog_r) /* ADC */
};

static const esqpanel_interface esqpanel_config =
{
	DEVCB_DEVICE_LINE_MEMBER("duart", duartn68681_device, rx_b_w),
	DEVCB_DRIVER_MEMBER16(esq5505_state, analog_w)
};

static SLOT_INTERFACE_START(midiin_slot)
	SLOT_INTERFACE("midiin", MIDIIN_PORT)
SLOT_INTERFACE_END

static const serial_port_interface midiin_intf =
{
	DEVCB_DEVICE_LINE_MEMBER("duart", duartn68681_device, rx_a_w)   // route MIDI Tx send directly to 68681 channel A Rx
};

static SLOT_INTERFACE_START(midiout_slot)
	SLOT_INTERFACE("midiout", MIDIOUT_PORT)
SLOT_INTERFACE_END

static const serial_port_interface midiout_intf =
{
	DEVCB_NULL  // midi out ports don't transmit inward
};

static MACHINE_CONFIG_START( vfx, esq5505_state )
	MCFG_CPU_ADD("maincpu", M68000, XTAL_10MHz)
	MCFG_CPU_PROGRAM_MAP(vfx_map)

	MCFG_CPU_ADD("esp", ES5510, XTAL_10MHz)
	MCFG_DEVICE_DISABLE()

	MCFG_ESQPANEL2x40_ADD("panel", esqpanel_config)

	MCFG_DUARTN68681_ADD("duart", 4000000, duart_config)

	MCFG_SERIAL_PORT_ADD("mdin", midiin_intf, midiin_slot, "midiin")
	MCFG_SERIAL_PORT_ADD("mdout", midiout_intf, midiout_slot, "midiout")

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("pump", ESQ_5505_5510_PUMP, XTAL_10MHz / (16 * 21))
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)

	MCFG_SOUND_ADD("otis", ES5505, XTAL_10MHz)
	MCFG_SOUND_CONFIG(es5505_config)
	MCFG_SOUND_ROUTE_EX(0, "pump", 1.0, 0)
	MCFG_SOUND_ROUTE_EX(1, "pump", 1.0, 1)
	MCFG_SOUND_ROUTE_EX(2, "pump", 1.0, 2)
	MCFG_SOUND_ROUTE_EX(3, "pump", 1.0, 3)
	MCFG_SOUND_ROUTE_EX(4, "pump", 1.0, 4)
	MCFG_SOUND_ROUTE_EX(5, "pump", 1.0, 5)
	MCFG_SOUND_ROUTE_EX(6, "pump", 1.0, 6)
	MCFG_SOUND_ROUTE_EX(7, "pump", 1.0, 7)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED(eps, vfx)
	MCFG_CPU_MODIFY( "maincpu" )
	MCFG_CPU_PROGRAM_MAP(eps_map)

	MCFG_ESQPANEL_2x40_REMOVE("panel")
	MCFG_ESQPANEL1x22_ADD("panel", esqpanel_config)

	MCFG_WD1772x_ADD("wd1772", 8000000)
	MCFG_FLOPPY_DRIVE_ADD("wd1772:0", ensoniq_floppies, "35dd", esq5505_state::floppy_formats)

	MCFG_HD63450_ADD( "mc68450", dmac_interface )   // MC68450 compatible
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED(vfxsd, vfx)
	MCFG_CPU_MODIFY( "maincpu" )
	MCFG_CPU_PROGRAM_MAP(vfxsd_map)

	MCFG_WD1772x_ADD("wd1772", 8000000)
	MCFG_FLOPPY_DRIVE_ADD("wd1772:0", ensoniq_floppies, "35dd", esq5505_state::floppy_formats)
MACHINE_CONFIG_END

// 32-voice machines with the VFX-SD type config
static MACHINE_CONFIG_START(vfx32, esq5505_state)
	MCFG_CPU_ADD("maincpu", M68000, XTAL_30_4761MHz / 2)
	MCFG_CPU_PROGRAM_MAP(vfxsd_map)

	MCFG_CPU_ADD("esp", ES5510, XTAL_10MHz)
	MCFG_DEVICE_DISABLE()

	MCFG_ESQPANEL2x40_ADD("panel", esqpanel_config)

	MCFG_DUARTN68681_ADD("duart", 4000000, duart_config)

	MCFG_SERIAL_PORT_ADD("mdin", midiin_intf, midiin_slot, "midiin")
	MCFG_SERIAL_PORT_ADD("mdout", midiout_intf, midiout_slot, "midiout")

	MCFG_SPEAKER_STANDARD_STEREO("lspeaker", "rspeaker")

	MCFG_SOUND_ADD("pump", ESQ_5505_5510_PUMP, XTAL_30_4761MHz / (2 * 16 * 32))
	MCFG_SOUND_ROUTE(0, "lspeaker", 1.0)
	MCFG_SOUND_ROUTE(1, "rspeaker", 1.0)

	MCFG_SOUND_ADD("otis", ES5505, XTAL_30_4761MHz / 2)
	MCFG_SOUND_CONFIG(es5505_config)
	MCFG_SOUND_ROUTE_EX(0, "pump", 1.0, 0)
	MCFG_SOUND_ROUTE_EX(1, "pump", 1.0, 1)
	MCFG_SOUND_ROUTE_EX(2, "pump", 1.0, 2)
	MCFG_SOUND_ROUTE_EX(3, "pump", 1.0, 3)
	MCFG_SOUND_ROUTE_EX(4, "pump", 1.0, 4)
	MCFG_SOUND_ROUTE_EX(5, "pump", 1.0, 5)
	MCFG_SOUND_ROUTE_EX(6, "pump", 1.0, 6)
	MCFG_SOUND_ROUTE_EX(7, "pump", 1.0, 7)

	MCFG_WD1772x_ADD("wd1772", 8000000)
	MCFG_FLOPPY_DRIVE_ADD("wd1772:0", ensoniq_floppies, "35dd", esq5505_state::floppy_formats)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED(sq1, vfx)
	MCFG_CPU_MODIFY( "maincpu" )
	MCFG_CPU_PROGRAM_MAP(sq1_map)

	MCFG_ESQPANEL_2x40_REMOVE("panel")
	MCFG_ESQPANEL2x40_SQ1_ADD("panel", esqpanel_config)
MACHINE_CONFIG_END

static INPUT_PORTS_START( vfx )
#if KEYBOARD_HACK
	PORT_START("KEY0")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_A) PORT_CHAR('a') PORT_CHAR('A') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x80)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_S) PORT_CHAR('s') PORT_CHAR('S') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x81)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_D) PORT_CHAR('d') PORT_CHAR('D') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x82)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_F) PORT_CHAR('f') PORT_CHAR('F') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x83)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_G) PORT_CHAR('g') PORT_CHAR('G') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x84)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_H) PORT_CHAR('h') PORT_CHAR('H') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x85)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_J) PORT_CHAR('j') PORT_CHAR('J') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x86)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_K) PORT_CHAR('k') PORT_CHAR('K') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x87)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_L) PORT_CHAR('l') PORT_CHAR('L') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x88)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Q) PORT_CHAR('q') PORT_CHAR('Q') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x89)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_W) PORT_CHAR('w') PORT_CHAR('W') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x8a)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_E) PORT_CHAR('e') PORT_CHAR('E') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x8b)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_R) PORT_CHAR('r') PORT_CHAR('R') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x8c)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_T) PORT_CHAR('t') PORT_CHAR('T') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x8d)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Y) PORT_CHAR('y') PORT_CHAR('Y') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x8e)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_U) PORT_CHAR('u') PORT_CHAR('U') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x8f)

	PORT_START("KEY1")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_I) PORT_CHAR('i') PORT_CHAR('I') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x90)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_O) PORT_CHAR('o') PORT_CHAR('O') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x91)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_P) PORT_CHAR('p') PORT_CHAR('P') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x92)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_Z) PORT_CHAR('z') PORT_CHAR('Z') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x93)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_X) PORT_CHAR('x') PORT_CHAR('X') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x94)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_C) PORT_CHAR('c') PORT_CHAR('C') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x95)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_V) PORT_CHAR('v') PORT_CHAR('V') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x96)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_B) PORT_CHAR('b') PORT_CHAR('B') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x97)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_N) PORT_CHAR('n') PORT_CHAR('N') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x98)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_M) PORT_CHAR('m') PORT_CHAR('M') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x99)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_1) PORT_CHAR('1') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x9a)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_2) PORT_CHAR('2') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x9b)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_3) PORT_CHAR('3') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x9c)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_4) PORT_CHAR('4') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x9d)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_5) PORT_CHAR('5') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x9e)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_6) PORT_CHAR('6') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0x9f)

	PORT_START("KEY2")
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_9) PORT_CHAR('9') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 0)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_0) PORT_CHAR('0') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 1)

	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_CODE(KEYCODE_BACKSLASH) PORT_CHAR('\\') PORT_CHANGED_MEMBER(DEVICE_SELF, esq5505_state, key_stroke, 2)
#endif
INPUT_PORTS_END

ROM_START( vfx )
	ROM_REGION(0x40000, "osrom", 0)
	ROM_LOAD16_BYTE( "vfx210b-low.bin",  0x000000, 0x010000, CRC(c51b19cd) SHA1(2a125b92ffa02ae9d7fb88118d525491d785e87e) )
	ROM_LOAD16_BYTE( "vfx210b-high.bin", 0x000001, 0x010000, CRC(59853be8) SHA1(8e07f69d53f80885d15f624e0b912aeaf3212ee4) )

	ROM_REGION(0x200000, "waverom", ROMREGION_ERASE00)
	ROM_LOAD16_BYTE( "u14.bin", 0x000001, 0x080000, CRC(85592299) SHA1(1aa7cf612f91972baeba15991d9686ccde01599c) )
	ROM_LOAD16_BYTE( "u15.bin", 0x100001, 0x080000, CRC(c0055975) SHA1(5a22f1d5e437c6277eb0cfb1ff1b3f8dcdea1cc6) )

	ROM_REGION(0x80000, "nibbles", ROMREGION_ERASE00)
	ROM_LOAD( "u16.bin", 0x000000, 0x080000, CRC(c3ddaf95) SHA1(44a7bd89cd7e82952cc5100479e110c385246559) )
ROM_END

ROM_START( vfxsd )
	ROM_REGION(0x40000, "osrom", 0)
	ROM_LOAD16_BYTE( "vfxsd_200_lower.bin", 0x000000, 0x010000, CRC(7bd31aea) SHA1(812bf73c4861a5d963f128def14a4a98171c93ad) )
	ROM_LOAD16_BYTE( "vfxsd_200_upper.bin", 0x000001, 0x010000, CRC(9a40efa2) SHA1(e38a2a4514519c1573361cb1526139bfcf94e45a) )

	ROM_REGION(0x200000, "waverom", ROMREGION_ERASE00)
	ROM_LOAD16_BYTE( "u57.bin", 0x000001, 0x080000, CRC(85592299) SHA1(1aa7cf612f91972baeba15991d9686ccde01599c) )
	ROM_LOAD16_BYTE( "u58.bin", 0x100001, 0x080000, CRC(c0055975) SHA1(5a22f1d5e437c6277eb0cfb1ff1b3f8dcdea1cc6) )

	ROM_REGION(0x200000, "waverom2", ROMREGION_ERASE00)

	ROM_REGION(0x80000, "nibbles", 0)
	ROM_LOAD( "u60.bin", 0x000000, 0x080000, CRC(c3ddaf95) SHA1(44a7bd89cd7e82952cc5100479e110c385246559) )
ROM_END

ROM_START( sd1 )
	ROM_REGION(0x40000, "osrom", 0)
	ROM_LOAD16_BYTE( "sd1_410_lo.bin", 0x000000, 0x020000, CRC(faa613a6) SHA1(60066765cddfa9d3b5d09057d8f83fb120f4e65e) )
	ROM_LOAD16_BYTE( "sd1_410_hi.bin", 0x000001, 0x010000, CRC(618c0aa8) SHA1(74acf458aa1d04a0a7a0cd5855c49e6855dbd301) )

	ROM_REGION(0x200000, "waverom", ROMREGION_ERASE00)  // BS=0 region (12-bit)
	ROM_LOAD16_BYTE( "u34.bin", 0x000001, 0x080000, CRC(85592299) SHA1(1aa7cf612f91972baeba15991d9686ccde01599c) )
	ROM_LOAD16_BYTE( "u35.bin", 0x100001, 0x080000, CRC(c0055975) SHA1(5a22f1d5e437c6277eb0cfb1ff1b3f8dcdea1cc6) )

	ROM_REGION(0x200000, "waverom2", ROMREGION_ERASE00) // BS=1 region (16-bit)
	ROM_LOAD16_WORD_SWAP( "u38.bin", 0x000000, 0x100000, CRC(a904190e) SHA1(e4fd4e1130906086fb4182dcb8b51269969e2836) )
	ROM_LOAD16_WORD_SWAP( "u37.bin", 0x100000, 0x100000, CRC(d706cef3) SHA1(24ba35248509e9ca45110e2402b8085006ea0cfc) )

	ROM_REGION(0x80000, "nibbles", 0)
	ROM_LOAD( "u36.bin", 0x000000, 0x080000, CRC(c3ddaf95) SHA1(44a7bd89cd7e82952cc5100479e110c385246559) )
ROM_END

ROM_START( sd132 )
	ROM_REGION(0x40000, "osrom", 0)
	ROM_LOAD16_BYTE( "sd1_410_lo.bin", 0x000000, 0x020000, CRC(faa613a6) SHA1(60066765cddfa9d3b5d09057d8f83fb120f4e65e) )
	ROM_LOAD16_BYTE( "sd1_410_hi.bin", 0x000001, 0x010000, CRC(618c0aa8) SHA1(74acf458aa1d04a0a7a0cd5855c49e6855dbd301) )

	ROM_REGION(0x200000, "waverom", ROMREGION_ERASE00)  // BS=0 region (12-bit)
	ROM_LOAD16_BYTE( "u34.bin", 0x000001, 0x080000, CRC(85592299) SHA1(1aa7cf612f91972baeba15991d9686ccde01599c) )
	ROM_LOAD16_BYTE( "u35.bin", 0x100001, 0x080000, CRC(c0055975) SHA1(5a22f1d5e437c6277eb0cfb1ff1b3f8dcdea1cc6) )

	ROM_REGION(0x200000, "waverom2", ROMREGION_ERASE00) // BS=1 region (16-bit)
	ROM_LOAD16_WORD_SWAP( "u38.bin", 0x000000, 0x100000, CRC(a904190e) SHA1(e4fd4e1130906086fb4182dcb8b51269969e2836) )
	ROM_LOAD16_WORD_SWAP( "u37.bin", 0x100000, 0x100000, CRC(d706cef3) SHA1(24ba35248509e9ca45110e2402b8085006ea0cfc) )

	ROM_REGION(0x80000, "nibbles", ROMREGION_ERASE00)
	ROM_LOAD( "u36.bin", 0x000000, 0x080000, CRC(c3ddaf95) SHA1(44a7bd89cd7e82952cc5100479e110c385246559) )
ROM_END


ROM_START( sq1 )
	ROM_REGION(0x40000, "osrom", 0)
	ROM_LOAD16_BYTE( "sq1lo.bin",    0x000000, 0x010000, CRC(b004cf05) SHA1(567b0dae2e35b06e39da108f9c041fd9bc38fa35) )
	ROM_LOAD16_BYTE( "sq1up.bin",    0x000001, 0x010000, CRC(2e927873) SHA1(06a948cb71fa254b23f4b9236f29035d10778da1) )

	ROM_REGION(0x200000, "waverom", 0)
	ROM_LOAD16_BYTE( "sq1-u25.bin",  0x000001, 0x080000, CRC(26312451) SHA1(9f947a11592fd8420fc581914bf16e7ade75390c) )
	ROM_LOAD16_BYTE( "sq1-u26.bin",  0x100001, 0x080000, CRC(2edaa9dc) SHA1(72fead505c4f44e5736ff7d545d72dfa37d613e2) )

	ROM_REGION(0x80000, "nibbles", ROMREGION_ERASE00)
ROM_END

ROM_START( sqrack )
	ROM_REGION(0x40000, "osrom", 0)
	ROM_LOAD16_BYTE( "sqr-102-lower.bin", 0x000000, 0x010000, CRC(186c85ad) SHA1(801c5cf82823ce31a88688fbee4c11ea5ffdbc10) )
	ROM_LOAD16_BYTE( "sqr-102-upper.bin", 0x000001, 0x010000, CRC(088c9d31) SHA1(30627f21d893888b6159c481bea08e3eedd21902) )

	ROM_REGION(0x200000, "waverom", 0)
	ROM_LOAD16_BYTE( "sq1-u25.bin",  0x000001, 0x080000, CRC(26312451) SHA1(9f947a11592fd8420fc581914bf16e7ade75390c) )
	ROM_LOAD16_BYTE( "sq1-u26.bin",  0x100001, 0x080000, CRC(2edaa9dc) SHA1(72fead505c4f44e5736ff7d545d72dfa37d613e2) )

	ROM_REGION(0x80000, "nibbles", ROMREGION_ERASE00)
ROM_END

ROM_START( eps )
	ROM_REGION(0x10000, "osrom", 0)
	ROM_LOAD16_BYTE( "eps-l.bin",    0x000000, 0x008000, CRC(382beac1) SHA1(110e31edb03fcf7bbde3e17423b21929e5b32db2) )
	ROM_LOAD16_BYTE( "eps-h.bin",    0x000001, 0x008000, CRC(d8747420) SHA1(460597751386eb5f08465699b61381c4acd78065) )

	ROM_REGION(0x200000, "waverom", ROMREGION_ERASE00)  // EPS-16 has no ROM sounds

	ROM_REGION(0x200000, "waverom2", ROMREGION_ERASE00)
ROM_END

DRIVER_INIT_MEMBER(esq5505_state,common)
{
	m_system_type = GENERIC;
	m_duart_io = 0;

	floppy_connector *con = machine().device<floppy_connector>("wd1772:0");
	floppy_image_device *floppy = con ? con->get_device() : 0;
	if (floppy)
	{
		m_fdc->set_floppy(floppy);
		floppy->ss_w(0);
	}
}

DRIVER_INIT_MEMBER(esq5505_state,eps)
{
	DRIVER_INIT_CALL(common);
	m_system_type = EPS;
}

DRIVER_INIT_MEMBER(esq5505_state,sq1)
{
	DRIVER_INIT_CALL(common);
	m_system_type = SQ1;
}

DRIVER_INIT_MEMBER(esq5505_state,denib)
{
	UINT8 *pNibbles = (UINT8 *)memregion("nibbles")->base();
	UINT8 *pBS0L = (UINT8 *)memregion("waverom")->base();
	UINT8 *pBS0H = pBS0L + 0x100000;

	DRIVER_INIT_CALL(common);

	// create the 12 bit samples by patching in the nibbles from the nibble ROM
	// low nibbles go with the lower ROM, high nibbles with the upper ROM
	for (int i = 0; i < 0x80000; i++)
	{
		*pBS0L = (*pNibbles & 0x0f) << 4;
		*pBS0H = (*pNibbles & 0xf0);
		pBS0L += 2;
		pBS0H += 2;
		pNibbles++;
	}
}

CONS( 1988, eps,   0, 0,   eps,   vfx, esq5505_state, eps,    "Ensoniq", "EPS", GAME_NOT_WORKING )   // custom VFD: one alphanumeric 22-char row, one graphics-capable row (alpha row can also do bar graphs)
CONS( 1989, vfx,   0, 0,   vfx,   vfx, esq5505_state, denib,  "Ensoniq", "VFX", GAME_NOT_WORKING )       // 2x40 VFD
CONS( 1989, vfxsd, 0, 0,   vfxsd, vfx, esq5505_state, denib,  "Ensoniq", "VFX-SD", GAME_NOT_WORKING )    // 2x40 VFD
CONS( 1990, sd1,   0, 0,   vfxsd, vfx, esq5505_state, denib,  "Ensoniq", "SD-1", GAME_NOT_WORKING )      // 2x40 VFD
CONS( 1990, sd132, sd1, 0, vfx32, vfx, esq5505_state, denib,  "Ensoniq", "SD-1 32", GAME_NOT_WORKING )   // 2x40 VFD
CONS( 1990, sq1,   0, 0,   sq1,   vfx, esq5505_state, sq1,    "Ensoniq", "SQ-1", GAME_NOT_WORKING )      // 2x16 LCD
CONS( 1990, sqrack,sq1, 0, sq1,   vfx, esq5505_state, sq1,    "Ensoniq", "SQ-Rack", GAME_NOT_WORKING )   // 2x16 LCD
