/*********************************************************************

    formats/pc_dsk.c

    PC disk images

*********************************************************************/

#include <string.h>

#include "formats/pc_dsk.h"
#include "formats/basicdsk.h"

struct pc_disk_sizes
{
	UINT32 image_size;
	int sectors;
	int heads;
};



static const struct pc_disk_sizes disk_sizes[] =
{
	{ 8*1*40*512,  8, 1},	/* 5 1/4 inch double density single sided */
	{ 8*2*40*512,  8, 2},	/* 5 1/4 inch double density */
	{ 9*1*40*512,  9, 1},	/* 5 1/4 inch double density single sided */
	{ 9*2*40*512,  9, 2},	/* 5 1/4 inch double density */
	{10*2*40*512, 10, 2},	/* 5 1/4 inch double density single sided */
	{ 9*2*80*512,  9, 2},	/* 80 tracks 5 1/4 inch drives rare in PCs */
	{ 9*2*80*512,  9, 2},	/* 3 1/2 inch double density */
	{15*2*80*512, 15, 2},	/* 5 1/4 inch high density (or japanese 3 1/2 inch high density) */
	{18*2*80*512, 18, 2},	/* 3 1/2 inch high density */
	{36*2*80*512, 36, 2}	/* 3 1/2 inch enhanced density */
};



static floperr_t pc_dsk_compute_geometry(floppy_image_legacy *floppy, struct basicdsk_geometry *geometry)
{
	int i;
	UINT64 size;

	memset(geometry, 0, sizeof(*geometry));
	size = floppy_image_size(floppy);

	for (i = 0; i < ARRAY_LENGTH(disk_sizes); i++)
	{
		if (disk_sizes[i].image_size == size)
		{
			geometry->sectors = disk_sizes[i].sectors;
			geometry->heads = disk_sizes[i].heads;
			geometry->sector_length = 512;
			geometry->first_sector_id = 1;
			geometry->tracks = (int) (size / disk_sizes[i].sectors / disk_sizes[i].heads / geometry->sector_length);
			return FLOPPY_ERROR_SUCCESS;
		}
	}

	if (size >= 0x1a)
	{
		/*
         * get info from boot sector.
         * not correct on all disks
         */
		UINT8 scl, spt, heads;
		floppy_image_read(floppy, &scl, 0x0c, 1);
		floppy_image_read(floppy, &spt, 0x18, 1);
		floppy_image_read(floppy, &heads, 0x1A, 1);

		if (size == ((UINT64) scl) * spt * heads * 0x200)
		{
			geometry->sectors = spt;
			geometry->heads = heads;
			geometry->sector_length = 512;
			geometry->first_sector_id = 1;
			geometry->tracks = scl;
			return FLOPPY_ERROR_SUCCESS;
		}
	}

	return FLOPPY_ERROR_SUCCESS;
}



static FLOPPY_IDENTIFY(pc_dsk_identify)
{
	floperr_t err;
	struct basicdsk_geometry geometry;

	err = pc_dsk_compute_geometry(floppy, &geometry);
	if (err)
		return err;

	*vote = geometry.heads ? 100 : 0;
	return FLOPPY_ERROR_SUCCESS;
}



static FLOPPY_CONSTRUCT(pc_dsk_construct)
{
	floperr_t err;
	struct basicdsk_geometry geometry;

	if (params)
	{
		/* create */
		memset(&geometry, 0, sizeof(geometry));
		geometry.heads = option_resolution_lookup_int(params, PARAM_HEADS);
		geometry.tracks = option_resolution_lookup_int(params, PARAM_TRACKS);
		geometry.sectors = option_resolution_lookup_int(params, PARAM_SECTORS);
		geometry.first_sector_id = 1;
		geometry.sector_length = 512;
	}
	else
	{
		/* open */
		err = pc_dsk_compute_geometry(floppy, &geometry);
		if (err)
			return err;
	}

	return basicdsk_construct(floppy, &geometry);
}



/* ----------------------------------------------------------------------- */

FLOPPY_OPTIONS_START( pc )
	FLOPPY_OPTION( pc_dsk, "dsk,ima,img,ufi,360",		"PC floppy disk image",	pc_dsk_identify, pc_dsk_construct, NULL,
		HEADS([1]-2)
		TRACKS(40/[80])
		SECTORS(8/[9]/10/15/18/36))
FLOPPY_OPTIONS_END
