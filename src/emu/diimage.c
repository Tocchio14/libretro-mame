/***************************************************************************

    diimage.c

    Device image interfaces.

****************************************************************************

    Copyright Miodrag Milanovic
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
#include "ui.h"
#include "pool.h"
#include "zippath.h"


//**************************************************************************
//  DEVICE CONFIG IMAGE INTERFACE
//**************************************************************************
const image_device_type_info device_config_image_interface::m_device_info_array[] =
	{
		{ IO_CARTSLOT,	"cartridge",	"cart" }, /*  0 */
		{ IO_FLOPPY,	"floppydisk",	"flop" }, /*  1 */
		{ IO_HARDDISK,	"harddisk",		"hard" }, /*  2 */
		{ IO_CYLINDER,	"cylinder",		"cyln" }, /*  3 */
		{ IO_CASSETTE,	"cassette",		"cass" }, /*  4 */
		{ IO_PUNCHCARD,	"punchcard",	"pcrd" }, /*  5 */
		{ IO_PUNCHTAPE,	"punchtape",	"ptap" }, /*  6 */
		{ IO_PRINTER,	"printer",		"prin" }, /*  7 */
		{ IO_SERIAL,	"serial",		"serl" }, /*  8 */
		{ IO_PARALLEL,	"parallel",		"parl" }, /*  9 */
		{ IO_SNAPSHOT,	"snapshot",		"dump" }, /* 10 */
		{ IO_QUICKLOAD,	"quickload",	"quik" }, /* 11 */
		{ IO_MEMCARD,	"memcard",		"memc" }, /* 12 */
		{ IO_CDROM,     "cdrom",        "cdrm" }, /* 13 */
		{ IO_MAGTAPE,	"magtape",		"magt" }, /* 14 */
	};

//-------------------------------------------------
//  device_config_image_interface - constructor
//-------------------------------------------------

device_config_image_interface::device_config_image_interface(const machine_config &mconfig, device_config &devconfig)
	: device_config_interface(mconfig, devconfig)
{
}


//-------------------------------------------------
//  ~device_config_image_interface - destructor
//-------------------------------------------------

device_config_image_interface::~device_config_image_interface()
{
}


//-------------------------------------------------
//  find_device_type - search trough list of
//  device types to extact data
//-------------------------------------------------

const image_device_type_info *device_config_image_interface::find_device_type(iodevice_t type)
{
	int i;
	for (i = 0; i < ARRAY_LENGTH(device_config_image_interface::m_device_info_array); i++)
	{
		if (m_device_info_array[i].m_type == type)
			return &m_device_info_array[i];
	}
	return NULL;
}

//-------------------------------------------------
//  device_typename - retrieves device type name
//-------------------------------------------------

const char *device_config_image_interface::device_typename(iodevice_t type)
{
	const image_device_type_info *info = find_device_type(type);
	return (info != NULL) ? info->m_name : NULL;
}

//-------------------------------------------------
//  device_brieftypename - retrieves device
//  brief type name
//-------------------------------------------------

const char *device_config_image_interface::device_brieftypename(iodevice_t type)
{
	const image_device_type_info *info = find_device_type(type);
	return (info != NULL) ? info->m_shortname : NULL;
}

//-------------------------------------------------
//  device_typeid - retrieves device type id
//-------------------------------------------------

iodevice_t device_config_image_interface::device_typeid(const char *name)
{
	int i;
	for (i = 0; i < ARRAY_LENGTH(device_config_image_interface::m_device_info_array); i++)
	{
		if (!mame_stricmp(name, m_device_info_array[i].m_name) || !mame_stricmp(name, m_device_info_array[i].m_shortname))
			return m_device_info_array[i].m_type;
	}
	return (iodevice_t)-1;
}

/*-------------------------------------------------
    device_compute_hash - compute a hash,
    using this device's partial hash if appropriate
-------------------------------------------------*/

void device_config_image_interface::device_compute_hash(hash_collection &hashes, const void *data, size_t length, const char *types) const
{
	/* retrieve the partial hash func */
	device_image_partialhash_func partialhash = get_partial_hash();

	/* compute the hash */
	if (partialhash)
		partialhash(hashes, (const unsigned char*)data, length, types);
	else
		hashes.compute(reinterpret_cast<const UINT8 *>(data), length, types);
}


//**************************************************************************
//  DEVICE image INTERFACE
//**************************************************************************

/*-------------------------------------------------
    memory_error - report a memory error
-------------------------------------------------*/

static void memory_error(const char *message)
{
    fatalerror("%s", message);
}


//-------------------------------------------------
//  device_image_interface - constructor
//-------------------------------------------------

device_image_interface::device_image_interface(running_machine &machine, const device_config &config, device_t &device)
	: device_interface(machine, config, device),
	  m_image_config(dynamic_cast<const device_config_image_interface &>(config)),
	  m_file(NULL),
	  m_mame_file(NULL),
	  m_full_software_name(NULL),
	  m_software_info_ptr(NULL),
	  m_software_part_ptr(NULL)
{
	m_mempool = pool_alloc_lib(memory_error);
}


