//Args for CmdLine used if experimental_cmdline enabled.
static char ARGUV[32][1024];
static char ARGUC=0;

void parse_cmdline( const char *argv ){

	static char buffer[512*4];
	
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


int executeGame_cmd(char* path) {

	// lot of redundant code of executeGame
	int paramCount;
	int result = 0;
	int gameRot=0;
	int driverIndex;

	FirstTimeUpdate = 1;
	
	FirstTimeUpdate = 1;

	screenRot = 0;

	//MAME 1 args -> arcade rom with full path
	if(ARGUC==1){ 
		//split the path to directory and the name without the zip extension
		result = parsePath(path, MgamePath, MgameName);
		if (result == 0) {
			write_log("parse path failed! path=%s\n", path);
			strcpy(MgameName,path );
		}
		//Find the game info. Exit if game driver was not found.
		if (getGameInfo(MgameName, &gameRot, &driverIndex) == 0) {
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
		if (getGameInfo(ARGUV[0], &gameRot, &driverIndex) == 0) {
			write_log("game not found: %s\n", MgameName);
			return -2;
		}
	}

	write_log("creating frontend... game=%s\n", MgameName);

	//find how many parameters we have
	for (paramCount = 0; xargv[paramCount] != NULL; paramCount++)
		printf("args: %s\n",xargv[paramCount]);

	if(ARGUC==1){// 1 args -> arcade rom with full path  
			xargv[paramCount++] = (char*)"-rp";	
			xargv[paramCount++] = (char*)g_rom_dir;	
	}

	xargv[paramCount++] = (char*)("-cfg_directory");
	
	char cfg_dir[256];
	sprintf(cfg_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "cfg");
	xargv[paramCount++] = (char*)(cfg_dir);
	
	xargv[paramCount++] = (char*)("-nvram_directory");
	
	char nv_dir[256];
	sprintf(nv_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "nvram");
	xargv[paramCount++] = (char*)(nv_dir);
	
	xargv[paramCount++] = (char*)("-memcard_directory");
	
	char mem_dir[256];
	sprintf(mem_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "memcard");
	xargv[paramCount++] = (char*)(mem_dir);
		
	xargv[paramCount++] = (char*)("-input_directory");
	
	char inp_dir[256];
	sprintf(inp_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "input");
	xargv[paramCount++] = (char*)(inp_dir);
	
	xargv[paramCount++] = (char*)("-state_directory");
	
	char state_dir[256];
	sprintf(state_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "states");
	xargv[paramCount++] = (char*)(state_dir);
		
	xargv[paramCount++] = (char*)("-snapshot_directory");
	
	char snap_dir[256];
	sprintf(snap_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "snaps");
	xargv[paramCount++] = (char*)(snap_dir);
		
	xargv[paramCount++] = (char*)("-diff_directory");

	char diff_dir[256];
	sprintf(diff_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash, "diff");
	xargv[paramCount++] = (char*)(diff_dir);
	
	xargv[paramCount++] = (char*)("-samplepath");
	
	char samples_dir[256];
	sprintf(samples_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash, "samples");
	xargv[paramCount++] = (char*)(samples_dir);
	
	xargv[paramCount++] = (char*)("-artpath");
	
	char art_dir[256];
	sprintf(art_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash, "artwork");
	xargv[paramCount++] = (char*)(art_dir);
		
	xargv[paramCount++] = (char*)("-cheatpath");
	
	char cheat_dir[256];
	sprintf(cheat_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash, "cheat");
	xargv[paramCount++] = (char*)(cheat_dir);
	
	xargv[paramCount++] = (char*)("-inipath");
	
	char ini_dir[256];
	sprintf(ini_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash, "ini");
	xargv[paramCount++] = (char*)(ini_dir);	
	xargv[paramCount++] = (char*)("-hashpath");
	
	char hash_dir[256];
	sprintf(hash_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash, "hash");
	xargv[paramCount++] = (char*)(hash_dir);				


        xargv[paramCount++] = (char*)"-mouse";

	//MAME 1 args -> arcade rom with full path
	if(ARGUC==1){
		xargv[paramCount++] = MgameName;
	}
	else { //MESS  pass all cmdline args
		for(int i=0;i<ARGUC;i++)
			xargv[paramCount++] = ARGUV[i];
	}

	write_log("frontend parameters:%i\n", paramCount);

	for (int i = 0; xargv[i] != NULL; i++){
		write_log("  %s\n",xargv[i]);
	}
	
	osd_init_midi();

	cli_options MRoptions;
	mini_osd_interface MRosd;
	cli_frontend frontend(MRoptions, MRosd);
	result = frontend.execute(paramCount, ( char **)xargv); 

	xargv[paramCount - 2] = NULL;
	return result;
}

