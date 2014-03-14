/***************************************************************************
TI-85 and TI-86 drivers by Krzysztof Strzecha

Notes:
1. After start TI-85 waits for ON key interrupt, so press ON key to start
   calculator. ************* PRESS THE "Q" KEY TO TURN IT ON. ********************
2. Only difference between all TI-85 drivers is ROM version.
3. TI-86 is TI-85 with more RAM and ROM.
4. Only difference between all TI-86 drivers is ROM version.
5. Video engine (with grayscale support) based on the idea found in VTI source
   emulator written by Rusty Wagner.
6. NVRAM is saved properly only when calculator is turned off before exiting MESS.
7. To receive data from TI press "R" immediately after TI starts to send data.
8. To request screen dump from calculator press "S".
9. TI-81 does not have a serial link.

Needed:
1. Info about ports 3 (bit 2 seems to be allways 0) and 4.
2. Any info on TI-81 hardware.
3. ROM dumps of unemulated models.
4. Artworks.

New:
05/10/2002 TI-85 serial link works again.
17/09/2002 TI-85 snapshots loading fixed. Few code cleanups.
       TI-86 SNAPSHOT LOADING DOESNT WORK.
       TI-85, TI-86 SERIAL LINK DOESNT WORK.
08/09/2001 TI-81, TI-85, TI-86 modified to new core.
       TI-81, TI-85, TI-86 reset corrected.
21/08/2001 TI-81, TI-85, TI-86 NVRAM corrected.
20/08/2001 TI-81 ON/OFF fixed.
       TI-81 ROM bank switching added (port 5).
       TI-81 NVRAM support added.
15/08/2001 TI-81 kayboard is now mapped as it should be.
14/08/2001 TI-81 preliminary driver added.
05/07/2001 Serial communication corrected (transmission works now after reset).
02/07/2001 Many source cleanups.
       PCR added.
01/07/2001 Possibility to request screen dump from TI (received dumps are saved
       as t85i file).
29/06/2001 Received variables can be saved now.
19/06/2001 Possibility to receive variables from calculator (they are nor saved
       yet).
17/06/2001 TI-86 reset fixed.
15/06/2001 Possibility to receive memory backups from calculator.
07/06/2001 TI-85 reset fixed.
       Work on receiving data from calculator started.
04/06/2001 TI-85 is able to receive variables and memory backups.
14/05/2001 Many source cleanups.
11/05/2001 Release years corrected. Work on serial link started.
26/04/2001 NVRAM support added.
25/04/2001 Video engine totally rewritten so grayscale works now.
17/04/2001 TI-86 snapshots loading added.
       ti86grom driver added.
16/04/2001 Sound added.
       Five TI-86 drivers added (all features of TI-85 drivers without
       snapshot loading).
13/04/2001 Snapshot loading (VTI 2.0 save state files).
18/02/2001 Palette (not perfect).
       Contrast control (port 2) implemented.
       LCD ON/OFF implemented (port 3).
       Interrupts corrected (port 3) - ON/OFF and APD works now.
       Artwork added.
09/02/2001 Keypad added.
       200Hz timer interrupts implemented.
       ON key and its interrupts implemented.
       Calculator is now fully usable.
02/02/2001 Preliminary driver

To do:
- port 7 (TI-86)
- port 4 (all models)
- artwork (all models)
- port 0 link (TI-82 and TI-83)
- add TI-73, TI-83+ and T84+ drivers


TI-81 memory map

    CPU: Z80 2MHz
        0000-7fff ROM
        8000-ffff RAM (?)

TI-82 memory map

    CPU: Z80 6MHz
        0000-3fff ROM 0
        4000-7fff ROM 1-7 (switched)
        8000-ffff RAM

TI-83 memory map

    CPU: Z80 6MHz
        0000-3fff ROM 0
        4000-7fff ROM 1-15 (switched)
        8000-ffff RAM

TI-83Plus memory map

    CPU: Z80 8MHz (running at 6 MHz)
        0000-3fff ROM 0
        4000-7fff ROM 0-31 or RAM 0-1 (switched)
        7000-bfff ROM 0-31 or RAM 0-1 (switched)
        c000-ffff RAM 0-31 or RAM 0-1 (switched)

TI-85 memory map

    CPU: Z80 6MHz
        0000-3fff ROM 0
        4000-7fff ROM 1-7 (switched)
        8000-ffff RAM

TI-86 memory map

    CPU: Z80 6MHz
        0000-3fff ROM 0
        4000-7fff ROM 0-15 or RAM 0-7 (switched)
        7000-bfff ROM 0-15 or RAM 0-7 (switched)
        c000-ffff RAM 0

Interrupts:

    IRQ: 200Hz timer
         ON key

TI-81 ports:
    0: Video buffer offset (write only)
    1: Keypad
    2: Contrast (write only)
    3: ON status, LCD power
    4: Video buffer width, interrupt control (write only)
    5: ?
    6:
    7: ?

TI-82 ports:
    0: Link
    1: Keypad
    2: Memory page
    3: ON status, LCD power
    4: Video buffer width, interrupt control (write only)
    10: Controll port for the display controller
    11: Data port for the display controller

TI-83 ports:
    0: Link + Memory page
    1: Keypad
    2: Memory page
    3: ON status, LCD power
    4: Video buffer width, interrupt control (write only)
    10: Controll port for the display controller
    11: Data port for the display controller
    14: Battery Status

TI-83Plus ports:
    0: Link
    1: Keypad
    2: ?
    3: ON status, LCD power
    4: Interrupt status
    6: Memory page 1
    7: Memory page 2
    10: Controll port for the display controller
    11: Data port for the display controller

TI-85 ports:
    0: Video buffer offset (write only)
    1: Keypad
    2: Contrast (write only)
    3: ON status, LCD power
    4: Video buffer width, interrupt control (write only)
    5: Memory page
    6: Power mode
    7: Link

TI-86 ports:
    0: Video buffer offset (write only)
    1: Keypad
    2: Contrast (write only)
    3: ON status, LCD power
    4: Power mode
    5: Memory page
    6: Memory page
    7: Link

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "imagedev/snapquik.h"
#include "machine/nvram.h"
#include "includes/ti85.h"
#include "mcfglgcy.h"

/* port i/o functions */

