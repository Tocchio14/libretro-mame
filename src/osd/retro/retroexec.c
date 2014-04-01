
int executeGame(char* path) {

	char tmp_dir[256];

	int gameRot=0;

	int driverIndex;

	FirstTimeUpdate = 1;

	screenRot = 0;

	for (int i = 0; i<64; i++)xargv_cmd[i]=NULL;

	Extract_AllPath(path);
	
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

		// handle case where Arcade game exist and game on a System also
		if(arcade==true){
			// test system
		   	if (getGameInfo(MsystemName, &gameRot, &driverIndex,&arcade) == 0) {
		      		write_log("System not found: %s\n", MsystemName);   		         	
	       		}
			else {
				write_log("System found: %s\n", MsystemName);   
				arcade=false;
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

	Set_Default_Option();

	Set_Path_Option();
	
	// useless ?
	if (tate) {
		if (screenRot == 3) {
			sprintf(XARGV[PARAMCOUNT++],"%s\0",(char*) "-rol");
		} else {
			sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)(screenRot ? "-mouse" : "-ror"));
		}
	} else {
		if (screenRot == 2) {
			sprintf(XARGV[PARAMCOUNT++],"%s\0",(char*)"-rol");
		} else {
			sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)(screenRot ? "-ror" : "-mouse"));
		}
	}

	sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)("-rompath"));
		
#ifdef WANT_MAME
   	sprintf(tmp_dir, "%s", MgamePath);
   	sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)(tmp_dir));		   
   	if(!boot_to_osd_enabled)
   		sprintf(XARGV[PARAMCOUNT++],"%s\0", MgameName);
  
#else
   	if(!commandline_enabled)
   	{
		if(!boot_to_osd_enabled)
	   	{
			sprintf(tmp_dir, "%s", MgamePath);
			sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)(tmp_dir));		   
		  	if(softlist_enabled)
		  	{
				if(!arcade)
				{
					sprintf(XARGV[PARAMCOUNT++],"%s\0",MsystemName);   
					if(!boot_to_bios_enabled)
					{
						if(!softlist_auto)
					  		sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)mediaType);
				   		sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)MgameName);
					}
			 	}
			 	else
			 	{
					sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)MgameName);
			 	}	     
		  	}
		  	else
		  	{
			 	if (strcmp(mediaType, "-rom") == 0) {
					sprintf(XARGV[PARAMCOUNT++],"%s\0", MgameName);
			 	} else {
					sprintf(XARGV[PARAMCOUNT++],"%s\0",MsystemName);
					sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)mediaType);
					sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)gameName);
			 	}   
	   
		  	}
		}
		else
		{
			sprintf(tmp_dir, "%s;%s", MgamePath,MparentPath);
			sprintf(XARGV[PARAMCOUNT++],"%s\0", (char*)(tmp_dir));		   	
		}
	}
	else	
		sprintf(XARGV[PARAMCOUNT++],"%s\0",(char*)gameName);	
 	
#endif 	 	 
	
	return 0;
} 