//-------------------------------------------------
//  ~device_image_interface - destructor
//-------------------------------------------------

device_image_interface::~device_image_interface()
{
    pool_free_lib(m_mempool);
}

/*-------------------------------------------------
    set_image_filename - specifies the filename of
    an image
-------------------------------------------------*/

image_error_t device_image_interface::set_image_filename(const char *filename)
{
    m_name = filename;
    zippath_parent(&m_working_directory, filename);
	m_basename = m_name.cpy(m_name);

	int loc1 = m_name.rchr(0,'\\');
	int loc2 = m_name.rchr(0,'/');
	int loc3 = m_name.rchr(0,':');
	int loc = MAX(loc1,MAX(loc2,loc3));
	if (loc!=-1) {
		m_basename = m_basename.substr(loc + 1,m_basename.len()-loc);
	}
	m_basename_noext = m_basename.cpy(m_basename);
	m_filetype = "";
	loc = m_basename_noext.rchr(0,'.');
	if (loc!=-1) {
		m_basename_noext = m_basename_noext.substr(0,loc);
		m_filetype = m_basename.cpy(m_basename);
		m_filetype = m_filetype.substr(loc + 1,m_filetype.len()-loc);
	}

    return IMAGE_ERROR_SUCCESS;
}

/****************************************************************************
    CREATION FORMATS
****************************************************************************/

/*-------------------------------------------------
    device_get_indexed_creatable_format -
    accesses a specific image format available for
    image creation by index
-------------------------------------------------*/

const image_device_format *device_image_interface::device_get_indexed_creatable_format(int index)
{
    const image_device_format *format = device_get_creatable_formats();
    while(index-- && (format != NULL))
        format = format->m_next;
    return format;
}



/*-------------------------------------------------
    device_get_named_creatable_format -
    accesses a specific image format available for
    image creation by name
-------------------------------------------------*/

const image_device_format *device_image_interface::device_get_named_creatable_format(const char *format_name)
{
    const image_device_format *format = device_get_creatable_formats();
    while((format != NULL) && strcmp(format->m_name, format_name))
        format = format->m_next;
    return format;
}

/****************************************************************************
    ERROR HANDLING
****************************************************************************/

/*-------------------------------------------------
    image_clear_error - clear out any specified
    error
-------------------------------------------------*/

void device_image_interface::clear_error()
{
    m_err = IMAGE_ERROR_SUCCESS;
    if (m_err_message)
    {
        m_err_message.reset();
    }
}



/*-------------------------------------------------
    error - returns the error text for an image
    error
-------------------------------------------------*/
static const char *const messages[] =
{
	"",
	"Internal error",
	"Unsupported operation",
	"Out of memory",
	"File not found",
	"Invalid image",
	"File already open",
	"Unspecified error"
};

const char *device_image_interface::error()
{
    return (m_err_message) ? m_err_message.cstr() : messages[m_err];
}



/*-------------------------------------------------
    seterror - specifies an error on an image
-------------------------------------------------*/

void device_image_interface::seterror(image_error_t err, const char *message)
{
    clear_error();
    m_err = err;
    if (message != NULL)
    {
        m_err_message = message;
    }
}



/*-------------------------------------------------
    message - used to display a message while
    loading
-------------------------------------------------*/

void device_image_interface::message(const char *format, ...)
{
    va_list args;
    char buffer[256];

    /* format the message */
    va_start(args, format);
    vsnprintf(buffer, ARRAY_LENGTH(buffer), format, args);
    va_end(args);

    /* display the popup for a standard amount of time */
    ui_popup_time(5, "%s: %s",
        basename(),
        buffer);
}


/***************************************************************************
    WORKING DIRECTORIES
***************************************************************************/

/*-------------------------------------------------
    try_change_working_directory - tries to change
    the working directory, but only if the directory
    actually exists
-------------------------------------------------*/
bool device_image_interface::try_change_working_directory(const char *subdir)
{
    osd_directory *directory;
    const osd_directory_entry *entry;
    bool success = FALSE;
    bool done = FALSE;

    directory = osd_opendir(m_working_directory.cstr());
    if (directory != NULL)
    {
        while(!done && (entry = osd_readdir(directory)) != NULL)
        {
            if (!mame_stricmp(subdir, entry->name))
            {
                done = TRUE;
                success = entry->type == ENTTYPE_DIR;
            }
        }

        osd_closedir(directory);
    }

    /* did we successfully identify the directory? */
    if (success)
        zippath_combine(&m_working_directory, m_working_directory, subdir);

    return success;
}
/*-------------------------------------------------
    setup_working_directory - sets up the working
    directory according to a few defaults
-------------------------------------------------*/

void device_image_interface::setup_working_directory()
{
    const game_driver *gamedrv;
	char *dst = NULL;

	osd_get_full_path(&dst,".");
    /* first set up the working directory to be the starting directory */
    m_working_directory = dst;

    /* now try browsing down to "software" */
    if (try_change_working_directory("software"))
    {
        /* now down to a directory for this computer */
        gamedrv = &device().machine->system();
        while(gamedrv && !try_change_working_directory(gamedrv->name))
        {
            gamedrv = driver_get_compatible(gamedrv);
        }
    }
	osd_free(dst);
}

