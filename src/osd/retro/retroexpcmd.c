
//Args for experimental_cmdline
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

	char tmp_dir[256];

	int paramCount;
	int result = 0;
	int gameRot=0;
	int driverIndex;
	bool CreateConf = ( strcmp(ARGUV[0],"-cc") == 0 || strcmp(ARGUV[0],"-createconfig") == 0 )?1:0;
	bool Only1Arg =   ( ARGUC==1 )?1:0;

	FirstTimeUpdate = 1;

	screenRot = 0;

	for (int i = 0; i<64; i++)xargv_cmd[i]=NULL;

	//split the path to directory and the name without the zip extension
	if (parsePath(Only1Arg?path:ARGUV[ARGUC-1], MgamePath, MgameName) == 0) {
		write_log("parse path failed! path=%s\n", path);
		strcpy(MgameName,path );
	}
	//Find the game info. Exit if game driver was not found.
	if (getGameInfo(Only1Arg?MgameName:ARGUV[0], &gameRot, &driverIndex,&arcade) == 0) {

		// handle -cc/-createconfig case
		if(CreateConf){
			write_log("create an %s config\n", core);
		}
		else {
			write_log("game not found: %s\n", MgameName);
			return -2;
		}
	}

	//some hardcoded default Options
	paramCount=0;
	sprintf(XARGV[paramCount++],"%s\0",core);
	sprintf(XARGV[paramCount++],"%s\0","-joystick");
	sprintf(XARGV[paramCount++],"%s\0","-samplerate");
	sprintf(XARGV[paramCount++],"%s\0","48000");
	sprintf(XARGV[paramCount++],"%s\0","-sound");
	sprintf(XARGV[paramCount++],"%s\0","-cheat");
        sprintf(XARGV[paramCount++],"%s\0","-mouse");

	//Setup path Option according to retro (save/system) directory or current if NULL 
	for(int i=0;i<NB_OPTPATH;i++){

		sprintf(XARGV[paramCount++],"%s\0",(char*)(opt_name[i]));

		if(opt_type[i]==0){
			if(retro_save_directory!=NULL)sprintf(tmp_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash,dir_name[i]);	
			else sprintf(tmp_dir, "%s%c%s%c%s", ".", slash, core, slash,dir_name[i]);
		}
		else {
			if(retro_system_directory!=NULL)sprintf(tmp_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash,dir_name[i]);	
			else sprintf(tmp_dir, "%s%c%s%c%s", ".", slash, core, slash,dir_name[i]);
		}

		sprintf(XARGV[paramCount++],"%s\0",(char*)(tmp_dir));
	}

	if(Only1Arg){// Assume arcade rom with full path or -cc  

		if(CreateConf)
			sprintf(XARGV[paramCount++],"%s\0",(char*)"-createconfig");
		else {
			sprintf(XARGV[paramCount++],"%s\0",(char*)"-rp");	
			sprintf(XARGV[paramCount++],"%s\0",(char*)g_rom_dir);	
			sprintf(XARGV[paramCount++],"%s\0",MgameName);
		}
	}
	else { // Pass all cmdline args
		for(int i=0;i<ARGUC;i++)
			sprintf(XARGV[paramCount++],"%s\0", ARGUV[i]);
	}

	write_log("frontend parameters:%i\n", paramCount);

	for (int i = 0; i<paramCount; i++){
		xargv_cmd[i] = (char*)(XARGV[i]);
		write_log("  %s\n",XARGV[i]);
	}

	osd_init_midi();

	cli_options MRoptions;
	mini_osd_interface MRosd;
	cli_frontend frontend(MRoptions, MRosd);
	result = frontend.execute(paramCount, ( char **)xargv_cmd); 

	xargv_cmd[paramCount - 2] = NULL;

	return result;
}

