
#define vdt911_chr_region "gfx1"

enum
{
	/* 10 bytes per character definition */
	vdt911_single_char_len = 10,

	vdt911_US_chr_offset        = 0,
	vdt911_UK_chr_offset        = vdt911_US_chr_offset+128*vdt911_single_char_len,
	vdt911_german_chr_offset    = vdt911_UK_chr_offset+128*vdt911_single_char_len,
	vdt911_swedish_chr_offset   = vdt911_german_chr_offset+128*vdt911_single_char_len,
	vdt911_norwegian_chr_offset = vdt911_swedish_chr_offset+128*vdt911_single_char_len,
	vdt911_frenchWP_chr_offset  = vdt911_norwegian_chr_offset+128*vdt911_single_char_len,
	vdt911_japanese_chr_offset  = vdt911_frenchWP_chr_offset+128*vdt911_single_char_len,

	vdt911_chr_region_len   = vdt911_japanese_chr_offset+256*vdt911_single_char_len
};

GFXDECODE_EXTERN( vdt911 );

enum vdt911_screen_size_t { char_960, char_1920 };
enum vdt911_model_t
{
	vdt911_model_US,
	vdt911_model_UK,
	vdt911_model_French,
	vdt911_model_German,
	vdt911_model_Swedish,   /* Swedish/Finnish */
	vdt911_model_Norwegian, /* Norwegian/Danish */
	vdt911_model_Japanese,  /* Katakana Japanese */
	/*vdt911_model_Arabic,*//* Arabic */
	vdt911_model_FrenchWP   /* French word processing */
};

struct vdt911_init_params_t
{
	vdt911_screen_size_t screen_size;
	vdt911_model_t model;
	void (*int_callback)(running_machine &machine, int state);
};

PALETTE_INIT( vdt911 );

void vdt911_init(running_machine &machine);
class vdt911_device : public device_t
{
public:
	vdt911_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~vdt911_device() { global_free(m_token); }

	// access to legacy token
	void *token() const { assert(m_token != NULL); return m_token; }
protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
private:
	// internal state
	void *m_token;
};

extern const device_type VDT911;


#define MCFG_VDT911_VIDEO_ADD(_tag, _intf) \
	MCFG_DEVICE_ADD(_tag, VDT911, 0) \
	MCFG_DEVICE_CONFIG(_intf)


	DECLARE_READ8_DEVICE_HANDLER(vdt911_cru_r);
DECLARE_WRITE8_DEVICE_HANDLER(vdt911_cru_w);

void vdt911_refresh(device_t *device, bitmap_ind16 &bitmap, const rectangle &cliprect, int x, int y);

void vdt911_keyboard(device_t *device);

