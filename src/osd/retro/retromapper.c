void retro_poll_mame_input();

static int rtwi=320,rthe=240,topw=1600; // DEFAULT TEXW/TEXH/PITCH
static float rtaspect=0;
static int max_width=0;
static int max_height=0;

static char option_mouse[50];     
static char option_cheats[50];
static char option_nag[50];
static char option_info[50];
static char option_renderer[50];
static char option_warnings[50];
static char option_osd[50];
static char option_cli[50];
static char option_bios[50];
static char option_softlist[50];
static char option_softlist_media[50];
static char option_media[50];
static char option_read_config[50];     
static char option_write_config[50];
static char option_auto_save[50];
static char option_throttle[50];
static char option_nobuffer[50];
static char option_saves[50];

int SHIFTON=-1,NEWGAME_FROM_OSD=0;
char RPATH[512];

const char *retro_save_directory;
const char *retro_system_directory;
const char *retro_content_directory;

retro_log_printf_t log_cb;

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
   sprintf(option_mouse,"%s_%s",core,"mouse_enable");
   sprintf(option_cheats,"%s_%s",core,"cheats_enable");
   sprintf(option_nag,"%s_%s",core,"hide_nagscreen");
   sprintf(option_info,"%s_%s",core,"hide_infoscreen");
   sprintf(option_warnings,"%s_%s",core,"hide_warnings");
   sprintf(option_renderer,"%s_%s",core,"alternate_renderer");
   sprintf(option_osd,"%s_%s",core,"boot_to_osd");
   sprintf(option_bios,"%s_%s",core,"boot_to_bios");
   sprintf(option_cli,"%s_%s",core,"boot_from_cli");
   sprintf(option_softlist,"%s_%s",core,"softlists_enable");
   sprintf(option_softlist_media,"%s_%s",core,"softlists_auto_media");
   sprintf(option_media,"%s_%s",core,"media_type");
   sprintf(option_read_config,"%s_%s",core,"read_config");
   sprintf(option_write_config,"%s_%s",core,"write_config");
   sprintf(option_auto_save,"%s_%s",core,"auto_save");
   sprintf(option_saves,"%s_%s",core,"saves");
   sprintf(option_throttle,"%s_%s",core,"throttle");
   sprintf(option_nobuffer,"%s_%s",core,"nobuffer");
      
   static const struct retro_variable vars[] = {
    //some ifdefs are redundant but I wanted to have these options in a logical order
    //common for MAME/MESS/UME	

    { option_read_config, "Read configuration; disabled|enabled" },

    // ONLY FOR MESS/UME
#if !defined(WANT_MAME)
    { option_write_config, "Write configuration; disabled|enabled" },
    { option_saves, "Save state naming; game|system" },
#endif

    //common for MAME/MESS/UME	
    { option_auto_save, "Auto save/load states; disabled|enabled" },
    { option_mouse, "Enable in-game mouse; disabled|enabled" },
    { option_throttle, "Enable throttle; disabled|enabled" },
    { option_cheats, "Enable cheats; disabled|enabled" },
    { option_nobuffer, "Nobuffer patch; disabled|enabled" },	
    { option_nag, "Hide nag screen; disabled|enabled" },
    { option_info, "Hide gameinfo screen; disabled|enabled" },
    { option_warnings, "Hide warnings screen; disabled|enabled" },
    { option_renderer, "Alternate render method; disabled|enabled" },

    // ONLY FOR MESS/UME
#if !defined(WANT_MAME) 
    { option_softlist, "Enable softlists; enabled|disabled" },
    { option_softlist_media, "Softlist automatic media type; enabled|disabled" },
#if defined(WANT_MESS)
    { option_media, "Media type; cart|flop|cdrm|cass|hard|serl|prin" },
#elif defined(WANT_UME)
    { option_media, "Media type; rom|cart|flop|cdrm|cass|hard|serl|prin" },
#endif
    { option_bios, "Boot to BIOS; disabled|enabled" },
#endif

    //common for MAME/MESS/UME	
    { option_osd, "Boot to OSD; disabled|enabled" },
    { option_cli, "Boot from CLI; disabled|enabled" },
    { NULL, NULL },

   };

   environ_cb = cb;

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, (void*)vars);
}