static ADDRESS_MAP_START( ti81_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(ti85_port_0000_r, ti85_port_0000_w )
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti85_port_0002_r, ti85_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti85_port_0003_r, ti85_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti85_port_0004_r, ti85_port_0004_w )
	AM_RANGE(0x0005, 0x0005) AM_READWRITE(ti85_port_0005_r, ti85_port_0005_w )
	AM_RANGE(0x0007, 0x0007) AM_WRITE(ti81_port_0007_w)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti85_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(ti85_port_0000_r, ti85_port_0000_w )
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti85_port_0002_r, ti85_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti85_port_0003_r, ti85_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti85_port_0004_r, ti85_port_0004_w )
	AM_RANGE(0x0005, 0x0005) AM_READWRITE(ti85_port_0005_r, ti85_port_0005_w )
	AM_RANGE(0x0006, 0x0006) AM_READWRITE(ti85_port_0006_r, ti85_port_0006_w )
	AM_RANGE(0x0007, 0x0007) AM_READWRITE(ti8x_serial_r, ti8x_serial_w )
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti82_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(ti8x_serial_r, ti8x_serial_w )
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti82_port_0002_r, ti82_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti85_port_0003_r, ti85_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti85_port_0004_r, ti85_port_0004_w )
	AM_RANGE(0x0010, 0x0010) AM_DEVREADWRITE("t6a04", t6a04_device, control_read, control_write)
	AM_RANGE(0x0011, 0x0011) AM_DEVREADWRITE("t6a04", t6a04_device, data_read, data_write)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti81v2_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti82_port_0002_r, ti82_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti85_port_0003_r, ti85_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti85_port_0004_r, ti85_port_0004_w )
	AM_RANGE(0x0010, 0x0010) AM_DEVREADWRITE("t6a04", t6a04_device, control_read, control_write)
	AM_RANGE(0x0011, 0x0011) AM_DEVREADWRITE("t6a04", t6a04_device, data_read, data_write)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti83_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(ti83_port_0000_r, ti83_port_0000_w )  //TODO
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti83_port_0002_r, ti83_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti83_port_0003_r, ti83_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti85_port_0004_r, ti85_port_0004_w )
	AM_RANGE(0x0010, 0x0010) AM_DEVREADWRITE("t6a04", t6a04_device, control_read, control_write)
	AM_RANGE(0x0011, 0x0011) AM_DEVREADWRITE("t6a04", t6a04_device, data_read, data_write)
	AM_RANGE(0x0014, 0x0014) AM_READ_PORT( "BATTERY" )
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti83p_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(ti8x_plus_serial_r, ti8x_plus_serial_w)
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti83p_port_0002_r, ti83p_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti83_port_0003_r, ti83p_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti83_port_0003_r, ti83p_port_0004_w )
	AM_RANGE(0x0006, 0x0006) AM_READWRITE(ti86_port_0005_r, ti83p_port_0006_w )
	AM_RANGE(0x0007, 0x0007) AM_READWRITE(ti86_port_0006_r, ti83p_port_0007_w )
	AM_RANGE(0x0010, 0x0010) AM_DEVREADWRITE("t6a04", t6a04_device, control_read, control_write)
	AM_RANGE(0x0011, 0x0011) AM_DEVREADWRITE("t6a04", t6a04_device, data_read, data_write)
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti86_io, AS_IO, 8, ti85_state )
	ADDRESS_MAP_GLOBAL_MASK(0xff)
	AM_RANGE(0x0000, 0x0000) AM_READWRITE(ti85_port_0000_r, ti85_port_0000_w )
	AM_RANGE(0x0001, 0x0001) AM_READWRITE(ti8x_keypad_r, ti8x_keypad_w )
	AM_RANGE(0x0002, 0x0002) AM_READWRITE(ti85_port_0002_r, ti85_port_0002_w )
	AM_RANGE(0x0003, 0x0003) AM_READWRITE(ti85_port_0003_r, ti85_port_0003_w )
	AM_RANGE(0x0004, 0x0004) AM_READWRITE(ti85_port_0006_r, ti85_port_0006_w )
	AM_RANGE(0x0005, 0x0005) AM_READWRITE(ti86_port_0005_r, ti86_port_0005_w )
	AM_RANGE(0x0006, 0x0006) AM_READWRITE(ti86_port_0006_r, ti86_port_0006_w )
	AM_RANGE(0x0007, 0x0007) AM_READWRITE(ti8x_serial_r, ti8x_serial_w )
