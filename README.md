mame-libretro
=============

Mainline MAME/MESS/UME for libretro (with libco). 
Always WIP, bugs are expected

BUILD INSTRUCTIONS:

* build for pc linux/win:

     make -f Makefile.libretro -j4
     (NB: for 64 bits build export PTR64=1 at least on win64)

* build for android:

     for now you must build in 2 pass 
     make -f Makefile.libretro "NATIVE=1" buildtools
     make -f Makefile.libretro "platform=android" emulator -j4

CONTROLS: 

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

        tips: R2 to tab and select newgame from mameui


NEW:

* separated core options between mame, mess and ume, lot's of redundant code but it's more consistent
* media type works with softlists too (useful for games with different versions with different media types (SMB2 nes and SMB2 famicom)
* ume works with arcade/console/computer with or without soflists
* boot to osd


TODO:

GLOBAL:

* sample rate or refresh rate on the fly
* rework global inputs
* rework per driver inputs
* core option to disable per driver inputs and default to a standard retropad assgnment
* core option to select additional content location (artwork/samples/etc) between CONTE
* commandline support is in place but not working yet

MAME

--

MESS

--

UME

--

NOTES:

* softlists are now supported (enabled by default)
* softlists will default to auto media type
* booting to bios on supported systems is now possible (disabled by default)
* path management has been reworked:

If RetroArch includes RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY it will try to use the path defined in retroarch.cfg. 
If no path is defined or RetroArch doesn't yet include RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY then it will use 
retro_system_directory.

Paths are mapped like this:

- cfg_directory
retro_save_directory\[mame|mess|ume]\cfg
- nvram_directory
retro_save_directory\[mame|mess|ume]\nvram
- memcard_directory
retro_save_directory\[mame|mess|ume]\memcard
- input_directory
retro_save_directory\[mame|mess|ume]\input
- state_directory (MAME save states, not libretro save states, core doesn't implement those)
retro_save_directory\[mame|mess|ume]\states
- snapshot_directory
retro_save_directory\[mame|mess|ume]\snaps
- diff_directory
retro_save_directory\[mame|mess|ume]\diff

Also there are some optional paths that might hold additional data for mame like artwork, samples, etc.
Only the following have been defined so far:

- samplepath
retro_system_directory\[mame|mess|ume]\samples
- artpath
retro_system_directory\[mame|mess|ume]\artwork
- cheatpath
retro_system_directory\[mame|mess|ume]\cheat
- hashpath
retro_system_directory\[mame|mess|ume]\hash
- inipath
retro_system_directory\[mame|mess|ume]\ini

** cheats need to be extracted, loading from a 7z didn't work for me but some users reported it is working.
   needs more testing since some users reported compressed cheats work
*** softlist games just work, you need the hash database and the roms in the correct folders with the correct names, for example:

NES SMB should be in rompath\nes\smb.zip
HASHES should be in SYSTEMDIR\[mame|mess|ume]\hashes
