/*********************************************************************

    flopimg.c

    Floppy disk image abstraction code

*********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <limits.h>

#include "emu.h"
#include "osdcore.h"
#include "ioprocs.h"
#include "flopimg.h"
#include "pool.h"
#include "imageutl.h"

#define TRACK_LOADED		0x01
#define TRACK_DIRTY			0x02


struct _floppy_image
{
	struct io_generic io;

	const struct FloppyFormat *floppy_option;
	struct FloppyCallbacks format;

	/* loaded track stuff */
	int loaded_track_head;
	int loaded_track_index;
	UINT32 loaded_track_size;
	void *loaded_track_data;
	UINT8 loaded_track_status;
	UINT8 flags;

	/* tagging system */
	object_pool *tags;
	void *tag_data;
};



struct _floppy_params
{
	int param;
	int value;
};



static floperr_t floppy_track_unload(floppy_image_legacy *floppy);

OPTION_GUIDE_START(floppy_option_guide)
	OPTION_INT('H', "heads",			"Heads")
	OPTION_INT('T', "tracks",			"Tracks")
	OPTION_INT('S', "sectors",			"Sectors")
	OPTION_INT('L', "sectorlength",		"Sector Bytes")
	OPTION_INT('I', "interleave",		"Interleave")
	OPTION_INT('F', "firstsectorid",	"First Sector")
OPTION_GUIDE_END


static void floppy_close_internal(floppy_image_legacy *floppy, int close_file);

/*********************************************************************
    opening, closing and creating of floppy images
*********************************************************************/

/* basic floppy_image_legacy initialization common to floppy_open() and floppy_create() */
static floppy_image_legacy *floppy_init(void *fp, const struct io_procs *procs, int flags)
{
	floppy_image_legacy *floppy;

	floppy = (floppy_image_legacy *)malloc(sizeof(struct _floppy_image));
	if (!floppy)
		return NULL;

	memset(floppy, 0, sizeof(*floppy));
	floppy->tags = pool_alloc_lib(NULL);
	floppy->tag_data = NULL;
	floppy->io.file = fp;
	floppy->io.procs = procs;
	floppy->io.filler = 0xFF;
	floppy->flags = (UINT8) flags;
	return floppy;
}



/* main code for identifying and maybe opening a disk image; not exposed
 * directly because this function is big and hideous */
static floperr_t floppy_open_internal(void *fp, const struct io_procs *procs, const char *extension,
	const struct FloppyFormat *floppy_options, int max_options, int flags, floppy_image_legacy **outfloppy,
	int *outoption)
{
	floperr_t err;
	floppy_image_legacy *floppy;
	int best_option = -1;
	int best_vote = 0;
	int vote;
	size_t i;

	floppy = floppy_init(fp, procs, flags);
	if (!floppy)
	{
		err = FLOPPY_ERROR_OUTOFMEMORY;
		goto done;
	}

	/* vote on the best format */
	for (i = 0; (i < max_options) && floppy_options[i].construct; i++)
	{
		if (!extension || !floppy_options[i].extensions || image_find_extension(floppy_options[i].extensions, extension))
		{
			if (floppy_options[i].identify)
			{
				vote = 0;
				err = floppy_options[i].identify(floppy, &floppy_options[i], &vote);
				if (err)
					goto done;
			}
			else
			{
				vote = 1;
			}

			/* is this option a better one? */
			if (vote > best_vote)
			{
				best_vote = vote;
				best_option = i;
			}
		}
	}

	/* did we find a format? */
	if (best_option == -1)
	{
		err = FLOPPY_ERROR_INVALIDIMAGE;
		goto done;
	}

	if (outfloppy)
	{
		/* call the format constructor */
		err = floppy_options[best_option].construct(floppy, &floppy_options[best_option], NULL);
		if (err)
			goto done;

		floppy->floppy_option = &floppy_options[best_option];
	}
	if (best_vote != 100)
	{
		printf("Loading image that is not 100%% recognized\n");
	}
	err = FLOPPY_ERROR_SUCCESS;

done:
	/* if we have a floppy disk and we either errored or are not keeping it, close it */
	if (floppy && (!outfloppy || err))
	{
		floppy_close_internal(floppy, FALSE);
		floppy = NULL;
	}

	if (outoption)
		*outoption = err ? -1 : best_option;
	if (outfloppy)
		*outfloppy = floppy;
	return err;
}



floperr_t floppy_identify(void *fp, const struct io_procs *procs, const char *extension,
	const struct FloppyFormat *formats, int *identified_format)
{
	return floppy_open_internal(fp, procs, extension, formats, INT_MAX, FLOPPY_FLAGS_READONLY, NULL, identified_format);
}



floperr_t floppy_open(void *fp, const struct io_procs *procs, const char *extension,
	const struct FloppyFormat *format, int flags, floppy_image_legacy **outfloppy)
{
	return floppy_open_internal(fp, procs, extension, format, 1, flags, outfloppy, NULL);
}



floperr_t floppy_open_choices(void *fp, const struct io_procs *procs, const char *extension,
	const struct FloppyFormat *formats, int flags, floppy_image_legacy **outfloppy)
{
	return floppy_open_internal(fp, procs, extension, formats, INT_MAX, flags, outfloppy, NULL);
}



static floperr_t option_to_floppy_error(optreserr_t oerr)
{
	floperr_t err;
	switch(oerr) {
	case OPTIONRESOLUTION_ERROR_SUCCESS:
		err = FLOPPY_ERROR_SUCCESS;
		break;
	case OPTIONRESOLUTION_ERROR_OUTOFMEMORY:
		err = FLOPPY_ERROR_OUTOFMEMORY;
		break;
	case OPTIONRESOLUTION_ERROR_PARAMOUTOFRANGE:
	case OPTIONRESOLUTION_ERROR_PARAMNOTSPECIFIED:
	case OPTIONRESOLUTION_ERROR_PARAMNOTFOUND:
	case OPTIONRESOLUTION_ERROR_PARAMALREADYSPECIFIED:
	case OPTIONRESOLUTION_ERROR_BADPARAM:
	case OPTIONRESOLUTION_ERROR_SYNTAX:
	default:
		err = FLOPPY_ERROR_INTERNAL;
		break;
	};
	return err;
}