ADDRESS_MAP_END

/* memory w/r functions */

static ADDRESS_MAP_START( ti81_mem , AS_PROGRAM, 8, ti85_state )
	AM_RANGE(0x0000, 0x3fff) AM_ROMBANK("bank1")
	AM_RANGE(0x4000, 0x7fff) AM_ROMBANK("bank2")
	AM_RANGE(0x8000, 0xffff) AM_RAM AM_SHARE("nvram")
ADDRESS_MAP_END

static ADDRESS_MAP_START( ti86_mem , AS_PROGRAM, 8, ti85_state )
	AM_RANGE(0x0000, 0x3fff) AM_ROMBANK("bank1")
	AM_RANGE(0x4000, 0x7fff) AM_RAMBANK("bank2")
	AM_RANGE(0x8000, 0xbfff) AM_RAMBANK("bank3")
	AM_RANGE(0xc000, 0xffff) AM_RAMBANK("bank4")
ADDRESS_MAP_END

/* keyboard input */

static INPUT_PORTS_START (ti81)
	PORT_START("BIT0")   /* bit 0 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(-)") PORT_CODE(KEYCODE_M)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("GRAPH") PORT_CODE(KEYCODE_F5)
	PORT_START("BIT1")   /* bit 1 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+") PORT_CODE(KEYCODE_EQUALS)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STORE") PORT_CODE(KEYCODE_TAB)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TRACE") PORT_CODE(KEYCODE_F4)
	PORT_START("BIT2")   /* bit 2 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_MINUS)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LN") PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ZOOM") PORT_CODE(KEYCODE_F3)
	PORT_START("BIT3")   /* bit 3 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Up") PORT_CODE(KEYCODE_UP)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*") PORT_CODE(KEYCODE_L)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LOG") PORT_CODE(KEYCODE_QUOTE)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("RANGE") PORT_CODE(KEYCODE_F2)
	PORT_START("BIT4")   /* bit 4 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/") PORT_CODE(KEYCODE_SLASH)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(")") PORT_CODE(KEYCODE_CLOSEBRACE)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(") PORT_CODE(KEYCODE_OPENBRACE)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EE") PORT_CODE(KEYCODE_END)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x^2") PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y=") PORT_CODE(KEYCODE_F1)
	PORT_START("BIT5")   /* bit 5 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^") PORT_CODE(KEYCODE_P)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAN") PORT_CODE(KEYCODE_PGUP)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("COS") PORT_CODE(KEYCODE_HOME)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SIN") PORT_CODE(KEYCODE_INSERT)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x^-1") PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2nd") PORT_CODE(KEYCODE_LALT)
	PORT_START("BIT6")   /* bit 6 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CLEAR") PORT_CODE(KEYCODE_PGDN)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VARS") PORT_CODE(KEYCODE_F9)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRGM") PORT_CODE(KEYCODE_F8)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MATRX") PORT_CODE(KEYCODE_F7)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MATH") PORT_CODE(KEYCODE_F6)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("INS") PORT_CODE(KEYCODE_TILDE)
	PORT_START("BIT7")   /* bit 7 */
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MODE") PORT_CODE(KEYCODE_ESC)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X|T") PORT_CODE(KEYCODE_X)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ALPHA") PORT_CODE(KEYCODE_CAPSLOCK)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL") PORT_CODE(KEYCODE_DEL)
	PORT_START("ON")   /* ON */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ON/OFF") PORT_CODE(KEYCODE_Q)
INPUT_PORTS_END

