// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    osdepend.c

    OS-dependent code interface.

*******************************************************************c********/


#include "emu.h"
#include "emuopts.h"
#include "osdepend.h"
#include "portmidi/portmidi.h"

extern bool g_print_verbose;

//-------------------------------------------------
//  osd_interface - constructor
//-------------------------------------------------

osd_interface::osd_interface()
	: m_machine(NULL)
{
}


//-------------------------------------------------
//  osd_interface - destructor
//-------------------------------------------------

osd_interface::~osd_interface()
{
}


//-------------------------------------------------
//  init - initialize the OSD system.
//-------------------------------------------------

void osd_interface::init(running_machine &machine)
{
	//
	// This function is responsible for initializing the OSD-specific
	// video and input functionality, and registering that functionality
	// with the MAME core.
	//
	// In terms of video, this function is expected to create one or more
	// render_targets that will be used by the MAME core to provide graphics
	// data to the system. Although it is possible to do this later, the
	// assumption in the MAME core is that the user interface will be
	// visible starting at init() time, so you will have some work to
	// do to avoid these assumptions.
	//
	// In terms of input, this function is expected to enumerate all input
	// devices available and describe them to the MAME core by adding
	// input devices and their attached items (buttons/axes) via the input
	// system.
	//
	// Beyond these core responsibilities, init() should also initialize
	// any other OSD systems that require information about the current
	// running_machine.
	//
	// This callback is also the last opportunity to adjust the options
	// before they are consumed by the rest of the core.
	//
	// Future work/changes:
	//
	// Audio initialization may eventually move into here as well,
	// instead of relying on independent callbacks from each system.
	//

	m_machine = &machine;
	
	emu_options &options = downcast<emu_options &>(machine.options());
	// extract the verbose printing option
	if (options.verbose())
		g_print_verbose = true;

	// ensure we get called on the way out
	machine.add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(FUNC(osd_interface::osd_exit), this));
}


//-------------------------------------------------
//  update - periodic system update
//-------------------------------------------------

void osd_interface::update(bool skip_redraw)
{
	//
	// This method is called periodically to flush video updates to the
	// screen, and also to allow the OSD a chance to update other systems
	// on a regular basis. In general this will be called at the frame
	// rate of the system being run; however, it may be called at more
	// irregular intervals in some circumstances (e.g., multi-screen games
	// or games with asynchronous updates).
	//
}


//-------------------------------------------------
//  init_debugger - perform debugger-specific
//  initialization
//-------------------------------------------------

void osd_interface::init_debugger()
{
	//
	// Unlike init() above, this method is only called if the debugger
	// is active. This gives any OSD debugger interface a chance to
	// create all of its structures.
	//
}


//-------------------------------------------------
//  wait_for_debugger - wait for a debugger
//  command to be processed
//-------------------------------------------------

void osd_interface::wait_for_debugger(device_t &device, bool firststop)
{
	//
	// When implementing an OSD-driver debugger, this method should be
	// overridden to wait for input, process it, and return. It will be
	// called repeatedly until a command is issued that resumes
	// execution.
	//
}

void osd_interface::debugger_update()
{
}

//-------------------------------------------------
//  update_audio_stream - update the stereo audio
//  stream
//-------------------------------------------------

void osd_interface::update_audio_stream(const INT16 *buffer, int samples_this_frame)
{
	//
	// This method is called whenever the system has new audio data to stream.
	// It provides an array of stereo samples in L-R order which should be
	// output at the configured sample_rate.
	//
}


//-------------------------------------------------
//  set_mastervolume - set the system volume
//-------------------------------------------------

void osd_interface::set_mastervolume(int attenuation)
{
	//
	// Attenuation is the attenuation in dB (a negative number).
	// To convert from dB to a linear volume scale do the following:
	//    volume = MAX_VOLUME;
	//    while (attenuation++ < 0)
	//       volume /= 1.122018454;      //  = (10 ^ (1/20)) = 1dB
	//
}


//-------------------------------------------------
//  customize_input_type_list - provide OSD
//  additions/modifications to the input list
//-------------------------------------------------

void osd_interface::customize_input_type_list(simple_list<input_type_entry> &typelist)
{
	//
	// inptport.c defines some general purpose defaults for key and joystick bindings.
	// They may be further adjusted by the OS dependent code to better match the
	// available keyboard, e.g. one could map pause to the Pause key instead of P, or
	// snapshot to PrtScr instead of F12. Of course the user can further change the
	// settings to anything he/she likes.
	//
	// This function is called on startup, before reading the configuration from disk.
	// Scan the list, and change the keys/joysticks you want.
	//
}


//-------------------------------------------------
//  font_open - attempt to "open" a handle to the
//  font with the given name
//-------------------------------------------------

osd_font osd_interface::font_open(const char *name, int &height)
{
	return NULL;
}


//-------------------------------------------------
//  font_close - release resources associated with
//  a given OSD font
//-------------------------------------------------

void osd_interface::font_close(osd_font font)
{
}


//-------------------------------------------------
//  font_get_bitmap - allocate and populate a
//  BITMAP_FORMAT_ARGB32 bitmap containing the
//  pixel values rgb_t(0xff,0xff,0xff,0xff)
//  or rgb_t(0x00,0xff,0xff,0xff) for each
//  pixel of a black & white font
//-------------------------------------------------

bool osd_interface::font_get_bitmap(osd_font font, unicode_char chnum, bitmap_argb32 &bitmap, INT32 &width, INT32 &xoffs, INT32 &yoffs)
{
	return false;
}

//-------------------------------------------------
//  get_slider_list - allocate and populate a
//  list of OS-dependent slider values.
//-------------------------------------------------
void *osd_interface::get_slider_list()
{
	return NULL;
}

void osd_interface::init_subsystems()
{
	if (!video_init())
	{
		video_exit();
		osd_printf_error("video_init: Initialization failed!\n\n\n");
		fflush(stderr);
		fflush(stdout);
		exit(-1);	
	}	
	sound_init();
	input_init();
	// we need pause callbacks
	machine().add_notifier(MACHINE_NOTIFY_PAUSE, machine_notify_delegate(FUNC(osd_interface::input_pause), this));
	machine().add_notifier(MACHINE_NOTIFY_RESUME, machine_notify_delegate(FUNC(osd_interface::input_resume), this));
	
	output_init();
#ifdef USE_NETWORK
	network_init();
#endif
	midi_init();
}

bool osd_interface::video_init()
{
	return true;
}

bool osd_interface::sound_init()
{
	return true;
}

bool osd_interface::input_init()
{
	return true;
}

void osd_interface::input_pause()
{
}

void osd_interface::input_resume()
{
}

bool osd_interface::output_init()
{
	return true;
}

bool osd_interface::network_init()
{
	return true;
}

bool osd_interface::midi_init()
{
	#ifndef DISABLE_MIDI
	Pm_Initialize();
	#endif
	return true;
}


void osd_interface::exit_subsystems()
{
	video_exit();
	sound_exit();
	input_exit();
	output_exit();
	#ifdef USE_NETWORK
	network_exit();
	#endif
	midi_exit();
	debugger_exit();
}
	
void osd_interface::video_exit()
{
}

void osd_interface::sound_exit()
{
}

void osd_interface::input_exit()
{
}

void osd_interface::output_exit()
{
}

void osd_interface::network_exit()
{
}

void osd_interface::midi_exit()
{
	#ifndef DISABLE_MIDI
	Pm_Terminate();
	#endif
}

void osd_interface::debugger_exit()
{
}

void osd_interface::osd_exit()
{
	exit_subsystems();
}

