#define MW_VERBOSE 0

/*
 * video/mystwarr.c - Konami "Pre-GX" video hardware (here there be dragons)
 *
 */

#include "driver.h"
#include "video/konamiic.h"
#include "includes/konamigx.h"

static int layer_colorbase[6];
static int oinprion, cbparam;
static int sprite_colorbase, sub1_colorbase, last_psac_colorbase, gametype;
static int roz_enable, roz_rombank;
static tilemap *ult_936_tilemap;

// create a decoding buffer to hold decodable tiles so that the ROM test will pass by
// reading the original raw data
static void mystwarr_decode_tiles(running_machine *machine)
{
	UINT8 *s = memory_region(machine, "gfx1");
	int len = memory_region_length(machine, "gfx1");
	UINT8 *pFinish = s+len-3;
	UINT8 *d, *decoded;
	int gfxnum;

	for (gfxnum = 0; gfxnum < ARRAY_LENGTH(machine->gfx); gfxnum++)
		if (machine->gfx[gfxnum] != NULL && machine->gfx[gfxnum]->srcdata == s)
			break;
	assert(gfxnum != ARRAY_LENGTH(machine->gfx));

	decoded = auto_alloc_array(machine, UINT8, len);
	d = decoded;

	// now convert the data into a drawable format so we can decode it
	while (s < pFinish)
	{
		/* convert the whole mess to 5bpp planar in System GX's format
               (p3 p1 p2 p0 p5)
               (the original ROMs are stored as chunky for the first 4 bits
               and the 5th bit is planar, which is undecodable as-is) */
		int d0 = ((s[0]&0x80)   )|((s[0]&0x08)<<3)|((s[1]&0x80)>>2)|((s[1]&0x08)<<1)|
		         ((s[2]&0x80)>>4)|((s[2]&0x08)>>1)|((s[3]&0x80)>>6)|((s[3]&0x08)>>3);
		int d1 = ((s[0]&0x40)<<1)|((s[0]&0x04)<<4)|((s[1]&0x40)>>1)|((s[1]&0x04)<<2)|
		         ((s[2]&0x40)>>3)|((s[2]&0x04)   )|((s[3]&0x40)>>5)|((s[3]&0x04)>>2);
		int d2 = ((s[0]&0x20)<<2)|((s[0]&0x02)<<5)|((s[1]&0x20)   )|((s[1]&0x02)<<3)|
		         ((s[2]&0x20)>>2)|((s[2]&0x02)<<1)|((s[3]&0x20)>>4)|((s[3]&0x02)>>1);
		int d3 = ((s[0]&0x10)<<3)|((s[0]&0x01)<<6)|((s[1]&0x10)<<1)|((s[1]&0x01)<<4)|
		         ((s[2]&0x10)>>1)|((s[2]&0x01)<<2)|((s[3]&0x10)>>3)|((s[3]&0x01)   );

		d[0] = d3;
		d[1] = d1;
		d[2] = d2;
		d[3] = d0;
		d[4] = s[4];

		s += 5;
		d += 5;
	}

	gfx_element_set_source(machine->gfx[gfxnum], decoded);
}


// Mystic Warriors requires tile based blending.
static void mystwarr_tile_callback(int layer, int *code, int *color, int *flags)
{
	if (layer==1) {if ((*code&0xff00)+(*color)==0x4101) cbparam++; else cbparam--;} //* water hack (TEMPORARY)

	*color = layer_colorbase[layer] | (*color>>1 & 0x1f);
}

// for games with 5bpp tile data
static void game5bpp_tile_callback(int layer, int *code, int *color, int *flags)
{
	*color = layer_colorbase[layer] | (*color>>1 & 0x1f);
}

// for games with 4bpp tile data
static void game4bpp_tile_callback(int layer, int *code, int *color, int *flags)
{
	*color = layer_colorbase[layer] | (*color>>2 & 0x0f);
}

static void mystwarr_sprite_callback(int *code, int *color, int *priority)
{
	int c = *color;

	*color = sprite_colorbase | (c & 0x001f);
	*priority = c & 0x00f0;
}

static void metamrph_sprite_callback(int *code, int *color, int *priority)
{
	int c = *color;
	int attr = c;

	c = (c & 0x1f) | sprite_colorbase;

	// Bit8 & 9 are effect attributes. It is not known whether the effects are generated by external logic.
	if ((attr & 0x300) != 0x300)
	{
		*color = c;
		*priority = (attr & 0xe0) >> 2;
	}
	else
	{
		*color = c | 3<<K055555_MIXSHIFT | K055555_SKIPSHADOW; // reflection?
		*priority = 0x1c;
	}
}