//-------------------------------------------------
//  working_directory - returns the working
//  directory to use for this image; this is
//  valid even if not mounted
//-------------------------------------------------

const char * device_image_interface::working_directory()
{
   /* check to see if we've never initialized the working directory */
    if (!m_working_directory)
        setup_working_directory();

    return m_working_directory;
}


/*-------------------------------------------------
    get_software_region
-------------------------------------------------*/

UINT8 *device_image_interface::get_software_region(const char *tag)
{
	char full_tag[256];

	if ( m_software_info_ptr == NULL || m_software_part_ptr == NULL )
		return NULL;

	sprintf( full_tag, "%s:%s", device().tag(), tag );
	return device().machine->region( full_tag )->base();
}


/*-------------------------------------------------
    image_get_software_region_length
-------------------------------------------------*/

UINT32 device_image_interface::get_software_region_length(const char *tag)
{
    char full_tag[256];

    sprintf( full_tag, "%s:%s", device().tag(), tag );
    return device().machine->region( full_tag )->bytes();
}


/*-------------------------------------------------
 image_get_feature
 -------------------------------------------------*/

const char *device_image_interface::get_feature(const char *feature_name)
{
	feature_list *feature;

	if ( ! m_software_part_ptr->featurelist )
		return NULL;

	for ( feature = m_software_part_ptr->featurelist; feature; feature = feature->next )
	{
		if ( ! strcmp( feature->name, feature_name ) )
			return feature->value;
	}

	return NULL;
}

/****************************************************************************
  Memory allocators

  These allow memory to be allocated for the lifetime of a mounted image.
  If these (and the above accessors) are used well enough, they should be
  able to eliminate the need for a unload function.
****************************************************************************/

void *device_image_interface::image_malloc(size_t size)
{
    return image_realloc(NULL, size);
}

char *device_image_interface::image_strdup(const char *src)
{
	return pool_strdup_lib(m_mempool, src);
}

void *device_image_interface::image_realloc(void *ptr, size_t size)
{
    return pool_realloc_lib(m_mempool, ptr, size);
}

void device_image_interface::image_freeptr(void *ptr)
{
	pool_object_remove(m_mempool, ptr, 0);
}


/****************************************************************************
  Hash info loading

  If the hash is not checked and the relevant info not loaded, force that info
  to be loaded
****************************************************************************/

void device_image_interface::run_hash(void (*partialhash)(hash_collection &, const unsigned char *, unsigned long, const char *),
    hash_collection &hashes, const char *types)
{
    UINT32 size;
    UINT8 *buf = NULL;

    hashes.reset();
    size = (UINT32) length();

    buf = (UINT8*)malloc(size);
	memset(buf,0,size);

    /* read the file */
    fseek(0, SEEK_SET);
    fread(buf, size);

    if (partialhash)
        partialhash(hashes, buf, size, types);
    else
        hashes.compute(buf, size, types);

    /* cleanup */
    free(buf);
    fseek(0, SEEK_SET);
}



void device_image_interface::image_checkhash()
{
    device_image_partialhash_func partialhash;

    /* only calculate CRC if it hasn't been calculated, and the open_mode is read only */
    if (m_hash.first() == NULL && !m_writeable && !m_created)
    {
        /* do not cause a linear read of 600 megs please */
        /* TODO: use SHA/MD5 in the CHD header as the hash */
        if (m_image_config.image_type() == IO_CDROM)
            return;

		/* Skip calculating the hash when we have an image mounted through a software list */
		if ( m_software_info_ptr )
			return;

        /* retrieve the partial hash func */
        partialhash = get_partial_hash();

        run_hash(partialhash, m_hash, hash_collection::HASH_TYPES_ALL);
    }
    return;
}

UINT32 device_image_interface::crc()
{
    UINT32 crc = 0;

	image_checkhash();
    m_hash.crc(crc);

    return crc;
}

/****************************************************************************
  Battery functions

  These functions provide transparent access to battery-backed RAM on an
  image; typically for cartridges.
****************************************************************************/


/*-------------------------------------------------
    battery_load - retrieves the battery
    backed RAM for an image. The file name is
    created from the machine driver name and the
    image name.
-------------------------------------------------*/
void device_image_interface::battery_load(void *buffer, int length, int fill)
{
    astring *fname = astring_assemble_4(astring_alloc(), device().machine->system().name, PATH_SEPARATOR, m_basename_noext, ".nv");

    image_battery_load_by_name(device().machine->options(), astring_c(fname), buffer, length, fill);
    astring_free(fname);
}

/*-------------------------------------------------
    battery_save - stores the battery
    backed RAM for an image. The file name is
    created from the machine driver name and the
    image name.
-------------------------------------------------*/
void device_image_interface::battery_save(const void *buffer, int length)
{
    astring *fname = astring_assemble_4(astring_alloc(), device().machine->system().name, PATH_SEPARATOR, m_basename_noext, ".nv");

    image_battery_save_by_name(device().machine->options(), astring_c(fname), buffer, length);
    astring_free(fname);
}

