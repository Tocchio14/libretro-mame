#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "osdepend.h"

#include "emu.h"
#include "clifront.h"
#include "render.h"
#include "ui/ui.h"
#include "uiinput.h"

#include "libretro.h" 

#include "log.h"

static int ui_ipt_pushchar=-1;

char g_rom_dir[1024];
char mediaType[10];

static bool softlist_enabled;
static bool softlist_auto;
static bool boot_to_bios_enabled;
static bool boot_to_osd_enabled;
static bool commandline_enabled;

static bool experimental_cmdline=FALSE;

static bool arcade=FALSE;

#ifdef _WIN32
char slash = '\\';
#else
char slash = '/';
#endif

#if defined(WANT_MAME)
const char core[] = "mame";
#elif defined(WANT_MESS)
const char core[] = "mess";
#elif defined(WANT_UME)
const char core[] = "ume";
#endif 	

#define M16B

#include "render.c"
#include "rendersw.inc"

static bool mouse_enable = false;
static bool videoapproach1_enable = false;
bool nagscreenpatch_enable = false;

static void extract_basename(char *buf, const char *path, size_t size)
{
   const char *base = strrchr(path, '/');
   if (!base)
      base = strrchr(path, '\\');
   if (!base)
      base = path;

   if (*base == '\\' || *base == '/')
      base++;

   strncpy(buf, base, size - 1);
   buf[size - 1] = '\0';

   char *ext = strrchr(buf, '.');
   if (ext)
      *ext = '\0';
}

static void extract_directory(char *buf, const char *path, size_t size)
{
   strncpy(buf, path, size - 1);
   buf[size - 1] = '\0';

   char *base = strrchr(buf, '/');
   if (!base)
      base = strrchr(buf, '\\');

   if (base)
      *base = '\0';
   else
      buf[0] = '\0';
}

//============================================================
//  CONSTANTS
//============================================================
#define MAX_BUTTONS         16

#ifdef DEBUG_LOG
# define LOG(msg) fprintf(stderr, "%s\n", msg)
#else
# define LOG(msg)
#endif 

//============================================================
//  GLOBALS
//============================================================

typedef struct joystate_t {
	int button[MAX_BUTTONS];
	int a1[2];
	int a2[2];
}Joystate;

// rendering target
static render_target *our_target = NULL;

// input device
static input_device *retrokbd_device; // KEYBD
static input_device *mouse_device;    // MOUSE
static input_device *joy_device[4];// JOY0/JOY1/JOY2/JOY3
static input_device *Pad_device[4];// PAD0/PAD1/PAD2/PAD3

// state 
static UINT16 retrokbd_state[RETROK_LAST];
static int mouseLX,mouseLY;
static int mouseBUT[4];
static Joystate joystate[4];

//enables / disables tate mode
static int tate = 0;
static int screenRot = 0;
int vertical,orient;

static char MgamePath[1024];
static char MparentPath[1024];
static char MgameName[512];
static char MsystemName[512];
static char gameName[1024];

//Args for CmdLine used if commandline_enabled.
static char ARGUV[20][1024];
static char ARGUC=0;

static char RSYSDIR[1024];
static char RCONDIR[1024];
static char RSAVDIR[1024];

static int FirstTimeUpdate = 1;

//============================================================
//  LIBCO
//============================================================
int pauseg=0; 

#include "libco/libco.h"

cothread_t mainThread;
cothread_t emuThread;

//============================================================
//  RETRO
//============================================================

#include "retromapper.c"
#include "retroinput.c"
#include "retroosd.c"

//============================================================
//  main
//============================================================

static const char* xargv[] = {
	core,
	"-joystick",
	"-samplerate",
	"48000",
	"-sound",
	"-cheat",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,	
	NULL,		
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,		
	NULL,		
	NULL,		
	NULL,		
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,	
	NULL,
};

static int parsePath(char* path, char* gamePath, char* gameName) {
	int i;
	int slashIndex = -1;
	int dotIndex = -1;
	int len = strlen(path);
	
		
	if (len < 1) {
		return 0;
	}

	for (i = len - 1; i >=0; i--) {
		if (path[i] == slash) {
			slashIndex = i;
			break;
		} else
		if (path[i] == '.') {
			dotIndex = i;
		}
	}
	if (slashIndex < 0 || dotIndex < 0) {
		return 0;
	}

	strncpy(gamePath, path, slashIndex);
	gamePath[slashIndex] = 0;
	strncpy(gameName, path + (slashIndex + 1), dotIndex - (slashIndex + 1));
	gameName[dotIndex - (slashIndex + 1)] = 0;

	write_log("gamePath=%s gameName=%s\n", gamePath, gameName);
	return 1;
}

