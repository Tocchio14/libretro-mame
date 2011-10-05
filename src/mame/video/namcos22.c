/**
 * video hardware for Namco System22
 *
 * todo:
 *
 * - emulate slave dsp!
 * - fog (not even hooked yet up on not-super)
 * - polygon seaming (fix those small gaps between polygons)
 * - missing/wrong sided textures (eg. ridgerac race start, propcycl scoreboard)
 * - spot
 *
 * - spritelayer:
 *   + xy offset
 *   + clipping to window (eg. timecris)
 *   + eliminate garbage in airco22b
 *   + some missing sprites in cycbrcycc (most easy to spot is the missing city picture at titlescreen)
 *
 * - lots of smaller issues
 *
 *
 *******************************
czram[0] = 1fff 1fdf 1fbf 1f9f 1f7f 1f5f 1f3f 1f1f 1eff 1edf 1ebf 1e9f 1e7f 1e5f 1e3f 1e1f 1dff 1ddf 1dbf 1d9f 1d7f 1d5f 1d3f 1d1f 1cff 1cdf 1cbf 1c9f 1c7f 1c5f 1c3f 1c1f 1bff 1bdf 1bbf 1b9f 1b7f 1b5f 1b3f 1b1f 1aff 1adf 1abf 1a9f 1a7f 1a5f 1a3f 1a1f 19ff 19df 19bf 199f 197f 195f 193f 191f 18ff 18df 18bf 189f 187f 185f 183f 181f 17ff 17df 17bf 179f 177f 175f 173f 171f 16ff 16df 16bf 169f 167f 165f 163f 161f 15ff 15df 15bf 159f 157f 155f 153f 151f 14ff 14df 14bf 149f 147f 145f 143f 141f 13ff 13df 13bf 139f 137f 135f 133f 131f 12ff 12df 12bf 129f 127f 125f 123f 121f 11ff 11df 11bf 119f 117f 115f 113f 111f 10ff 10df 10bf 109f 107f 105f 103f 101f 0fff 0fdf 0fbf 0f9f 0f7f 0f5f 0f3f 0f1f 0eff 0edf 0ebf 0e9f 0e7f 0e5f 0e3f 0e1f 0dff 0ddf 0dbf 0d9f 0d7f 0d5f 0d3f 0d1f 0cff 0cdf 0cbf 0c9f 0c7f 0c5f 0c3f 0c1f 0bff 0bdf 0bbf 0b9f 0b7f 0b5f 0b3f 0b1f 0aff 0adf 0abf 0a9f 0a7f 0a5f 0a3f 0a1f 09ff 09df 09bf 099f 097f 095f 093f 091f 08ff 08df 08bf 089f 087f 085f 083f 081f 07ff 07df 07bf 079f 077f 075f 073f 071f 06ff 06df 06bf 069f 067f 065f 063f 061f 05ff 05df 05bf 059f 057f 055f 053f 051f 04ff 04df 04bf 049f 047f 045f 043f 041f 03ff 03df 03bf 039f 037f 035f 033f 031f 02ff 02df 02bf 029f 027f 025f 023f 021f 01ff 01df 01bf 019f 017f 015f 013f 011f 00ff 00df 00bf 009f 007f 005f 003f 001f
czram[1] = 0000 0000 0000 0001 0002 0003 0005 0007 0009 000b 000e 0011 0014 0017 001b 001f 0023 0027 002c 0031 0036 003b 0041 0047 004d 0053 005a 0061 0068 006f 0077 007f 0087 008f 0098 00a1 00aa 00b3 00bd 00c7 00d1 00db 00e6 00f1 00fc 0107 0113 011f 012b 0137 0144 0151 015e 016b 0179 0187 0195 01a3 01b2 01c1 01d0 01df 01ef 01ff 020f 021f 0230 0241 0252 0263 0275 0287 0299 02ab 02be 02d1 02e4 02f7 030b 031f 0333 0347 035c 0371 0386 039b 03b1 03c7 03dd 03f3 040a 0421 0438 044f 0467 047f 0497 04af 04c8 04e1 04fa 0513 052d 0547 0561 057b 0596 05b1 05cc 05e7 0603 061f 063b 0657 0674 0691 06ae 06cb 06e9 0707 0725 0743 0762 0781 07a0 07bf 07df 07ff 081f 083f 0860 0881 08a2 08c3 08e5 0907 0929 094b 096e 0991 09b4 09d7 09fb 0a1f 0a43 0a67 0a8c 0ab1 0ad6 0afb 0b21 0b47 0b6d 0b93 0bba 0be1 0c08 0c2f 0c57 0c7f 0ca7 0ccf 0cf8 0d21 0d4a 0d73 0d9d 0dc7 0df1 0e1b 0e46 0e71 0e9c 0ec7 0ef3 0f1f 0f4b 0f77 0fa4 0fd1 0ffe 102b 1059 1087 10b5 10e3 1112 1141 1170 119f 11cf 11ff 122f 125f 1290 12c1 12f2 1323 1355 1387 13b9 13eb 141e 1451 1484 14b7 14eb 151f 1553 1587 15bc 15f1 1626 165b 1691 16c7 16fd 1733 176a 17a1 17d8 180f 1847 187f 18b7 18ef 1928 1961 199a 19d3 1a0d 1a47 1a81 1abb 1af6 1b31 1b6c 1ba7 1be3 1c1f 1c5b 1c97 1cd4 1d11 1d4e 1d8b 1dc9 1e07 1e45 1e83 1ec2 1f01 1f40 1f7f 1fbf 1fff
czram[2] = 003f 007f 00be 00fd 013c 017b 01b9 01f7 0235 0273 02b0 02ed 032a 0367 03a3 03df 041b 0457 0492 04cd 0508 0543 057d 05b7 05f1 062b 0664 069d 06d6 070f 0747 077f 07b7 07ef 0826 085d 0894 08cb 0901 0937 096d 09a3 09d8 0a0d 0a42 0a77 0aab 0adf 0b13 0b47 0b7a 0bad 0be0 0c13 0c45 0c77 0ca9 0cdb 0d0c 0d3d 0d6e 0d9f 0dcf 0dff 0e2f 0e5f 0e8e 0ebd 0eec 0f1b 0f49 0f77 0fa5 0fd3 1000 102d 105a 1087 10b3 10df 110b 1137 1162 118d 11b8 11e3 120d 1237 1261 128b 12b4 12dd 1306 132f 1357 137f 13a7 13cf 13f6 141d 1444 146b 1491 14b7 14dd 1503 1528 154d 1572 1597 15bb 15df 1603 1627 164a 166d 1690 16b3 16d5 16f7 1719 173b 175c 177d 179e 17bf 17df 17ff 181f 183f 185e 187d 189c 18bb 18d9 18f7 1915 1933 1950 196d 198a 19a7 19c3 19df 19fb 1a17 1a32 1a4d 1a68 1a83 1a9d 1ab7 1ad1 1aeb 1b04 1b1d 1b36 1b4f 1b67 1b7f 1b97 1baf 1bc6 1bdd 1bf4 1c0b 1c21 1c37 1c4d 1c63 1c78 1c8d 1ca2 1cb7 1ccb 1cdf 1cf3 1d07 1d1a 1d2d 1d40 1d53 1d65 1d77 1d89 1d9b 1dac 1dbd 1dce 1ddf 1def 1dff 1e0f 1e1f 1e2e 1e3d 1e4c 1e5b 1e69 1e77 1e85 1e93 1ea0 1ead 1eba 1ec7 1ed3 1edf 1eeb 1ef7 1f02 1f0d 1f18 1f23 1f2d 1f37 1f41 1f4b 1f54 1f5d 1f66 1f6f 1f77 1f7f 1f87 1f8f 1f96 1f9d 1fa4 1fab 1fb1 1fb7 1fbd 1fc3 1fc8 1fcd 1fd2 1fd7 1fdb 1fdf 1fe3 1fe7 1fea 1fed 1ff0 1ff3 1ff5 1ff7 1ff9 1ffb 1ffc 1ffd 1ffe 1fff 1fff 1fff
czram[3] = 0000 001f 003f 005f 007f 009f 00bf 00df 00ff 011f 013f 015f 017f 019f 01bf 01df 01ff 021f 023f 025f 027f 029f 02bf 02df 02ff 031f 033f 035f 037f 039f 03bf 03df 03ff 041f 043f 045f 047f 049f 04bf 04df 04ff 051f 053f 055f 057f 059f 05bf 05df 05ff 061f 063f 065f 067f 069f 06bf 06df 06ff 071f 073f 075f 077f 079f 07bf 07df 07ff 081f 083f 085f 087f 089f 08bf 08df 08ff 091f 093f 095f 097f 099f 09bf 09df 09ff 0a1f 0a3f 0a5f 0a7f 0a9f 0abf 0adf 0aff 0b1f 0b3f 0b5f 0b7f 0b9f 0bbf 0bdf 0bff 0c1f 0c3f 0c5f 0c7f 0c9f 0cbf 0cdf 0cff 0d1f 0d3f 0d5f 0d7f 0d9f 0dbf 0ddf 0dff 0e1f 0e3f 0e5f 0e7f 0e9f 0ebf 0edf 0eff 0f1f 0f3f 0f5f 0f7f 0f9f 0fbf 0fdf 0fff 101f 103f 105f 107f 109f 10bf 10df 10ff 111f 113f 115f 117f 119f 11bf 11df 11ff 121f 123f 125f 127f 129f 12bf 12df 12ff 131f 133f 135f 137f 139f 13bf 13df 13ff 141f 143f 145f 147f 149f 14bf 14df 14ff 151f 153f 155f 157f 159f 15bf 15df 15ff 161f 163f 165f 167f 169f 16bf 16df 16ff 171f 173f 175f 177f 179f 17bf 17df 17ff 181f 183f 185f 187f 189f 18bf 18df 18ff 191f 193f 195f 197f 199f 19bf 19df 19ff 1a1f 1a3f 1a5f 1a7f 1a9f 1abf 1adf 1aff 1b1f 1b3f 1b5f 1b7f 1b9f 1bbf 1bdf 1bff 1c1f 1c3f 1c5f 1c7f 1c9f 1cbf 1cdf 1cff 1d1f 1d3f 1d5f 1d7f 1d9f 1dbf 1ddf 1dff 1e1f 1e3f 1e5f 1e7f 1e9f 1ebf 1edf 1eff 1f1f 1f3f 1f5f 1f7f 1f9f 1fbf 1fdf

CZ (NORMAL) 00810000: 00000000 00000000 75550000 00e40000
CZ (OFFSET) 00810000: 7fff8000 7fff8000 75550000 00e40000
CZ (OFF)    00810000: 00000000 00000000 31110000 00e40000

SPOT TABLE test
03F282: 13FC 0000 0082 4011        move.b  #$0, $824011.l
03F28A: 13FC 0000 0082 4015        move.b  #$0, $824015.l
03F292: 13FC 0080 0082 400D        move.b  #$80, $82400d.l
03F29A: 13FC 0001 0082 400E        move.b  #$1, $82400e.l
03F2A2: 13FC 0001 0082 4021        move.b  #$1, $824021.l
03F2AA: 33FC 4038 0080 0000        move.w  #$4038, $800000.l
03F2B2: 06B9 0000 0001 00E0 AB08   addi.l  #$1, $e0ab08.l
*/

#include "emu.h"
#include "video/rgbutil.h"
#include "includes/namcos22.h"
#include "video/poly.h"


static UINT8
nthbyte( const UINT32 *pSource, int offs )
{
	pSource += offs/4;
	return (pSource[0]<<((offs&3)*8))>>24;
}

static UINT16
nthword( const UINT32 *pSource, int offs )
{
	pSource += offs/2;
	return (pSource[0]<<((offs&1)*16))>>16;
}

INLINE UINT8
Clamp256( int v )
{
	if( v<0 )
	{
		v = 0;
	}
	else if( v>255 )
	{
		v = 255;
	}
	return v;
} /* Clamp256 */

#ifdef MAME_DEBUG
static void Dump( address_space *space, FILE *f, unsigned addr1, unsigned addr2, const char *name );
#endif


static struct
{
	int flags;
	int rFogColor;
	int gFogColor;
	int bFogColor;
	int rFogColor2;
	int gFogColor2;
	int bFogColor2;
	int rPolyFadeColor;
	int gPolyFadeColor;
	int bPolyFadeColor;
	int PolyFade_enabled;
	int rFadeColor;
	int gFadeColor;
	int bFadeColor;
	int fadeFactor;
	int spot_translucency;
	int poly_translucency;
	int palBase;
} mixer;

static void
UpdateVideoMixer( running_machine &machine )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	poly_wait(state->m_poly, "UpdateVideoMixer");
	memset( &mixer, 0, sizeof(mixer) );
#if 0 // show reg contents
	char msg1[0x1000]={0}, msg2[0x1000]={0};
	int i,st=0x000/16;
	for (i=st;i<(st+3);i++) {
		sprintf(msg2,"%04X %08X %08X %08X %08X\n",i*16,state->m_gamma[i*4+0],state->m_gamma[i*4+1],state->m_gamma[i*4+2],state->m_gamma[i*4+3]);
		strcat(msg1,msg2);
	}
	popmessage("%s",msg1);