static INPUT_PORTS_START (ti85)
	PORT_START("BIT0")   /* bit 0 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER   (ENTRY)") PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(-)     (ANS     |_|)") PORT_CODE(KEYCODE_M)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".       (:       Z)") PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0       (CHAR    Y)") PORT_CODE(KEYCODE_0)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F5      (M5)") PORT_CODE(KEYCODE_F5)
	PORT_START("BIT1")   /* bit 1 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+       (MEM     X)") PORT_CODE(KEYCODE_EQUALS)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3       (VARS    W)") PORT_CODE(KEYCODE_3)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2       (TEST    V)") PORT_CODE(KEYCODE_2)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1       (BASE    U)") PORT_CODE(KEYCODE_1)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STORE   (RCL     =)") PORT_CODE(KEYCODE_TAB)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F4      (M4)") PORT_CODE(KEYCODE_F4)
	PORT_START("BIT2")   /* bit 2 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-       (LIST    T)") PORT_CODE(KEYCODE_MINUS)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6       (STRNG   S)") PORT_CODE(KEYCODE_6)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5       (CONV    R)") PORT_CODE(KEYCODE_5)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4       (CONS    Q)") PORT_CODE(KEYCODE_4)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",       (ANGLE   P)") PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F3      (M3)") PORT_CODE(KEYCODE_F3)
	PORT_START("BIT3")   /* bit 3 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Up") PORT_CODE(KEYCODE_UP)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*       (MATH    O)") PORT_CODE(KEYCODE_L)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9       (CPLX    N)") PORT_CODE(KEYCODE_9)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8       (VECTR   M)") PORT_CODE(KEYCODE_8)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7       (MATRX   L)") PORT_CODE(KEYCODE_7)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x^2     (SQRT    K)") PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F2      (M2)") PORT_CODE(KEYCODE_F2)
	PORT_START("BIT4")   /* bit 4 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/       (CALC    J)") PORT_CODE(KEYCODE_SLASH)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(")       (]       I)") PORT_CODE(KEYCODE_CLOSEBRACE)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(       ([       H)") PORT_CODE(KEYCODE_OPENBRACE)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EE      (X^-1    G)") PORT_CODE(KEYCODE_END)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LN      (e^x     F)") PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F1      (M1)") PORT_CODE(KEYCODE_F1)
	PORT_START("BIT5")   /* bit 5 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^       (PI      E)") PORT_CODE(KEYCODE_P)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAN     (TAN^-1  D)") PORT_CODE(KEYCODE_PGUP)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("COS     (COS^-1  C)") PORT_CODE(KEYCODE_HOME)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SIN     (SIN^-1  B)") PORT_CODE(KEYCODE_INSERT)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LOG     (10^x    A)") PORT_CODE(KEYCODE_QUOTE)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2nd") PORT_CODE(KEYCODE_LALT)
	PORT_START("BIT6")   /* bit 6 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CLEAR   (TOLER)") PORT_CODE(KEYCODE_PGDN)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CUSTOM  (CATALOG)") PORT_CODE(KEYCODE_F9)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRGM    (POLY)") PORT_CODE(KEYCODE_F8)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STAT    (SIMULT)") PORT_CODE(KEYCODE_F7)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("GRAPH   (SOLVER)") PORT_CODE(KEYCODE_F6)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("EXIT    (QUIT)") PORT_CODE(KEYCODE_ESC)
	PORT_START("BIT7")   /* bit 7 */
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL     (INS)") PORT_CODE(KEYCODE_DEL)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x-VAR   (LINK    x)") PORT_CODE(KEYCODE_LCONTROL)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ALPHA   (alpha)") PORT_CODE(KEYCODE_CAPSLOCK)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MORE    (MODE)") PORT_CODE(KEYCODE_TILDE)
	PORT_START("ON")   /* ON */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ON/OFF") PORT_CODE(KEYCODE_Q)
INPUT_PORTS_END

static INPUT_PORTS_START (ti82)
	PORT_START("BIT0")   /* bit 0 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Down") PORT_CODE(KEYCODE_DOWN)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(-)") PORT_CODE(KEYCODE_M)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_STOP)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("GRAPH") PORT_CODE(KEYCODE_F5)
	PORT_START("BIT1")   /* bit 1 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Left") PORT_CODE(KEYCODE_LEFT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("+") PORT_CODE(KEYCODE_EQUALS)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STORE") PORT_CODE(KEYCODE_TAB)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TRACE") PORT_CODE(KEYCODE_F4)
	PORT_START("BIT2")   /* bit 2 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Right") PORT_CODE(KEYCODE_RIGHT)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_MINUS)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LN") PORT_CODE(KEYCODE_BACKSLASH)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ZOOM") PORT_CODE(KEYCODE_F3)
	PORT_START("BIT3")   /* bit 3 */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Up") PORT_CODE(KEYCODE_UP)
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("*") PORT_CODE(KEYCODE_L)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("LOG") PORT_CODE(KEYCODE_QUOTE)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("WINDOW") PORT_CODE(KEYCODE_F2)
	PORT_START("BIT4")   /* bit 4 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/") PORT_CODE(KEYCODE_SLASH)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(")") PORT_CODE(KEYCODE_CLOSEBRACE)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(") PORT_CODE(KEYCODE_OPENBRACE)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",") PORT_CODE(KEYCODE_END)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x^2") PORT_CODE(KEYCODE_COLON)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y=") PORT_CODE(KEYCODE_F1)
	PORT_START("BIT5")   /* bit 5 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("^") PORT_CODE(KEYCODE_P)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAN") PORT_CODE(KEYCODE_PGUP)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("COS") PORT_CODE(KEYCODE_HOME)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SIN") PORT_CODE(KEYCODE_INSERT)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x^-1") PORT_CODE(KEYCODE_COMMA)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2nd") PORT_CODE(KEYCODE_LALT)
	PORT_START("BIT6")   /* bit 6 */
		PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CLEAR") PORT_CODE(KEYCODE_PGDN)
		PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("VARS") PORT_CODE(KEYCODE_F9)
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("PRGM") PORT_CODE(KEYCODE_F8)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MATRX") PORT_CODE(KEYCODE_F7)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MATH") PORT_CODE(KEYCODE_F6)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("MODE") PORT_CODE(KEYCODE_ESC)
	PORT_START("BIT7")   /* bit 7 */
		PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("STAT") PORT_CODE(KEYCODE_TILDE)
		PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("x-VAR") PORT_CODE(KEYCODE_X)
		PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ALPHA") PORT_CODE(KEYCODE_CAPSLOCK)
		PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("DEL") PORT_CODE(KEYCODE_DEL)
	PORT_START("ON")   /* ON */
		PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ON/OFF") PORT_CODE(KEYCODE_Q)
INPUT_PORTS_END