static int parseSystemName(char* path, char* systemName) {
	int i;
	int j=0;
	int slashIndex[2];
	int len = strlen(path);
	
	if (len < 1) {
		return 0;
	}

	for (i = len - 1; i >=0; i--) {
		if (j<2)
		{
		   if (path[i] == slash) {
			   slashIndex[j] = i;
			   j++;
		   } 
		}
		else
		   break;
	}
	if (slashIndex < 0 ) {
		return 0;
	}
	
	strncpy(systemName, path + (slashIndex[1] +1), slashIndex[0]-slashIndex[1]-1);
	
	write_log("systemName=%s\n", systemName);
	return 1;
}

static int parseParentPath(char* path, char* parentPath) {
	int i;
	int j=0;
	int slashIndex[2];
	int len = strlen(path);
	
	if (len < 1) {
		return 0;
	}

	for (i = len - 1; i >=0; i--) {
		if (j<2)
		{
		   if (path[i] == slash) {
			   slashIndex[j] = i;
			   j++;
		   } 
		}
		else
		   break;
	}
	if (slashIndex < 0 ) {
		return 0;
	}
	
	strncpy(parentPath, path, slashIndex[1]);
	
	write_log("parentPath=%s\n", parentPath);
	return 1;
}

static int getGameInfo(char* gameName) {
	
	int gameFound = 0;
	int num=driver_list::find(gameName);

	if (num != -1){
		fprintf(stderr, "%s%s\n", driver_list::driver(num).name, driver_list::driver(num).description);
		if(driver_list::driver(num).flags& GAME_TYPE_ARCADE)
		{
		   write_log("type: ARCADE system\n");
		   arcade=TRUE;
		}
		else if(driver_list::driver(num).flags& GAME_TYPE_CONSOLE)write_log("type: CONSOLE system\n");
		else if(driver_list::driver(num).flags& GAME_TYPE_COMPUTER)write_log("type: COMPUTER system\n");
		gameFound = 1;
	}
	else	write_log("driver %s not found %i\n", gameName, num);

	return gameFound;
}

#include "../portmedia/pmmidi.inc"

int Add_Path_To_Args(){

	int paramCount;

	//find how many parameters we have
	for (paramCount = 0; xargv[paramCount] != NULL; paramCount++)
		printf("args: %s\n",xargv[paramCount]);

	if(experimental_cmdline/*commandline_enabled*/){
  		if(ARGUC==1){// 1 args -> arcade rom with full path  
			xargv[paramCount++] = (char*)"-rp";	
			xargv[paramCount++] = (char*)g_rom_dir;	
		}
	}

	xargv[paramCount++] = (char*)("-cfg_directory");
	
	static char cfg_dir[256];
	sprintf(cfg_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "cfg");
	xargv[paramCount++] = (char*)(cfg_dir);
	
	xargv[paramCount++] = (char*)("-nvram_directory");
	
	static char nv_dir[256];
	sprintf(nv_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "nvram");
	xargv[paramCount++] = (char*)(nv_dir);
	
	xargv[paramCount++] = (char*)("-memcard_directory");
	
	static char mem_dir[256];
	sprintf(mem_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "memcard");
	xargv[paramCount++] = (char*)(mem_dir);
		
	xargv[paramCount++] = (char*)("-input_directory");
	
	static char inp_dir[256];
	sprintf(inp_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "input");
	xargv[paramCount++] = (char*)(inp_dir);
	
	xargv[paramCount++] = (char*)("-state_directory");
	
	static char state_dir[256];
	sprintf(state_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "states");
	xargv[paramCount++] = (char*)(state_dir);
		
	xargv[paramCount++] = (char*)("-snapshot_directory");
	
	static char snap_dir[256];
	sprintf(snap_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "snaps");
	xargv[paramCount++] = (char*)(snap_dir);
		
	xargv[paramCount++] = (char*)("-diff_directory");

	static char diff_dir[256];
	sprintf(diff_dir, "%s%c%s%c%s", RSAVDIR, slash, core, slash, "diff");
	xargv[paramCount++] = (char*)(diff_dir);
	
	xargv[paramCount++] = (char*)("-samplepath");
	
	static char samples_dir[256];
	sprintf(samples_dir, "%s%c%s%c%s", RSYSDIR, slash, core, slash, "samples");
	xargv[paramCount++] = (char*)(samples_dir);
	
	xargv[paramCount++] = (char*)("-artpath");
	
	static char art_dir[256];
	sprintf(art_dir, "%s%c%s%c%s", RSYSDIR, slash, core, slash, "artwork");
	xargv[paramCount++] = (char*)(art_dir);
		
	xargv[paramCount++] = (char*)("-cheatpath");
	
	static char cheat_dir[256];
	sprintf(cheat_dir, "%s%c%s%c%s", RSYSDIR, slash, core, slash, "cheat");
	xargv[paramCount++] = (char*)(cheat_dir);
	
	xargv[paramCount++] = (char*)("-inipath");
	
	static char ini_dir[256];
	sprintf(ini_dir, "%s%c%s%c%s", RSYSDIR, slash, core, slash, "ini");
	xargv[paramCount++] = (char*)(ini_dir);	

	xargv[paramCount++] = (char*)("-hashpath");
	
	static char hash_dir[256];
	sprintf(hash_dir, "%s%c%s%c%s", RSYSDIR, slash, core, slash, "hash");
	xargv[paramCount++] = (char*)(hash_dir);				

	return paramCount;
}