#endif

	if( state->m_mbSuperSystem22 )
	{
/*
           0 1 2 3  4 5 6 7  8 9 a b  c d e f 10       14       18       1c
00824000: ffffff00 00000000 0000007f 00ff0000 1000ff00 0f000000 00ff007f 00010007 // time crisis
00824000: ffffff00 00000000 1830407f 00800000 0000007f 0f000000 0000037f 00010007 // trans sprite
00824000: ffffff00 00000000 3040307f 00000000 0080007f 0f000000 0000037f 00010007 // trans poly
00824000: ffffff00 00000000 1800187f 00800000 0080007f 0f000000 0000037f 00010007 // trans poly(2)
00824000: ffffff00 00000000 1800187f 00000000 0000007f 0f800000 0000037f 00010007 // trans text

	00,01,02		polygon fade rgb
	03
	04
	05,06,07		world fog rgb
	08,09,0a		background color
	0b
	0c
	0d				spot related?
	0e
	0f
	10
	11				global polygon alpha factor
	12,13			textlayer alpha pen comparison
	14				textlayer alpha pen mask?
	15				textlayer alpha factor
	16,17,18		global fade rgb
	19				global fade factor
	1a				fade target flags
	1b				textlayer palette base?
	1c
	1d
	1e
	1f				layer enable
*/
		mixer.rPolyFadeColor    = nthbyte( state->m_gamma, 0x00 );
		mixer.gPolyFadeColor    = nthbyte( state->m_gamma, 0x01 );
		mixer.bPolyFadeColor    = nthbyte( state->m_gamma, 0x02 ); mixer.PolyFade_enabled = (mixer.rPolyFadeColor == 0xff && mixer.gPolyFadeColor == 0xff && mixer.bPolyFadeColor == 0xff) ? 0 : 1;
		mixer.rFogColor         = nthbyte( state->m_gamma, 0x05 );
		mixer.gFogColor         = nthbyte( state->m_gamma, 0x06 );
		mixer.bFogColor         = nthbyte( state->m_gamma, 0x07 );
		mixer.poly_translucency = nthbyte( state->m_gamma, 0x11 );
		mixer.rFadeColor        = nthbyte( state->m_gamma, 0x16 );
		mixer.gFadeColor        = nthbyte( state->m_gamma, 0x17 );
		mixer.bFadeColor        = nthbyte( state->m_gamma, 0x18 );
		mixer.fadeFactor        = nthbyte( state->m_gamma, 0x19 );
		mixer.flags             = nthbyte( state->m_gamma, 0x1a );
		mixer.palBase           = nthbyte( state->m_gamma, 0x1b ) & 0x7f;
	}
	else
	{
/*
90020000: 4f030000 7f00007f 4d4d4d42 0c00c0c0
90020010: c0010001 00010000 00000000 00000000
90020080: 00010101 01010102 00000000 00000000
900200c0: 00000000 00000000 00000000 03000000
90020100: fff35000 00000000 00000000 00000000
90020180: ff713700 00000000 00000000 00000000
90020200: ff100000 00000000 00000000 00000000

	00,01			display flags
	02
	03
	04
	05
	06
	07				textlayer palette base?
	08,09,0a		textlayer pen c shadow rgb
	0b,0c,0d		textlayer pen d shadow rgb
	0e,0f,10		textlayer pen e shadow rgb
	11,12			global fade factor red
	13,14			global fade factor green
	15,16			global fade factor blue

	100,180,200		world fog rgb (not implemented)
	101,181,201		specific fog rgb? (not implemented)
*/
		mixer.flags             = nthbyte( state->m_gamma, 0x00 )*256 + nthbyte( state->m_gamma, 0x01 );
		mixer.rPolyFadeColor    = nthbyte( state->m_gamma, 0x11 )*256 + nthbyte( state->m_gamma, 0x12 ); // 0x0100 = 1.0
		mixer.gPolyFadeColor    = nthbyte( state->m_gamma, 0x13 )*256 + nthbyte( state->m_gamma, 0x14 );
		mixer.bPolyFadeColor    = nthbyte( state->m_gamma, 0x15 )*256 + nthbyte( state->m_gamma, 0x16 );
		mixer.PolyFade_enabled  = (mixer.rPolyFadeColor == 0x100 && mixer.gPolyFadeColor == 0x100 && mixer.bPolyFadeColor == 0x100) ? 0 : 1;

		mixer.rFogColor  = nthbyte( state->m_gamma, 0x0100 );
		mixer.rFogColor2 = nthbyte( state->m_gamma, 0x0101 ); // eg. used for heating of brake-disc on raveracw
		mixer.gFogColor  = nthbyte( state->m_gamma, 0x0180 );
		mixer.gFogColor2 = nthbyte( state->m_gamma, 0x0181 );
		mixer.bFogColor  = nthbyte( state->m_gamma, 0x0200 );
		mixer.bFogColor2 = nthbyte( state->m_gamma, 0x0201 );

		mixer.palBase = 0x7f;
		mixer.poly_translucency = 0;
	}
}

READ32_HANDLER( namcos22_gamma_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	return state->m_gamma[offset];
}

WRITE32_HANDLER( namcos22_gamma_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	COMBINE_DATA( &state->m_gamma[offset] );
}

static struct
{
	int cx,cy;
	rectangle scissor;
} mClip;

static void
poly3d_Clip( float vx, float vy, float vw, float vh )
{
	int cx = 320+vx;
	int cy = 240+vy;
	mClip.cx = cx;
	mClip.cy = cy;
	mClip.scissor.min_x = cx + vw;
	mClip.scissor.min_y = cy + vh;
	mClip.scissor.max_x = cx - vw;
	mClip.scissor.max_y = cy - vh;
	if( mClip.scissor.min_x<0 )   mClip.scissor.min_x = 0;
	if( mClip.scissor.min_y<0 )   mClip.scissor.min_y = 0;
	if( mClip.scissor.max_x>639 ) mClip.scissor.max_x = 639;
	if( mClip.scissor.max_y>479 ) mClip.scissor.max_y = 479;
}

static void
poly3d_NoClip( void )
{
	mClip.cx = 640/2;
	mClip.cy = 480/2;
	mClip.scissor.min_x = 0;
	mClip.scissor.max_x = 639;
	mClip.scissor.min_y = 0;
	mClip.scissor.max_x = 479;
}

typedef struct
{
	float x,y,z;
	int u,v; /* 0..0xfff */
	int bri; /* 0..0xff */
} Poly3dVertex;

#define MIN_Z (10.0f)

typedef struct
{
	float x,y;
	float u,v,i,z;
} vertex;

typedef struct
{
	float x;
	float u,v,i,z;
} edge;

#define SWAP(A,B) { const void *temp = A; A = B; B = temp; }


INLINE unsigned texel( namcos22_state *state, unsigned x, unsigned y )
{
	unsigned offs = ((y&0xfff0)<<4)|((x&0xff0)>>4);
	unsigned tile = state->m_mpTextureTileMap16[offs];
	return state->m_mpTextureTileData[(tile<<8)|state->m_mXYAttrToPixel[state->m_mpTextureTileMapAttr[offs]][x&0xf][y&0xf]];
} /* texel */



typedef struct _poly_extra_data poly_extra_data;
struct _poly_extra_data
{
	/* poly / sprites */
	running_machine *machine;
	rgbint fogColor;
	rgbint fadeColor;
	rgbint polyColor;
	const pen_t *pens;
	bitmap_t *priority_bitmap;
	int bn;
	UINT16 flags;
	int cmode;
	int fogFactor;
	int fadeFactor;
	int pfade_enabled;
	int prioverchar;

	/* sprites */
	const UINT8 *source;
	int alpha;
	int line_modulo;
	int flipx;
	int flipy;
};


static void renderscanline_uvi_full(void *destbase, INT32 scanline, const poly_extent *extent, const void *extradata, int threadid)
{
	const poly_extra_data *extra = (const poly_extra_data *)extradata;
	namcos22_state *state = extra->machine->driver_data<namcos22_state>();
	float z = extent->param[0].start;
	float u = extent->param[1].start;
	float v = extent->param[2].start;
	float i = extent->param[3].start;
	float dz = extent->param[0].dpdx;
	float du = extent->param[1].dpdx;
	float dv = extent->param[2].dpdx;
	float di = extent->param[3].dpdx;
	bitmap_t *destmap = (bitmap_t *)destbase;
	int bn = extra->bn * 0x1000;
	const pen_t *pens = extra->pens;
	int fogFactor = 0xff - extra->fogFactor;
	int fadeFactor = 0xff - extra->fadeFactor;
	int alphaFactor = 0xff - mixer.poly_translucency;
	rgbint fogColor = extra->fogColor;
	rgbint fadeColor = extra->fadeColor;
	rgbint polyColor = extra->polyColor;
	int polyfade_enabled = extra->pfade_enabled;
	int penmask = 0xff;
	int penshift = 0;
	int prioverchar = extra->prioverchar;
	UINT32 *dest = BITMAP_ADDR32(destmap, scanline, 0);
	UINT8 *primap = BITMAP_ADDR8(extra->priority_bitmap, scanline, 0);
	int x;

	if (extra->cmode & 4)
	{
		pens += 0xec + ((extra->cmode & 8) << 1);
		penmask = 0x03;
		penshift = 2 * (~extra->cmode & 3);
	}
	else if (extra->cmode & 2)
	{
		pens += 0xe0 + ((extra->cmode & 8) << 1);
		penmask = 0x0f;
		penshift = 4 * (~extra->cmode & 1);
	}

	for( x=extent->startx; x<extent->stopx; x++ )
	{
		float ooz = 1.0f / z;
		int pen = texel(state, (int)(u*ooz), bn+(int)(v*ooz));
		int shade = i*ooz;
		rgbint rgb;

		rgb_to_rgbint(&rgb, pens[pen>>penshift&penmask]);
		rgbint_scale_immediate_and_clamp(&rgb, shade << 2);

		if( fogFactor != 0xff )
			rgbint_blend(&rgb, &fogColor, fogFactor);

		if( polyfade_enabled )
			rgbint_scale_channel_and_clamp(&rgb, &polyColor);

		if( fadeFactor != 0xff )
			rgbint_blend(&rgb, &fadeColor, fadeFactor);

		if( alphaFactor != 0xff )
		{
			rgbint mix;
			rgb_to_rgbint(&mix, dest[x]);
			rgbint_blend(&rgb, &mix, alphaFactor);
		}

		dest[x] = rgbint_to_rgb(&rgb);
		primap[x] |= prioverchar;

		u += du;
		v += dv;
		i += di;
		z += dz;
	}
} /* renderscanline_uvi_full */

