#include "retroosd.h"
#include "../../emu/drawgfx.h"

#ifndef M16B
#define PIXEL_TYPE UINT32
#else
#define PIXEL_TYPE UINT16
#endif

//FIX ME DO CLEAN EXIT
//============================================================
//  constructor
//============================================================

retro_osd_interface::retro_osd_interface()
{
}


//============================================================
//  destructor
//============================================================

retro_osd_interface::~retro_osd_interface()
{
}

void retro_osd_interface::osd_exit(running_machine &machine)
{	
	write_log("osd_exit called \n");

	global_free(Pad_device[0]);
	global_free(Pad_device[1]);
	global_free(joy_device[0]);
	global_free(joy_device[1]);
	global_free(retrokbd_device);
	global_free(mouse_device);
}

void retro_osd_interface::init(running_machine &machine)
{
	int gamRot=0;

	osd_interface::init(machine);
	our_target = machine.render().target_alloc();

	initInput(machine);

	write_log("machine screen orientation: %s \n", (
		machine.system().flags & ORIENTATION_SWAP_XY) ? "VERTICAL" : "HORIZONTAL"
	);

        orient  = (machine.system().flags & ORIENTATION_MASK);
	vertical = (machine.system().flags & ORIENTATION_SWAP_XY);
        
        gamRot = (ROT270 == orient) ? 1 : gamRot;
        gamRot = (ROT180 == orient) ? 2 : gamRot;
        gamRot = (ROT90  == orient) ? 3 : gamRot;
        
	//prep_retro_rotation(gamRot);
	our_target->compute_minimum_size(rtwi, rthe);
	topw=rtwi;
	//Equivalent to rtaspect=our_target->view_by_index((our_target->view()))->effective_aspect(render_layer_config layer_config())
	int width,height;
	our_target->compute_visible_area(1000,1000,1,ROT0,width,height);
	rtaspect=(float)width/(float)height;
	write_log("W:%d H:%d , aspect ratio %d/%d=%f\n",rtwi,rthe,width,height,rtaspect);
	NEWGAME_FROM_OSD=1;
	write_log("osd init done\n");
	co_switch(mainThread);
}

bool draw_this_frame;

void retro_osd_interface::update(bool skip_redraw)
{
	//const render_primitive_list *primlist;
	UINT8 *surfptr;

	if(pauseg==-1){
		machine().schedule_exit();
		return;
	}

	if (FirstTimeUpdate == 1)
		skip_redraw = 0; //force redraw to make sure the video texture is created

   if (!skip_redraw)
   {

      draw_this_frame = true;
      // get the minimum width/height for the current layout
      int minwidth, minheight;

	if(videoapproach1_enable==false){	     
		our_target->compute_minimum_size(minwidth, minheight);
	}
	else{
     		 minwidth=1600;minheight=1200;
        }

      if (FirstTimeUpdate == 1) {

         FirstTimeUpdate++;			
         write_log("game screen w=%i h=%i  rowPixels=%i\n", minwidth, minheight,minwidth );

         rtwi=minwidth;
         rthe=minheight;
         topw=minwidth;			

         int gamRot=0;
         orient  = (machine().system().flags & ORIENTATION_MASK);
         vertical = (machine().system().flags & ORIENTATION_SWAP_XY);

         gamRot = (ROT270 == orient) ? 1 : gamRot;
         gamRot = (ROT180 == orient) ? 2 : gamRot;
         gamRot = (ROT90  == orient) ? 3 : gamRot;

         //prep_retro_rotation(gamRot);

      }

      if (minwidth != rtwi || minheight != rthe || minwidth != topw ){
         write_log("Res change: old(%d,%d) new(%d,%d) %d\n",rtwi,rthe,minwidth,minheight,topw);
         rtwi=minwidth;
         rthe=minheight;
         topw=minwidth;
      }

      if(videoapproach1_enable){
		rtwi=topw=1600;
		rthe=1200;
      }

      // make that the size of our target
      our_target->set_bounds(rtwi,rthe);
      // get the list of primitives for the target at the current size
      render_primitive_list &primlist = our_target->get_primitives();

      // lock them, and then render them
      primlist.acquire_lock();

      surfptr = (UINT8 *) videoBuffer;

#ifdef M16B
software_renderer<UINT16, 3,2,3, 11,5,0>::draw_primitives(primlist, surfptr, minwidth, minheight,minwidth );
#else
software_renderer<UINT32, 0,0,0, 16,8,0>::draw_primitives(primlist, surfptr, minwidth, minheight,minwidth );
#endif
      

      primlist.release_lock();
   } 
	else
    		draw_this_frame = false;

	if(ui_ipt_pushchar!=-1){
		ui_input_push_char_event(machine(), our_target, (unicode_char)ui_ipt_pushchar);
		ui_ipt_pushchar=-1;
	}

   co_switch(mainThread);
}  
 
 
//============================================================
//  update_audio_stream
//============================================================
void retro_osd_interface::update_audio_stream(const INT16 *buffer, int samples_this_frame) 
{
	if(pauseg!=-1)audio_batch_cb(buffer, samples_this_frame);
}
  

//============================================================
//  set_mastervolume
//============================================================
void retro_osd_interface::set_mastervolume(int attenuation)
{
	// if we had actual sound output, we would adjust the global
	// volume in response to this function
}


//============================================================
//  customize_input_type_list
//============================================================
void retro_osd_interface::customize_input_type_list(simple_list<input_type_entry> &typelist)
{
	// This function is called on startup, before reading the
	// configuration from disk. Scan the list, and change the
	// default control mappings you want. It is quite possible
	// you won't need to change a thing.
}
 