static void gaiapols_sprite_callback(int *code, int *color, int *priority)
{
	int c = *color;

	*color = sprite_colorbase | (c>>4 & 0x20) | (c & 0x001f);
	*priority = c & 0x00e0;
}

static void martchmp_sprite_callback(int *code, int *color, int *priority)
{
	int c = *color;

	// Bit8 & 9 are effect attributes. It is not known whether the effects are generated by external logic.
	if ((c & 0x3ff) == 0x11f)
		*color = K055555_FULLSHADOW;
	else
		*color = sprite_colorbase | (c & 0x1f);

	if (oinprion & 0xf0)
		*priority = cbparam;  // use PCU2 internal priority
	else
		*priority = c & 0xf0; // use color implied priority
}



static TILE_GET_INFO( get_gai_936_tile_info )
{
	int tileno, colour;
	UINT8 *ROM = memory_region(machine, "gfx4");
	UINT8 *dat1 = ROM, *dat2 = ROM + 0x20000, *dat3 = ROM + 0x60000;

	tileno = dat3[tile_index] | ((dat2[tile_index]&0x3f)<<8);

	if (tile_index & 1)
		colour = (dat1[tile_index>>1]&0xf);
	else
		colour = ((dat1[tile_index>>1]>>4)&0xf);

	if (dat2[tile_index] & 0x80) colour |= 0x10;

	colour |= sub1_colorbase << 4;

	SET_TILE_INFO(0, tileno, colour, 0);
}

VIDEO_START(gaiapols)
{
	K055555_vh_start(machine);
	K054338_vh_start(machine);

	gametype = 0;

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, game4bpp_tile_callback, 0);

	mystwarr_decode_tiles(machine);

	K055673_vh_start(machine, "gfx2", 1, -61, -22, gaiapols_sprite_callback); // stage2 brick walls

	konamigx_mixer_init(machine, 0);

	K056832_set_LayerOffset(0, -2+2-1, 0-1);
	K056832_set_LayerOffset(1,  0+2, 0);
	K056832_set_LayerOffset(2,  2+2, 0);
	K056832_set_LayerOffset(3,  3+2, 0);

	K053936_wraparound_enable(0, 1);
	K053936GP_set_offset(0, -10,  0); // floor tiles in demo loop2 (Elaine vs. boss)

	ult_936_tilemap = tilemap_create(machine, get_gai_936_tile_info, tilemap_scan_rows,  16, 16, 512, 512);
	tilemap_set_transparent_pen(ult_936_tilemap, 0);
}

static TILE_GET_INFO( get_ult_936_tile_info )
{
	int tileno, colour;
	UINT8 *ROM = memory_region(machine, "gfx4");
	UINT8 *dat1 = ROM, *dat2 = ROM + 0x40000;

	tileno = dat2[tile_index] | ((dat1[tile_index]&0x1f)<<8);

	colour = sub1_colorbase;

	SET_TILE_INFO(0, tileno, colour, (dat1[tile_index]&0x40) ? TILE_FLIPX : 0);
}

VIDEO_START(dadandrn)
{
	K055555_vh_start(machine);
	K054338_vh_start(machine);

	gametype = 1;

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, game5bpp_tile_callback, 0);

	mystwarr_decode_tiles(machine);

	K055673_vh_start(machine, "gfx2", 0, -42, -22, gaiapols_sprite_callback);

	konamigx_mixer_init(machine, 0);

	konamigx_mixer_primode(1);

	K056832_set_LayerOffset(0, -2+4, 0);
	K056832_set_LayerOffset(1,  0+4, 0);
	K056832_set_LayerOffset(2,  2+4, 0);
	K056832_set_LayerOffset(3,  3+4, 0);

	K053936_wraparound_enable(0, 1);
	K053936GP_set_offset(0, -8, 0); // Brainy's laser

	ult_936_tilemap = tilemap_create(machine, get_ult_936_tile_info, tilemap_scan_rows,  16, 16, 512, 512);
	tilemap_set_transparent_pen(ult_936_tilemap, 0);
}

VIDEO_START(mystwarr)
{
	K055555_vh_start(machine);
	K054338_vh_start(machine);

	gametype = 0;

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, mystwarr_tile_callback, 0);

	mystwarr_decode_tiles(machine);

	K055673_vh_start(machine, "gfx2", 0, -48, -24, mystwarr_sprite_callback);

	konamigx_mixer_init(machine, 0);

	K056832_set_LayerOffset(0, -2-3, 0);
	K056832_set_LayerOffset(1,  0-3, 0);
	K056832_set_LayerOffset(2,  2-3, 0);
	K056832_set_LayerOffset(3,  3-3, 0);

	cbparam = 0;
}