static void poly3d_DrawQuad(running_machine &machine, bitmap_t *bitmap, int textureBank, int color, Poly3dVertex pv[4], UINT16 flags, int direct, int cmode )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(state->m_poly);
	poly_vertex v[4], clipv[6];
	int clipverts;
	int vertnum;

	extra->machine = &machine;
	extra->fogFactor = 0;
	extra->fadeFactor = 0;

	extra->pens = &machine.pens[(color&0x7f)<<8];
	extra->priority_bitmap = machine.priority_bitmap;
	extra->bn = textureBank;
	extra->flags = flags;
	extra->cmode = cmode;
	extra->prioverchar = (state->m_mbSuperSystem22 << 1) | ((cmode & 7) == 1);

	/* non-direct case: project and z-clip */
	if (!direct)
	{
		for (vertnum = 0; vertnum < 4; vertnum++)
		{
			v[vertnum].x = pv[vertnum].x;
			v[vertnum].y = pv[vertnum].y;
			v[vertnum].p[0] = pv[vertnum].z;
			v[vertnum].p[1] = pv[vertnum].u;
			v[vertnum].p[2] = pv[vertnum].v;
			v[vertnum].p[3] = pv[vertnum].bri;
		}
		clipverts = poly_zclip_if_less(4, v, clipv, 4, MIN_Z);
		if (clipverts < 3)
			return;
		assert(clipverts <= ARRAY_LENGTH(clipv));
		for (vertnum = 0; vertnum < clipverts; vertnum++)
		{
			float ooz = 1.0f / clipv[vertnum].p[0];
			clipv[vertnum].x = mClip.cx + clipv[vertnum].x * ooz;
			clipv[vertnum].y = mClip.cy - clipv[vertnum].y * ooz;
			clipv[vertnum].p[0] = ooz;
			clipv[vertnum].p[1] = (clipv[vertnum].p[1] + 0.5f) * ooz;
			clipv[vertnum].p[2] = (clipv[vertnum].p[2] + 0.5f) * ooz;
			clipv[vertnum].p[3] = (clipv[vertnum].p[3] + 0.5f) * ooz;
		}
	}

	/* direct case: don't clip, and treat pv->z as 1/z */
	else
	{
		clipverts = 4;
		for (vertnum = 0; vertnum < 4; vertnum++)
		{
			float ooz = pv[vertnum].z;
			clipv[vertnum].x = mClip.cx + pv[vertnum].x;
			clipv[vertnum].y = mClip.cy - pv[vertnum].y;
			clipv[vertnum].p[0] = ooz;
			clipv[vertnum].p[1] = (pv[vertnum].u + 0.5f) * ooz;
			clipv[vertnum].p[2] = (pv[vertnum].v + 0.5f) * ooz;
			clipv[vertnum].p[3] = (pv[vertnum].bri + 0.5f) * ooz;
		}
	}

	if( state->m_mbSuperSystem22 )
	{
		// global fade
		if (mixer.flags&1)
		{
			extra->fadeFactor = mixer.fadeFactor;
			rgb_comp_to_rgbint(&extra->fadeColor, mixer.rFadeColor, mixer.gFadeColor, mixer.bFadeColor);
		}

		// poly fade
		extra->pfade_enabled = mixer.PolyFade_enabled;
		rgb_comp_to_rgbint(&extra->polyColor, mixer.rPolyFadeColor, mixer.gPolyFadeColor, mixer.bPolyFadeColor);

		// fog (prelim!)
		if( !(color&0x80) )
		{
			int cz = flags>>8;
			static const int cztype_remap[4] = { 3,1,2,0 };
			int cztype = flags&3;
			if( nthword(state->m_czattr,4)&(0x4000>>(cztype*4)) )
			{
				int fogDelta = (INT16)nthword(state->m_czattr, cztype);
				int fogDensity = fogDelta + state->m_czram[cztype_remap[cztype]][cz];
				//if( fogDelta == 0x8000 ) fogDelta = -0x7fff;
				//cz = Clamp256(cz+fogDelta);
				if( fogDensity<0x0000 )
				{
					fogDensity = 0x0000;
				}
				else if( fogDensity>0x1fff )
				{
					fogDensity = 0x1fff;
				}
				extra->fogFactor = fogDensity >> 5;

				// disallow 100% fogFactor for now (completely breaks alpinr2 otherwise)
				if (extra->fogFactor == 0xff)
					extra->fogFactor = 0;

				rgb_comp_to_rgbint(&extra->fogColor, mixer.rFogColor, mixer.gFogColor, mixer.bFogColor);
			}
		}
	}
	else
	{
		// global fade
		if (mixer.flags&1)
		{
			extra->pfade_enabled = mixer.PolyFade_enabled;
			rgb_comp_to_rgbint(&extra->polyColor, mixer.rPolyFadeColor, mixer.gPolyFadeColor, mixer.bPolyFadeColor);
		}

		// fog not hooked up yet
	}

	poly_render_triangle_fan(state->m_poly, bitmap, &mClip.scissor, renderscanline_uvi_full, 4, clipverts, clipv);
}

static void renderscanline_sprite(void *destbase, INT32 scanline, const poly_extent *extent, const void *extradata, int threadid)
{
	const poly_extra_data *extra = (const poly_extra_data *)extradata;
	int y_index = extent->param[1].start - extra->flipy;
	float x_index = extent->param[0].start - extra->flipx;
	float dx = extent->param[0].dpdx;
	bitmap_t *destmap = (bitmap_t *)destbase;
	const pen_t *pal = extra->pens;
	int prioverchar = extra->prioverchar;
	int alphaFactor = extra->alpha;
	int fogFactor = 0xff - extra->fogFactor;
	int fadeFactor = 0xff - extra->fadeFactor;
	rgbint fogColor = extra->fogColor;
	rgbint fadeColor = extra->fadeColor;
	UINT8 *source = (UINT8 *)extra->source + y_index * extra->line_modulo;
	UINT32 *dest = BITMAP_ADDR32(destmap, scanline, 0);
	UINT8 *primap = BITMAP_ADDR8(extra->priority_bitmap, scanline, 0);
	int x;

	for( x=extent->startx; x<extent->stopx; x++ )
	{
		int pen = source[(int)x_index];
		if( pen != 0xff )
		{
			rgbint rgb;
			rgb_to_rgbint(&rgb, pal[pen]);

			if( fogFactor != 0xff )
				rgbint_blend(&rgb, &fogColor, fogFactor);

			if( fadeFactor != 0xff )
				rgbint_blend(&rgb, &fadeColor, fadeFactor);

			if( alphaFactor != 0xff )
			{
				rgbint mix;
				rgb_to_rgbint(&mix, dest[x]);
				rgbint_blend(&rgb, &mix, alphaFactor);
			}

			dest[x] = rgbint_to_rgb(&rgb);
			primap[x] |= prioverchar;
		}
		x_index += dx;
	}
}


static void
mydrawgfxzoom(
	bitmap_t *dest_bmp,const rectangle *clip,const gfx_element *gfx,
	UINT32 code,UINT32 color,int flipx,int flipy,int sx,int sy,
	int scalex, int scaley, int z, int prioverchar, int alpha )
{
	namcos22_state *state = gfx->machine().driver_data<namcos22_state>();
	int sprite_screen_height = (scaley*gfx->height+0x8000)>>16;
	int sprite_screen_width = (scalex*gfx->width+0x8000)>>16;
	if (sprite_screen_width && sprite_screen_height)
	{
		float fsx = sx;
		float fsy = sy;
		float fwidth = gfx->width;
		float fheight = gfx->height;
		float fsw = sprite_screen_width;
		float fsh = sprite_screen_height;
		poly_extra_data *extra = (poly_extra_data *)poly_get_extra_data(state->m_poly);
		poly_vertex vert[4];

		extra->fadeFactor = 0;
		extra->fogFactor = 0;
		extra->flags = 0;

		extra->machine = &gfx->machine();
		extra->alpha = alpha;
		extra->prioverchar = 2 | prioverchar;
		extra->line_modulo = gfx->line_modulo;
		extra->flipx = flipx;
		extra->flipy = flipy;
		extra->pens = &gfx->machine().pens[gfx->color_base + gfx->color_granularity * (color&0x7f)];
		extra->priority_bitmap = gfx->machine().priority_bitmap;
		extra->source = gfx_element_get_data(gfx, code % gfx->total_elements);

		vert[0].x = fsx;
		vert[0].y = fsy;
		vert[0].p[0] = 0;
		vert[0].p[1] = 0;
		vert[1].x = fsx + fsw;
		vert[1].y = fsy;
		vert[1].p[0] = fwidth;
		vert[1].p[1] = 0;
		vert[2].x = fsx + fsw;
		vert[2].y = fsy + fsh;
		vert[2].p[0] = fwidth;
		vert[2].p[1] = fheight;
		vert[3].x = fsx;
		vert[3].y = fsy + fsh;
		vert[3].p[0] = 0;
		vert[3].p[1] = fheight;

		// global fade
		if (mixer.flags&2)
		{
			extra->fadeFactor = mixer.fadeFactor;
			rgb_comp_to_rgbint(&extra->fadeColor, mixer.rFadeColor, mixer.gFadeColor, mixer.bFadeColor);
		}

		// fog (prelim!)
		if (z != 0xffff && nthword(state->m_czattr,4)&0x4000) // ?
		{
			INT16 fogDelta = nthword(state->m_czattr, 0);
			int zc = Clamp256(fogDelta + z);
			UINT16 fogDensity = state->m_czram[3][zc];
			if (fogDensity < 0x2000)
			{
				extra->fogFactor = fogDensity >> 5;
				rgb_comp_to_rgbint(&extra->fogColor, mixer.rFogColor, mixer.gFogColor, mixer.bFogColor);
			}
		}

		poly_render_triangle_fan(state->m_poly, dest_bmp, clip, renderscanline_sprite, 2, 4, vert);
	}
} /* mydrawgfxzoom */

static void
ApplyGamma( running_machine &machine, bitmap_t *bitmap )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int x,y;
	if( state->m_mbSuperSystem22 )
	{ /* super system 22 */
#define XORPAT NATIVE_ENDIAN_VALUE_LE_BE(3,0)
		const UINT8 *rlut = (const UINT8 *)&state->m_gamma[0x100/4];
		const UINT8 *glut = (const UINT8 *)&state->m_gamma[0x200/4];
		const UINT8 *blut = (const UINT8 *)&state->m_gamma[0x300/4];
		for( y=0; y<bitmap->height; y++ )
		{
			UINT32 *dest = BITMAP_ADDR32(bitmap, y, 0);
			for( x=0; x<bitmap->width; x++ )
			{
				int rgb = dest[x];
				int r = rlut[XORPAT^((rgb>>16)&0xff)];
				int g = glut[XORPAT^((rgb>>8)&0xff)];
				int b = blut[XORPAT^(rgb&0xff)];
				dest[x] = (r<<16)|(g<<8)|b;
			}
		}
	}
	else
	{ /* system 22 */
		const UINT8 *rlut = 0x000+(const UINT8 *)machine.region("user1")->base();
		const UINT8 *glut = 0x100+rlut;
		const UINT8 *blut = 0x200+rlut;
		for( y=0; y<bitmap->height; y++ )
		{
			UINT32 *dest = BITMAP_ADDR32(bitmap, y, 0);
			for( x=0; x<bitmap->width; x++ )
			{
				int rgb = dest[x];
				int r = rlut[(rgb>>16)&0xff];
				int g = glut[(rgb>>8)&0xff];
				int b = blut[rgb&0xff];
				dest[x] = (r<<16)|(g<<8)|b;
			}
		}
	}
} /* ApplyGamma */

static void
poly3d_Draw3dSprite( bitmap_t *bitmap, const gfx_element *gfx, int tileNumber, int color, int flipx, int flipy, int sx, int sy, int width, int height, int translucency, int zc, UINT32 pri )
{
	rectangle clip;
	clip.min_x = 0;
	clip.min_y = 0;
	clip.max_x = 640-1;
	clip.max_y = 480-1;
	mydrawgfxzoom(
		bitmap,
		&clip,
		gfx,
		tileNumber,
		color,
		flipx, flipy,
		sx, sy,
		(width<<16)/32,
		(height<<16)/32,
		zc, pri, 0xff - translucency );
}

#define DSP_FIXED_TO_FLOAT( X ) (((INT16)(X))/(float)0x7fff)
#define SPRITERAM_SIZE (0x9b0000-0x980000)
#define CGRAM_SIZE 0x1e000
#define NUM_CG_CHARS ((CGRAM_SIZE*8)/(64*16)) /* 0x3c0 */


/* modal rendering properties */
static void
matrix3d_Multiply( float A[4][4], float B[4][4] )
{
	float temp[4][4];
	int row,col;

	for( row=0;row<4;row++ )
	{
		for(col=0;col<4;col++)
		{
			float sum = 0.0f;
			int i;
			for( i=0; i<4; i++ )
			{
				sum += A[row][i]*B[i][col];
			}
			temp[row][col] = sum;
		}
	}
	memcpy( A, temp, sizeof(temp) );
} /* matrix3d_Multiply */

static void
matrix3d_Identity( float M[4][4] )
{
	int r,c;

	for( r=0; r<4; r++ )
	{
		for( c=0; c<4; c++ )
		{
			M[r][c] = (r==c)?1.0:0.0;
		}
	}
} /* matrix3d_Identity */

static void
TransformPoint( float *vx, float *vy, float *vz, float m[4][4] )
{
	float x = *vx;
	float y = *vy;
	float z = *vz;
	*vx = m[0][0]*x + m[1][0]*y + m[2][0]*z + m[3][0];
	*vy = m[0][1]*x + m[1][1]*y + m[2][1]*z + m[3][1];
	*vz = m[0][2]*x + m[1][2]*y + m[2][2]*z + m[3][2];
}

static void
TransformNormal( float *nx, float *ny, float *nz, float m[4][4] )
{
	float x = *nx;
	float y = *ny;
	float z = *nz;
	*nx = m[0][0]*x + m[1][0]*y + m[2][0]*z;
	*ny = m[0][1]*x + m[1][1]*y + m[2][1]*z;
	*nz = m[0][2]*x + m[1][2]*y + m[2][2]*z;
}



static struct
{
	float zoom, vx, vy, vw, vh;
	float lx,ly,lz; /* unit vector for light direction */
	int ambient; /* 0.0..1.0 */
	int power;	/* 0.0..1.0 */
} mCamera;

typedef enum
{
	eSCENENODE_NONLEAF,
	eSCENENODE_QUAD3D,
	eSCENENODE_SPRITE
} SceneNodeType;

#define RADIX_BITS 4
#define RADIX_BUCKETS (1<<RADIX_BITS)
#define RADIX_MASK (RADIX_BUCKETS-1)

struct SceneNode
{
	SceneNodeType type;
	struct SceneNode *nextInBucket;
	union
	{
		struct
		{
			struct SceneNode *next[RADIX_BUCKETS];
		} nonleaf;

		struct
		{
			float vx,vy,vw,vh;
			int textureBank;
			int color;
			int cmode;
			int flags;
			int direct;
			Poly3dVertex v[4];
		} quad3d;

		struct
		{
			int tile, color, pri;
			int flipx, flipy;
			int linkType;
			int numcols, numrows;
			int xpos, ypos;
			int sizex, sizey;
			int translucency;
			int cz;
		} sprite;
	} data;
};
static struct SceneNode mSceneRoot;
static struct SceneNode *mpFreeSceneNode;

