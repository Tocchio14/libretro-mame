void retro_poll_mame_input();

static int rtwi=320,rthe=240,topw=1600; // DEFAULT TEXW/TEXH/PITCH
static float rtaspect=0;
int SHIFTON=-1,NEWGAME_FROM_OSD=0;
char RPATH[512];

const char *retro_save_directory;
const char *retro_system_directory;
const char *retro_content_directory;

extern "C" int mmain(int argc, const char *argv);
extern bool draw_this_frame;

#ifdef M16B
	uint16_t videoBuffer[1600*1200];
	#define LOG_PIXEL_BYTES 1
#else
	unsigned int videoBuffer[1600*1200];
	#define LOG_PIXEL_BYTES 2*1
#endif 

retro_video_refresh_t video_cb = NULL;
retro_environment_t environ_cb = NULL;

static retro_input_state_t input_state_cb = NULL;
/*static*/ retro_audio_sample_batch_t audio_batch_cb = NULL;

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) { audio_batch_cb = cb; }
static retro_input_poll_t input_poll_cb;

void retro_set_input_state(retro_input_state_t cb) { input_state_cb = cb; }
void retro_set_input_poll(retro_input_poll_t cb) { input_poll_cb = cb; }

void retro_set_video_refresh(retro_video_refresh_t cb) { video_cb = cb; }
void retro_set_audio_sample(retro_audio_sample_t cb) { }

void retro_set_environment(retro_environment_t cb)
{
   static const struct retro_variable vars[] = {
	
	//common for MAME/MESS/UME
	{ "core_current_mouse_enable", "Enable mouse; disabled|enabled" },
	{ "core_current_nagscreenpatch_enable", "Enable nagscreen patch; disabled|enabled" },      
	{ "core_current_videoapproach1_enable", "Enable video approach 1; disabled|enabled" },

	// ONLY FOR MESS/UME
#if !defined(WANT_MAME)
        { "core_softlist_enable", "Enable softlists; enabled|disabled" },
	{ "core_softlist_auto", "Softlist automatic media type; enabled|disabled" },
	{ "core_media_type", "Media type; cart|flop|cdrm|cass|hard|serl|prin" },   	  
	{ "core_boot_bios", "Boot to BIOS; disabled|enabled" },
	{ "core_commandline", "Boot from CLI; disabled|enabled" },
#endif
	{ "core_boot_osd", "Boot to OSD; disabled|enabled" },
	{ "core_exp_commandline", "Experimental CLI; disabled|enabled" },
	{ NULL, NULL },

   };

   environ_cb = cb;

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

static void check_variables(void)
{
   struct retro_variable var = {0};

   var.key = "core_exp_commandline";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "enabled") == 0)
         experimental_cmdline = true;
      if (strcmp(var.value, "disabled") == 0)
         experimental_cmdline = false;       
   }  
 
   var.key = "core_current_mouse_enable";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "disabled") == 0)
         mouse_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         mouse_enable = true;
   }

   var.key = "core_current_nagscreenpatch_enable";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "disabled") == 0)
         nagscreenpatch_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         nagscreenpatch_enable = true;
   }

   var.key = "core_current_videoapproach1_enable";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "disabled") == 0)
         videoapproach1_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         videoapproach1_enable = true;
   }

   var.key = "core_boot_osd";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "enabled") == 0)
         boot_to_osd_enabled = true;
      if (strcmp(var.value, "disabled") == 0)
         boot_to_osd_enabled = false;       
   }

#if !defined(WANT_MAME)

   var.key = "core_media_type";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      sprintf(mediaType,"-%s",var.value);
      fprintf(stderr, "value: %s\n", mediaType);
   }
   
   var.key = "core_softlist_enable";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "enabled") == 0)
         softlist_enabled = true;
      if (strcmp(var.value, "disabled") == 0)
         softlist_enabled = false;       
   }      
   
   var.key = "core_softlist_auto";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "enabled") == 0)
         softlist_auto = true;
      if (strcmp(var.value, "disabled") == 0)
         softlist_auto = false;       
   }       

   var.key = "core_boot_bios";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "enabled") == 0)
         boot_to_bios_enabled = true;
      if (strcmp(var.value, "disabled") == 0)
         boot_to_bios_enabled = false;       
   } 
 
   var.key = "core_commandline";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      fprintf(stderr, "value: %s\n", var.value);
      if (strcmp(var.value, "enabled") == 0)
         commandline_enabled = true;
      if (strcmp(var.value, "disabled") == 0)
         commandline_enabled = false;       
   }   
    
