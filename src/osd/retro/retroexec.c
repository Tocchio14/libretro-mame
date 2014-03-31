
int executeGame(char* path) {
	// cli_frontend does the heavy lifting; if we have osd-specific options, we
	// create a derivative of cli_options and add our own
	char rom_dir[256];
	char tmp_dir[256];

	int paramCount;
	int result = 0;
	int gameRot=0;

	int driverIndex;

	FirstTimeUpdate = 1;

	screenRot = 0;

	for (int i = 0; i<64; i++)xargv_cmd[i]=NULL;

	//split the path to directory and the name without the zip extension
	result = parsePath(path, MgamePath, MgameName);
	if (result == 0) {
		write_log("parse path failed! path=%s\n", path);
		strcpy(MgameName,path);
	//	return -1;
	}
	
	//split the path to directory and the name without the zip extension
	result = parseSystemName(path, MsystemName);
	if (result == 0) {
		write_log("parse path failed! path=%s\n", path);
		strcpy(MsystemName,path );
	//	return -1;
	}
	//get the parent path
	result = parseParentPath(path, MparentPath);
	if (result == 0) {
		write_log("parse path failed! path=%s\n", path);
		strcpy(MparentPath,path );
	//	return -1;
	}	
	
#ifdef WANT_MAME	
	//find if the driver exists for MgameName, if not, exit
	if (getGameInfo(MgameName, &gameRot, &driverIndex,&arcade) == 0) {
		write_log("driver not found: %s\n", MgameName);
		return -2;
	}	
#else
    	if(!commandline_enabled)
	{
	   	//find if the driver exists for MgameName, if not, check if a driver exists for MsystemName, if not, exit
	   	if (getGameInfo(MgameName, &gameRot, &driverIndex,&arcade) == 0) {
			write_log("driver not found: %s\n", MgameName);
		   	if (getGameInfo(MsystemName, &gameRot, &driverIndex,&arcade) == 0) {
		      		write_log("driver not found: %s\n", MsystemName);
   		         	return -2;
	       		}
	   	}
    	}	   
#endif	

	// useless ?
	if (tate) {
		//horizontal game
		if (gameRot == ROT0) {
			screenRot = 1;
		} else
		if (gameRot &  ORIENTATION_FLIP_X) {
			write_log("*********** flip X \n");
			screenRot = 3;
		}

	} else
	{
		if (gameRot != ROT0) {
			screenRot = 1;
			if (gameRot &  ORIENTATION_FLIP_X) {
				write_log("*********** flip X \n");
				screenRot = 2;
			}
		}
	}
	
	write_log("creating frontend... game=%s\n", MgameName);
	printf("using softlists: %d\n", softlist_enabled);

	//some hardcoded default Options
	paramCount=0;
	sprintf(XARGV[paramCount++],"%s\0",core);
	sprintf(XARGV[paramCount++],"%s\0","-joystick");
	sprintf(XARGV[paramCount++],"%s\0","-samplerate");
	sprintf(XARGV[paramCount++],"%s\0","48000");
	sprintf(XARGV[paramCount++],"%s\0","-sound");
	sprintf(XARGV[paramCount++],"%s\0","-cheat");

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
	
	// useless ?
	if (tate) {
		if (screenRot == 3) {
			sprintf(XARGV[paramCount++],"%s\0",(char*) "-rol");
		} else {
			sprintf(XARGV[paramCount++],"%s\0", (char*)(screenRot ? "-mouse" : "-ror"));
		}
	} else {
		if (screenRot == 2) {
			sprintf(XARGV[paramCount++],"%s\0",(char*)"-rol");
		} else {
			sprintf(XARGV[paramCount++],"%s\0", (char*)(screenRot ? "-ror" : "-mouse"));
		}
	}

	sprintf(XARGV[paramCount++],"%s\0", (char*)("-rompath"));
		
#ifdef WANT_MAME
   	sprintf(rom_dir, "%s", MgamePath);
   	sprintf(XARGV[paramCount++],"%s\0", (char*)(rom_dir));		   
   	if(!boot_to_osd_enabled)
   		sprintf(XARGV[paramCount++],"%s\0", MgameName);
  
#else
   	if(!commandline_enabled)
   	{
		if(!boot_to_osd_enabled)
	   	{
			sprintf(rom_dir, "%s", MgamePath);
			sprintf(XARGV[paramCount++],"%s\0", (char*)(rom_dir));		   
		  	if(softlist_enabled)
		  	{
				if(!arcade)
				{
					sprintf(XARGV[paramCount++],"%s\0",MsystemName);   
					if(!boot_to_bios_enabled)
					{
						if(!softlist_auto)
					  		sprintf(XARGV[paramCount++],"%s\0", (char*)mediaType);
				   		sprintf(XARGV[paramCount++],"%s\0", (char*)MgameName);
					}
			 	}
			 	else
			 	{
					sprintf(XARGV[paramCount++],"%s\0", (char*)MgameName);
			 	}	     
		  	}
		  	else
		  	{
			 	if (strcmp(mediaType, "-rom") == 0) {
					sprintf(XARGV[paramCount++],"%s\0", MgameName);
			 	} else {
					sprintf(XARGV[paramCount++],"%s\0",MsystemName);
					sprintf(XARGV[paramCount++],"%s\0", (char*)mediaType);
					sprintf(XARGV[paramCount++],"%s\0", (char*)gameName);
			 	}   
	   
		  	}
		}
		else
		{
			sprintf(rom_dir, "%s;%s", MgamePath,MparentPath);
			sprintf(XARGV[paramCount++],"%s\0", (char*)(rom_dir));		   	
		}
	}
	else	
		sprintf(XARGV[paramCount++],"%s\0",(char*)gameName);	
 	
#endif 	 	 
	
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

