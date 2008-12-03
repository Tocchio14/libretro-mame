/*********************************************************************

    debugvw.h

    Debugger view engine.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

#ifndef __DEBUGVIEW_H__
#define __DEBUGVIEW_H__


/***************************************************************************
    CONSTANTS
***************************************************************************/

/* types passed to debug_view_alloc */
#define DVT_CONSOLE							(1)
#define DVT_REGISTERS						(2)
#define DVT_DISASSEMBLY						(3)
#define DVT_MEMORY							(4)
#define DVT_LOG								(5)
#define DVT_TIMERS							(6)
#define DVT_ALLOCS							(7)

enum _disasm_right_column
{
	DASM_RIGHTCOL_NONE,
	DASM_RIGHTCOL_RAW,
	DASM_RIGHTCOL_ENCRYPTED,
	DASM_RIGHTCOL_COMMENTS
};
typedef enum _disasm_right_column disasm_right_column;

/* properties available for disassembly views */
#define DVP_DASM_EXPRESSION					(101)	/* r/w - const char * */
#define DVP_DASM_TRACK_LIVE					(102)	/* r/w - UINT32 */
#define DVP_DASM_RIGHT_COLUMN				(103)	/* r/w - UINT32 */
#define DVP_DASM_BACKWARD_STEPS				(104)	/* r/w - UINT32 */
#define DVP_DASM_WIDTH						(105)	/* r/w - UINT32 */
#define DVP_DASM_ACTIVE_ADDRESS				(112)	/* r/w - UINT32 */

/* properties available for memory views */
#define DVP_MEM_SPACE						(100)	/* r/w - address space * */
#define DVP_MEM_EXPRESSION					(101)	/* r/w - const char * */
#define DVP_MEM_TRACK_LIVE					(102)	/* r/w - UINT32 */
#define DVP_MEM_SPACENUM					(103)	/* r/w - UINT32 */
#define DVP_MEM_BYTES_PER_CHUNK				(104)	/* r/w - UINT32 */
#define DVP_MEM_REVERSE_VIEW				(105)	/* r/w - UINT32 */
#define DVP_MEM_ASCII_VIEW					(106)	/* r/w - UINT32 */
#define DVP_MEM_RAW_BASE					(107)	/* r/w - void * */
#define DVP_MEM_RAW_LENGTH					(108)	/* r/w - UINT32 */
#define DVP_MEM_RAW_OFFSET_XOR				(109)	/* r/w - UINT32 */
#define DVP_MEM_RAW_LITTLE_ENDIAN			(110)	/* r/w - UINT32 */
#define DVP_MEM_WIDTH						(111)	/* r/w - UINT32 */
#define DVP_MEM_NO_TRANSLATION				(113)	/* r/w - UINT32 */

/* attribute bits for debug_view_char.attrib */
#define DCA_NORMAL							(0x00)	/* in Windows: black on white */
#define DCA_CHANGED							(0x01)	/* in Windows: red foreground */
#define DCA_SELECTED						(0x02)	/* in Windows: light red background */
#define DCA_INVALID							(0x04)	/* in Windows: dark blue foreground */
#define DCA_DISABLED						(0x08)	/* in Windows: darker foreground */
#define DCA_ANCILLARY						(0x10)	/* in Windows: grey background */
#define DCA_CURRENT							(0x20)	/* in Windows: yellow background */
#define DCA_COMMENT							(0x40)	/* in Windows: green foreground */

/* special characters that can be passed as a DVP_CHARACTER */
#define DCH_UP								(1)		/* up arrow */
#define DCH_DOWN							(2)		/* down arrow */
#define DCH_LEFT							(3)		/* left arrow */
#define DCH_RIGHT							(4)		/* right arrow */
#define DCH_PUP								(5)		/* page up */
#define DCH_PDOWN							(6)		/* page down */
#define DCH_HOME							(7)		/* home */
#define DCH_CTRLHOME						(8)		/* ctrl+home */
#define DCH_END								(9)		/* end */
#define DCH_CTRLEND							(10)	/* ctrl+end */
#define DCH_CTRLRIGHT						(11)	/* ctrl+right */
#define DCH_CTRLLEFT						(12)	/* ctrl+left */



/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/* opaque structure representing a debug view */
typedef struct _debug_view debug_view;


/* OSD callback function for a view */
typedef void (*debug_view_osd_update_func)(debug_view *view, void *osdprivate);


/* pair of X,Y coordinates for sizing */
typedef struct _debug_view_xy debug_view_xy;
struct _debug_view_xy
{
	INT32		x;
	INT32		y;
};


/* a single "character" in the debug view has an ASCII value and an attribute byte */
typedef struct _debug_view_char debug_view_char;
struct _debug_view_char
{
	UINT8		byte;
	UINT8		attrib;
};


/* description of a raw memory space */
typedef struct _memory_view_raw memory_view_raw;
struct _memory_view_raw
{
	void *		base;
	offs_t		length;
	offs_t		offsetxor;
	UINT8		endianness;
};



/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/


/* ----- initialization and cleanup ----- */

/* initializes the view system */
void debug_view_init(running_machine *machine);



/* ----- view creation/deletion ----- */

/* allocate a new debug view */
debug_view *debug_view_alloc(running_machine *machine, int type, debug_view_osd_update_func osdupdate, void *osdprivate);

/* free a debug view */
void debug_view_free(debug_view *view);



/* ----- update management ----- */