floperr_t floppy_create(void *fp, const struct io_procs *procs, const struct FloppyFormat *format, option_resolution *parameters, floppy_image_legacy **outfloppy)
{
	floppy_image_legacy *floppy = NULL;
	optreserr_t oerr;
	floperr_t err;
	int heads, tracks, h, t;
	option_resolution *alloc_resolution = NULL;

	assert(format);

	/* create the new image */
	floppy = floppy_init(fp, procs, 0);
	if (!floppy)
	{
		err = FLOPPY_ERROR_OUTOFMEMORY;
		goto done;
	}

	/* if this format expects creation parameters and none were specified, create some */
	if (!parameters && format->param_guidelines)
	{
		alloc_resolution = option_resolution_create(floppy_option_guide, format->param_guidelines);
		if (!alloc_resolution)
		{
			err = FLOPPY_ERROR_OUTOFMEMORY;
			goto done;
		}
		parameters = alloc_resolution;
	}

	/* finish the parameters, if specified */
	if (parameters)
	{
		oerr = option_resolution_finish(parameters);
		if (oerr)
		{
			err = option_to_floppy_error(oerr);
			goto done;
		}
	}

	/* call the format constructor */
	err = format->construct(floppy, format, parameters);
	if (err)
		goto done;

	/* format the disk, ignoring if formatting not implemented */
	if (floppy->format.format_track)
	{
		heads = floppy_get_heads_per_disk(floppy);
		tracks = floppy_get_tracks_per_disk(floppy);

		for (h = 0; h < heads; h++)
		{
			for (t = 0; t < tracks; t++)
			{
				err = floppy->format.format_track(floppy, h, t, parameters);
				if (err)
					goto done;
			}
		}
	}

	/* call the post_format function, if present */
	if (floppy->format.post_format)
	{
		err = floppy->format.post_format(floppy, parameters);
		if (err)
			goto done;
	}

	floppy->floppy_option = format;
	err = FLOPPY_ERROR_SUCCESS;

done:
	if (err && floppy)
	{
		floppy_close_internal(floppy, FALSE);
		floppy = NULL;
	}

	if (outfloppy)
		*outfloppy = floppy;
	else if (floppy)
		floppy_close_internal(floppy, FALSE);

	if (alloc_resolution)
		option_resolution_close(alloc_resolution);
	return err;
}



static void floppy_close_internal(floppy_image_legacy *floppy, int close_file)
{
	if (floppy) {
		floppy_track_unload(floppy);

		if(floppy->floppy_option && floppy->floppy_option->destruct)
			floppy->floppy_option->destruct(floppy, floppy->floppy_option);
		if (close_file)
			io_generic_close(&floppy->io);
		if (floppy->loaded_track_data)
			free(floppy->loaded_track_data);
		pool_free_lib(floppy->tags);

		free(floppy);
	}
}



void floppy_close(floppy_image_legacy *floppy)
{
	floppy_close_internal(floppy, TRUE);
}



/*********************************************************************
    functions useful in format constructors
*********************************************************************/

struct FloppyCallbacks *floppy_callbacks(floppy_image_legacy *floppy)
{
	assert(floppy);
	return &floppy->format;
}



void *floppy_tag(floppy_image_legacy *floppy)
{
	assert(floppy);
	return floppy->tag_data;
}



void *floppy_create_tag(floppy_image_legacy *floppy, size_t tagsize)
{
	floppy->tag_data = pool_malloc_lib(floppy->tags,tagsize);
	return floppy->tag_data;
}



UINT8 floppy_get_filler(floppy_image_legacy *floppy)
{
	return floppy->io.filler;
}



void floppy_set_filler(floppy_image_legacy *floppy, UINT8 filler)
{
	floppy->io.filler = filler;
}



/*********************************************************************
    calls for accessing the raw disk image
*********************************************************************/

void floppy_image_read(floppy_image_legacy *floppy, void *buffer, UINT64 offset, size_t length)
{
	io_generic_read(&floppy->io, buffer, offset, length);
}



void floppy_image_write(floppy_image_legacy *floppy, const void *buffer, UINT64 offset, size_t length)
{
	io_generic_write(&floppy->io, buffer, offset, length);
}



void floppy_image_write_filler(floppy_image_legacy *floppy, UINT8 filler, UINT64 offset, size_t length)
{
	io_generic_write_filler(&floppy->io, filler, offset, length);
}



UINT64 floppy_image_size(floppy_image_legacy *floppy)
{
	return io_generic_size(&floppy->io);
}



/*********************************************************************
    calls for accessing disk image data
*********************************************************************/

static floperr_t floppy_readwrite_sector(floppy_image_legacy *floppy, int head, int track, int sector, int offset,
	void *buffer, size_t buffer_len, int writing, int indexed, int ddam)
{
	floperr_t err;
	const struct FloppyCallbacks *fmt;
	size_t this_buffer_len;
	UINT8 *alloc_buf = NULL;
	UINT32 sector_length;
	UINT8 *buffer_ptr = (UINT8 *)buffer;
	floperr_t (*read_sector)(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buflen);
	floperr_t (*write_sector)(floppy_image_legacy *floppy, int head, int track, int sector, const void *buffer, size_t buflen, int ddam);

	fmt = floppy_callbacks(floppy);

	/* choose proper calls for indexed vs non-indexed */
	if (indexed)
	{
		read_sector = fmt->read_indexed_sector;
		write_sector = fmt->write_indexed_sector;
		if (!fmt->get_indexed_sector_info)
		{
			err = FLOPPY_ERROR_UNSUPPORTED;
			goto done;
		}
	}
	else
	{
		read_sector = fmt->read_sector;
		write_sector = fmt->write_sector;
		if (!fmt->get_sector_length)
		{
			err = FLOPPY_ERROR_UNSUPPORTED;
			goto done;
		}
	}

	/* check to make sure that the operation is supported */
	if (!read_sector || (writing && !write_sector))
	{
		err = FLOPPY_ERROR_UNSUPPORTED;
		goto done;
	}

	/* main loop */
	while(buffer_len > 0)
	{
		/* find out the size of this sector */
		if (indexed)
			err = fmt->get_indexed_sector_info(floppy, head, track, sector, NULL, NULL, NULL, &sector_length, NULL);
		else
			err = fmt->get_sector_length(floppy, head, track, sector, &sector_length);
		if (err)
			goto done;

		/* do we even do anything with this sector? */
		if (offset < sector_length)
		{
			/* ok we will be doing something */
			if ((offset > 0) || (buffer_len < sector_length))
			{
				/* we will be doing an partial read/write; in other words we
                 * will not be reading/writing a full sector */
				if (alloc_buf) free(alloc_buf);
				alloc_buf = (UINT8*)malloc(sector_length);
				if (!alloc_buf)
				{
					err = FLOPPY_ERROR_OUTOFMEMORY;
					goto done;
				}

				/* read the sector (we need to do this even when writing */
				err = read_sector(floppy, head, track, sector, alloc_buf, sector_length);
				if (err)
					goto done;

				this_buffer_len = MIN(buffer_len, sector_length - offset);

				if (writing)
				{
					memcpy(alloc_buf + offset, buffer_ptr, this_buffer_len);

					err = write_sector(floppy, head, track, sector, alloc_buf, sector_length, ddam);
					if (err)
						goto done;
				}
				else
				{
					memcpy(buffer_ptr, alloc_buf + offset, this_buffer_len);
				}
				offset += this_buffer_len;
				offset %= sector_length;
			}
			else
			{
				this_buffer_len = sector_length;

				if (writing)
					err = write_sector(floppy, head, track, sector, buffer_ptr, sector_length, ddam);
				else
					err = read_sector(floppy, head, track, sector, buffer_ptr, sector_length);
				if (err)
					goto done;
			}
		}
		else
		{
			/* skip this sector */
			offset -= sector_length;
			this_buffer_len = 0;
		}

		buffer_ptr += this_buffer_len;
		buffer_len -= this_buffer_len;
		sector++;
	}

	err = FLOPPY_ERROR_SUCCESS;

done:
	if (alloc_buf)
		free(alloc_buf);
	return err;
}