int executeGame(char* path) {
	// cli_frontend does the heavy lifting; if we have osd-specific options, we
	// create a derivative of cli_options and add our own
	
	int paramCount0;
	int result = 0;

	FirstTimeUpdate = 1;

	screenRot = 0;

	if(experimental_cmdline/*commandline_enabled*/){
		
		int i;

		//MAME 1 args -> arcade rom with full path
		if(ARGUC==1){ 
			//split the path to directory and the name without the zip extension
			result = parsePath(path, MgamePath, MgameName);
			if (result == 0) {
				write_log("parse path failed! path=%s\n", path);
				strcpy(MgameName,path );
			}
			//Find the game info. Exit if game driver was not found.
			if (getGameInfo(MgameName) == 0) {
				write_log("game not found: %s\n", MgameName);
				return -2;
			}
		}
		else {  //MESS
			//split the path to directory and the name without the zip extension
			result = parsePath(ARGUV[ARGUC-1], MgamePath, MgameName);
			if (result == 0) {
				write_log("parse path failed! path=%s\n", path);
				strcpy(MgameName,path );
			}
			//Find the game info. Exit if game driver was not found.
			if (getGameInfo(ARGUV[0]) == 0) {
				write_log("game not found: %s\n", MgameName);
				return -2;
			}
		}

		write_log("creating frontend... game=%s\n", MgameName);

		paramCount0=Add_Path_To_Args();

	        xargv[paramCount0++] = (char*)"-mouse";

		//MAME 1 args -> arcade rom with full path
		if(ARGUC==1){
			xargv[paramCount0++] = MgameName;
		}
		else { //MESS  pass all cmdline args
			for(i=0;i<ARGUC;i++)
				xargv[paramCount0++] = ARGUV[i];
		}

	}
	else //!cmdline
	{

		//split the path to directory and the name without the zip extension
		result = parsePath(path, MgamePath, MgameName);
		if (result == 0) {
			write_log("parse path failed! path=%s\n", path);
			strcpy(MgameName,path);
		}
		
		//split the path to directory and the name without the zip extension
		result = parseSystemName(path, MsystemName);
		if (result == 0) {
			write_log("parse path failed! path=%s\n", path);
			strcpy(MsystemName,path );
		}
		//get the parent path
		result = parseParentPath(path, MparentPath);
		if (result == 0) {
			write_log("parse path failed! path=%s\n", path);
			strcpy(MparentPath,path );
		}	
	
#ifdef WANT_MAME	
		//find if the driver exists for MgameName, if not, exit
		if (getGameInfo(MgameName) == 0) {
			write_log("driver not found: %s\n", MgameName);
			return -2;
		}	
#else
		if(!commandline_enabled)
		{
		   //find if the driver exists for MgameName, if not, check if a driver exists for MsystemName, if not, exit
		   if (getGameInfo(MgameName) == 0) {
			   write_log("driver not found: %s\n", MgameName);
			   if (getGameInfo(MsystemName) == 0) {
			      write_log("driver not found: %s\n", MsystemName);
	   		         return -2;
			   }
		   }
	    	}	   
#endif	

		write_log("creating frontend... game=%s\n", MgameName);
		printf("using softlists: %d\n", softlist_enabled);
		//find how many parameters we have
		paramCount0=Add_Path_To_Args();
		xargv[paramCount0++] = (char*)("-rompath");
	
		char rom_dir[256];
		
#ifdef WANT_MAME
   		sprintf(rom_dir, "%s", MgamePath);
   		xargv[paramCount0++] = (char*)(rom_dir);		   
   		if(!boot_to_osd_enabled)
      			xargv[paramCount0++] = MgameName;
  
#else
   		if(!commandline_enabled)
   		{
	   		if(!boot_to_osd_enabled)
	   		{
		  		sprintf(rom_dir, "%s", MgamePath);
		  		xargv[paramCount0++] = (char*)(rom_dir);		   
		  		if(softlist_enabled)
		  		{
			 		if(!arcade)
			 		{
						xargv[paramCount0++] = MsystemName;   
						if(!boot_to_bios_enabled)
						{
				   			if(!softlist_auto)
					  			xargv[paramCount0++] = (char*)mediaType;
				   			xargv[paramCount0++] = (char*)MgameName;
						}
			 		}
			 		else
			 		{
						xargv[paramCount0++] = (char*)MgameName;
			 		}	     
		  		}
		  		else
		  		{
				 	if (strcmp(mediaType, "-rom") == 0) {
						xargv[paramCount0++] = MgameName;
				 	} else {
						xargv[paramCount0++] = MsystemName;
						xargv[paramCount0++] = (char*)mediaType;
						xargv[paramCount0++] = (char*)gameName;
				 	}	   
	   
		  		}
			}
			else
			{
		  		sprintf(rom_dir, "%s;%s", MgamePath,MparentPath);
		  		xargv[paramCount0++] = (char*)(rom_dir);		   	
			}
		}
		else	
	   		xargv[paramCount0++] = (char*)gameName;
	   	
#endif 	 	 
	
	} //end if cmdline

	write_log("frontend parameters:%i\n", paramCount0);

	for (int i = 0; xargv[i] != NULL; i++){
		write_log("  %s\n",xargv[i]);
	}
	
	osd_init_midi();

	cli_options MRoptions;
	mini_osd_interface MRosd;
	cli_frontend frontend(MRoptions, MRosd);
	result = frontend.execute(paramCount0, ( char **)xargv); 

	xargv[paramCount0 - 2] = NULL;
	return result;
} 
 