static void
FreeSceneNode( struct SceneNode *node )
{
	node->nextInBucket = mpFreeSceneNode;
	mpFreeSceneNode = node;
} /* FreeSceneNode */

static struct SceneNode *
MallocSceneNode( running_machine &machine )
{
	struct SceneNode *node = mpFreeSceneNode;
	if( node )
	{ /* use free pool */
		mpFreeSceneNode = node->nextInBucket;
	}
	else
	{
		node = auto_alloc(machine, struct SceneNode);
	}
	memset( node, 0, sizeof(*node) );
	return node;
} /* MallocSceneNode */

static struct SceneNode *
NewSceneNode( running_machine &machine, UINT32 zsortvalue24, SceneNodeType type )
{
	struct SceneNode *node = &mSceneRoot;
	int i;
	for( i=0; i<24; i+=RADIX_BITS )
	{
		int hash = (zsortvalue24>>20)&RADIX_MASK;
		struct SceneNode *next = node->data.nonleaf.next[hash];
		if( !next )
		{ /* lazily allocate tree node for this radix */
			next = MallocSceneNode(machine);
			next->type = eSCENENODE_NONLEAF;
			node->data.nonleaf.next[hash] = next;
		}
		node = next;
		zsortvalue24 <<= RADIX_BITS;
	}

	if( node->type == eSCENENODE_NONLEAF )
	{ /* first leaf allocation on this branch */
		node->type = type;
		return node;
	}
	else
	{
		struct SceneNode *leaf = MallocSceneNode(machine);
		leaf->type = type;
#if 0
		leaf->nextInBucket = node->nextInBucket;
		node->nextInBucket = leaf;
#else
		/* stable insertion sort */
		leaf->nextInBucket = NULL;
		while( node->nextInBucket )
		{
			node = node->nextInBucket;
		}
		node->nextInBucket = leaf;
#endif
		return leaf;
	}
} /* NewSceneNode */


static void RenderSprite(running_machine &machine, bitmap_t *bitmap, struct SceneNode *node )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int tile = node->data.sprite.tile;
	int col,row;
	int i = 0;
	for( row=0; row<node->data.sprite.numrows; row++ )
	{
		for( col=0; col<node->data.sprite.numcols; col++ )
		{
			int code = tile;
			if( node->data.sprite.linkType == 0xff )
			{
				code += i;
			}
			else
			{
				code += nthword( &state->m_spriteram[0x800/4], i+node->data.sprite.linkType*4 );
			}
			poly3d_Draw3dSprite(
					bitmap,
					machine.gfx[GFX_SPRITE],
					code,
					node->data.sprite.color,
					node->data.sprite.flipx,
					node->data.sprite.flipy,
					node->data.sprite.xpos+col*node->data.sprite.sizex,
					node->data.sprite.ypos+row*node->data.sprite.sizey,
					node->data.sprite.sizex,
					node->data.sprite.sizey,
					node->data.sprite.translucency,
					node->data.sprite.cz,
					node->data.sprite.pri );
		i++;
		} /* next col */
	} /* next row */
} /* RenderSprite */

static void RenderSceneHelper(running_machine &machine, bitmap_t *bitmap, struct SceneNode *node )
{
	if( node )
	{
		if( node->type == eSCENENODE_NONLEAF )
		{
			int i;
			for( i=RADIX_BUCKETS-1; i>=0; i-- )
			{
				RenderSceneHelper(machine, bitmap, node->data.nonleaf.next[i] );
			}
			FreeSceneNode( node );
		}
		else
		{
			while( node )
			{
				struct SceneNode *next = node->nextInBucket;

				switch( node->type )
				{
				case eSCENENODE_QUAD3D:
					poly3d_Clip(
						node->data.quad3d.vx,
						node->data.quad3d.vy,
						node->data.quad3d.vw,
						node->data.quad3d.vh );
					poly3d_DrawQuad(machine,
						bitmap,
						node->data.quad3d.textureBank,
						node->data.quad3d.color,
						node->data.quad3d.v,
						node->data.quad3d.flags,
						node->data.quad3d.direct,
						node->data.quad3d.cmode );
					break;

				case eSCENENODE_SPRITE:
					poly3d_NoClip();
					RenderSprite(machine, bitmap,node );
					break;

				default:
					fatalerror("invalid node->type");
					break;
				}
				FreeSceneNode( node );
				node = next;
			}
		}
	}
} /* RenderSceneHelper */

static void RenderScene(running_machine &machine, bitmap_t *bitmap )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	struct SceneNode *node = &mSceneRoot;
	int i;
	for( i=RADIX_BUCKETS-1; i>=0; i-- )
	{
		RenderSceneHelper(machine, bitmap, node->data.nonleaf.next[i] );
		node->data.nonleaf.next[i] = NULL;
	}
	poly3d_NoClip();
	poly_wait(state->m_poly, "DrawPolygons");
} /* RenderScene */

static float
DspFloatToNativeFloat( UINT32 iVal )
{
	INT16 mantissa = (INT16)iVal;
	float result = mantissa;//?((float)mantissa):((float)0x10000);
	int exponent = (iVal>>16)&0xff;
	while( exponent<0x2e )
	{
		result /= 2.0;
		exponent++;
	}
	return result;
} /* DspFloatToNativeFloat */

static INT32
GetPolyData( namcos22_state *state, INT32 addr )
{
	UINT32 result;
	if( addr<0 || addr>=state->m_mPtRomSize )
	{
		// point ram, only used in ram test?
		if( state->m_mbSuperSystem22 )
		{
			if( addr>=0xf80000 && addr<=0xf9ffff )
				result = state->m_mpPointRAM[addr-0xf80000];
			else return -1;
		}
		else
		{
			if( addr>=0xf00000 && addr<=0xf1ffff )
				result = state->m_mpPointRAM[addr-0xf00000];
			else return -1;
		}
	}
	else
	{
		result = (state->m_mpPolyH[addr]<<16)|(state->m_mpPolyM[addr]<<8)|state->m_mpPolyL[addr];
	}
	if( result&0x00800000 )
		result |= 0xff000000; /* sign extend */
	return result;
} /* GetPolyData */

UINT32
namcos22_point_rom_r( running_machine &machine, offs_t offs )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	return GetPolyData(state, offs & 0x00ffffff);
}


/*
                         0    2    4    6    8    a    b
                      ^^^^ ^^^^ ^^^^ ^^^^                        cz offset
                                          ^^^^                   target (4==poly)
                                                    ^^^^         ????
         //00810000:  0000 0000 0000 0000 4444 0000 0000 0000 // solitar
         //00810000:  0000 0000 0000 0000 7555 0000 00e4 0000 // normal
         //00810000:  7fff 8000 7fff 8000 7555 0000 00e4 0000 // offset
         //00810000:  0000 0000 0000 0000 3111 0000 00e4 0000 // off
         //00810000:  0004 0004 0004 0004 4444 0000 0000 0000 // out pool
         //00810000:  00a4 00a4 00a4 00a4 4444 0000 0000 0000 // in pool
         //00810000:  ff80 ff80 ff80 ff80 4444 0000 0000 0000 // ending
         //00810000:  ff80 ff80 ff80 ff80 0000 0000 0000 0000 // hs entry
         //00810000:  ff01 ff01 0000 0000 0000 0000 00e4 0000 // alpine racer
*/
READ32_HANDLER( namcos22s_czram_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	int bank = nthword(state->m_czattr,0xa/2);
	const UINT16 *czram = state->m_czram[bank&3];
	return (czram[offset*2]<<16)|czram[offset*2+1];
}

WRITE32_HANDLER( namcos22s_czram_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	int bank = nthword(state->m_czattr,0xa/2);
	UINT16 *czram = state->m_czram[bank&3];
	UINT32 dat = (czram[offset*2]<<16)|czram[offset*2+1];
	COMBINE_DATA( &dat );
	czram[offset*2] = dat>>16;
	czram[offset*2+1] = dat&0xffff;
}

static void
InitXYAttrToPixel( namcos22_state *state )
{
	unsigned attr,x,y,ix,iy,temp;
	for( attr=0; attr<16; attr++ )
	{
		for( y=0; y<16; y++ )
		{
			for( x=0; x<16; x++ )
			{
				ix = x; iy = y;
				if( attr&4 ) ix = 15-ix;
				if( attr&2 ) iy = 15-iy;
				if( attr&8 ){ temp = ix; ix = iy; iy = temp; }
				state->m_mXYAttrToPixel[attr][x][y] = (iy<<4)|ix;
			}
		}
	}
} /* InitXYAttrToPixel */

static void
PatchTexture( namcos22_state *state )
{
	int i;
	switch( state->m_gametype )
	{
		case NAMCOS22_RIDGE_RACER:
		case NAMCOS22_RIDGE_RACER2:
		case NAMCOS22_ACE_DRIVER:
		case NAMCOS22_CYBER_COMMANDO:
			for( i=0; i<0x100000; i++ )
			{
				int tile = state->m_mpTextureTileMap16[i];
				int attr = state->m_mpTextureTileMapAttr[i];
				if( (attr&0x1)==0 )
				{
					tile = (tile&0x3fff)|0x8000;
					state->m_mpTextureTileMap16[i] = tile;
				}
			}
			break;

		default:
			break;
	}
} /* PatchTexture */

void
namcos22_draw_direct_poly( running_machine &machine, const UINT16 *pSource )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
   /**
    * word#0:
    *    x--------------- end-of-display-list marker
    *    ----xxxxxxxxxxxx priority (lo)
    *
    * word#1:
    *    ----xxxxxxxxxxxx priority (hi)
    *
    * word#2:
    *    xxxxxxxx-------- PAL (high bit is fog enable)
    *    --------xxxx---- CMODE (color mode for texture unpack)
    *    ------------xxxx BN (texture bank)
    *
    * word#3:
    *    xxxxxxxx-------- ZC
    *    --------------xx depth cueing table select
    *
    * for each vertex:
    *    xxxx xxxx // u,v
    *
    *    xxxx xxxx // sx,sy
    *
    *    xx-- ---- // BRI
    *    --xx xxxx // zpos
    */
	INT32 zsortvalue24 = ((pSource[1]&0xfff)<<12)|(pSource[0]&0xfff);
	struct SceneNode *node = NewSceneNode(machine, zsortvalue24,eSCENENODE_QUAD3D);
	int i;
	node->data.quad3d.flags = ((pSource[3]&0x7f00)*2)|(pSource[3]&3);
	node->data.quad3d.color = (pSource[2]&0xff00)>>8;
	if( state->m_mbSuperSystem22 )
	{
		node->data.quad3d.cmode       = (pSource[2]&0x00f0)>>4;
		node->data.quad3d.textureBank = (pSource[2]&0x000f);
	}
	else
	{
		node->data.quad3d.cmode       = (pSource[0+4]&0xf000)>>12;
		node->data.quad3d.textureBank = (pSource[1+4]&0xf000)>>12;
	}
	pSource += 4;
	for( i=0; i<4; i++ )
	{
		Poly3dVertex *p = &node->data.quad3d.v[i];
		if( state->m_mbSuperSystem22 )
		{
			p->u = pSource[0] >> 4;
			p->v = pSource[1] >> 4;
		}
		else
		{
			p->u = pSource[0] & 0x0fff;
			p->v = pSource[1] & 0x0fff;
		}

		int mantissa = (INT16)pSource[5];
		float zf = (float)mantissa;
		int exponent = (pSource[4])&0xff;
		if( mantissa )
		{
			while( exponent<0x2e )
			{
				zf /= 2.0;
				exponent++;
			}
			if( state->m_mbSuperSystem22 )
				p->z = zf;
			else
				p->z = 1.0f/zf;
		}
		else
		{
			zf = (float)0x10000;
			exponent = 0x40-exponent;
			while( exponent<0x2e )
			{
				zf /= 2.0;
				exponent++;
			}
			p->z = 1.0f/zf;
		}

		p->x = ((INT16)pSource[2]);
		p->y = (-(INT16)pSource[3]);
		p->bri = pSource[4]>>8;
		pSource += 6;
	}
	node->data.quad3d.direct = 1;
	node->data.quad3d.vx = 0;
	node->data.quad3d.vy = 0;
	node->data.quad3d.vw = -320;
	node->data.quad3d.vh = -240;
} /* namcos22_draw_direct_poly */

static void
Prepare3dTexture( running_machine &machine, void *pTilemapROM, void *pTextureROM )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int i;
	assert( pTilemapROM && pTextureROM );
	{ /* following setup is Namco System 22 specific */
		const UINT8 *pPackedTileAttr = 0x200000 + (UINT8 *)pTilemapROM;
		UINT8 *pUnpackedTileAttr = auto_alloc_array(machine, UINT8, 0x080000*2);
		{
			InitXYAttrToPixel(state);
			state->m_mpTextureTileMapAttr = pUnpackedTileAttr;
			for( i=0; i<0x80000; i++ )
			{
				*pUnpackedTileAttr++ = (*pPackedTileAttr)>>4;
				*pUnpackedTileAttr++ = (*pPackedTileAttr)&0xf;
				pPackedTileAttr++;
			}
			state->m_mpTextureTileMap16 = (UINT16 *)pTilemapROM;
			state->m_mpTextureTileData = (UINT8 *)pTextureROM;
			PatchTexture(state);
		}
	}
} /* Prepare3dTexture */