floperr_t floppy_read_sector(floppy_image_legacy *floppy, int head, int track, int sector, int offset,	void *buffer, size_t buffer_len)
{
	return floppy_readwrite_sector(floppy, head, track, sector, offset, buffer, buffer_len, FALSE, FALSE, 0);
}



floperr_t floppy_write_sector(floppy_image_legacy *floppy, int head, int track, int sector, int offset, const void *buffer, size_t buffer_len, int ddam)
{
	return floppy_readwrite_sector(floppy, head, track, sector, offset, (void *) buffer, buffer_len, TRUE, FALSE, ddam);
}



floperr_t floppy_read_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector_index, int offset,	void *buffer, size_t buffer_len)
{
	return floppy_readwrite_sector(floppy, head, track, sector_index, offset, buffer, buffer_len, FALSE, TRUE, 0);
}



floperr_t floppy_write_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector_index, int offset, const void *buffer, size_t buffer_len, int ddam)
{
	return floppy_readwrite_sector(floppy, head, track, sector_index, offset, (void *) buffer, buffer_len, TRUE, TRUE, ddam);
}


static floperr_t floppy_get_track_data_offset(floppy_image_legacy *floppy, int head, int track, UINT64 *offset)
{
	floperr_t err;
	const struct FloppyCallbacks *callbacks;

	*offset = 0;
	callbacks = floppy_callbacks(floppy);
	if (callbacks->get_track_data_offset)
	{
		err = callbacks->get_track_data_offset(floppy, head, track, offset);
		if (err)
			return err;
	}
	return FLOPPY_ERROR_SUCCESS;
}



static floperr_t floppy_read_track_offset(floppy_image_legacy *floppy, int head, int track, UINT64 offset, void *buffer, size_t buffer_len)
{
	floperr_t err;
	const struct FloppyCallbacks *format;

	format = floppy_callbacks(floppy);

	if (!format->read_track)
		return FLOPPY_ERROR_UNSUPPORTED;

	err = floppy_track_unload(floppy);
	if (err)
		return err;

	err = format->read_track(floppy, head, track, offset, buffer, buffer_len);
	if (err)
		return err;

	return FLOPPY_ERROR_SUCCESS;
}



floperr_t floppy_read_track(floppy_image_legacy *floppy, int head, int track, void *buffer, size_t buffer_len)
{
	return floppy_read_track_offset(floppy, head, track, 0, buffer, buffer_len);
}



floperr_t floppy_read_track_data(floppy_image_legacy *floppy, int head, int track, void *buffer, size_t buffer_len)
{
	floperr_t err;
	UINT64 offset;

	err = floppy_get_track_data_offset(floppy, head, track, &offset);
	if (err)
		return err;

	return floppy_read_track_offset(floppy, head, track, offset, buffer, buffer_len);
}



static floperr_t floppy_write_track_offset(floppy_image_legacy *floppy, int head, int track, UINT64 offset, const void *buffer, size_t buffer_len)
{
	floperr_t err;

	/* track writing supported? */
	if (!floppy_callbacks(floppy)->write_track)
		return FLOPPY_ERROR_UNSUPPORTED;

	/* read only? */
	if (floppy->flags & FLOPPY_FLAGS_READONLY)
		return FLOPPY_ERROR_READONLY;

	err = floppy_track_unload(floppy);
	if (err)
		return err;

	err = floppy_callbacks(floppy)->write_track(floppy, head, track, offset, buffer, buffer_len);
	if (err)
		return err;

	return FLOPPY_ERROR_SUCCESS;
}



floperr_t floppy_write_track(floppy_image_legacy *floppy, int head, int track, const void *buffer, size_t buffer_len)
{
	return floppy_write_track_offset(floppy, head, track, 0, buffer, buffer_len);
}



floperr_t floppy_write_track_data(floppy_image_legacy *floppy, int head, int track, const void *buffer, size_t buffer_len)
{
	floperr_t err;
	UINT64 offset;

	err = floppy_get_track_data_offset(floppy, head, track, &offset);
	if (err)
		return err;

	return floppy_write_track_offset(floppy, head, track, offset, buffer, buffer_len);
}



floperr_t floppy_format_track(floppy_image_legacy *floppy, int head, int track, option_resolution *parameters)
{
	floperr_t err;
	struct FloppyCallbacks *format;
	option_resolution *alloc_resolution = NULL;
	optreserr_t oerr;

	/* supported? */
	format = floppy_callbacks(floppy);
	if (!format->format_track)
	{
		err = FLOPPY_ERROR_UNSUPPORTED;
		goto done;
	}

	/* create a dummy resolution; if no parameters were specified */
	if (!parameters)
	{
		alloc_resolution = option_resolution_create(floppy_option_guide, floppy->floppy_option->param_guidelines);
		if (!alloc_resolution)
		{
			err = FLOPPY_ERROR_OUTOFMEMORY;
			goto done;
		}
		parameters = alloc_resolution;
	}

	oerr = option_resolution_finish(parameters);
	if (oerr)
	{
		err = option_to_floppy_error(oerr);
		goto done;
	}

	err = format->format_track(floppy, head, track, parameters);
	if (err)
		goto done;

done:
	if (alloc_resolution)
		option_resolution_close(alloc_resolution);
	return err;
}



int floppy_get_tracks_per_disk(floppy_image_legacy *floppy)
{
	return floppy_callbacks(floppy)->get_tracks_per_disk(floppy);
}



int floppy_get_heads_per_disk(floppy_image_legacy *floppy)
{
	return floppy_callbacks(floppy)->get_heads_per_disk(floppy);
}



UINT32 floppy_get_track_size(floppy_image_legacy *floppy, int head, int track)
{
	const struct FloppyCallbacks *fmt;

	fmt = floppy_callbacks(floppy);
	if (!fmt->get_track_size)
		return 0;

	return fmt->get_track_size(floppy, head, track);
}



