/*************************************************************************

    Taito Air System

*************************************************************************/

enum { TAITOAIR_FRAC_SHIFT = 16, TAITOAIR_POLY_MAX_PT = 16 };

struct taitoair_spoint {
	INT32 x, y;
};

struct taitoair_poly {
	struct taitoair_spoint p[TAITOAIR_POLY_MAX_PT];
	int pcount;
	int col;
};


typedef struct _taitoair_state taitoair_state;
struct _taitoair_state
{
	/* memory pointers */
	UINT16 *      m68000_mainram;
	UINT16 *      line_ram;
	UINT16 *      dsp_ram;	/* Shared 68000/TMS32025 RAM */
	UINT16 *      paletteram;

	/* video-related */
	struct {
		int x1, y1, x2, y2;
	} view;

	taitoair_poly  q;

	/* misc */
	int           dsp_hold_signal;
	INT32         banknum;

	/* devices */
	const device_config *audiocpu;
	const device_config *dsp;
	const device_config *tc0080vco;
};


/*----------- defined in video/taitoair.c -----------*/

VIDEO_UPDATE( taitoair );