/* begin a sequence of changes so that only one update occurs */
void debug_view_begin_update(debug_view *view);

/* complete a sequence of changes so that only one update occurs */
void debug_view_end_update(debug_view *view);

/* force all views to refresh */
void debug_view_update_all(void);

/* force all views of a given type to refresh */
void debug_view_update_type(int type);

/* force all disassembly views to refresh */
void debug_disasm_update_all(void);



/* ----- standard view properties ----- */

/* return a pointer to a 2-dimentional array of characters that represent the visible area of the view */
const debug_view_char *debug_view_get_chars(debug_view *view);

/* type a character into a view */
void debug_view_type_character(debug_view *view, int character);



/* ----- standard view sizing ----- */

/* return the total view size in rows and columns */
debug_view_xy debug_view_get_total_size(debug_view *view);

/* return the visible size in rows and columns */
debug_view_xy debug_view_get_visible_size(debug_view *view);

/* return the top left position of the visible area in rows and columns */
debug_view_xy debug_view_get_visible_position(debug_view *view);

/* set the visible size in rows and columns */
void debug_view_set_visible_size(debug_view *view, debug_view_xy size);

/* set the top left position of the visible area in rows and columns */
void debug_view_set_visible_position(debug_view *view, debug_view_xy pos);



/* ----- standard view cursor management ----- */

/* return the current cursor position as a row and column */
debug_view_xy debug_view_get_cursor_position(debug_view *view);

/* return TRUE if a cursor is supported for this view type */
int debug_view_get_cursor_supported(debug_view *view);

/* return TRUE if a cursor is currently visible */
int debug_view_get_cursor_visible(debug_view *view);

/* set the current cursor position as a row and column */
void debug_view_set_cursor_position(debug_view *view, debug_view_xy pos);

/* set the visible state of the cursor */
void debug_view_set_cursor_visible(debug_view *view, int visible);



/* ----- registers view-specific properties ----- */

/* return the CPU whose registers are currently displayed */
const device_config *registers_view_get_cpu(debug_view *view);

/* specify the CPU whose registers are to be displayed */
void registers_view_set_cpu(debug_view *view, const device_config *device);



/* ----- disassembly view-specific properties ----- */

/* return the address space whose disassembly is being displayed */
const address_space *disasm_view_get_address_space(debug_view *view);

/* return the expression string describing the home address */
const char *disasm_view_get_expression(debug_view *view);

/* return the contents of the right column */
disasm_right_column disasm_view_get_right_column(debug_view *view);

/* return the number of instructions displayed before the home address */
UINT32 disasm_view_get_backward_steps(debug_view *view);

/* return the width in characters of the main disassembly section */
UINT32 disasm_view_get_disasm_width(debug_view *view);

/* return the PC of the currently selected address in the view */
offs_t disasm_view_get_selected_address(debug_view *view);

/* set the address space whose disassembly is being displayed */
void disasm_view_set_address_space(debug_view *view, const address_space *space);

/* set the expression string describing the home address */
void disasm_view_set_expression(debug_view *view, const char *expression);

/* set the contents of the right column */
void disasm_view_set_right_column(debug_view *view, disasm_right_column contents);

/* set the number of instructions displayed before the home address */
void disasm_view_set_backward_steps(debug_view *view, UINT32 steps);

/* set the width in characters of the main disassembly section */
void disasm_view_set_disasm_width(debug_view *view, UINT32 width);

/* set the PC of the currently selected address in the view */
void disasm_view_set_selected_address(debug_view *view, offs_t address);



/* ----- memory view-specific properties ----- */

/* return the address space whose memory is being displayed */
const address_space *memory_view_get_address_space(debug_view *view);

/* return a raw description of the memory being displayed */
const memory_view_raw *memory_view_get_raw(debug_view *view);

/* return the expression string describing the home address */
const char *memory_view_get_expression(debug_view *view);

/* return the currently displayed bytes per chunk */
UINT8 memory_view_get_bytes_per_chunk(debug_view *view);

/* return the number of chunks displayed across a row */
UINT32 memory_view_get_chunks_per_row(debug_view *view);

/* return TRUE if the memory view is displayed reverse */
UINT8 memory_view_get_reverse(debug_view *view);

/* return TRUE if the memory view is displaying an ASCII representation */
UINT8 memory_view_get_ascii(debug_view *view);

/* return TRUE if the memory view is displaying physical addresses versus logical addresses */
UINT8 memory_view_get_physical(debug_view *view);

/* set the address space whose memory is being displayed */
void memory_view_set_address_space(debug_view *view, const address_space *space);

/* provide a raw description of the memory to be displayed */
void memory_view_set_raw(debug_view *view, const memory_view_raw *raw);

/* set the expression string describing the home address */
void memory_view_set_expression(debug_view *view, const char *expression);

/* specify the number of bytes displayed per chunk */
void memory_view_set_bytes_per_chunk(debug_view *view, UINT8 chunkbytes);

/* specify the number of chunks displayed across a row */
void memory_view_set_chunks_per_row(debug_view *view, UINT32 rowchunks);

/* specify TRUE if the memory view is displayed reverse */
void memory_view_set_reverse(debug_view *view, UINT8 reverse);

/* specify TRUE if the memory view should display an ASCII representation */
void memory_view_set_ascii(debug_view *view, UINT8 ascii);

/* specify TRUE if the memory view should display physical addresses versus logical addresses */
void memory_view_set_physical(debug_view *view, UINT8 physical);


#endif