floperr_t floppy_get_sector_length(floppy_image_legacy *floppy, int head, int track, int sector, UINT32 *sector_length)
{
	const struct FloppyCallbacks *fmt;

	fmt = floppy_callbacks(floppy);
	if (!fmt->get_sector_length)
		return FLOPPY_ERROR_UNSUPPORTED;

	return fmt->get_sector_length(floppy, head, track, sector, sector_length);
}



floperr_t floppy_get_indexed_sector_info(floppy_image_legacy *floppy, int head, int track, int sector_index, int *cylinder, int *side, int *sector, UINT32 *sector_length, unsigned long *flags)
{
	const struct FloppyCallbacks *fmt;

	fmt = floppy_callbacks(floppy);
	if (!fmt->get_indexed_sector_info)
		return FLOPPY_ERROR_UNSUPPORTED;

	return fmt->get_indexed_sector_info(floppy, head, track, sector_index, cylinder, side, sector, sector_length, flags);
}



floperr_t floppy_get_sector_count(floppy_image_legacy *floppy, int head, int track, int *sector_count)
{
	floperr_t err;
	int sector_index = 0;

	do
	{
		err = floppy_get_indexed_sector_info(floppy, head, track, sector_index, NULL, NULL, NULL, NULL, NULL);
		if (!err)
			sector_index++;
	}
	while(!err);

	if (sector_index && (err == FLOPPY_ERROR_SEEKERROR))
		err = FLOPPY_ERROR_SUCCESS;
	if (sector_count)
		*sector_count = err ? 0 : sector_index;
	return err;
}



int floppy_is_read_only(floppy_image_legacy *floppy)
{
	return floppy->flags & FLOPPY_FLAGS_READONLY;
}



UINT8 floppy_random_byte(floppy_image_legacy *floppy)
{
	/* can't use mame_rand(); this might not be in the core */
#ifdef rand
#undef rand
#endif
	return rand();
}



/*********************************************************************
    calls for track based IO
*********************************************************************/

floperr_t floppy_load_track(floppy_image_legacy *floppy, int head, int track, int dirtify, void **track_data, size_t *track_length)
{
	floperr_t err;
	void *new_loaded_track_data;
	UINT32 track_size;

	/* have we already loaded this track? */
	if (((floppy->loaded_track_status & TRACK_LOADED) == 0) || (head != floppy->loaded_track_head) || (track != floppy->loaded_track_index))
	{
		err = floppy_track_unload(floppy);
		if (err)
			goto error;

		track_size = floppy_callbacks(floppy)->get_track_size(floppy, head, track);

		if (floppy->loaded_track_data) free(floppy->loaded_track_data);
		new_loaded_track_data = malloc(track_size);
		if (!new_loaded_track_data)
		{
			err = FLOPPY_ERROR_OUTOFMEMORY;
			goto error;
		}

		floppy->loaded_track_data = new_loaded_track_data;
		floppy->loaded_track_size = track_size;
		floppy->loaded_track_head = head;
		floppy->loaded_track_index = track;

		err = floppy_callbacks(floppy)->read_track(floppy, floppy->loaded_track_head, floppy->loaded_track_index, 0, floppy->loaded_track_data, floppy->loaded_track_size);
		if (err)
			goto error;

		floppy->loaded_track_status |= TRACK_LOADED | (dirtify ? TRACK_DIRTY : 0);
	}
	else
		floppy->loaded_track_status |= (dirtify ? TRACK_DIRTY : 0);

	if (track_data)
		*track_data = floppy->loaded_track_data;
	if (track_length)
		*track_length = floppy->loaded_track_size;
	return FLOPPY_ERROR_SUCCESS;

error:
	if (track_data)
		*track_data = NULL;
	if (track_length)
		*track_length = 0;
	return err;
}



static floperr_t floppy_track_unload(floppy_image_legacy *floppy)
{
	int err;
	if (floppy->loaded_track_status & TRACK_DIRTY)
	{
		err = floppy_callbacks(floppy)->write_track(floppy, floppy->loaded_track_head, floppy->loaded_track_index, 0, floppy->loaded_track_data, floppy->loaded_track_size);
		if (err)
			return (floperr_t)err;
	}

	floppy->loaded_track_status &= ~(TRACK_LOADED | TRACK_DIRTY);
	return FLOPPY_ERROR_SUCCESS;
}



/*********************************************************************
    accessors for meta information about the image
*********************************************************************/

const char *floppy_format_description(floppy_image_legacy *floppy)
{
	return floppy->floppy_option->description;
}



/*********************************************************************
    misc calls
*********************************************************************/

const char *floppy_error(floperr_t err)
{
	static const char *const error_messages[] =
	{
		"The operation completed successfully",
		"Fatal internal error",
		"This operation is unsupported",
		"Out of memory",
		"Seek error",
		"Invalid image",
		"Attempted to write to read only image",
		"No space left on image",
		"Parameter out of range",
		"Required parameter not specified"
	};

	if ((err < 0) || (err >= ARRAY_LENGTH(error_messages)))
		return NULL;
	return error_messages[err];
}


LEGACY_FLOPPY_OPTIONS_START(default)
LEGACY_FLOPPY_OPTIONS_END


//////////////////////////////////////////////////////////
/// New implementation
//////////////////////////////////////////////////////////

floppy_image::floppy_image(int _tracks, int _heads)
{
	tracks = _tracks;
	heads = _heads;

	memset(cell_data, 0, sizeof(cell_data));
	memset(track_size, 0, sizeof(track_size));
	memset(track_alloc_size, 0, sizeof(track_alloc_size));
}

floppy_image::~floppy_image()
{
	for (int i=0;i<tracks;i++) {
		for (int j=0;j<heads;j++) {
			global_free(cell_data[(i<<1) + j]);
		}
	}
}

void floppy_image::get_maximal_geometry(int &_tracks, int &_heads)
{
	_tracks = tracks;
	_heads = heads;
}

void floppy_image::get_actual_geometry(int &_tracks, int &_heads)
{
	int maxt = tracks-1, maxh = heads-1;

	while(maxt >= 0) {
		for(int i=0; i<=maxh; i++)
			if(get_track_size(maxt, i))
				goto track_done;
		maxt--;
	}
 track_done:
	if(maxt >= 0)
		while(maxh >= 0) {
			for(int i=0; i<=maxt; i++)
				if(get_track_size(i, maxh))
					goto head_done;
		}
 head_done:
	_tracks = maxt+1;
	_heads = maxh+1;
}

void floppy_image::ensure_alloc(int track, int head)
{
	int idx = (track << 1) + head;
	if(track_size[idx] > track_alloc_size[idx]) {
		UINT32 new_size = track_size[idx]*11/10;
		UINT32 *new_array = global_alloc_array(UINT32, new_size);
		if(track_alloc_size[idx]) {
			memcpy(new_array, cell_data[idx], track_alloc_size[idx]*4);
			global_free(cell_data[idx]);
		}
		cell_data[idx] = new_array;
		track_alloc_size[idx] = new_size;
	}
}