VIDEO_START(metamrph)
{
	const char * rgn_250 = "gfx3";

	gametype = 0;

	K055555_vh_start(machine);
	K054338_vh_start(machine);
	K053250_vh_start(machine, 1, &rgn_250);

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, game4bpp_tile_callback, 0);

	mystwarr_decode_tiles(machine);

	K055673_vh_start(machine, "gfx2", 1, -51, -22, metamrph_sprite_callback);

	konamigx_mixer_init(machine, 0);

	// other reference, floor at first boss
	K056832_set_LayerOffset(0, -2+4, 0); // text
	K056832_set_LayerOffset(1,  0+4, 0); // attract sea
	K056832_set_LayerOffset(2,  2+4, 0); // attract red monster in background of sea
	K056832_set_LayerOffset(3,  3+4, 0); // attract sky background to sea

	K053250_set_LayerOffset(0, -7, 0);
}

VIDEO_START(viostorm)
{
	gametype = 0;

	K055555_vh_start(machine);
	K054338_vh_start(machine);

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, game4bpp_tile_callback, 0);

	mystwarr_decode_tiles(machine);

	K055673_vh_start(machine, "gfx2", 1, -62, -23, metamrph_sprite_callback);

	konamigx_mixer_init(machine, 0);

	K056832_set_LayerOffset(0, -2+1, 0);
	K056832_set_LayerOffset(1,  0+1, 0);
	K056832_set_LayerOffset(2,  2+1, 0);
	K056832_set_LayerOffset(3,  3+1, 0);
}

VIDEO_START(martchmp)
{
	gametype = 0;

	K055555_vh_start(machine);
	K054338_vh_start(machine);

	K056832_vh_start(machine, "gfx1", K056832_BPP_5, 0, NULL, game5bpp_tile_callback, 0);

	mystwarr_decode_tiles(machine);

	K055673_vh_start(machine, "gfx2", 0, -58, -23, martchmp_sprite_callback);

	konamigx_mixer_init(machine, 0);

	K056832_set_LayerOffset(0, -2-4, 0);
	K056832_set_LayerOffset(1,  0-4, 0);
	K056832_set_LayerOffset(2,  2-4, 0);
	K056832_set_LayerOffset(3,  3-4, 0);

	K054338_invert_alpha(0);
}



VIDEO_UPDATE(mystwarr)
{
	int i, old, blendmode=0;

	if (cbparam<0) cbparam=0; else if (cbparam>=32) blendmode=(1<<16|GXMIX_BLEND_FORCE)<<2; //* water hack (TEMPORARY)

	for (i = 0; i < 4; i++)
	{
		old = layer_colorbase[i];
		layer_colorbase[i] = K055555_get_palette_index(i)<<4;
		if( old != layer_colorbase[i] ) K056832_mark_plane_dirty(i);
	}

	sprite_colorbase = K055555_get_palette_index(4)<<5;

	konamigx_mixer(screen->machine, bitmap, cliprect, 0, 0, 0, 0, blendmode, 0, 0);
	return 0;
}

VIDEO_UPDATE(metamrph)
{
	int i, old;

	for (i = 0; i < 4; i++)
	{
		old = layer_colorbase[i];
		layer_colorbase[i] = K055555_get_palette_index(i)<<4;
		if (old != layer_colorbase[i]) K056832_mark_plane_dirty(i);
	}

	sprite_colorbase = K055555_get_palette_index(4)<<4;

	konamigx_mixer(screen->machine, bitmap, cliprect, 0, GXSUB_K053250 | GXSUB_4BPP, 0, 0, 0, 0, 0);
	return 0;
}

VIDEO_UPDATE(martchmp)
{
	int i, old, blendmode;

	for (i = 0; i < 4; i++)
	{
		old = layer_colorbase[i];
		layer_colorbase[i] = K055555_get_palette_index(i)<<4;
		if (old != layer_colorbase[i]) K056832_mark_plane_dirty(i);
	}

	sprite_colorbase = K055555_get_palette_index(4)<<5;

	cbparam = K055555_read_register(K55_PRIINP_8);
	oinprion = K055555_read_register(K55_OINPRI_ON);

	// not quite right
	blendmode = (oinprion==0xef && K054338_read_register(K338_REG_PBLEND)) ? ((1<<16|GXMIX_BLEND_FORCE)<<2) : 0;

	konamigx_mixer(screen->machine, bitmap, cliprect, 0, 0, 0, 0, blendmode, 0, 0);
	return 0;
}



WRITE16_HANDLER(ddd_053936_enable_w)
{
	if (ACCESSING_BITS_8_15)
	{
		roz_enable = data & 0x0100;
		roz_rombank = (data & 0xc000)>>14;
	}
}