static void
DrawSpritesHelper(
	running_machine &machine,
	bitmap_t *bitmap,
	const rectangle *cliprect,
	const UINT32 *pSource,
	const UINT32 *pPal,
	int num_sprites,
	int deltax,
	int deltay )
{
	int i;
	num_sprites--;
	pSource += num_sprites*4;
	pPal += num_sprites*2;

	for( i=num_sprites; i>=0; i-- )
	{
		/* attrs:
        xxxx.x---.----.----.----.----.----.---- always 0?
        ----.-xxx.----.----.----.----.----.---- enable mask?
        ----.----.xxxx.xxxx.----.----.----.---- linktype?
        ----.----.----.----.xxxx.xx--.----.---- always 0?
        ----.----.----.----.----.--x-.----.---- right justify
        ----.----.----.----.----.---x.----.---- bottom justify
        ----.----.----.----.----.----.x---.---- flipx
        ----.----.----.----.----.----.-xxx.---- numcols
        ----.----.----.----.----.----.----.x--- flipy
        ----.----.----.----.----.----.----.-xxx numrows
        */
		UINT32 attrs = pSource[2];
		if( (attrs&0x04000000)==0 )
		{
			/* sprite is not hidden */
			INT32 zcoord = pPal[0];
			int color = pPal[1]>>16;
			int cz = pPal[1]&0xffff;
			int pri = ((pPal[1] & 0xffff) == 0x00fe); // ? priority over textlayer, trusted by testmode and timecris (not cz&0x80 or color&0x80 or in attrs)
			UINT32 xypos = pSource[0];
			UINT32 size = pSource[1];
			UINT32 code = pSource[3];
			int xpos = (xypos>>16)-deltax;
			int ypos = (xypos&0xffff)-deltay;
			int sizex = size>>16;
			int sizey = size&0xffff;
			int flipy = attrs>>3&0x1;
			int numrows = attrs&0x7;
			int linkType = (attrs&0x00ff0000)>>16;
			int flipx = (attrs>>7)&0x1;
			int numcols = (attrs>>4)&0x7;
			int tile = code>>16;
			int translucency = (code&0xff00)>>8;

			if (numrows == 0) numrows = 8;
			if (numcols == 0) numcols = 8;

			/* right justify */
			if (attrs & 0x0200)
				xpos -= sizex*numcols-1;

			/* bottom justify */
			if (attrs & 0x0100)
				ypos -= sizey*numrows-1;

			if (flipy)
			{
				ypos += sizey*numrows-1;
				sizey = -sizey;
			}

			if (flipx)
			{
				xpos += sizex*numcols-1;
				sizex = -sizex;
			}

			if (sizex && sizey)
			{
				struct SceneNode *node = NewSceneNode(machine, zcoord, eSCENENODE_SPRITE);

				node->data.sprite.tile = tile;
				node->data.sprite.flipx = flipx;
				node->data.sprite.flipy = flipy;
				node->data.sprite.numcols = numcols;
				node->data.sprite.numrows = numrows;
				node->data.sprite.linkType = linkType;
				node->data.sprite.xpos = xpos;
				node->data.sprite.ypos = ypos;
				node->data.sprite.sizex = sizex;
				node->data.sprite.sizey = sizey;
				node->data.sprite.translucency = translucency;
				node->data.sprite.color = color;
				node->data.sprite.cz = cz;
				node->data.sprite.pri = pri;
			}
		} /* visible sprite */
		pSource -= 4;
		pPal -= 2;
	}
} /* DrawSpritesHelper */

static void
DrawSprites( running_machine &machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	UINT32 *spriteram32 = state->m_spriteram;
	const UINT32 *pSource;
	const UINT32 *pPal;

#if 0 // show reg contents
	int i;
	char msg1[0x1000]={0}, msg2[0x1000]={0};
	// 980000-98023f (spriteram header)
	for (i=0x00;i<0x02;i++) {
		sprintf(msg2,"98%04X %08X %08X %08X %08X\n",i*16,spriteram32[i*4+0],spriteram32[i*4+1],spriteram32[i*4+2],spriteram32[i*4+3]);
		strcat(msg1,msg2);
	}
	for (i=0x20;i<0x24;i++) {
		sprintf(msg2,"98%04X %08X %08X %08X %08X\n",i*16,spriteram32[i*4+0],spriteram32[i*4+1],spriteram32[i*4+2],spriteram32[i*4+3]);
		strcat(msg1,msg2);
	}
	strcat(msg1,"\n");
	// 940000-94007c (vics control)
	for (i=0x00;i<0x08;i++) {
		sprintf(msg2,"94%04X %08X %08X %08X %08X\n",i*16,state->m_vics_control[i*4+0],state->m_vics_control[i*4+1],state->m_vics_control[i*4+2],state->m_vics_control[i*4+3]);
		strcat(msg1,msg2);
	}
	if (machine.input().code_pressed(KEYCODE_S))
		popmessage("%s",msg1);
	else popmessage("[S] shows spite/vics regs");
#endif
/*
// time crisis:
00980000: 00060000 000b0053 03000200 03000000
00980010: 00200020 028004ff 032a0509 00000000
00980200: 000007ff 000007ff 000007ff 032a0509
00980210: 000007ff 000007ff 000007ff 000007ff
00980220: 000007ff 000007ff 000007ff 000007ff
00980230: 000007ff 000007ff 05000500 050a050a

// prop normal
00980000: 00060000 00040053 03000200 03000000
00980010: 00200020 028004ff 032a0509 00000000
00980200: 028004ff 032a0509 028004ff 032a0509
00980210: 028004ff 032a0509 028004ff 032a0509
00980220: 028004ff 032a0509 028004ff 032a0509
00980230: 028004ff 032a0509 028004ff 032a0509

//alpine normal / prop test (-48,-43)
00980000: 00060000 00000000 02ff0000 000007ff
00980010: 00200020 000002ff 000007ff 00000000
00980200: 000007ff 000007ff 000007ff 000007ff
00980210: 000007ff 000007ff 000007ff 000007ff
00980220: 000007ff 000007ff 000007ff 000007ff
00980230: 000007ff 000007ff 000007ff 000007ff


        0x980000:   00060000 00010000 02ff0000 000007ff
                    ^^^^                                7 = disable
                             ^^^^                       num sprites
                                      ^^^^              probably deltax related
                                               ^^^^     definitely deltay related!

        0x980010:   00200020 000002ff 000007ff 00000000
                    ^^^^^^^^                            character size?
                             ^^^^                       delta xpos?
                                      ^^^^              delta ypos?

        0x980200:   000007ff 000007ff       delta xpos, delta ypos?
        0x980208:   000007ff 000007ff
        0x980210:   000007ff 000007ff
        0x980218:   000007ff 000007ff
        0x980220:   000007ff 000007ff
        0x980228:   000007ff 000007ff
        0x980230:   000007ff 000007ff
        0x980238:   000007ff 000007ff

        //time crisis
        00980200:  000007ff 000007ff 000007ff 032a0509
        00980210:  000007ff 000007ff 000007ff 000007ff
        00980220:  000007ff 000007ff 000007ff 000007ff
        00980230:  000007ff 000007ff 05000500 050a050a

        0x980400:   hzoom table
        0x980600:   vzoom table

        link table:
        0x980800:   0000 0001 0002 0003 ... 03ff

        eight words per sprite:
        0x984000:   010f 007b   xpos, ypos
        0x984004:   0020 0020   size x, size y
        0x984008:   00ff 0311   00ff, chr x;chr y;flip x;flip y
        0x98400c:   0001 0000   sprite code, translucency
        ...

        additional sorting/color data for sprite:
        0x9a0000:   C381 Z (sort)
        0x9a0004:   palette, C381 ZC (depth cueing)
        ...
    */

	// xy offs, prelim!
	int deltax=spriteram32[5]>>16; // usually 0x280
	int deltay=spriteram32[6]>>16; // usually 0x32a

	if (deltax == 0 || deltay == 0)
	switch (state->m_gametype)
	{
		case NAMCOS22_AQUA_JET:
			deltax = 0x07f;
			deltay = 0x0fe; // spriteram32[3]>>16(0x0d6) + default(0x02a) is 0x100, close to 0x0fe
			break;

		case NAMCOS22_CYBER_CYCLES:
			// approx (not enough testdata)
			deltax = 0x280;
			deltay = 0x400; // is spriteram32[3]>>16
			break;

		case NAMCOS22_TOKYO_WARS:
			// approx (not enough testdata)
			deltax = 190;
			deltay = 0x10e; // spriteram32[3]>>16(0x0e6) is 0x10 more than aquajet
			break;

		default:
			// accurate in testmode
			deltax = 0x02e;
			deltay = 0x02a;
			break;
	}

	// previous xy implementation
#if 0
	deltax=spriteram32[5]>>16;
	deltay=spriteram32[6]>>16;
	/* HACK for Tokyo Wars */
	if (deltax == 0 && deltay == 0)
	{
		deltax = 190;
		deltay = 250;
	}

	if( spriteram32[0x14/4] == 0x000002ff &&
	spriteram32[0x18/4] == 0x000007ff )
	{ /* HACK (fixes alpine racer and self test) */
		deltax = 48;
		deltay = 43;
	}
#endif

	int base = spriteram32[0] & 0xffff; // alpinesa/alpinr2b
	int num_sprites = (spriteram32[1]>>16) - base;

	// 'enable' bits: assume that bit 0 affects spritecount by 1 (alpinr2b), and all bits set means off (aquajet)
	int enable = spriteram32[0]>>16&7;
	num_sprites += (~enable & 1);

	if( num_sprites > 0 && num_sprites < 0x400 && enable != 7 )
	{
		pSource = &spriteram32[0x04000/4 + base*4];
		pPal    = &spriteram32[0x20000/4 + base*2];
		DrawSpritesHelper( machine, bitmap, cliprect, pSource, pPal, num_sprites, deltax, deltay );
	}

	/* VICS RAM provides two additional banks (also many unknown regs here) */
	/*
    0x940000 -x------       sprite chip busy
    0x940018 xxxx----       clr.w   $940018.l

    0x940034 xxxxxxxx       0x3070b0f

    0x940040 xxxxxxxx       sprite attribute size
    0x940048 xxxxxxxx       sprite attribute list baseaddr
    0x940050 xxxxxxxx       sprite color size
    0x940058 xxxxxxxx       sprite color list baseaddr

    0x940060..0x94007c      set#2
    */
	num_sprites = state->m_vics_control[0x60/4] >> 4 & 0x1ff;
	if( num_sprites > 0 )
	{
		pSource = &state->m_vics_data[(state->m_vics_control[0x68/4]&0xffff)/4];
		pPal    = &state->m_vics_data[(state->m_vics_control[0x78/4]&0xffff)/4];
		DrawSpritesHelper( machine, bitmap, cliprect, pSource, pPal, num_sprites, deltax, deltay );
	}

	num_sprites = state->m_vics_control[0x40/4] >> 4 & 0x1ff;
	if( num_sprites > 0 )
	{
		pSource = &state->m_vics_data[(state->m_vics_control[0x48/4]&0xffff)/4];
		pPal    = &state->m_vics_data[(state->m_vics_control[0x58/4]&0xffff)/4];
		DrawSpritesHelper( machine, bitmap, cliprect, pSource, pPal, num_sprites, deltax, deltay );
	}
} /* DrawSprites */

static void UpdatePalette(running_machine &machine)
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int i,j;
	for( i=0; i<NAMCOS22_PALETTE_SIZE/4; i++ )
	{
		if( state->m_dirtypal[i] )
		{
			for( j=0; j<4; j++ )
			{
				int which = i*4+j;
				int r = nthbyte(machine.generic.paletteram.u32,which+0x00000);
				int g = nthbyte(machine.generic.paletteram.u32,which+0x08000);
				int b = nthbyte(machine.generic.paletteram.u32,which+0x10000);
				palette_set_color( machine,which,MAKE_RGB(r,g,b) );
			}
			state->m_dirtypal[i] = 0;
		}
	}
} /* UpdatePalette */


static TILE_GET_INFO( TextTilemapGetInfo )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	UINT16 data = nthword( state->m_textram,tile_index );
   /**
    * xxxx.----.----.---- palette select
    * ----.xx--.----.---- flip
    * ----.--xx.xxxx.xxxx code
    */
	SET_TILE_INFO( GFX_CHAR,data&0x03ff,data>>12,TILE_FLIPYX((data&0x0c00)>>10) );
}

READ32_HANDLER( namcos22_textram_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	return state->m_textram[offset];
}

WRITE32_HANDLER( namcos22_textram_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	COMBINE_DATA( &state->m_textram[offset] );
	tilemap_mark_tile_dirty( state->m_bgtilemap, offset*2 );
	tilemap_mark_tile_dirty( state->m_bgtilemap, offset*2+1 );
}