floppy_image_format_t::floppy_image_format_t()
{
	next = 0;
}

floppy_image_format_t::~floppy_image_format_t()
{
}

void floppy_image_format_t::append(floppy_image_format_t *_next)
{
	if(next)
		next->append(_next);
	else
		next = _next;
}

bool floppy_image_format_t::save(io_generic *, floppy_image *)
{
	return false;
}

bool floppy_image_format_t::type_no_data(int type) const
{
	return type == CRC_CCITT_START ||
		type == CRC_AMIGA_START ||
		type == CRC_END ||
		type == SECTOR_LOOP_START ||
		type == SECTOR_LOOP_END ||
		type == END;
}

bool floppy_image_format_t::type_data_mfm(int type, int p1, const gen_crc_info *crcs) const
{
	return !type_no_data(type) &&
		type != RAW &&
		type != RAWBITS &&
		(type != CRC || (crcs[p1].type != CRC_CCITT && crcs[p1].type != CRC_AMIGA));
}

void floppy_image_format_t::collect_crcs(const desc_e *desc, gen_crc_info *crcs)
{
	memset(crcs, 0, MAX_CRC_COUNT * sizeof(*crcs));
	for(int i=0; i != MAX_CRC_COUNT; i++)
		crcs[i].write = -1;

	for(int i=0; desc[i].type != END; i++)
		switch(desc[i].type) {
		case CRC_CCITT_START:
			crcs[desc[i].p1].type = CRC_CCITT;
			break;
		case CRC_AMIGA_START:
			crcs[desc[i].p1].type = CRC_AMIGA;
			break;
		}

	for(int i=0; desc[i].type != END; i++)
		if(desc[i].type == CRC) {
			int j;
			for(j = i+1; desc[j].type != END && type_no_data(desc[j].type); j++);
			crcs[desc[i].p1].fixup_mfm_clock = type_data_mfm(desc[j].type, desc[j].p1, crcs);
		}
}

int floppy_image_format_t::crc_cells_size(int type) const
{
	switch(type) {
	case CRC_CCITT: return 32;
	case CRC_AMIGA: return 64;
	default: return 0;
	}
}

bool floppy_image_format_t::bit_r(UINT8 *buffer, int offset)
{
	return (buffer[offset >> 3] >> ((offset & 7) ^ 7)) & 1;
}

void floppy_image_format_t::bit_w(UINT8 *buffer, int offset, bool val)
{
	if(val)
		buffer[offset >> 3] |= 0x80 >> (offset & 7);
	else
		buffer[offset >> 3] &= ~(0x80 >> (offset & 7));
}

void floppy_image_format_t::raw_w(UINT8 *buffer, int &offset, int n, UINT32 val)
{
	for(int i=n-1; i>=0; i--)
		bit_w(buffer, offset++, (val >> i) & 1);
}

void floppy_image_format_t::mfm_w(UINT8 *buffer, int &offset, int n, UINT32 val)
{
	int prec = offset ? bit_r(buffer, offset-1) : 0;
	for(int i=n-1; i>=0; i--) {
		int bit = (val >> i) & 1;
		bit_w(buffer, offset++, !(prec || bit));
		bit_w(buffer, offset++, bit);
		prec = bit;
	}
}

void floppy_image_format_t::mfm_half_w(UINT8 *buffer, int &offset, int start_bit, UINT32 val)
{
	int prec = offset ? bit_r(buffer, offset-1) : 0;
	for(int i=start_bit; i>=0; i-=2) {
		int bit = (val >> i) & 1;
		bit_w(buffer, offset++, !(prec || bit));
		bit_w(buffer, offset++, bit);
		prec = bit;
	}
}

void floppy_image_format_t::fixup_crc_amiga(UINT8 *buffer, const gen_crc_info *crc)
{
	UINT16 res = 0;
	int size = crc->end - crc->start;
	for(int i=1; i<size; i+=2)
		if(bit_r(buffer, crc->start + i))
			res = res ^ (0x8000 >> ((i >> 1) & 15));
	int offset = crc->write;
	mfm_w(buffer, offset, 16, 0);
	mfm_w(buffer, offset, 16, res);
}

void floppy_image_format_t::fixup_crc_ccitt(UINT8 *buffer, const gen_crc_info *crc)
{
	UINT32 res = 0xffff;
	int size = crc->end - crc->start;
	for(int i=1; i<size; i+=2) {
		res <<= 1;
		if(bit_r(buffer, crc->start + i))
			res ^= 0x10000;
		if(res & 0x10000)
			res ^= 0x11021;
	}
	int offset = crc->write;
	mfm_w(buffer, offset, 16, res);
}

void floppy_image_format_t::fixup_crcs(UINT8 *buffer, gen_crc_info *crcs)
{
	for(int i=0; i != MAX_CRC_COUNT; i++)
		if(crcs[i].write != -1) {
			switch(crcs[i].type) {
			case CRC_AMIGA: fixup_crc_amiga(buffer, crcs+i); break;
			case CRC_CCITT: fixup_crc_ccitt(buffer, crcs+i); break;
			}
			if(crcs[i].fixup_mfm_clock) {
				int offset = crcs[i].write + crc_cells_size(crcs[i].type);
				bit_w(buffer, offset, !((offset ? bit_r(buffer, offset-1) : false) || bit_r(buffer, offset+1)));
			}
			crcs[i].write = -1;
		}
}