WRITE16_HANDLER(ddd_053936_clip_w)
{
	static UINT16 clip;
	int old, clip_x, clip_y, size_x, size_y;
	int minx, maxx, miny, maxy;

	if (offset == 1)
	{
 		if (ACCESSING_BITS_8_15) K053936GP_clip_enable(0, data & 0x0100);
	}
	else
	{
		old = clip;
		COMBINE_DATA(&clip);
		if (clip != old)
		{
			clip_x = (clip & 0x003f) >> 0;
			clip_y = (clip & 0x0fc0) >> 6;
			size_x = (clip & 0x3000) >> 12;
			size_y = (clip & 0xc000) >> 14;

			switch (size_x)
			{
				case 0x3: size_x = 1; break;
				case 0x2: size_x = 2; break;
				default:  size_x = 4; break;
			}

			switch (size_y)
			{
				case 0x3: size_y = 1; break;
				case 0x2: size_y = 2; break;
				default:  size_y = 4; break;
			}

			minx = clip_x << 7;
			maxx = ((clip_x + size_x) << 7) - 1;
			miny = clip_y << 7;
			maxy = ((clip_y + size_y) << 7) - 1;

			K053936GP_set_cliprect(0, minx, maxx, miny, maxy);
		}
	}
}

// reference: 223e5c in gaiapolis (ROMs 34j and 36m)
READ16_HANDLER(gai_053936_tilerom_0_r)
{
	UINT8 *ROM1 = (UINT8 *)memory_region(space->machine, "gfx4");
	UINT8 *ROM2 = (UINT8 *)memory_region(space->machine, "gfx4");

	ROM1 += 0x20000;
	ROM2 += 0x20000+0x40000;

	return ((ROM1[offset]<<8) | ROM2[offset]);
}

READ16_HANDLER(ddd_053936_tilerom_0_r)
{
	UINT8 *ROM1 = (UINT8 *)memory_region(space->machine, "gfx4");
	UINT8 *ROM2 = (UINT8 *)memory_region(space->machine, "gfx4");

	ROM2 += 0x40000;

	return ((ROM1[offset]<<8) | ROM2[offset]);
}

// reference: 223e1a in gaiapolis (ROM 36j)
READ16_HANDLER(ddd_053936_tilerom_1_r)
{
	UINT8 *ROM = (UINT8 *)memory_region(space->machine, "gfx4");

	return ROM[offset/2];
}

// reference: 223db0 in gaiapolis (ROMs 32n, 29n, 26n)
READ16_HANDLER(gai_053936_tilerom_2_r)
{
	UINT8 *ROM = (UINT8 *)memory_region(space->machine, "gfx3");

	offset += (roz_rombank * 0x100000);

	return ROM[offset/2]<<8;
}

READ16_HANDLER(ddd_053936_tilerom_2_r)
{
	UINT8 *ROM = (UINT8 *)memory_region(space->machine, "gfx3");

	offset += (roz_rombank * 0x100000);

	return ROM[offset]<<8;
}

VIDEO_UPDATE(dadandrn) /* and gaiapols */
{
	int i, newbase, dirty, rozmode;

	if (gametype == 0)
	{
		sprite_colorbase = (K055555_get_palette_index(4)<<4)&0x7f;
		rozmode = GXSUB_4BPP;
	}
	else
	{
		sprite_colorbase = (K055555_get_palette_index(4)<<3)&0x7f;
		rozmode = GXSUB_8BPP;
	}

	if (K056832_get_LayerAssociation())
	{
		for (i=0; i<4; i++)
		{
			newbase = K055555_get_palette_index(i)<<4;
			if (layer_colorbase[i] != newbase)
			{
				layer_colorbase[i] = newbase;
				K056832_mark_plane_dirty(i);
			}
		}
	}
	else
	{
		for (dirty=0, i=0; i<4; i++)
		{
			newbase = K055555_get_palette_index(i)<<4;
			if (layer_colorbase[i] != newbase)
			{
				layer_colorbase[i] = newbase;
				dirty = 1;
			}
		}
		if (dirty) K056832_MarkAllTilemapsDirty();

	}

	last_psac_colorbase = sub1_colorbase;
	sub1_colorbase = K055555_get_palette_index(5);

	if (last_psac_colorbase != sub1_colorbase)
	{
		tilemap_mark_all_tiles_dirty(ult_936_tilemap);

		if (MW_VERBOSE)
			popmessage("K053936: PSAC colorbase changed");
	}

	konamigx_mixer(screen->machine, bitmap, cliprect, (roz_enable) ? ult_936_tilemap : 0, rozmode, 0, 0, 0, 0, 0);
	return 0;
}