READ32_HANDLER( namcos22_tilemapattr_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();

	switch (offset)
	{
		case 2:
		{
			UINT16 lo,hi = state->m_tilemapattr[offset] & 0xffff0000;
			// assume current scanline, 0x1ff if in vblank (used in alpinesa)
			// or maybe relative to posirq?
			if (space->machine().primary_screen->vblank()) lo = 0x1ff;
			else lo = space->machine().primary_screen->vpos() >> 1;
			return hi|lo;
		}

		case 3:
			// don't know, maybe also scanline related
			// timecris reads it everytime the gun triggers and will decline if it's 0xffff
			return 0;

		default:
			break;
	}

	return state->m_tilemapattr[offset];
}

WRITE32_HANDLER( namcos22_tilemapattr_w )
{
	/*
	0.hiword	R/W		x offset
	0.loword	R/W		y offset, bit 9 for interlacing?(cybrcomm, tokyowar)
	1.hiword	R/W		??? always 0x006e?
	1.loword	?		unused?
	2.hiword	R/W		posirq scanline, not hooked up yet
	2.loword	R		assume current scanline
	3.hiword	?		unused?
	3.loword	R		???
	*/
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	COMBINE_DATA( &state->m_tilemapattr[offset] );
//	popmessage("%08x\n%08x\n%08x\n%08x\n",state->m_tilemapattr[0],state->m_tilemapattr[1],state->m_tilemapattr[2],state->m_tilemapattr[3]);
}

static void namcos22s_mix_textlayer( running_machine &machine, bitmap_t *bitmap, const rectangle *cliprect, int prival )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	const pen_t *pens = machine.pens;
	UINT16 *src;
	UINT32 *dest;
	UINT8 *pri;
	int x,y;

	// prepare fader and alpha
	UINT8 alpha_check12 = nthbyte(state->m_gamma, 0x12);
	UINT8 alpha_check13 = nthbyte(state->m_gamma, 0x13);
	UINT8 alpha_mask = nthbyte(state->m_gamma, 0x14);
	UINT8 alpha_factor = nthbyte(state->m_gamma, 0x15);
	bool fade_enabled = mixer.flags&2 && mixer.fadeFactor;
	int fade_factor = 0xff - mixer.fadeFactor;
	rgbint fade_color;

	rgb_comp_to_rgbint(&fade_color, mixer.rFadeColor, mixer.gFadeColor, mixer.bFadeColor);

	// mix textlayer with poly/sprites
	for (y=0;y<480;y++)
	{
		src = BITMAP_ADDR16(state->m_mix_bitmap, y, 0);
		dest = BITMAP_ADDR32(bitmap, y, 0);
		pri = BITMAP_ADDR8(machine.priority_bitmap, y, 0);
		for (x=0;x<640;x++)
		{
			// skip if transparent or under poly/sprite
			if (pri[x] == prival)
			{
				rgbint rgb;
				rgb_to_rgbint(&rgb, pens[src[x]]);

				// apply alpha
				if (alpha_factor)
				{
					UINT8 pen = src[x]&0xff;
					if ((pen&0xf) == alpha_mask || pen == alpha_check12 || pen == alpha_check13)
					{
						rgbint mix;
						rgb_to_rgbint(&mix, dest[x]);
						rgbint_blend(&rgb, &mix, 0xff - alpha_factor);
					}
				}

				if (fade_enabled)
					rgbint_blend(&rgb, &fade_color, fade_factor);

				dest[x] = rgbint_to_rgb(&rgb);
			}
		}
	}
}

static void namcos22_mix_textlayer( running_machine &machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	const pen_t *pens = machine.pens;
	UINT16 *src;
	UINT32 *dest;
	UINT8 *pri;
	int x,y;

	// prepare fader and shadow factor
	bool fade_enabled = mixer.flags&2 && mixer.PolyFade_enabled;
	bool shadow_enabled = mixer.flags&0x100; // ? (ridgerac is the only game not using shadow)
	rgbint fade_color, rgb_mix[3];

	rgb_comp_to_rgbint(&fade_color, mixer.rPolyFadeColor, mixer.gPolyFadeColor, mixer.bPolyFadeColor);
	rgb_comp_to_rgbint(&rgb_mix[0], nthbyte(state->m_gamma, 0x08), nthbyte(state->m_gamma, 0x09), nthbyte(state->m_gamma, 0x0a)); // pen c
	rgb_comp_to_rgbint(&rgb_mix[1], nthbyte(state->m_gamma, 0x0b), nthbyte(state->m_gamma, 0x0c), nthbyte(state->m_gamma, 0x0d)); // pen d
	rgb_comp_to_rgbint(&rgb_mix[2], nthbyte(state->m_gamma, 0x0e), nthbyte(state->m_gamma, 0x0f), nthbyte(state->m_gamma, 0x10)); // pen e

	// mix textlayer with poly/sprites
	for (y=0;y<480;y++)
	{
		src = BITMAP_ADDR16(state->m_mix_bitmap, y, 0);
		dest = BITMAP_ADDR32(bitmap, y, 0);
		pri = BITMAP_ADDR8(machine.priority_bitmap, y, 0);
		for (x=0;x<640;x++)
		{
			// skip if transparent or under poly
			if (pri[x] == 2)
			{
				// apply shadow
				rgbint rgb;
				switch (src[x] & 0xff)
				{
					case 0xfc:
					case 0xfd:
					case 0xfe:
						if (shadow_enabled)
						{
							rgb_to_rgbint(&rgb, dest[x]);
							rgbint_scale_channel_and_clamp(&rgb, &rgb_mix[(src[x]&0xf)-0xc]);
							break;
						}
						// (fall through)
					default:
						rgb_to_rgbint(&rgb, pens[src[x]]);
						break;
				}

				if (fade_enabled)
					rgbint_scale_channel_and_clamp(&rgb, &fade_color);

				dest[x] = rgbint_to_rgb(&rgb);
			}
		}
	}
}

static void DrawCharacterLayer(running_machine &machine, bitmap_t *bitmap, const rectangle *cliprect )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int scroll_x = state->m_tilemapattr[0]>>16;
	int scroll_y = state->m_tilemapattr[0]&0xffff;

	tilemap_set_scrollx( state->m_bgtilemap,0, (scroll_x-0x35c) & 0x3ff );
	tilemap_set_scrolly( state->m_bgtilemap,0, scroll_y & 0x3ff );
	tilemap_set_palette_offset( state->m_bgtilemap, mixer.palBase*256 );

	if (state->m_mbSuperSystem22)
	{
		tilemap_draw_primask(state->m_mix_bitmap, cliprect, state->m_bgtilemap, 0, 4, 4);
		namcos22s_mix_textlayer(machine, bitmap, cliprect, 4);
	}
	else
	{
		tilemap_draw_primask(state->m_mix_bitmap, cliprect, state->m_bgtilemap, 0, 2, 3);
		namcos22_mix_textlayer(machine, bitmap, cliprect);
	}
}

/*********************************************************************************************/

static int
Cap( int val, int minval, int maxval )
{
	if( val<minval )
	{
		val = minval;
	}
	else if( val>maxval )
	{
		val = maxval;
	}
	return val;
}

#define LSB21 (0x1fffff)
#define LSB18 (0x03ffff)

static INT32
Signed18( UINT32 value )
{
	INT32 offset = value&LSB18;
	if( offset&0x20000 )
	{ /* sign extend */
		offset |= ~LSB18;
	}
	return offset;
}

/**
 * @brief render a single quad
 *
 * @param flags
 *     x1.----.----.---- ?
 *     --.-xxx.----.---- representative z algorithm?
 *     --.----.-1x-.---- backface cull enable
 *     --.----.----.--1x fog enable?
 *
 *      1163 // sky
 *      1262 // score (front)
 *      1242 // score (hinge)
 *      1243 // ?
 *      1063 // n/a
 *      1243 // various (2-sided?)
 *      1263 // everything else (1-sided?)
 *      1663 // ?
 *
 * @param color
 *      -------- xxxxxxxx unused?
 *      -xxxxxxx -------- palette select
 *      x------- -------- ?
 *
 * @param polygonShiftValue22
 *    0x1fbd0 - sky+sea
 *    0x0c350 - mountains
 *    0x09c40 - boats, surf, road, buildings
 *    0x07350 - guardrail
 *    0x061a8 - red car
 */
static void
BlitQuadHelper(
		running_machine &machine,
		bitmap_t *bitmap,
		unsigned color,
		unsigned addr,
		float m[4][4],
		INT32 polygonShiftValue22, /* 22 bits */
		int flags,
		int packetFormat )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int absolutePriority = state->m_mAbsolutePriority;
	UINT32 zsortvalue24;
	float zmin = 0.0f;
	float zmax = 0.0f;
	Poly3dVertex v[4];
	int i;

	for( i=0; i<4; i++ )
	{
		Poly3dVertex *pVerTex = &v[i];
		pVerTex->x = GetPolyData( state, 8+i*3+addr );
		pVerTex->y = GetPolyData( state, 9+i*3+addr );
		pVerTex->z = GetPolyData( state, 10+i*3+addr );
		TransformPoint( &pVerTex->x, &pVerTex->y, &pVerTex->z, m );
	} /* for( i=0; i<4; i++ ) */

	/* backface cull one-sided polygons */
	if( flags&0x0020 &&
		(v[2].x*((v[0].z*v[1].y)-(v[0].y*v[1].z)))+
		(v[2].y*((v[0].x*v[1].z)-(v[0].z*v[1].x)))+
		(v[2].z*((v[0].y*v[1].x)-(v[0].x*v[1].y))) >= 0 &&

		(v[0].x*((v[2].z*v[3].y)-(v[2].y*v[3].z)))+
		(v[0].y*((v[2].x*v[3].z)-(v[2].z*v[3].x)))+
		(v[0].z*((v[2].y*v[3].x)-(v[2].x*v[3].y))) >= 0 )
	{
		return;
	}

	for( i=0; i<4; i++ )
	{
		Poly3dVertex *pVerTex = &v[i];
		int bri;

		pVerTex->u = GetPolyData( state, 0+i*2+addr );
		pVerTex->v = GetPolyData( state, 1+i*2+addr );

		if( i==0 || pVerTex->z > zmax ) zmax = pVerTex->z;
		if( i==0 || pVerTex->z < zmin ) zmin = pVerTex->z;

		if( state->m_mLitSurfaceCount )
		{
			bri = state->m_mLitSurfaceInfo[state->m_mLitSurfaceIndex%state->m_mLitSurfaceCount];
			if( state->m_mSurfaceNormalFormat == 0x6666 )
			{
				if( i==3 )
					state->m_mLitSurfaceIndex++;
			}
			else if( state->m_mSurfaceNormalFormat == 0x4000 )
				state->m_mLitSurfaceIndex++;
			else
				logerror( "unknown normal format: 0x%x\n", state->m_mSurfaceNormalFormat );
		} /* pLitSurfaceInfo */
		else if( packetFormat & 0x40 )
			bri = (GetPolyData(state, i+addr)>>16)&0xff;
		else
			bri = 0x40;
		pVerTex->bri = bri;
	} /* for( i=0; i<4; i++ ) */

	if( zmin<0.0f ) zmin = 0.0f;
	if( zmax<0.0f ) zmax = 0.0f;

	switch( (flags&0x0f00)>>8 )
	{
		case 0:
			zsortvalue24 = (INT32)zmin;
			break;

		case 1:
			zsortvalue24 = (INT32)zmax;
			break;

		case 2:
		default:
			zsortvalue24 = (INT32)((zmin+zmax)/2.0f);
			break;
	}

	/* relative: representative z + shift values
    * 1x.xxxx.xxxxxxxx.xxxxxxxx fixed z value
    * 0x.xx--.--------.-------- absolute priority shift
    * 0-.--xx.xxxxxxxx.xxxxxxxx z-representative value shift
    */
	if( polygonShiftValue22 & 0x200000 )
		zsortvalue24 = polygonShiftValue22 & LSB21;
	else
	{
		zsortvalue24 += Signed18( polygonShiftValue22 );
		absolutePriority += (polygonShiftValue22&0x1c0000)>>18;
	}

	if( state->m_mObjectShiftValue22 & 0x200000 )
		zsortvalue24 = state->m_mObjectShiftValue22 & LSB21;
	else
	{
		zsortvalue24 += Signed18( state->m_mObjectShiftValue22 );
		absolutePriority += (state->m_mObjectShiftValue22&0x1c0000)>>18;
	}

	absolutePriority &= 7;
	zsortvalue24 = Cap(zsortvalue24,0,0x1fffff);
	zsortvalue24 |= (absolutePriority<<21);

	{
		struct SceneNode *node = NewSceneNode(machine, zsortvalue24,eSCENENODE_QUAD3D);
		node->data.quad3d.cmode = (v[0].u>>12)&0xf;
		node->data.quad3d.textureBank = (v[0].v>>12)&0xf;
		node->data.quad3d.color = (color>>8)&0xff;

		{
			INT32 cz = (INT32)((zmin+zmax)/2.0f);
			cz = Clamp256(cz/0x2000);
			node->data.quad3d.flags = (cz<<8)|(flags&3);
		}

		for( i=0; i<4; i++ )
		{
			Poly3dVertex *p = &node->data.quad3d.v[i];
			p->x     = v[i].x*mCamera.zoom;
			p->y     = v[i].y*mCamera.zoom;
			p->z     = v[i].z;
			p->u     = v[i].u&0xfff;
			p->v     = v[i].v&0xfff;
			p->bri   = v[i].bri;
		}
		node->data.quad3d.direct = 0;
		node->data.quad3d.vx = mCamera.vx;
		node->data.quad3d.vy = mCamera.vy;
		node->data.quad3d.vw = mCamera.vw;
		node->data.quad3d.vh = mCamera.vh;
	}
} /* BlitQuadHelper */