void floppy_image_format_t::generate_track(const desc_e *desc, UINT8 track, UINT8 head, const desc_s *sect, int sect_count, int track_size, floppy_image *image)
{
	UINT8 *buffer = global_alloc_array_clear(UINT8, (track_size+7)/8);

	gen_crc_info crcs[MAX_CRC_COUNT];
	collect_crcs(desc, crcs);

	int offset = 0;
	int index = 0;
	int sector_loop_start = 0;
	int sector_id = 0;
	int sector_limit = 0;

	while(desc[index].type != END) {
		//      printf("%d.%d.%d (%d) - %d %d\n", desc[index].type, desc[index].p1, desc[index].p2, index, offset, offset/8);
		switch(desc[index].type) {
		case MFM:
			for(int i=0; i<desc[index].p2; i++)
				mfm_w(buffer, offset, 8, desc[index].p1);
			break;

		case MFMBITS:
			mfm_w(buffer, offset, desc[index].p2, desc[index].p1);
			break;

		case RAW:
			for(int i=0; i<desc[index].p2; i++)
				raw_w(buffer, offset, 16, desc[index].p1);
			break;

		case RAWBITS:
			raw_w(buffer, offset, desc[index].p2, desc[index].p1);
			break;

		case TRACK_ID:
			mfm_w(buffer, offset, 8, track);
			break;

		case HEAD_ID:
			mfm_w(buffer, offset, 8, head);
			break;

		case SECTOR_ID:
			mfm_w(buffer, offset, 8, sector_id);
			break;

		case SIZE_ID: {
			int size = sect[sector_id].size;
			int id;
			for(id = 0; size > 128; size >>=1, id++);
			mfm_w(buffer, offset, 8, id);
			break;
		}

		case OFFSET_ID_O:
			mfm_half_w(buffer, offset, 7, track*2+head);
			break;

		case OFFSET_ID_E:
			mfm_half_w(buffer, offset, 6, track*2+head);
			break;

		case SECTOR_ID_O:
			mfm_half_w(buffer, offset, 7, sector_id);
			break;

		case SECTOR_ID_E:
			mfm_half_w(buffer, offset, 6, sector_id);
			break;

		case REMAIN_O:
			mfm_half_w(buffer, offset, 7, desc[index].p1 - sector_id);
			break;

		case REMAIN_E:
			mfm_half_w(buffer, offset, 6, desc[index].p1 - sector_id);
			break;

		case SECTOR_LOOP_START:
			fixup_crcs(buffer, crcs);
			sector_loop_start = index;
			sector_id = desc[index].p1;
			sector_limit = desc[index].p2;
			break;

		case SECTOR_LOOP_END:
			fixup_crcs(buffer, crcs);
			if(sector_id < sector_limit) {
				sector_id++;
				index = sector_loop_start;
			}
			break;

		case CRC_AMIGA_START:
		case CRC_CCITT_START:
			crcs[desc[index].p1].start = offset;
			break;

		case CRC_END:
			crcs[desc[index].p1].end = offset;
			break;

		case CRC:
			crcs[desc[index].p1].write = offset;
			offset += crc_cells_size(crcs[desc[index].p1].type);
			break;

		case SECTOR_DATA: {
			const desc_s *csect = sect + (desc[index].p1 >= 0 ? desc[index].p1 : sector_id);
			for(int i=0; i != csect->size; i++)
				mfm_w(buffer, offset, 8, csect->data[i]);
			break;
		}

		case SECTOR_DATA_O: {
			const desc_s *csect = sect + (desc[index].p1 >= 0 ? desc[index].p1 : sector_id);
			for(int i=0; i != csect->size; i++)
				mfm_half_w(buffer, offset, 7, csect->data[i]);
			break;
		}

		case SECTOR_DATA_E: {
			const desc_s *csect = sect + (desc[index].p1 >= 0 ? desc[index].p1 : sector_id);
			for(int i=0; i != csect->size; i++)
				mfm_half_w(buffer, offset, 6, csect->data[i]);
			break;
		}

		default:
			printf("%d.%d.%d (%d) unhandled\n", desc[index].type, desc[index].p1, desc[index].p2, index);
			break;
		}
		index++;
	}

	fixup_crcs(buffer, crcs);

	generate_track_from_bitstream(track, head, buffer, track_size, image);
	global_free(buffer);
}

void floppy_image_format_t::normalize_times(UINT32 *buffer, int bitlen)
{
	unsigned int total_sum = 0;
	for(int i=0; i != bitlen; i++)
		total_sum += buffer[i] & floppy_image::TIME_MASK;

	unsigned int current_sum = 0;
	for(int i=0; i != bitlen; i++) {
		UINT32 time = buffer[i] & floppy_image::TIME_MASK;
		buffer[i] = (buffer[i] & floppy_image::MG_MASK) | (200000000ULL * current_sum / total_sum);
		current_sum += time;
	}
}

void floppy_image_format_t::generate_track_from_bitstream(UINT8 track, UINT8 head, const UINT8 *trackbuf, int track_size, floppy_image *image)
{
	// Maximal number of cells which happens when the buffer is all 1
	image->set_track_size(track, head, track_size+1);
	UINT32 *dest = image->get_buffer(track, head);
	UINT32 *base = dest;

	UINT32 cbit = floppy_image::MG_A;
	UINT32 count = 0;
	for(int i=0; i != track_size; i++)
		if(trackbuf[i >> 3] & (0x80 >> (i & 7))) {
			*dest++ = cbit | (count+1);
			cbit = cbit == floppy_image::MG_A ? floppy_image::MG_B : floppy_image::MG_A;
			count = 1;
		} else
			count += 2;

	if(count)
		*dest++ = cbit | count;

	int size = dest - base;
	normalize_times(base, size);
	image->set_track_size(track, head, size);
}

//  Atari ST Fastcopy Pro layouts

#define SECTOR_42_HEADER(cid)   \
	{ CRC_CCITT_START, cid },   \
	{   RAW, 0x4489, 3 },       \
	{   MFM, 0xfe, 1 },         \
	{   TRACK_ID },             \
	{   HEAD_ID },              \
	{   MFM, 0x42, 1 },         \
	{   MFM, 0x02, 1 },         \
	{ CRC_END, cid },           \
	{ CRC, cid }