#endif   
  
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_get_system_info(struct retro_system_info *info)
{   	
   memset(info, 0, sizeof(*info));

#if defined(WANT_MAME)
   info->library_name = "MAME 2014";
#elif defined(WANT_MESS)
   info->library_name = "MESS 2014";
#elif defined(WANT_UME)
   info->library_name = "UME 2014";
#else
   info->library_name = "N/D";
#endif 

   info->library_version = "Git";
   info->valid_extensions = "zip|chd|7z";
   info->need_fullpath = true;   
   info->block_extract = true;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->geometry.base_width = rtwi;
   info->geometry.base_height =rthe;

   info->geometry.max_width = 1600;
   info->geometry.max_height = 1200;

   info->geometry.aspect_ratio =rtaspect;
   info->timing.fps = 60;
   info->timing.sample_rate = 48000.0;
}

static void retro_wrap_emulator()
{    
    mmain(1,RPATH);

    pauseg=-1;

    environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, 0); 

    // Were done here
    co_switch(mainThread);
        
    // Dead emulator, but libco says not to return
    while(true)
    {
        LOGI("Running a dead emulator.");
        co_switch(mainThread);
    }
}

void retro_init (void){ 

#ifndef M16B
    	enum retro_pixel_format fmt =RETRO_PIXEL_FORMAT_XRGB8888;
#else
    	enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_RGB565;
#endif

		const char *system_dir = NULL;
   
		if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &system_dir) && system_dir)
		{
			// if defined, use the system directory			
			retro_system_directory=system_dir;		
		}		   
		
		const char *content_dir = NULL;
   
		if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
		{
			// if defined, use the system directory			
			retro_content_directory=content_dir;		
		}			
		
		const char *save_dir = NULL;
   
		if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &save_dir) && save_dir)
		{
			// If save directory is defined use it, otherwise use system directory
			retro_save_directory = *save_dir ? save_dir : retro_system_directory;      
		}
		else
		{
			// make retro_save_directory the same in case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY is not implemented by the frontend
			retro_save_directory=retro_system_directory;
		}
		
		printf("Retro SYSTEM_DIRECTORY %s\n",retro_system_directory);
		printf("Retro SAVE_DIRECTORY %s\n",retro_save_directory);
		printf("Retro CONTENT_DIRECTORY %s\n",retro_content_directory);


    	if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
    	{
    		fprintf(stderr, "RGB pixel format is not supported.\n");
    		exit(0);
    	}

	if(!emuThread && !mainThread)
    	{
        	mainThread = co_active();
        	emuThread = co_create(65536*sizeof(void*), retro_wrap_emulator);
    	}

}

void retro_deinit(void)
{
   if(emuThread)
   { 
      co_delete(emuThread);
      emuThread = 0;
   }

   LOGI("Retro DeInit\n");
}

void retro_reset (void)
{
   mame_reset = 1;
}



void retro_run (void)
{
   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();

   if(NEWGAME_FROM_OSD==1)
   {
      struct retro_system_av_info ninfo;
      retro_get_system_av_info(&ninfo);

      environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &ninfo);

      printf("ChangeAV: w:%d h:%d ra:%f %f \n",ninfo.geometry.base_width,ninfo.geometry.base_height,ninfo.geometry.aspect_ratio);
      NEWGAME_FROM_OSD=0;
   }

	retro_poll_mame_input();

	if (draw_this_frame)
      		video_cb(videoBuffer,rtwi, rthe, topw << LOG_PIXEL_BYTES);
   	else
      		video_cb(NULL,rtwi, rthe, topw << LOG_PIXEL_BYTES);

	co_switch(emuThread);
}

void prep_retro_rotation(int rot)
{
   LOGI("Rotation:%d\n",rot);
   environ_cb(RETRO_ENVIRONMENT_SET_ROTATION, &rot);
}

bool retro_load_game(const struct retro_game_info *info) 
{
	check_variables();

#ifdef M16B
	memset(videoBuffer,0,1600*1200*2);
#else
	memset(videoBuffer,0,1600*1200*2*2);
#endif
	char basename[256];
	
	extract_basename(basename, info->path, sizeof(basename));
  	extract_directory(g_rom_dir, info->path, sizeof(g_rom_dir));
	strcpy(RPATH,info->path);

	co_switch(emuThread);
	return 1;
}

void retro_unload_game(void)
{
	if(pauseg==0)
   	{
		pauseg=-1;				
      		co_switch(emuThread);
	}

	LOGI("Retro unload_game\n");	
}

// Stubs
size_t retro_serialize_size(void){ return 0; }
bool retro_serialize(void *data, size_t size){ return false; }
bool retro_unserialize(const void * data, size_t size){ return false; }

unsigned retro_get_region (void) {return RETRO_REGION_NTSC;}
void *retro_get_memory_data(unsigned type) {return 0;}
size_t retro_get_memory_size(unsigned type) {return 0;}
bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info){return false;}
void retro_cheat_reset(void){}
void retro_cheat_set(unsigned unused, bool unused1, const char* unused2){}
void retro_set_controller_port_device(unsigned in_port, unsigned device){}