#define VDT911_KEY_PORTS                                                                        \
	PORT_START("KEY0")  /* keys 1-16 */                                                                 \
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F1") PORT_CODE(KEYCODE_F1)        \
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F2") PORT_CODE(KEYCODE_F2)        \
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F3") PORT_CODE(KEYCODE_F3)        \
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F4") PORT_CODE(KEYCODE_F4)        \
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F5") PORT_CODE(KEYCODE_F5)        \
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F6") PORT_CODE(KEYCODE_F6)        \
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F7") PORT_CODE(KEYCODE_F7)        \
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F8") PORT_CODE(KEYCODE_F8)        \
		PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CMD") PORT_CODE(KEYCODE_F9)       \
		PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(red)") PORT_CODE(KEYCODE_F10)        \
		PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ERASE FIELD") PORT_CODE(KEYCODE_END)  \
		PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ERASE INPUT") PORT_CODE(KEYCODE_PGDN) \
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(grey)") PORT_CODE(KEYCODE_F11)       \
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("UPPER CAPS LOCK") PORT_CODE(KEYCODE_CAPSLOCK)  PORT_TOGGLE\
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1 !") PORT_CODE(KEYCODE_1)        \
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2 @") PORT_CODE(KEYCODE_2)        \
																								\
	PORT_START("KEY1")  /* keys 17-32 */                                                                \
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3 #") PORT_CODE(KEYCODE_3)        \
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4 $") PORT_CODE(KEYCODE_4)        \
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5 %") PORT_CODE(KEYCODE_5)        \
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6 ^") PORT_CODE(KEYCODE_6)        \
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7 &") PORT_CODE(KEYCODE_7)        \
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8 *") PORT_CODE(KEYCODE_8)        \
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9 (") PORT_CODE(KEYCODE_9)        \
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0 )") PORT_CODE(KEYCODE_0)        \
		PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+ [") PORT_CODE(KEYCODE_MINUS)    \
		PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("- ]") PORT_CODE(KEYCODE_EQUALS)   \
		PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("_ =") PORT_CODE(KEYCODE_BACKSPACE)    \
		PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC)      \
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7 (numpad)") PORT_CODE(KEYCODE_7_PAD) \
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8 (numpad)") PORT_CODE(KEYCODE_8_PAD) \
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9 (numpad)") PORT_CODE(KEYCODE_9_PAD) \
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRINT") PORT_CODE(KEYCODE_PRTSCR) \
																								\
	PORT_START("KEY2")  /* keys 33-48 */                                                                \
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(up)") PORT_CODE(KEYCODE_UP)      \
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("REPEAT") PORT_CODE(KEYCODE_LALT)  \
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER_PAD)  \
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q") PORT_CODE(KEYCODE_Q)      \
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W") PORT_CODE(KEYCODE_W)      \
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)      \
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R") PORT_CODE(KEYCODE_R)      \
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T") PORT_CODE(KEYCODE_T)      \
		PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y") PORT_CODE(KEYCODE_Y)      \
		PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U") PORT_CODE(KEYCODE_U)      \
		PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I") PORT_CODE(KEYCODE_I)      \
		PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O") PORT_CODE(KEYCODE_O)      \
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P") PORT_CODE(KEYCODE_P)      \
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CHAR (left/right)") PORT_CODE(KEYCODE_OPENBRACE)  \
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("FIELD (left/right)") PORT_CODE(KEYCODE_CLOSEBRACE)    \
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RETURN") PORT_CODE(KEYCODE_ENTER) \
																								\
	PORT_START("KEY3")  /* keys 49-64 */                                                                \
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4 (numpad)") PORT_CODE(KEYCODE_4_PAD) \
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5 (numpad)") PORT_CODE(KEYCODE_5_PAD) \
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6 (numpad)") PORT_CODE(KEYCODE_6_PAD) \
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(left)") PORT_CODE(KEYCODE_LEFT)  \
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("HOME") PORT_CODE(KEYCODE_HOME)    \
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(right)") PORT_CODE(KEYCODE_RIGHT)    \
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CONTROL") PORT_CODE(KEYCODE_LCONTROL) \
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)      \
		PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S") PORT_CODE(KEYCODE_S)      \
		PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)      \
		PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)      \
		PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G") PORT_CODE(KEYCODE_G)      \
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H") PORT_CODE(KEYCODE_H)      \
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J") PORT_CODE(KEYCODE_J)      \
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K") PORT_CODE(KEYCODE_K)      \
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L") PORT_CODE(KEYCODE_L)      \
																								\
	PORT_START("KEY4")  /* keys 65-80 */                                                                \
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("; :") PORT_CODE(KEYCODE_COLON)    \
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("' \"") PORT_CODE(KEYCODE_QUOTE)   \
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(not on US keyboard)") PORT_CODE(KEYCODE_BACKSLASH)   \
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SKIP TAB") PORT_CODE(KEYCODE_TAB) \
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1 (numpad)") PORT_CODE(KEYCODE_1_PAD) \
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2 (numpad)") PORT_CODE(KEYCODE_2_PAD) \
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3 (numpad)") PORT_CODE(KEYCODE_3_PAD) \
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("INS CHAR") PORT_CODE(KEYCODE_INSERT)  \
		PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(down)") PORT_CODE(KEYCODE_DOWN)  \
		PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL CHAR") PORT_CODE(KEYCODE_DEL) \
		PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SHIFT") PORT_CODE(KEYCODE_LSHIFT) \
		PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z") PORT_CODE(KEYCODE_Z)      \
		PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X") PORT_CODE(KEYCODE_X)      \
		PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)      \
		PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V") PORT_CODE(KEYCODE_V)      \
		PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)      \
																								\
	PORT_START("KEY5")  /* keys 81-91 */                                                                \
		PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N") PORT_CODE(KEYCODE_N)      \
		PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M") PORT_CODE(KEYCODE_M)      \
		PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(", <") PORT_CODE(KEYCODE_COMMA)    \
		PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(". >") PORT_CODE(KEYCODE_STOP) \
		PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/ ?") PORT_CODE(KEYCODE_SLASH)    \
		PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SHIFT") PORT_CODE(KEYCODE_RSHIFT) \
		PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0 (numpad)") PORT_CODE(KEYCODE_0_PAD) \
		PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(". (numpad)") PORT_CODE(KEYCODE_DEL_PAD)   \
		PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(not on US keyboard)") PORT_CODE(KEYCODE_MINUS_PAD)   \
		PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SPACE") PORT_CODE(KEYCODE_SPACE)  \
		PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(not on US keyboard)") PORT_CODE(KEYCODE_PLUS_PAD)