static void
RegisterNormals( namcos22_state *state, INT32 addr, float m[4][4] )
{
	int i;
	for( i=0; i<4; i++ )
	{
		float nx = DSP_FIXED_TO_FLOAT(GetPolyData(state, addr+i*3+0));
		float ny = DSP_FIXED_TO_FLOAT(GetPolyData(state, addr+i*3+1));
		float nz = DSP_FIXED_TO_FLOAT(GetPolyData(state, addr+i*3+2));
		float dotproduct;

		/* transform normal vector */
		TransformNormal( &nx, &ny, &nz, m );
		dotproduct = nx*mCamera.lx + ny*mCamera.ly + nz*mCamera.lz;
		if( dotproduct<0.0f )
			dotproduct = 0.0f;
		state->m_mLitSurfaceInfo[state->m_mLitSurfaceCount++] = mCamera.ambient + mCamera.power*dotproduct;
	}
} /* RegisterNormals */

static void
BlitQuads( running_machine &machine, bitmap_t *bitmap, INT32 addr, float m[4][4], INT32 base )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
//  int numAdditionalNormals = 0;
	int chunkLength = GetPolyData(state, addr++);
	int finish = addr + chunkLength;

	if( chunkLength>0x100 )
		fatalerror( "bad packet length" );

	while( addr<finish )
	{
		int packetLength = GetPolyData( state, addr++ );
		int packetFormat = GetPolyData( state, addr+0 );
		int flags, color, bias;

		/**
        * packetFormat:
        *      800000 final packet in chunk
        *      080000 ?
        *      020000 color word exists?
        *      010000 z-offset word exists?
        *      002000 ?
        *      001000 z-offset word exists?
        *      000400 ?
        *      000080 tex# or UV or CMODE?
        *      000040 use I
        *      000001 ?
        *
        * flags:
        *      1042 (always set)
        *      0c00 depth-cueing mode (brake-disc(2108)=001e43, model font)
        *      0200 usually 1
        *      0100 ?
        *      0040 1 ... polygon palette?
        *      0020 cull backface
        *      0002 ?
        *      0001 ?
        *
        * color:
        *      ff0000 type?
        *      008000 depth-cueing off
        *      007f00 palette#
        */
		switch( packetLength )
		{
			case 0x17:
				/**
                * word 0: opcode (8a24c0)
                * word 1: flags
                * word 2: color
                */
				flags = GetPolyData(state, addr+1);
				color = GetPolyData(state, addr+2);
				bias = 0;
				BlitQuadHelper( machine,bitmap,color,addr+3,m,bias,flags,packetFormat );
				break;

			case 0x18:
				/**
                * word 0: opcode (0b3480 for first N-1 quads or 8b3480 for final quad in primitive)
                * word 1: flags
                * word 2: color
                * word 3: depth bias
                */
				flags = GetPolyData(state, addr+1);
				color = GetPolyData(state, addr+2);
				bias  = GetPolyData(state, addr+3);
				BlitQuadHelper( machine,bitmap,color,addr+4,m,bias,flags,packetFormat );
				break;

			case 0x10: /* vertex lighting */
				/*
                333401 (opcode)
                000000  [count] [type]
                000000  000000  007fff // normal vector
                000000  000000  007fff // normal vector
                000000  000000  007fff // normal vector
                000000  000000  007fff // normal vector
                */
//              numAdditionalNormals = GetPolyData(state, addr+2);
				state->m_mSurfaceNormalFormat = GetPolyData(state, addr+3);
				state->m_mLitSurfaceCount = 0;
				state->m_mLitSurfaceIndex = 0;
				RegisterNormals( state, addr+4, m );
				break;

			case 0x0d: /* additional normals */
				/*
                300401 (opcode)
                007b09 ffdd04 0004c2
                007a08 ffd968 0001c1
                ff8354 ffe401 000790
                ff84f7 ffdd04 0004c2
                */
				RegisterNormals( state, addr+1, m );
				break;

			default:
				break;
		}
		addr += packetLength;
	}
} /* BlitQuads */

static void
BlitPolyObject( running_machine &machine, bitmap_t *bitmap, int code, float M[4][4] )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	unsigned addr1 = GetPolyData(state, code);
	state->m_mLitSurfaceCount = 0;
	state->m_mLitSurfaceIndex = 0;
	for(;;)
	{
		INT32 addr2 = GetPolyData(state, addr1++);
		if( addr2<0 )
			break;
		BlitQuads( machine, bitmap, addr2, M, code );
	}
} /* BlitPolyObject */

/*******************************************************************************/

READ32_HANDLER( namcos22_dspram_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	return state->m_polygonram[offset] | 0xff000000; // only d0-23 are connected
}

WRITE32_HANDLER( namcos22_dspram_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	if (mem_mask & 0x00ff0000)
	{
		// sign extend or crop
		mem_mask |= 0xff000000;
		if (data & 0x00800000)
			data |= 0xff000000;
		else
			data &= 0xffffff;
	}

	COMBINE_DATA( &state->m_polygonram[offset] );
}

/*******************************************************************************/

/**
 * master DSP can write directly to render device via port 0xc.
 * This is used for "direct drawn" polygons, and "direct draw from point rom"
 * feature - both opcodes exist in Ridge Racer's display-list processing
 *
 * record format:
 *  header (3 words)
 *      polygonShiftValue22
 *      color
 *      flags
 *
 *  per-vertex data (4*6 words)
 *      u,v
 *      sx,sy
 *      intensity;z.exponent
 *      z.mantissa
 *
 * master DSP can specify 3d objects indirectly (along with view transforms),
 * via the "transmit" PDP opcode.  the "render device" sends quad data to the slave DSP
 * viewspace clipping and projection
 *
 * most "3d object" references are 0x45 and greater.  references less than 0x45 are "special"
 * commands, using a similar point rom format.  the point rom header may point to point ram.
 *
 * slave DSP reads records via port 4
 * its primary purpose is applying lighting calculations
 * the slave DSP forwards draw commands to a "draw device"
 */

/*******************************************************************************/

/**
 * 0xfffd
 * 0x0: transform
 * 0x1
 * 0x2
 * 0x5: transform
 * >=0x45: draw primitive
 */
static void
HandleBB0003( namcos22_state *state, const INT32 *pSource )
{
   /*
        bb0003 or 3b0003

        14.00c8            light.ambient     light.power
        01.0000            ?                 light.dx
        06.5a82            window priority   light.dy
        00.a57e            ?                 light.dz

        c8.0081            vx=200,vy=129
        29.6092            zoom = 772.5625
        1e.95f8 1e.95f8            0.5858154296875   0.5858154296875 // 452
        1e.b079 1e.b079            0.6893463134765   0.6893463134765 // 532
        29.58e8                   711.25 (border? see time crisis)

        7ffe 0000 0000
        0000 7ffe 0000
        0000 0000 7ffe
    */
	mCamera.ambient = pSource[0x1]>>16;
	mCamera.power   = pSource[0x1]&0xffff;

	mCamera.lx       = DSP_FIXED_TO_FLOAT(pSource[0x2]);
	mCamera.ly       = DSP_FIXED_TO_FLOAT(pSource[0x3]);
	mCamera.lz       = DSP_FIXED_TO_FLOAT(pSource[0x4]);

	state->m_mAbsolutePriority = pSource[0x3]>>16;
	mCamera.vx      = (INT16)(pSource[5]>>16);
	mCamera.vy      = (INT16)pSource[5];
	mCamera.zoom    = DspFloatToNativeFloat(pSource[6]);
	mCamera.vw      = DspFloatToNativeFloat(pSource[7])*mCamera.zoom;
	mCamera.vh      = DspFloatToNativeFloat(pSource[9])*mCamera.zoom;

	state->m_mViewMatrix[0][0] = DSP_FIXED_TO_FLOAT(pSource[0x0c]);
	state->m_mViewMatrix[1][0] = DSP_FIXED_TO_FLOAT(pSource[0x0d]);
	state->m_mViewMatrix[2][0] = DSP_FIXED_TO_FLOAT(pSource[0x0e]);

	state->m_mViewMatrix[0][1] = DSP_FIXED_TO_FLOAT(pSource[0x0f]);
	state->m_mViewMatrix[1][1] = DSP_FIXED_TO_FLOAT(pSource[0x10]);
	state->m_mViewMatrix[2][1] = DSP_FIXED_TO_FLOAT(pSource[0x11]);

	state->m_mViewMatrix[0][2] = DSP_FIXED_TO_FLOAT(pSource[0x12]);
	state->m_mViewMatrix[1][2] = DSP_FIXED_TO_FLOAT(pSource[0x13]);
	state->m_mViewMatrix[2][2] = DSP_FIXED_TO_FLOAT(pSource[0x14]);

	TransformNormal( &mCamera.lx, &mCamera.ly, &mCamera.lz, state->m_mViewMatrix );
} /* HandleBB0003 */

static void
Handle200002( running_machine &machine, bitmap_t *bitmap, const INT32 *pSource )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	if( state->m_mPrimitiveID>=0x45 )
	{
		float m[4][4]; /* row major */

		matrix3d_Identity( m );

		m[0][0] = DSP_FIXED_TO_FLOAT(pSource[0x1]);
		m[1][0] = DSP_FIXED_TO_FLOAT(pSource[0x2]);
		m[2][0] = DSP_FIXED_TO_FLOAT(pSource[0x3]);

		m[0][1] = DSP_FIXED_TO_FLOAT(pSource[0x4]);
		m[1][1] = DSP_FIXED_TO_FLOAT(pSource[0x5]);
		m[2][1] = DSP_FIXED_TO_FLOAT(pSource[0x6]);

		m[0][2] = DSP_FIXED_TO_FLOAT(pSource[0x7]);
		m[1][2] = DSP_FIXED_TO_FLOAT(pSource[0x8]);
		m[2][2] = DSP_FIXED_TO_FLOAT(pSource[0x9]);

		m[3][0] = pSource[0xa]; /* xpos */
		m[3][1] = pSource[0xb]; /* ypos */
		m[3][2] = pSource[0xc]; /* zpos */

		matrix3d_Multiply( m, state->m_mViewMatrix );
		BlitPolyObject( machine, bitmap, state->m_mPrimitiveID, m );
	}
	else if( state->m_mPrimitiveID !=0 && state->m_mPrimitiveID !=2 )
	{
		logerror( "Handle200002:unk code=0x%x\n", state->m_mPrimitiveID );
		// ridgerac title screen waving flag: 0x5
	}
} /* Handle200002 */

static void
Handle300000( namcos22_state *state, const INT32 *pSource )
{
	state->m_mViewMatrix[0][0] = DSP_FIXED_TO_FLOAT(pSource[1]);
	state->m_mViewMatrix[1][0] = DSP_FIXED_TO_FLOAT(pSource[2]);
	state->m_mViewMatrix[2][0] = DSP_FIXED_TO_FLOAT(pSource[3]);

	state->m_mViewMatrix[0][1] = DSP_FIXED_TO_FLOAT(pSource[4]);
	state->m_mViewMatrix[1][1] = DSP_FIXED_TO_FLOAT(pSource[5]);
	state->m_mViewMatrix[2][1] = DSP_FIXED_TO_FLOAT(pSource[6]);

	state->m_mViewMatrix[0][2] = DSP_FIXED_TO_FLOAT(pSource[7]);
	state->m_mViewMatrix[1][2] = DSP_FIXED_TO_FLOAT(pSource[8]);
	state->m_mViewMatrix[2][2] = DSP_FIXED_TO_FLOAT(pSource[9]);
} /* Handle300000 */

static void
Handle233002( namcos22_state *state, const INT32 *pSource )
{
   /*
    00233002
       00000000 // zc adjust?
       0003dd00 // z bias adjust
       001fffff // far plane?
       00007fff 00000000 00000000
       00000000 00007fff 00000000
       00000000 00000000 00007fff
       00000000 00000000 00000000
   */
	state->m_mObjectShiftValue22 = pSource[2];
} /* Handle233002 */