static void check_variables(void)
{
   struct retro_variable var = {0};

   var.key = option_cli;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
         experimental_cmdline = true;
      if (strcmp(var.value, "disabled") == 0)
         experimental_cmdline = false;       
   }  
   
   var.key = option_mouse;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         mouse_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         mouse_enable = true;
   }
   
   var.key = option_throttle;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         throttle_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         throttle_enable = true;
   }   
   
   var.key = option_nobuffer;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         nobuffer_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         nobuffer_enable = true;
   }   
   
   var.key = option_cheats;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         cheats_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         cheats_enable = true;
   }   

   var.key = option_nag;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         hide_nagscreen = false;
      if (strcmp(var.value, "enabled") == 0)
         hide_nagscreen = true;
   }

   var.key = option_info;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         hide_gameinfo = false;
      if (strcmp(var.value, "enabled") == 0)
         hide_gameinfo = true;
   }   
   
   var.key = option_warnings;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         hide_warnings = false;
      if (strcmp(var.value, "enabled") == 0)
         hide_warnings = true;
   }      
   
   var.key = option_renderer;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
      {
         alternate_renderer = false;	 
      }
      if (strcmp(var.value, "enabled") == 0)
      {
         alternate_renderer = true;
      }
   }

   var.key = option_osd;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
         boot_to_osd_enable = true;
      if (strcmp(var.value, "disabled") == 0)
         boot_to_osd_enable = false;       
   }
   
   var.key = option_read_config;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         read_config_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         read_config_enable = true;
   }   

   var.key = option_auto_save;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         auto_save_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         auto_save_enable = true;
   }
   
   var.key = option_saves;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "game") == 0)
         game_specific_saves_enable = true;
      if (strcmp(var.value, "system") == 0)
         game_specific_saves_enable = false;
   }         

#if !defined(WANT_MAME)

   var.key = option_media;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      sprintf(mediaType,"-%s",var.value);
   }
   
   var.key = option_softlist;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
         softlist_enable = true;
      if (strcmp(var.value, "disabled") == 0)
         softlist_enable = false;       
   }      
   
   var.key = option_softlist_media;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
         softlist_auto = true;
      if (strcmp(var.value, "disabled") == 0)
         softlist_auto = false;       
   }       

   var.key = option_bios;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "enabled") == 0)
         boot_to_bios_enable = true;
      if (strcmp(var.value, "disabled") == 0)
         boot_to_bios_enable = false;       
   } 
   
   var.key = option_write_config;
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      if (strcmp(var.value, "disabled") == 0)
         write_config_enable = false;
      if (strcmp(var.value, "enabled") == 0)
         write_config_enable = true;
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
   check_variables();
   
   info->geometry.base_width = rtwi;
   info->geometry.base_height = rthe;
   
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: width=%d height=%d\n",info->geometry.base_width,info->geometry.base_height);
      
   info->geometry.max_width = 1600;
   info->geometry.max_height = 1200;
   
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: max_width=%d max_height=%d\n",info->geometry.max_width,info->geometry.max_height);   

   info->geometry.aspect_ratio = rtaspect;
   
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: aspect_ratio=%f\n",info->geometry.aspect_ratio);	  
   
   info->timing.fps = 60;
   info->timing.sample_rate = 48000.0;
   
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "AV_INFO: fps=%f sample_rate=%f\n",info->timing.fps,info->timing.sample_rate);	  
   
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

       // initialize logger interface
       struct retro_log_callback log;
       if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
           log_cb = log.log;
       else 
           log_cb = NULL;

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
       if (log_cb)
           log_cb(RETRO_LOG_INFO, "SYSTEM_DIRECTORY: %s", retro_system_directory);
       
       const char *content_dir = NULL;
   
       if (environ_cb(RETRO_ENVIRONMENT_GET_CONTENT_DIRECTORY, &content_dir) && content_dir)
       {
           // if defined, use the system directory
           retro_content_directory=content_dir;
       }
       if (log_cb)
           log_cb(RETRO_LOG_INFO, "CONTENT_DIRECTORY: %s", retro_content_directory);
       
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
       if (log_cb)
           log_cb(RETRO_LOG_INFO, "SAVE_DIRECTORY: %s", retro_save_directory);
       
        if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
        {
           if (log_cb)
              log_cb(RETRO_LOG_ERROR, "pixel format not supported");
           exit(0);
        }

        if(!emuThread && !mainThread)
        {
            mainThread = co_active();
            emuThread = co_create(65536*sizeof(void*), retro_wrap_emulator);
        }
        //sprintf(retro_system_directory,"%s%c",retro_system_directory,slash);
        //sprintf(retro_save_directory,"%s%c",retro_system_directory,slash);

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

