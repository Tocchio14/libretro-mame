/***************************************************************************

    divideo.c

    Device video interfaces.

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#include "emu.h"


const char device_video_interface::s_unconfigured_screen_tag[] = "!!UNCONFIGURED!!";



//**************************************************************************
//  DEVICE VIDEO INTERFACE
//**************************************************************************

//-------------------------------------------------
//  device_video_interface - constructor
//-------------------------------------------------

device_video_interface::device_video_interface(const machine_config &mconfig, device_t &device, bool screen_required)
	: device_interface(device),
		m_screen_required(screen_required),
		m_screen_tag(s_unconfigured_screen_tag),
		m_screen(NULL)
{
}


//-------------------------------------------------
//  ~device_video_interface - destructor
//-------------------------------------------------

device_video_interface::~device_video_interface()
{
}


//-------------------------------------------------
//  static_add_route - configuration helper to add
//  a new route to the device
//-------------------------------------------------

void device_video_interface::static_set_screen(device_t &device, const char *tag)
{
	// find our video interface
	device_video_interface *video;
	if (!device.interface(video))
		throw emu_fatalerror("MCFG_VIDEO_SET_SCREEN called on device '%s' with no video interface", device.tag());
	video->m_screen_tag = tag;
}


//-------------------------------------------------
//  interface_validity_check - validation for a
//  device after the configuration has been
//  constructed
//-------------------------------------------------

void device_video_interface::interface_validity_check(validity_checker &valid) const
{
	// only look up screens if we haven't explicitly requested no screen
	screen_device *screen = NULL;
	if (m_screen_tag != NULL)
	{
		// find the screen device if explicitly configured
		if (strcmp(m_screen_tag, s_unconfigured_screen_tag) != 0)
		{
			screen = device().siblingdevice<screen_device>(m_screen_tag);
			if (screen == NULL)
				mame_printf_error("Screen '%s' not found, explicitly set for device '%s'", m_screen_tag, device().tag());
		}
		
		// otherwise, look for a single match
		else
		{
			screen_device_iterator iter(device().mconfig().root_device());
			screen = iter.first();
			if (iter.next() != NULL)
				mame_printf_error("No screen specified for device '%s', but multiple screens found", device().tag());
		}
	}

	// error if no screen is found
	if (screen == NULL && m_screen_required)
		mame_printf_error("Device '%s' requires a screen", device().tag());
}


//-------------------------------------------------
//  interface_pre_start - make sure all our input
//  devices are started
//-------------------------------------------------

void device_video_interface::interface_pre_start()
{
	// only look up screens if we haven't explicitly requested no screen
	if (m_screen_tag != NULL)
	{
		// find the screen device if explicitly configured
		if (strcmp(m_screen_tag, s_unconfigured_screen_tag) != 0)
		{
			m_screen = device().siblingdevice<screen_device>(m_screen_tag);
			if (m_screen == NULL)
				throw emu_fatalerror("Screen '%s' not found, explicitly set for device '%s'", m_screen_tag, device().tag());
		}
		
		// otherwise, look for a single match
		else
		{
			screen_device_iterator iter(device().machine().root_device());
			m_screen = iter.first();
			if (iter.next() != NULL)
				throw emu_fatalerror("No screen specified for device '%s', but multiple screens found", device().tag());
		}
	}

	// fatal error if no screen is found
	if (m_screen == NULL && m_screen_required)
		throw emu_fatalerror("Device '%s' requires a screen", device().tag());

	// if we have a screen and it's not started, wait for it
	if (m_screen != NULL && !m_screen->started())
		throw device_missing_dependencies();
}