static INPUT_PORTS_START (ti83)
	PORT_INCLUDE( ti82 )

	PORT_START("BATTERY")
		PORT_DIPNAME( 0x01, 0x01, "Battery Status" )
		PORT_DIPSETTING( 0x01, DEF_STR( Normal ) )
		PORT_DIPSETTING( 0x00, "Low Battery" )
INPUT_PORTS_END

/* machine definition */
static MACHINE_CONFIG_START( ti81, ti85_state )
	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", Z80, 2000000)        /* 2 MHz */
	MCFG_CPU_PROGRAM_MAP(ti81_mem)
	MCFG_CPU_IO_MAP(ti81_io)
	MCFG_QUANTUM_TIME(attotime::from_hz(60))


	/* video hardware */
	MCFG_SCREEN_ADD("screen", LCD)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_VBLANK_TIME(0)
	MCFG_SCREEN_SIZE(96, 64)
	MCFG_SCREEN_VISIBLE_AREA(0, 96-1, 0, 64-1)
	MCFG_SCREEN_UPDATE_DRIVER(ti85_state, screen_update_ti85)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_PALETTE_ADD("palette", 224)
	MCFG_PALETTE_INDIRECT_ENTRIES(224)
	MCFG_PALETTE_INIT_OWNER(ti85_state, ti85)

	MCFG_NVRAM_ADD_0FILL("nvram")
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( ti85, ti81 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK( 6000000)        /* 6 MHz */
	MCFG_CPU_IO_MAP(ti85_io)

	MCFG_MACHINE_RESET_OVERRIDE(ti85_state, ti85 )

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_SIZE(128, 64)
	MCFG_SCREEN_VISIBLE_AREA(0, 128-1, 0, 64-1)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END


static MACHINE_CONFIG_DERIVED( ti85d, ti85 )
	MCFG_SNAPSHOT_ADD("snapshot", ti85_state, ti8x, "sav", 0)
	//MCFG_TI85SERIAL_ADD( "tiserial" )
MACHINE_CONFIG_END


static const t6a04_interface ti82_display =
{
	64,                 // number of lines
	96,                 // pixels for line
};

static MACHINE_CONFIG_DERIVED( ti82, ti81 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK( 6000000)        /* 6 MHz */
	MCFG_CPU_IO_MAP(ti82_io)

	MCFG_MACHINE_RESET_OVERRIDE(ti85_state, ti85 )

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_UPDATE_DEVICE("t6a04", t6a04_device, screen_update)
	
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(2)
	MCFG_PALETTE_INIT_OWNER(ti85_state, ti82 )

	MCFG_T6A04_ADD("t6a04", ti82_display)

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	//MCFG_TI82SERIAL_ADD( "tiserial" )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( ti81v2, ti82 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_IO_MAP(ti81v2_io)

	//MCFG_DEVICE_REMOVE( "tiserial" )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( ti83, ti81 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK( 6000000)        /* 6 MHz */
	MCFG_CPU_IO_MAP(ti83_io)

	MCFG_MACHINE_RESET_OVERRIDE(ti85_state, ti85 )

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_UPDATE_DEVICE("t6a04", t6a04_device, screen_update)
	
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(2)
	MCFG_PALETTE_INIT_OWNER(ti85_state, ti82 )

	MCFG_T6A04_ADD("t6a04", ti82_display)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( ti86, ti85 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_PROGRAM_MAP(ti86_mem)
	MCFG_CPU_IO_MAP(ti86_io)

	MCFG_MACHINE_START_OVERRIDE(ti85_state, ti86 )
	MCFG_MACHINE_RESET_OVERRIDE(ti85_state, ti85 )

	MCFG_DEVICE_REMOVE("nvram")
	MCFG_NVRAM_HANDLER( ti86 )

	MCFG_SNAPSHOT_ADD("snapshot", ti85_state, ti8x, "sav", 0)
	//MCFG_TI86SERIAL_ADD( "tiserial" )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( ti83p, ti81 )
	MCFG_CPU_MODIFY("maincpu")
	MCFG_CPU_CLOCK( 6000000)        /* 8 MHz running at 6 MHz */
	MCFG_CPU_PROGRAM_MAP(ti86_mem)
	MCFG_CPU_IO_MAP(ti83p_io)

	MCFG_MACHINE_START_OVERRIDE(ti85_state, ti83p )
	MCFG_MACHINE_RESET_OVERRIDE(ti85_state, ti85 )

	MCFG_SCREEN_MODIFY("screen")
	MCFG_SCREEN_UPDATE_DEVICE("t6a04", t6a04_device, screen_update)
	
	MCFG_PALETTE_MODIFY("palette")
	MCFG_PALETTE_ENTRIES(2)
	MCFG_PALETTE_INIT_OWNER(ti85_state, ti82 )

	MCFG_T6A04_ADD("t6a04", ti82_display)

	MCFG_DEVICE_REMOVE("nvram")
	MCFG_NVRAM_HANDLER(ti83p)

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	//MCFG_TI83PSERIAL_ADD( "tiserial" )
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( ti73, ti83p )
	//MCFG_DEVICE_REMOVE( "tiserial" )
	//MCFG_TI73SERIAL_ADD( "tiserial" )
MACHINE_CONFIG_END

ROM_START (ti73)
	ROM_REGION (0x80000, "bios",0)
	ROM_DEFAULT_BIOS("v16")
	ROM_SYSTEM_BIOS( 0, "v16", "V 1.6" )
	ROMX_LOAD( "ti73v160.rom", 0x00000, 0x80000, CRC(bb0e3a16) SHA1(d62c2c7532698962818a747a7f32e35e41dfe338), ROM_BIOS(1) )
ROM_END

ROM_START (ti81)
	ROM_REGION (0x08000, "bios",0)
	ROM_DEFAULT_BIOS("v18")
	ROM_SYSTEM_BIOS( 0, "v11", "V 1.1K" )
	ROMX_LOAD( "ti81v11k.bin", 0x00000, 0x8000, CRC(0b860a63) SHA1(84a71cfc8818ca4b7d0caa76ffbf6d0463eaf7c6), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v16", "V 1.6K" )
	ROMX_LOAD( "ti81v16k.bin", 0x00000, 0x8000, CRC(452ca838) SHA1(92649f0f3bce7d8829d950cecd6532d7f7db1297), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "v18", "V 1.8K" )
	ROMX_LOAD( "ti81v18k.bin", 0x00000, 0x8000, CRC(94ac58e2) SHA1(ba915cfe2fe50a452ef8287db8f2244e29056d54), ROM_BIOS(3) )
	//No dumps 1.0, and 2.0 from ticalc.org, less sure about 1.6K
ROM_END

ROM_START (ti81v2)
	ROM_REGION (0x08000, "bios",0)
	ROM_DEFAULT_BIOS("v20")
	ROM_SYSTEM_BIOS( 0, "v20", "V 2.0V" )
	ROMX_LOAD( "ti81v20v.bin", 0x00000, 0x8000, CRC(cfbd12da) SHA1(d2a923526d98f1046fcb583e46951939ba66bdb9), ROM_BIOS(1) )
ROM_END

ROM_START (ti82)
	ROM_REGION (0x20000, "bios",0)
	ROM_DEFAULT_BIOS("v19")
	ROM_SYSTEM_BIOS( 0, "v16", "V 16.0" )
	ROMX_LOAD( "ti82v16.bin", 0x00000, 0x20000, CRC(e2f5721c) SHA1(df300ae52e105faf2785a8ae9f42e84e4308d460), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v17", "V 17.0" )
	ROMX_LOAD( "ti82v17.bin", 0x00000, 0x20000, CRC(0fc956d4) SHA1(77eef7d2db5ad1fb5de9129086a18428ddf66195), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "v18", "V 18.0" )
	ROMX_LOAD( "ti82v18.bin", 0x00000, 0x20000, CRC(6a320f03) SHA1(9ee15ebf0a1f8bde5bef982b5db4ce120c605d29), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "v19", "V 19.0" )
	ROMX_LOAD( "ti82v19.bin", 0x00000, 0x20000, CRC(ed4cf9ff) SHA1(10dc2d01c62b4e971a6ed7ebc75ca0f2e3dc4f95), ROM_BIOS(4) )
	//Rom versions according to ticalc.org 3*, 4*, 7*, 8.0, 10.0, 12.0, 15.0, 16.0, 17.0, 18.0, 19.0, 19.006
ROM_END

ROM_START (ti83)
	ROM_REGION (0x40000, "bios",0)
	ROM_DEFAULT_BIOS("v110")
	ROM_SYSTEM_BIOS( 0, "v102", "V 1.02" )
	ROMX_LOAD( "ti83v102.bin", 0x00000, 0x40000, CRC(7ee5d27b) SHA1(ce08f6a808701fc6672230a790167ee485157561), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v103", "V 1.03" )
	ROMX_LOAD( "ti83v103.bin", 0x00000, 0x40000, CRC(926f72a4) SHA1(8399e384804d8d29866caa4c8763d7a61946a467), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "v104", "V 1.04" )
	ROMX_LOAD( "ti83v104.bin", 0x00000, 0x40000, CRC(dccb73d3) SHA1(33877ff637dc5f4c5388799fd7e2159b48e72893), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "v106", "V 1.06" )
	ROMX_LOAD( "ti83v106.bin", 0x00000, 0x40000, CRC(2eae1cf0) SHA1(3d65c2a1b771ce8e5e5a0476ec1aa9c9cdc0e833), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "v107", "V 1.07" )
	ROMX_LOAD( "ti83v107.bin", 0x00000, 0x40000, CRC(4bf05697) SHA1(ef66dad3e7b2b6a86f326765e7dfd7d1a308ad8f), ROM_BIOS(5) )
	ROM_SYSTEM_BIOS( 5, "v108", "V 1.08" )
	ROMX_LOAD( "ti83v108.bin", 0x00000, 0x40000, CRC(0c6aafcc) SHA1(9c74f0b61655e9e160e92164db472ad7ee02b0f8), ROM_BIOS(6) )
	ROM_SYSTEM_BIOS( 6, "v110", "V 1.10" )
	ROMX_LOAD( "ti83v110.bin", 0x00000, 0x40000, CRC(7faee2d2) SHA1(25b373b58523647bb7b904001d391615e0b79bee), ROM_BIOS(7) )
	ROM_SYSTEM_BIOS( 7, "v110-2", "V 1.10 (2)" )
	ROMX_LOAD( "ti83v110-2.bin", 0x00000, 0x40000, CRC(56182912) SHA1(4c77fb77f023502b685a49a8013568b494384b25), ROM_BIOS(8) )
	//Rom versions according to ticalc.org 1.02, 1.03, 1.04, 1.06, 1.07, 1.08, 1.10
ROM_END

ROM_START (ti83p)
	ROM_REGION (0x80000, "bios",0)
	ROM_DEFAULT_BIOS("v116")
	ROM_SYSTEM_BIOS( 0, "v103", "V 1.03" )
	ROMX_LOAD( "ti83pv103.bin", 0x00000, 0x80000, CRC(da466be0) SHA1(37eaeeb9fb5c18fb494e322b75070e80cc4d858e), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v103m", "V 1.03 [m]" )
	ROMX_LOAD( "ti83pv103m.bin", 0x00000, 0x80000, CRC(281c9375) SHA1(80d698fed42976015a3e53fd59ebe7f49699b27e), ROM_BIOS(2))
	ROM_SYSTEM_BIOS( 2, "v103m2", "V 1.03 [m2]" )
	ROMX_LOAD( "ti83pv103m2.bin", 0x00000, 0x80000, CRC(690d9d30) SHA1(d215d3880e06c2ae31ec24b21d542d5bb2f3935b), ROM_BIOS(3))
	ROM_SYSTEM_BIOS( 3, "v110", "V 1.10" )
	ROMX_LOAD( "ti83pv110.bin", 0x00000, 0x80000, CRC(62683990) SHA1(F86CDEFE4ED5EF9965CD9EB667CB859E2CB10E19), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "v112", "V 1.12" )
	ROMX_LOAD( "ti83pv112.bin", 0x00000, 0x80000, CRC(ddca5026) SHA1(6615df5554076b6b81bd128bf847d2ff046e556b), ROM_BIOS(5) )
	ROM_SYSTEM_BIOS( 5, "v110-2", "V 1.10-2" )
	ROMX_LOAD( "ti83pv110-2.bin", 0x00000, 0x80000, CRC(504b9879) SHA1(8841d501870e8fc7173642d8a438205a040640fc), ROM_BIOS(6) )
	ROM_SYSTEM_BIOS( 6, "v112-2", "V 1.12-2" )
	ROMX_LOAD( "ti83pv112-2.bin", 0x00000, 0x80000, CRC(2126de12) SHA1(cbedc3a8cf8335eebf2b279d58720d8e7f86c569), ROM_BIOS(7) )
	ROM_SYSTEM_BIOS( 7, "v113", "V 1.13" )
	ROMX_LOAD( "ti83pv113.bin", 0x00000, 0x80000, CRC(30a243aa) SHA1(9b79e994ea1ce7af05b68f8ecee8b1b1fc3f0810), ROM_BIOS(8) )
	ROM_SYSTEM_BIOS( 8, "v114", "V 1.14" )
	ROMX_LOAD( "ti83pv114.bin", 0x00000, 0x80000, CRC(b32059c7) SHA1(46c66ba0421c03fc42f5afb06c7d3af812786140), ROM_BIOS(9) )
	ROM_SYSTEM_BIOS( 9, "v115", "V 1.15" )
	ROMX_LOAD( "ti83pv115.bin", 0x00000, 0x80000, CRC(9288029b) SHA1(8bd05fd47cab4028f275d1cc5383fd4f0e193474), ROM_BIOS(10) )
	ROM_SYSTEM_BIOS( 10, "v116", "V 1.16" )
	ROMX_LOAD( "ti83pv116.bin", 0x00000, 0x80000, CRC(0b7cd006) SHA1(290bc81159ea061d8ccb56a6f63e042f150afb32), ROM_BIOS(11) )
ROM_END

ROM_START (ti85)
	ROM_REGION (0x20000, "bios",0)
	ROM_DEFAULT_BIOS("v100")
	ROM_SYSTEM_BIOS( 0, "v30a", "V 3.0A" )
	ROMX_LOAD( "ti85v30a.bin", 0x00000, 0x20000, CRC(de4c0b1a) SHA1(f4cf4b8309372dbe26187bb279545f5d4bd48fc1), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v40",  "V 4.0" )
	ROMX_LOAD( "ti85v40.bin",  0x00000, 0x20000, CRC(a1723a17) SHA1(ff5866636bb3f206a6bf39cc9c9dc8308332aaf0), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "v50",  "V 5.0" )
	ROMX_LOAD( "ti85v50.bin",  0x00000, 0x20000, CRC(781fa403) SHA1(bf20d520d8efd7e5ae269789ca4b3c71848ac32a), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "v60",  "V 6.0" )
	ROMX_LOAD( "ti85v60.bin",  0x00000, 0x20000, CRC(b694a117) SHA1(36d58e2723e5ae4ffe0f8da691fa9a83bfe9e06b), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "v80",  "V 8.0" )
	ROMX_LOAD( "ti85v80.bin",  0x00000, 0x20000, CRC(7f296338) SHA1(765d5c612b6ffc0d1ded8f79bcbe880b1b562a98), ROM_BIOS(5) )
	ROM_SYSTEM_BIOS( 5, "v90",  "V 9.0" )
	ROMX_LOAD( "ti85v90.bin",  0x00000, 0x20000, CRC(6a0a94d0) SHA1(7742bf8a6929a21d06f306b494fc03b1fbdfe3e4), ROM_BIOS(6) )
	ROM_SYSTEM_BIOS( 6, "v100", "V 10.0" )
	ROMX_LOAD( "ti85v100.bin", 0x00000, 0x20000, CRC(053325b0) SHA1(36da1080c34e7b53cbe8463be5804e30e4a50dc8), ROM_BIOS(7) )
	//No_dumps 1.0, 2.0 and 7.0 according to ticalc.org
ROM_END

ROM_START (ti86)
	ROM_REGION (0x40000, "bios",0)
	ROM_DEFAULT_BIOS("v16")
	ROM_SYSTEM_BIOS( 0, "v12", "V 1.2" )
	ROMX_LOAD( "ti86v12.bin", 0x00000, 0x40000, CRC(bdf16105) SHA1(e40b22421c31bf0af104518b748ae79cd21d9c57), ROM_BIOS(1) )
	ROM_SYSTEM_BIOS( 1, "v13", "V 1.3" )
	ROMX_LOAD( "ti86v13.bin", 0x00000, 0x40000, CRC(073ef70f) SHA1(5702d4bb835bdcbfa8075ffd620fca0eaf3a1592), ROM_BIOS(2) )
	ROM_SYSTEM_BIOS( 2, "v14", "V 1.4" )
	ROMX_LOAD( "ti86v14.bin", 0x00000, 0x40000, CRC(fe6e2986) SHA1(23e0fb9a1763d5b9a7b0e593f09c2ff30c760866), ROM_BIOS(3) )
	ROM_SYSTEM_BIOS( 3, "v15", "V 1.5" )
	ROMX_LOAD( "ti86v15.bin", 0x00000, 0x40000, BAD_DUMP CRC(e6e10546) SHA1(5ca63fdfc965ae3fb8e0695263cf9da41f6ecb90), ROM_BIOS(4) )
	ROM_SYSTEM_BIOS( 4, "v16", "V 1.6" )
	ROMX_LOAD( "ti86v16.bin", 0x00000, 0x40000, CRC(37e02acc) SHA1(b5ad204885e5dde23a22f18f8d5eaffca69d638d), ROM_BIOS(5) )
	//Rom versions according to ticalc.org 1.2, 1.3, 1.4, 1.5, 1.6
ROM_END


ROM_START (ti83pse)
	ROM_REGION (0x200000, "bios",0)
	ROM_DEFAULT_BIOS("v116")
	ROM_SYSTEM_BIOS( 0, "v116", "V 1.16" )
	ROMX_LOAD( "ti83psev116.bin", 0x00000, 0x200000, CRC(d2570863) SHA1(d4214b3c0ebb26e10fe95294ac72a90d2ba99537), ROM_BIOS(1) )
ROM_END

ROM_START (ti84pse)
	ROM_REGION (0x200000, "bios",0)
	ROM_DEFAULT_BIOS("v241")
	ROM_SYSTEM_BIOS( 0, "v241", "V 2.41" )
	ROMX_LOAD( "ti84sev241.bin", 0x00000, 0x200000, CRC(5758db36) SHA1(7daa4f22e9b5dc8a1cc8fd31bceece9fa8b43515), ROM_BIOS(1) )
ROM_END


/*    YEAR  NAME        PARENT  COMPAT  MACHINE INPUT   INIT   COMPANY                 FULLNAME                        FLAGS */
COMP( 1990, ti81,       0,      0,      ti81,   ti81, driver_device,   0,     "Texas Instruments",    "TI-81",                        GAME_NO_SOUND )
COMP( 1992, ti85,       0,      0,      ti85d,  ti85, driver_device,   0,     "Texas Instruments",    "TI-85",                        GAME_NO_SOUND )
COMP( 1993, ti82,       0,      0,      ti82,   ti82, driver_device,   0,     "Texas Instruments",    "TI-82",                        GAME_NO_SOUND )
COMP( 1994, ti81v2,     ti81,   0,      ti81v2, ti81, driver_device,   0,     "Texas Instruments",    "TI-81 v2.0",                   GAME_NO_SOUND )
COMP( 1996, ti83,       0,      0,      ti83,   ti83, driver_device,   0,     "Texas Instruments",    "TI-83",                        GAME_NO_SOUND )
COMP( 1997, ti86,       0,      0,      ti86,   ti85, driver_device,   0,     "Texas Instruments",    "TI-86",                        GAME_NO_SOUND )
COMP( 1998, ti73,       0,      0,      ti73,   ti82, driver_device,   0,     "Texas Instruments",    "TI-73",                        GAME_NO_SOUND )
COMP( 1999, ti83p,      0,      0,      ti83p,  ti82, driver_device,   0,     "Texas Instruments",    "TI-83 Plus",                   GAME_NO_SOUND )
COMP( 2001, ti83pse,    0,      0,      ti85,   ti85, driver_device,   0,     "Texas Instruments",    "TI-83 Plus Silver Edition",    GAME_NOT_WORKING | GAME_NO_SOUND)
//COMP( 2004, ti84p,      0,      0,      ti85,   ti85, driver_device,   0,   "Texas Instruments",    "TI-84 Plus",                   GAME_NOT_WORKING | GAME_NO_SOUND)
COMP( 2004, ti84pse,    0,      0,      ti85,   ti85, driver_device,   0,     "Texas Instruments",    "TI-84 Plus Silver Edition",    GAME_NOT_WORKING | GAME_NO_SOUND)