static void
SimulateSlaveDSP( running_machine &machine, bitmap_t *bitmap )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	const INT32 *pSource = 0x300 + (INT32 *)state->m_polygonram;
	INT16 len;

	matrix3d_Identity( state->m_mViewMatrix );

	if( state->m_mbSuperSystem22 )
	{
		pSource += 4; /* FFFE 0400 */
	}
	else
	{
		pSource--;
	}

	for(;;)
	{
		INT16 next;
		state->m_mPrimitiveID = *pSource++;
		len  = (INT16)*pSource++;

		switch( len )
		{
		case 0x15:
			HandleBB0003( state, pSource ); /* define viewport */
			break;

		case 0x10:
			Handle233002( state, pSource ); /* set modal rendering options */
			break;

		case 0x0a:
			Handle300000( state, pSource ); /* modify view transform */
			break;

		case 0x0d:
			Handle200002( machine, bitmap, pSource ); /* render primitive */
			break;

		default:
			logerror( "unk 3d data(%d) addr=0x%x!", len, (int)(pSource-(INT32*)state->m_polygonram) );
			{
				int i;
				for( i=0; i<len; i++ )
				{
					logerror( " %06x", pSource[i]&0xffffff );
				}
				logerror( "\n" );
			}
			return;
		}

		/* hackery! commands should be streamed, not parsed here */
		pSource += len;
//      marker = (INT16)*pSource++; /* always 0xffff */
		pSource++;
		next   = (INT16)*pSource++; /* link to next command */
		if( (next&0x7fff) != (pSource - (INT32 *)state->m_polygonram) )
		{ /* end of list */
			break;
		}
	} /* for(;;) */
} /* SimulateSlaveDSP */

static void
DrawPolygons( running_machine &machine, bitmap_t *bitmap )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	if( state->m_mbDSPisActive )
	{
		SimulateSlaveDSP( machine, bitmap );
		poly_wait(state->m_poly, "DrawPolygons");
	}
} /* DrawPolygons */

void
namcos22_enable_slave_simulation( running_machine &machine, int enable )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	state->m_mbDSPisActive = enable;
}

/*********************************************************************************************/

READ32_HANDLER( namcos22_cgram_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	return state->m_cgram[offset];
}

WRITE32_HANDLER( namcos22_cgram_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	COMBINE_DATA( &state->m_cgram[offset] );
	gfx_element_mark_dirty(space->machine().gfx[GFX_CHAR],offset/32);
}

READ32_HANDLER( namcos22_paletteram_r )
{
	return space->machine().generic.paletteram.u32[offset];
}

WRITE32_HANDLER( namcos22_paletteram_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	COMBINE_DATA( &space->machine().generic.paletteram.u32[offset] );
	state->m_dirtypal[offset&(0x7fff/4)] = 1;
}

static void namcos22_reset(running_machine &machine)
{
	memset(&mSceneRoot, 0, sizeof(mSceneRoot));
	mpFreeSceneNode = NULL;
}

static void namcos22_exit(running_machine &machine)
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	poly_free(state->m_poly);
}

static VIDEO_START( common )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	int code;

	state->m_mix_bitmap = auto_bitmap_alloc(machine,640,480,BITMAP_FORMAT_INDEXED16);
	state->m_bgtilemap = tilemap_create( machine, TextTilemapGetInfo,tilemap_scan_rows,16,16,64,64 );
	tilemap_set_transparent_pen(state->m_bgtilemap, 0xf);

	state->m_mbDSPisActive = 0;
	memset( state->m_polygonram, 0xcc, 0x20000 );

	for (code = 0; code < machine.gfx[GFX_TEXTURE_TILE]->total_elements; code++)
		gfx_element_decode(machine.gfx[GFX_TEXTURE_TILE], code);

	Prepare3dTexture(machine, machine.region("textilemap")->base(), machine.gfx[GFX_TEXTURE_TILE]->gfxdata );
	state->m_dirtypal = auto_alloc_array(machine, UINT8, NAMCOS22_PALETTE_SIZE/4);
	state->m_mPtRomSize = machine.region("pointrom")->bytes()/3;
	state->m_mpPolyL = machine.region("pointrom")->base();
	state->m_mpPolyM = state->m_mpPolyL + state->m_mPtRomSize;
	state->m_mpPolyH = state->m_mpPolyM + state->m_mPtRomSize;

	state->m_poly = poly_alloc(machine, 4000, sizeof(poly_extra_data), 0);
	machine.add_notifier(MACHINE_NOTIFY_RESET, machine_notify_delegate(FUNC(namcos22_reset), &machine));
	machine.add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(FUNC(namcos22_exit), &machine));

	gfx_element_set_source(machine.gfx[GFX_CHAR], (UINT8 *)state->m_cgram);
}

VIDEO_START( namcos22 )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	state->m_mbSuperSystem22 = 0;
	VIDEO_START_CALL(common);
}

VIDEO_START( namcos22s )
{
	namcos22_state *state = machine.driver_data<namcos22_state>();
	state->m_mbSuperSystem22 = 1;
	state->m_czram[0] = auto_alloc_array(machine, UINT16, 0x200/2 );
	state->m_czram[1] = auto_alloc_array(machine, UINT16, 0x200/2 );
	state->m_czram[2] = auto_alloc_array(machine, UINT16, 0x200/2 );
	state->m_czram[3] = auto_alloc_array(machine, UINT16, 0x200/2 );

	memset(state->m_czram[0], 0, 0x200);
	memset(state->m_czram[1], 0, 0x200);
	memset(state->m_czram[2], 0, 0x200);
	memset(state->m_czram[3], 0, 0x200);

	VIDEO_START_CALL(common);
}

SCREEN_UPDATE( namcos22s )
{
	namcos22_state *state = screen->machine().driver_data<namcos22_state>();
	UpdateVideoMixer(screen->machine());
	UpdatePalette(screen->machine());
	bitmap_fill(screen->machine().priority_bitmap, cliprect, 0);

	// background color
	rgbint bg_color;
	rgb_comp_to_rgbint(&bg_color, nthbyte(state->m_gamma,0x08), nthbyte(state->m_gamma,0x09), nthbyte(state->m_gamma,0x0a));
	if (mixer.flags&1 && mixer.fadeFactor)
	{
		rgbint fade_color;
		rgb_comp_to_rgbint(&fade_color, mixer.rFadeColor, mixer.gFadeColor, mixer.bFadeColor);
		rgbint_blend(&bg_color, &fade_color, 0xff - mixer.fadeFactor);
	}
	bitmap_fill( bitmap, cliprect, rgbint_to_rgb(&bg_color));

	// layers
	UINT8 layer = nthbyte(state->m_gamma,0x1f);
	if (layer&4) DrawCharacterLayer(screen->machine(), bitmap, cliprect);
	if (layer&1) DrawPolygons(screen->machine(), bitmap);
	if (layer&2) DrawSprites(screen->machine(), bitmap, cliprect);
	if (layer&3) RenderScene(screen->machine(), bitmap );
	if (layer&4) namcos22s_mix_textlayer(screen->machine(), bitmap, cliprect, 6);
	ApplyGamma(screen->machine(), bitmap);

#ifdef MAME_DEBUG
	if( screen->machine().input().code_pressed(KEYCODE_D) )
	{
		namcos22_state *state = screen->machine().driver_data<namcos22_state>();
		FILE *f = fopen( "dump.txt", "wb" );
		if( f )
		{
			address_space *space = screen->machine().device("maincpu")->memory().space(AS_PROGRAM);

			{
				int i,bank;
				for( bank=0; bank<4; bank++ )
				{
					fprintf( f, "czram[%d] =", bank );
					for( i=0; i<256; i++ )
					{
						fprintf( f, " %04x", state->m_czram[bank][i] );
					}
					fprintf( f, "\n" );
				}
			}

			Dump(space, f,0x810000, 0x81000f, "cz attr" );
			Dump(space, f,0x820000, 0x8202ff, "unk_ac" );
			Dump(space, f,0x824000, 0x8243ff, "gamma");
			Dump(space, f,0x828000, 0x83ffff, "palette" );
			Dump(space, f,0x8a0000, 0x8a000f, "tilemap_attr");
			Dump(space, f,0x880000, 0x89ffff, "cgram/textram");
			Dump(space, f,0x900000, 0x90ffff, "vics_data");
			Dump(space, f,0x940000, 0x94007f, "vics_control");
			Dump(space, f,0x980000, 0x9affff, "sprite374" );
			Dump(space, f,0xc00000, 0xc1ffff, "polygonram");
			fclose( f );
		}
		while( screen->machine().input().code_pressed(KEYCODE_D) ){}
	}
#endif
	return 0;
}

SCREEN_UPDATE( namcos22 )
{
	UpdateVideoMixer(screen->machine());
	UpdatePalette(screen->machine());
	bitmap_fill(screen->machine().priority_bitmap, cliprect, 0);
	bitmap_fill(bitmap, cliprect, get_black_pen(screen->machine()));
	DrawPolygons(screen->machine(), bitmap);
	RenderScene(screen->machine(), bitmap);
	DrawCharacterLayer(screen->machine(), bitmap, cliprect);
	ApplyGamma(screen->machine(), bitmap);

#ifdef MAME_DEBUG
	if( screen->machine().input().code_pressed(KEYCODE_D) )
	{
		FILE *f = fopen( "dump.txt", "wb" );
		if( f )
		{
			address_space *space = screen->machine().device("maincpu")->memory().space(AS_PROGRAM);

			//Dump(space, f,0x90000000, 0x90000003, "led?" );
			//Dump(space, f,0x90010000, 0x90017fff, "cz_ram");
			//Dump(space, f,0x900a0000, 0x900a000f, "tilemap_attr");
			Dump(space, f,0x90020000, 0x90027fff, "gamma");
			//Dump(space, f,0x70000000, 0x7001ffff, "polygonram");
			fclose( f );
		}
		while( screen->machine().input().code_pressed(KEYCODE_D) ){}
	}
#endif
	return 0;
}

WRITE16_HANDLER( namcos22_dspram16_bank_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	COMBINE_DATA( &state->m_dspram_bank );
}

READ16_HANDLER( namcos22_dspram16_r )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	UINT32 value = state->m_polygonram[offset];
	switch( state->m_dspram_bank )
	{
	case 0:
		value &= 0xffff;
		break;

	case 1:
		value>>=16;
		break;

	case 2:
		state->m_mUpperWordLatch = value>>16;
		value &= 0xffff;
		break;

	default:
		break;
	}
	return (UINT16)value;
} /* namcos22_dspram16_r */

WRITE16_HANDLER( namcos22_dspram16_w )
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	UINT32 value = state->m_polygonram[offset];
	UINT16 lo = value&0xffff;
	UINT16 hi = value>>16;
	switch( state->m_dspram_bank )
	{
	case 0:
		COMBINE_DATA( &lo );
		break;

	case 1:
		COMBINE_DATA( &hi );
		break;

	case 2:
		COMBINE_DATA( &lo );
		hi = state->m_mUpperWordLatch;
		break;

	default:
		break;
	}
	state->m_polygonram[offset] = (hi<<16)|lo;
} /* namcos22_dspram16_w */

#ifdef MAME_DEBUG
static void
Dump( address_space *space, FILE *f, unsigned addr1, unsigned addr2, const char *name )
{
	unsigned addr;
	fprintf( f, "%s:\n", name );
	for( addr=addr1; addr<=addr2; addr+=16 )
	{
		UINT8 data[16];
		int bHasNonZero = 0;
		int i;
		for( i=0; i<16; i++ )
		{
			data[i] = space->read_byte(addr+i );
			if( data[i] )
			{
				bHasNonZero = 1;
			}
		}
		if( bHasNonZero )
		{
			fprintf( f,"%08x:", addr );
			for( i=0; i<16; i++ )
			{
				if( (i&0x03)==0 )
				{
					fprintf( f, " " );
				}
				fprintf( f, "%02x", data[i] );
			}
			fprintf( f, "\n" );
		}
	}
	fprintf( f, "\n" );
}
#endif

/**
 * 4038 spot enable?
 * 0828 pre-initialization
 * 0838 post-initialization
 **********************************************
 * upload:
 *   #bits data
 *    0010 FEC0
 *    0010 FF10
 *    0004 0004
 *    0004 000E
 *    0003 0007
 *    0002 0002
 *    0002 0003
 *    0001 0001
 *    0001 0001
 *    0001 0000
 *    0001 0001
 *    0001 0001
 *    0001 0000
 *    0001 0000
 *    0001 0000
 *    0001 0001
 **********************************************
 *    0008 00EA // 0x0ff
 *    000A 0364 // 0x3ff
 *    000A 027F // 0x3ff
 *    0003 0005 // 0x007
 *    0001 0001 // 0x001
 *    0001 0001 // 0x001
 *    0001 0001 // 0x001
 **********************************************
 */
WRITE32_HANDLER(namcos22_port800000_w)
{
	namcos22_state *state = space->machine().driver_data<namcos22_state>();
	/* 00000011011111110000100011111111001001111110111110110001 */
	UINT16 word = data>>16;
	logerror( "%x: C304/C399: 0x%04x\n", cpu_get_previouspc(&space->device()), word );
	if( word == 0x4038 )
	{
		state->m_mbSpotlightEnable = 1;
	}
	else
	{
		state->m_mbSpotlightEnable = 0;
	}
} /* namcos22_port800000_w */