#define NORMAL_SECTOR(cid)      \
	{ CRC_CCITT_START, cid },   \
	{   RAW, 0x4489, 3 },       \
	{   MFM, 0xfe, 1 },         \
	{   TRACK_ID },             \
	{   HEAD_ID },              \
	{   SECTOR_ID },            \
	{   SIZE_ID },              \
	{ CRC_END, cid },           \
	{ CRC, cid },               \
	{ MFM, 0x4e, 22 },          \
	{ MFM, 0x00, 12 },          \
	{ CRC_CCITT_START, cid+1 }, \
	{   RAW, 0x4489, 3 },       \
	{   MFM, 0xfb, 1 },         \
	{   SECTOR_DATA, -1 },      \
	{ CRC_END, cid+1 },         \
	{ CRC, cid+1 }

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_9[] = {
	{ MFM, 0x4e, 501 },
	{ MFM, 0x00, 12 },

	SECTOR_42_HEADER(1),

	{ MFM, 0x4e, 22 },
	{ MFM, 0x00, 12 },

	{ SECTOR_LOOP_START, 1, 9 },
	NORMAL_SECTOR(2),
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	{ SECTOR_LOOP_END },

	SECTOR_42_HEADER(4),

	{ MFM, 0x4e, 157 },

	{ END }
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_0[] = {
	{ MFM, 0x4e, 46 },
	SECTOR_42_HEADER(1),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 10 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END }
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_1[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 10, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 9 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_2[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 9, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 8 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_3[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 8, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 7 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_4[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 7, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 6 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_5[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 6, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 5 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_6[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 5, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 4 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_7[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 4, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 3 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_8[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 3, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 2 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_10_9[] = {
	{ MFM, 0x4e, 20 },
	{ SECTOR_LOOP_START, 2, 10 },
	{   MFM, 0x4e, 40 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(1),
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 26 },
	SECTOR_42_HEADER(3),
	{ MFM, 0x4e, 5 },

	{ SECTOR_LOOP_START, 1, 1 },
	{   MFM, 0x00, 12 },
	NORMAL_SECTOR(4),
	{   MFM, 0x4e, 40 },
	{ SECTOR_LOOP_END },

	{ MFM, 0x4e, 49 },
	{ END },
};

const floppy_image_format_t::desc_e *const floppy_image_format_t::atari_st_fcp_10[10] = {
	atari_st_fcp_10_0,
	atari_st_fcp_10_1,
	atari_st_fcp_10_2,
	atari_st_fcp_10_3,
	atari_st_fcp_10_4,
	atari_st_fcp_10_5,
	atari_st_fcp_10_6,
	atari_st_fcp_10_7,
	atari_st_fcp_10_8,
	atari_st_fcp_10_9,
};


const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_0[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_1[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_2[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_3[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_4[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_5[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_6[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_7[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_8[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_9[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};

const floppy_image_format_t::desc_e floppy_image_format_t::atari_st_fcp_11_10[] = {
	{ MFM, 0x4e, 3 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  2,  2 }, NORMAL_SECTOR( 5), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  8,  8 }, NORMAL_SECTOR( 7), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  3,  3 }, NORMAL_SECTOR( 9), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  9,  9 }, NORMAL_SECTOR(11), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  4,  4 }, NORMAL_SECTOR(13), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 10, 10 }, NORMAL_SECTOR(15), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  5,  5 }, NORMAL_SECTOR(17), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START, 11, 11 }, NORMAL_SECTOR(19), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  6,  6 }, NORMAL_SECTOR(21), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  1,  1 }, NORMAL_SECTOR( 1), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 2 }, { MFM, 0x00, 2 },
	{ SECTOR_LOOP_START,  7,  7 }, NORMAL_SECTOR( 3), { SECTOR_LOOP_END },
	{ MFM, 0x4e, 23 },
	{ END },
};


const floppy_image_format_t::desc_e *const floppy_image_format_t::atari_st_fcp_11[11] = {
	atari_st_fcp_11_0,
	atari_st_fcp_11_1,
	atari_st_fcp_11_2,
	atari_st_fcp_11_3,
	atari_st_fcp_11_4,
	atari_st_fcp_11_5,
	atari_st_fcp_11_6,
	atari_st_fcp_11_7,
	atari_st_fcp_11_8,
	atari_st_fcp_11_9,
	atari_st_fcp_11_10,
};

#undef SECTOR_42_HEADER
#undef NORMAL_SECTOR

const floppy_image_format_t::desc_e *floppy_image_format_t::atari_st_fcp_get_desc(UINT8 track, UINT8 head, UINT8 head_count, UINT8 sect_count)
{
	switch(sect_count) {
	case 9:
		return atari_st_fcp_9;
	case 10:
		return atari_st_fcp_10[(track*head_count + head) % 10];
	case 11:
		return atari_st_fcp_11[(track*head_count + head) % 11];
	}
	return 0;
}

//  Amiga layouts

const floppy_image_format_t::desc_e floppy_image_format_t::amiga_11[] = {
	{ SECTOR_LOOP_START, 0, 10 },
	{   MFM, 0x00, 2 },
	{   RAW, 0x4489, 2 },
	{   CRC_AMIGA_START, 1 },
	{     MFMBITS, 0xf, 4 },
	{     OFFSET_ID_O },
	{     SECTOR_ID_O },
	{     REMAIN_O, 11 },
	{     MFMBITS, 0xf, 4 },
	{     OFFSET_ID_E },
	{     SECTOR_ID_E },
	{     REMAIN_E, 11 },
	{     MFM, 0x00, 16 },
	{   CRC_END, 1 },
	{   CRC, 1 },
	{   CRC, 2 },
	{   CRC_AMIGA_START, 2 },
	{     SECTOR_DATA_O, -1 },
	{     SECTOR_DATA_E, -1 },
	{   CRC_END, 2 },
	{ SECTOR_LOOP_END },
	{ MFM, 0x00, 266 },
	{ END }
};

void floppy_image_format_t::generate_bitstream_from_track(UINT8 track, UINT8 head, int cell_size, UINT8 *trackbuf, int &track_size, floppy_image *image)
{
	int tsize = image->get_track_size(track, head);
	if(!tsize || tsize == 1) {
		// Unformatted track
		track_size = 200000000/cell_size;
		memset(trackbuf, 0, (track_size+7)/8);
		return;
	}

	// Start a little before the end of the track to pre-synchronize
	// the pll
	const UINT32 *tbuf = image->get_buffer(track, head);
	int cur_pos = 190000000;
	int cur_entry = tsize-1;
	while((tbuf[cur_entry] & floppy_image::TIME_MASK) > cur_pos)
		cur_entry--;
	cur_entry++;
	if(cur_entry == tsize)
		cur_entry = 0;

	int cur_bit = -1;

	int period = cell_size;
	int period_adjust_base = period * 0.05;

	int min_period = int(cell_size*0.75);
	int max_period = int(cell_size*1.25);
	int phase_adjust = 0;
	int freq_hist = 0;

	for(;;) {
		// Note that all magnetic cell type changes are considered
		// edges.  No randomness added for neutral/damaged cells
		int edge = tbuf[cur_entry] & floppy_image::TIME_MASK;
		if(edge < cur_pos)
			edge += 200000000;
		int next = cur_pos + period + phase_adjust;

		if(edge >= next) {
			// No transition in the window means 0 and pll in free run mode
			if(cur_bit >= 0) {
				trackbuf[cur_bit >> 3] &= ~(0x80 >> (cur_bit & 7));
				cur_bit++;
			}
			phase_adjust = 0;

		} else {
			// Transition in the window means 1, and the pll is adjusted
			if(cur_bit >= 0) {
				trackbuf[cur_bit >> 3] |= 0x80 >> (cur_bit & 7);
				cur_bit++;
			}

			int delta = edge - (next - period/2);

			phase_adjust = 0.65*delta;

			if(delta < 0) {
				if(freq_hist < 0)
					freq_hist--;
				else
					freq_hist = -1;
			} else if(delta > 0) {
				if(freq_hist > 0)
					freq_hist++;
				else
					freq_hist = 1;
			} else
				freq_hist = 0;

			if(freq_hist) {
				int afh = freq_hist < 0 ? -freq_hist : freq_hist;
				if(afh > 1) {
					int aper = period_adjust_base*delta/period;
					if(!aper)
						aper = freq_hist < 0 ? -1 : 1;
					period += aper;

					if(period < min_period)
						period = min_period;
					else if(period > max_period)
						period = max_period;
				}
			}
		}

		cur_pos = next;
		if(cur_pos >= 200000000) {
			if(cur_bit >= 0)
				break;
			cur_bit = 0;
			cur_pos -= 200000000;
			cur_entry = 0;
		}
		while(cur_entry < tsize-1 && (tbuf[cur_entry] & floppy_image::TIME_MASK) < cur_pos)
			cur_entry++;

		// Wrap around
		if(cur_entry == tsize-1 &&
		   (tbuf[cur_entry] & floppy_image::TIME_MASK) < cur_pos) {
			// Wrap to index 0 or 1 depending on whether there is a transition exactly at the index hole
			cur_entry = (tbuf[tsize-1] & floppy_image::MG_MASK) != (tbuf[0] & floppy_image::MG_MASK) ?
				0 : 1;
		}
	}
	// Clean the leftover bottom bits just in case
	trackbuf[cur_bit >> 3] &= ~(0x7f >> (cur_bit & 7));
	track_size = cur_bit;
}

int floppy_image_format_t::sbit_r(const UINT8 *bitstream, int pos)
{
	return (bitstream[pos >> 3] & (0x80 >> (pos & 7))) != 0;
}

int floppy_image_format_t::sbit_rp(const UINT8 *bitstream, int &pos, int track_size)
{
	int res = sbit_r(bitstream, pos);
	pos ++;
	if(pos == track_size)
		pos = 0;
	return res;
}

UINT8 floppy_image_format_t::sbyte_mfm_r(const UINT8 *bitstream, int &pos, int track_size)
{
	UINT8 res = 0;
	for(int i=0; i<8; i++) {
		sbit_rp(bitstream, pos, track_size);
		if(sbit_rp(bitstream, pos, track_size))
			res |= 0x80 >> i;
	}
	return res;
}


void floppy_image_format_t::extract_sectors_from_bitstream_mfm_pc(const UINT8 *bitstream, int track_size, desc_xs *sectors, UINT8 *sectdata, int sectdata_size)
{
	memset(sectors, 0, 256*sizeof(desc_xs));

	// Don't bother if it's just too small
	if(track_size < 100)
		return;

	// Start by detecting all id and data blocks

	// If 100 is not enough, that track is too funky to be worth
	// bothering anyway

	int idblk[100], dblk[100];
	int idblk_count = 0, dblk_count = 0;

	// Precharge the shift register to detect over-the-index stuff
	UINT16 shift_reg = 0;
	for(int i=0; i<16; i++)
		if(sbit_r(bitstream, track_size-16+i))
			shift_reg |= 0x8000 >> i;

	// Scan the bitstream for sync marks and follow them to check for
	// blocks
	for(int i=0; i<track_size; i++) {
		shift_reg = (shift_reg << 1) | sbit_r(bitstream, i);
		if(shift_reg == 0x4489) {
			UINT16 header;
			int pos = i+1;
			do {
				header = 0;
				for(int j=0; j<16; j++)
					if(sbit_rp(bitstream, pos, track_size))
						header |= 0x8000 >> j;
				// Accept strings of sync marks as long and they're not wrapping

				// Wrapping ones have already been take into account
				// thanks to the precharging
			} while(header == 0x4489 && pos > i);

			// fe, ff
			if(header == 0x5554 || header == 0x5555) {
				if(idblk_count < 100)
					idblk[idblk_count++] = pos;
				i = pos-1;
			}
			// fa, fb, fc, fd
			if(header == 0x5544 || header == 0x5545 || header == 0x5553 || header == 0x5551) {
				if(dblk_count < 100)
					dblk[dblk_count++] = pos;
				i = pos-1;
			}
		}
	}

	// Then extract the sectors
	int sectdata_pos = 0;
	for(int i=0; i<idblk_count; i++) {
		int pos = idblk[i];
		UINT8 track = sbyte_mfm_r(bitstream, pos, track_size);
		UINT8 head = sbyte_mfm_r(bitstream, pos, track_size);
		UINT8 sector = sbyte_mfm_r(bitstream, pos, track_size);
		UINT8 size = sbyte_mfm_r(bitstream, pos, track_size);
		if(size >= 8)
			continue;
		int ssize = 128 << size;

		// If we don't have enough space for a sector's data, skip it
		if(ssize + sectdata_pos > sectdata_size)
			continue;

		// Start of IDAM and DAM are supposed to be exactly 704 cells
		// apart.  Of course the hardware is tolerant, but not that
		// tolerant.  Accept +/- 32 cells of shift.

		int d_index;
		for(d_index = 0; d_index < dblk_count; d_index++) {
			int delta = dblk[d_index] - idblk[i];
			if(delta >= 704-32 && delta <= 704+32)
				break;
		}
		if(d_index == dblk_count)
			continue;

		pos = dblk[d_index];

		sectors[sector].track = track;
		sectors[sector].head = head;
		sectors[sector].size = ssize;
		sectors[sector].data = sectdata + sectdata_pos;
		for(int j=0; j<ssize; j++)
			sectdata[sectdata_pos++] = sbyte_mfm_r(bitstream, pos, track_size);
	}
}

void floppy_image_format_t::get_geometry_mfm_pc(floppy_image *image, int cell_size, int &track_count, int &head_count, int &sector_count)
{
	image->get_actual_geometry(track_count, head_count);

	UINT8 bitstream[500000/8];
	UINT8 sectdata[50000];
	desc_xs sectors[256];
	int track_size;

	// Extract an arbitrary track to get an idea of the number of
	// sectors

	// 20 was rarely used for protections, not near the start like
	// 0-10, not near the end like 70+, no special effects on sync
	// like 33

	generate_bitstream_from_track(20, 0, cell_size, bitstream, track_size, image);
	extract_sectors_from_bitstream_mfm_pc(bitstream, track_size, sectors, sectdata, sizeof(sectdata));

	for(sector_count = 44; sector_count > 0 && !sectors[sector_count].data; sector_count--);
}


void floppy_image_format_t::get_track_data_mfm_pc(int track, int head, floppy_image *image, int cell_size, int sector_size, int sector_count, UINT8 *sectdata)
{
	UINT8 bitstream[500000/8];
	UINT8 sectbuf[50000];
	desc_xs sectors[256];
	int track_size;

	generate_bitstream_from_track(track, head, cell_size, bitstream, track_size, image);
	extract_sectors_from_bitstream_mfm_pc(bitstream, track_size, sectors, sectbuf, sizeof(sectbuf));
	for(int sector=1; sector <= sector_count; sector++) {
		UINT8 *sd = sectdata + (sector-1)*sector_size;
		if(sectors[sector].data && sectors[sector].track == track && sectors[sector].head == head) {
			int asize = sectors[sector].size;
			if(asize > sector_size)
				asize = sector_size;
			memcpy(sd, sectors[sector].data, asize);
			if(asize < sector_size)
				memset(sd+asize, 0, sector_size-asize);
		} else
			memset(sd, 0, sector_size);
	}
}

