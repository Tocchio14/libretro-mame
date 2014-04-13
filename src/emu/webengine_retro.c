// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

    webengine_retro.c

    Handle MAME internal web server (stub for libretro).

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "ui/ui.h"
#include "webengine.h"


//**************************************************************************
//  WEB ENGINE
//**************************************************************************

void web_engine::websocket_ready_handler(struct mg_connection *conn) {
}

// Arguments:
//   flags: first byte of websocket frame, see websocket RFC,
//          http://tools.ietf.org/html/rfc6455, section 5.2
//   data, data_len: payload data. Mask, if any, is already applied.
int web_engine::websocket_data_handler(struct mg_connection *conn, int flags,
									char *data, size_t data_len)
{
   return 0;
}

char* websanitize_statefilename ( char* unsanitized )
{
   return unsanitized;
}

int web_engine::json_game_handler(struct mg_connection *conn)
{
	return 1;
}

int web_engine::json_slider_handler(struct mg_connection *conn)
{
	return 1;
}

// This function will be called by mongoose on every new request.
int web_engine::begin_request_handler(struct mg_connection *conn)
{
	return 0;
}

void *web_engine::websocket_keepalive()
{
	return NULL;
}

//-------------------------------------------------
//  web_engine - constructor
//-------------------------------------------------

web_engine::web_engine(emu_options &options)
	: m_options(options),
		m_machine(NULL),
		m_ctx(NULL),
		m_lastupdatetime(0),
		m_exiting_core(false)

{
}

//-------------------------------------------------
//  ~web_engine - destructor
//-------------------------------------------------

web_engine::~web_engine()
{
}

//-------------------------------------------------
//  close - close and cleanup of lua engine
//-------------------------------------------------

void web_engine::close()
{
}


void web_engine::push_message(const char *message)
{
}
