MAME libretro
=============

Mainline MAME/MESS/UME for libretro (with libco). 
Always WIP, bugs are expected

## Building

* build instructions for pc linux/win:

     make -f Makefile.libretro -j4
     (NB: for 64 bits build export PTR64=1 at least on win64), also you need python2.exe in your path

* build instructions for android:

     for now you must build in 2 pass 
     make -f Makefile.libretro "NATIVE=1" buildtools
     make -f Makefile.libretro "platform=android" emulator -j4

## Running

To run this core you need up-to date ROMs. Additionally MESS/UME need an specific folder structure and the up-to-date hash database. Softlists are supported and loading with softlists is enabled by default.
Example:

NES SMB should be in rompath\nes\smb.zip
HASHES should be in SYSTEMDIR\[mame|mess|ume]\hashes

There are some optional paths that might hold additional data for mame like artwork, samples, etc. Those are mapped like this:

- samplepath
SYSTEMDIR\[mame|mess|ume]\samples
- artpath
SYSTEMDIR\[mame|mess|ume]\artwork
- cheatpath
SYSTEMDIR\[mame|mess|ume]\cheat
- hashpath
SYSTEMDIR\[mame|mess|ume]\hash
- inipath
SYSTEMDIR\[mame|mess|ume]\ini

MAME creates some files on certain directories, those are mapped like this:

- cfg_directory
SAVEDIR\[mame|mess|ume]\cfg
- nvram_directory
SAVEDIR\[mame|mess|ume]\nvram
- memcard_directory
SAVEDIR\[mame|mess|ume]\memcard
- input_directory
SAVEDIR\[mame|mess|ume]\input
- state_directory (MAME save states, not libretro save states, core doesn't implement those)
SAVEDIR\[mame|mess|ume]\states
- snapshot_directory
SAVEDIR\[mame|mess|ume]\snaps
- diff_directory
SAVEDIR\[mame|mess|ume]\diff

MAME cheats can be either compressed under SYSTEMDIR\[mame|mess|ume]\ or extracted under SYSTEMDIR\[mame|mess|ume]\cheat\

## Controls 

	RETRO_DEVICE_ID_JOYPAD_L 		[KEY_BUTTON_5]
	RETRO_DEVICE_ID_JOYPAD_R		[KEY_BUTTON_6]
	RETRO_DEVICE_ID_JOYPAD_R2		[KEY_TAB]
	RETRO_DEVICE_ID_JOYPAD_L2;		[KEY_F11]
	RETRO_DEVICE_ID_JOYPAD_R3		[KEY_F2]
	RETRO_DEVICE_ID_JOYPAD_L3;		[KEY_F3]
	RETRO_DEVICE_ID_JOYPAD_START		[KEY_START]
	RETRO_DEVICE_ID_JOYPAD_SELECT		[KEY_COIN]
	RETRO_DEVICE_ID_JOYPAD_A		[KEY_BUTTON_1]
	RETRO_DEVICE_ID_JOYPAD_B		[KEY_BUTTON_2]
	RETRO_DEVICE_ID_JOYPAD_X		[KEY_BUTTON_3]
	RETRO_DEVICE_ID_JOYPAD_Y		[KEY_BUTTON_4]
	RETRO_DEVICE_ID_JOYPAD_UP		[KEY_JOYSTICK_U]
	RETRO_DEVICE_ID_JOYPAD_DOWN		[KEY_JOYSTICK_D]
	RETRO_DEVICE_ID_JOYPAD_LEFT		[KEY_JOYSTICK_L]
	RETRO_DEVICE_ID_JOYPAD_RIGHT		[KEY_JOYSTICK_R]

        tips: L2 activates MAME OSD

## Options

[MAME/MAME/MESS/UME]
* Read configuration - attempts to load settings from MAME.ini
* Auto save/load states - uses the core builtin save functionality to save/load progress automatically
* Enable in-game mouse - enable mouse support
* Enable cheats - enables the core builtin cheat functionality
* Alternate render method - causes MAME to use the new video renderer
* Boot to OSD - attempts to start to MAME/MESS/UME OSD
* Boot from CLI - allows you to boot using any MAME's builtin commandline parameters via CLI

[MESS/UME]
* Write configuration - writes per driver configuration to a file on exit
* Enable soflists - enables softlist usage
* Softlist automatic media type - attempts to guess mediatype based on the folder structure
* Media type - forces an specific media type, softlists or not
* Boot to BIOS - attempts to boot to BIOS instead of booting to the game



* separated core options between mame, mess and ume, lot's of redundant code but it's more consistent
* media type works with softlists too (useful for games with different versions with different media types (SMB2 nes and SMB2 famicom)
* ume works with arcade/console/computer with or without soflists
* boot to osd


## To-do

* sample rate or refresh rate on the fly
* rework global inputs
* rework per driver inputs
* core option to disable per driver inputs and default to a standard retropad assgnment
* core option to select additional content location (artwork/samples/etc) between CONTE
