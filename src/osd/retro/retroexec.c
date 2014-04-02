
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
			Add_Option((char*) "-rol");
		} else {
			Add_Option((char*)(screenRot ? "-mouse" : "-ror"));
		}
	} else {
		if (screenRot == 2) {
			Add_Option((char*)"-rol");
		} else {
			Add_Option((char*)(screenRot ? "-ror" : "-mouse"));
		}
	}

	Add_Option((char*)("-rompath"));
		
#ifdef WANT_MAME
   	sprintf(tmp_dir, "%s", MgamePath);
   	Add_Option((char*)(tmp_dir));		   
   	if(!boot_to_osd_enabled)
   		Add_Option(MgameName);
  
#else
   	if(!commandline_enabled)
   	{
		if(!boot_to_osd_enabled)
	   	{
			sprintf(tmp_dir, "%s", MgamePath);
			Add_Option((char*)(tmp_dir));		   
		  	if(softlist_enabled)
		  	{
				if(!arcade)
				{
					Add_Option(MsystemName);   
					if(!boot_to_bios_enabled)
					{
						if(!softlist_auto)
					  		Add_Option((char*)mediaType);
				   		Add_Option((char*)MgameName);
					}
			 	}
			 	else
			 	{
					Add_Option((char*)MgameName);
			 	}	     
		  	}
		  	else
		  	{
			 	if (strcmp(mediaType, "-rom") == 0) {
					Add_Option(MgameName);
			 	} else {
					Add_Option(MsystemName);
					Add_Option((char*)mediaType);
					Add_Option((char*)gameName);
			 	}   
	   
		  	}
		}
		else
		{
			sprintf(tmp_dir, "%s;%s", MgamePath,MparentPath);
			Add_Option((char*)(tmp_dir));		   	
		}
	}
	else	
		Add_Option((char*)gameName);	
 	
#endif 	 	 
	
	return 0;
} 