//============================================================
//  main
//============================================================

void parse_cmdline( const char *argv ){

	static char buffer[512*2];
	
	strcpy(buffer,argv);
	strcat(buffer," \0");
	
	char *p,*p2,*start_of_word;
	int c,c2;
	enum states { DULL, IN_WORD, IN_STRING } state = DULL;

	for (p = buffer; *p != '\0'; p++) {
		c = (unsigned char) *p; /* convert to unsigned char for is* functions */
		switch (state) {
			case DULL: /* not in a word, not in a double quoted string */
				if (isspace(c)) {
					/* still not in a word, so ignore this char */
				continue;
			}
			/* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
			if (c == '"') {
				state = IN_STRING;
				start_of_word = p + 1; /* word starts at *next* char, not this one */
				continue;
			}
			state = IN_WORD;
			start_of_word = p; /* word starts here */
			continue;

		case IN_STRING:
			/* we're in a double quoted string, so keep going until we hit a close " */
			if (c == '"') {
				/* word goes from start_of_word to p-1 */
				//... do something with the word ...
				for (c2=0,p2 = start_of_word; p2 <p; p2++,c2++)ARGUV[ARGUC][c2] = (unsigned char) *p2;
				ARGUC++; 
				
				state = DULL; /* back to "not in word, not in string" state */
			}
			continue; /* either still IN_STRING or we handled the end above */

		case IN_WORD:
			/* we're in a word, so keep going until we get to a space */
			if (isspace(c)) {
				/* word goes from start_of_word to p-1 */
				//... do something with the word ...
				for (c2=0,p2 = start_of_word; p2 <p; p2++,c2++)ARGUV[ARGUC][c2] = (unsigned char) *p2;
				ARGUC++; 
				
				state = DULL; /* back to "not in word, not in string" state */
			}
			continue; /* either still IN_WORD or we handled the end above */
		}	
	}
	
}

#ifdef __cplusplus
extern "C"
#endif
int mmain(int argc, const char *argv)
{

	int result = 0;

	static char tempstring[512];
	char *pch;
	
	strcpy(gameName,argv);

	if(experimental_cmdline/*commandline_enabled*/){
/*
		strcpy(tempstring,gameName);
	        pch = strtok (tempstring," ");
   
	  	while (pch != NULL)
	  	{   		  
			strcpy(ARGUV[ARGUC],pch);
			ARGUC++; 
	
	   		pch = strtok (NULL, " ");
	  	}
*/	  	
		parse_cmdline(argv);
	        write_log("executing from experimental cmdline... %s\n", gameName);
		result = executeGame(ARGUV[ARGUC-1]);
	}
	else{
		write_log("executing game... %s\n", gameName);
		result = executeGame(gameName);
	}

	return 1;
}

