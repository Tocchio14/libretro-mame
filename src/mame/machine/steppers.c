///////////////////////////////////////////////////////////////////////////
//                                                                       //
// steppers.c steppermotor emulation                                     //
//                                                                       //
// Emulates : Stepper motors driven with full step or half step          //
//            also emulates the index optic                              //
//                                                                       //
// 04-04-2011: J. Wallace - Added reverse spin (this is necessary for    //
//                          accuracy), and improved wraparound logic     //
//    03-2011:              New 2D array to remove reel bounce and       //
//                          make more realistic                          //
// 26-01-2007: J. Wallace - Rewritten to make it more flexible           //
//                          and to allow indices to be set in drivers    //
// 29-12-2006: J. Wallace - Added state save support                     //
// 05-03-2004: Re-Animator                                               //
//                                                                       //
// TODO:  add further different types of stepper motors if needed        //
//        Someone who understands the device system may want to convert  //
//        this                                                           //
///////////////////////////////////////////////////////////////////////////

#include "emu.h"
#include "steppers.h"

/* local prototypes */

static void update_optic(int which);

/* local vars */

typedef struct _stepper
{
	const stepper_interface *intf;
	UINT8	 pattern,	/* coil pattern */
		 old_pattern,	/* old coil pattern */
		       phase,	/* motor phase */
		   old_phase,	/* old phase */
			    type,	/* reel type */
		  stator1[2],
		  stator2[2],
			 reverse;	/* Does reel spin backwards (construction of unit, not wiring) */
	INT16	step_pos,	/* step position 0 - max_steps */
			max_steps;	/* maximum step position */

	INT16 index_start,	/* start position of index (in half steps) */
			index_end,	/* end position of index (in half steps) */
			index_patt;	/* pattern needed on coils (0=don't care) */

	UINT8 optic;
} stepper;

static stepper step[MAX_STEPPERS];

static const int StarpointStepTab[8][16] =
{//   0000  0001  0010  0011  0100  0101  0110  0111  1000  1001  1010  1011  1100  1101  1110  1111    Phase
	{ 0,    2,    0,    0,    2,    1,    3,    0,   -2,   -1,   -1,   0,    0,    0,    0,    0   },// 0
	{ 0,   -1,    3,    0,    1,    2,    2,    0,   -3,   -2,   -2,   0,    0,    0,    0,    0   },// 1
	{ 0,   -2,    2,    0,    2,   -1,    1,    0,    2,   -3,    3,   0,    0,    0,    0,    0   },// 2
	{ 0,   -1,    1,    0,   -1,   -2,   -2,    0,    3,    2,    2,   0,    0,    0,    0,    0   },// 3
	{ 0,   -2,    0,    0,   -2,   -3,   -1,    0,    2,    3,    1,   0,    0,    0,    0,    0   },// 4
	{ 0,    3,   -1,    0,   -1,   -2,   -2,    0,    1,    0,    2,   0,    0,    0,    0,    0   },// 5
	{ 0,    2,   -2,    0,   -2,    3,   -3,    0,   -2,    1,   -1,   0,    0,    0,    0,    0   },// 6
	{ 0,    1,   -3,    0,    3,    2,    2,    0,   -1,   -2,   -2,   0,    0,    0,    0,    0   },// 7
};

/* useful interfaces (Starpoint is a very common setup)*/
/* step table, use active coils as row, phase as column*/
const stepper_interface starpoint_interface_48step =
{
	STARPOINT_48STEP_REEL,
	16,
	24,
	0x09
};

const stepper_interface starpoint_interface_48step_reverse =
{
	STARPOINT_48STEP_REEL,
	16,
	24,
	0x09,
	1
};

///////////////////////////////////////////////////////////////////////////
void stepper_config(running_machine &machine, int which, const stepper_interface *intf)
{
	assert_always(machine.phase() == MACHINE_PHASE_INIT, "Can only call stepper_config at init time!");
	assert_always((which >= 0) && (which < MAX_STEPPERS), "stepper_config called on an invalid stepper motor!");
	assert_always(intf, "stepper_config called with an invalid interface!");

	step[which].intf = intf;

	step[which].type = intf->type;
	step[which].index_start = intf->index_start;/* location of first index value in half steps */
	step[which].index_end	= intf->index_end;	/* location of last index value in half steps */
	step[which].index_patt	= intf->index_patt; /* hex value of coil pattern (0 if not needed)*/
	step[which].reverse     = intf->reverse;
	step[which].phase       = 0;
	step[which].pattern     = 0;
	step[which].old_pattern = 0;
	step[which].step_pos    = 0;


	switch ( step[which].type )
	{	default:
		case STARPOINT_48STEP_REEL:  /* STARPOINT RMxxx */
		case BARCREST_48STEP_REEL :  /* Barcrest Reel unit */
		case MPU3_48STEP_REEL :
		step[which].max_steps = (48*2);
		break;
		case STARPOINT_144STEPS_DICE :/* STARPOINT 1DCU DICE mechanism */
		step[which].max_steps = (144*2);
		break;
	}

	state_save_register_item(machine, "stepper", NULL, which, step[which].index_start);
	state_save_register_item(machine, "stepper", NULL, which, step[which].index_end);
	state_save_register_item(machine, "stepper", NULL, which, step[which].index_patt);
	state_save_register_item(machine, "stepper", NULL, which, step[which].phase);
	state_save_register_item(machine, "stepper", NULL, which, step[which].old_phase);
	state_save_register_item(machine, "stepper", NULL, which, step[which].pattern);
	state_save_register_item(machine, "stepper", NULL, which, step[which].stator1[0]);
	state_save_register_item(machine, "stepper", NULL, which, step[which].stator1[1]);
	state_save_register_item(machine, "stepper", NULL, which, step[which].stator2[0]);
	state_save_register_item(machine, "stepper", NULL, which, step[which].stator2[1]);
	state_save_register_item(machine, "stepper", NULL, which, step[which].old_pattern);
	state_save_register_item(machine, "stepper", NULL, which, step[which].step_pos);
	state_save_register_item(machine, "stepper", NULL, which, step[which].max_steps);
	state_save_register_item(machine, "stepper", NULL, which, step[which].type);
	state_save_register_item(machine, "stepper", NULL, which, step[which].reverse);
}

///////////////////////////////////////////////////////////////////////////
int stepper_get_position(int which)
{
	return step[which].step_pos;
}

///////////////////////////////////////////////////////////////////////////

int stepper_get_max(int which)
{
	return step[which].max_steps;
}

///////////////////////////////////////////////////////////////////////////

static void update_optic(int which)
{
	int pos   = step[which].step_pos,
		start = step[which].index_start,
		end = step[which].index_end;

	if (start > end) // cope with index patterns that wrap around
	{
		if ( (( pos > start ) || ( pos < end )) &&
		( ( step[which].pattern == step[which].index_patt || step[which].index_patt==0) ||
		( step[which].pattern == 0 &&
		(step[which].old_pattern == step[which].index_patt || step[which].index_patt==0)
		) ) )
		{
			step[which].optic = 1;
		}
		else step[which].optic = 0;
		}
	else
	{
		if ( (( pos > start ) && ( pos < end )) &&
		( ( step[which].pattern == step[which].index_patt || step[which].index_patt==0) ||
		( step[which].pattern == 0 &&
		(step[which].old_pattern == step[which].index_patt || step[which].index_patt==0)
		) ) )
		{
		step[which].optic = 1;
		}
		else step[which].optic = 0;
	}
}
///////////////////////////////////////////////////////////////////////////

void stepper_reset_position(int which)
{
	step[which].step_pos    = 0;
	step[which].pattern     = 0x00;
	step[which].old_pattern = 0x00;
	step[which].phase		= 0x00;
	step[which].old_phase	= 0x00;

	update_optic(which);
}

///////////////////////////////////////////////////////////////////////////

int stepper_optic_state(int which)
{
	int result = 0;

	if ( which < MAX_STEPPERS )
	{
		result = step[which].optic;
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////

int stepper_update(int which, UINT8 pattern)
{
	int changed = 0;

	/* This code probably makes more sense if you visualise what is being emulated, namely
	a spinning drum with two electromagnets inside. Essentially, the CPU
	activates a pair of windings on these magnets leads as necessary to attract and repel the drum to pull it round and 
	display as appropriate. To attempt to visualise the rotation effect, take a look at the compass rose below, the numbers
	indicate the phase information as used

	    7
	    N
	1 W   E 5
		S
		3

	For sake of accuracy, we're representing all possible phases of the motor, effectively moving the motor one half step at a time, so a 48 step motor becomes
	96 half steps. This is necessary because of some programs running the wiring in series with a distinct delay between the pair being completed. This causes
	a small movement that may trigger the optic.

	*/

	{
		int pos,steps=0;
		switch ( step[which].type )
		{
			default:
			case STARPOINT_48STEP_REEL :	/* STARPOINT RMxxx */
			case STARPOINT_144STEPS_DICE :  /* STARPOINT 1DCU DICE mechanism */
			if ( step[which].pattern != pattern )
			{
				//NOTE: Eventually we will convert Starpoint to the Stator method below, at which point the table can be removed
				steps = StarpointStepTab[(step[which].step_pos % 8)][pattern];
				step[which].pattern = pattern;
			}
			break;
			case BARCREST_48STEP_REEL :	    /* Barcrest reel units have different windings */
			{
				logerror("step%x pattern %x\n",which,pattern);
				step[which].stator1[0] = (BIT(pattern,0)? 1 : 0);//orange
				step[which].stator1[1] = (BIT(pattern,2)? 1 : 0);//yellow
				step[which].stator2[0] = (BIT(pattern,1)? 1 : 0);//brown
				step[which].stator2[1] = (BIT(pattern,3)? 1 : 0);//black
				if (step[which].stator1[0] && !step[which].stator1[1] && !step[which].stator2[0] && !step[which].stator2[1])
				{
					step[which].phase = 7;
				}

				if (step[which].stator1[0] && !step[which].stator1[1] && step[which].stator2[0] && !step[which].stator2[1])
				{
					step[which].phase = 6;
				}

				if (step[which].stator1[0] && !step[which].stator1[1] && !step[which].stator2[0] && !step[which].stator2[1])
				{
					step[which].phase = 5;
				}

				if (step[which].stator1[0] && !step[which].stator1[1] && step[which].stator2[0] && !step[which].stator2[1])
				{
					step[which].phase = 4;
				}

				if (!step[which].stator1[0] && !step[which].stator1[1] && !step[which].stator2[0] && step[which].stator2[1])
				{
					step[which].phase = 3;
				}

				if (!step[which].stator1[0] && step[which].stator1[1] && !step[which].stator2[0] && step[which].stator2[1])
				{
					step[which].phase = 2;
				}

				if (!step[which].stator1[0] && !step[which].stator1[1] && !step[which].stator2[0] && step[which].stator2[1])
				{
					step[which].phase = 1;
				}

				if (step[which].stator1[0] && !step[which].stator1[1] && !step[which].stator2[0] && step[which].stator2[1])
				{
					step[which].phase = 0;
				}

				if (step[which].stator1[0] && step[which].stator1[1] && !step[which].stator2[0] && !step[which].stator2[1])
				{
					if ((step[which].old_phase ==6)||(step[which].old_phase == 0)) // if the previous pattern had the drum in the northern quadrant, it will point north now
					{
						step[which].phase = 7;
					}
					else //otherwise it will line up due south
					{
						step[which].phase = 3;
					}
				}
				
				if (!step[which].stator1[0] && !step[which].stator1[1] && step[which].stator2[0] && step[which].stator2[1])
				{
					if ((step[which].old_phase ==6)||(step[which].old_phase == 4)) // if the previous pattern had the drum in the eastern quadrant, it will point east now
					{
						step[which].phase = 5;
					}
					else //otherwise it will line up due west
					{
						step[which].phase = 1;
					}
				}
			}	
			break;
			case MPU3_48STEP_REEL :	    /* Same unit as above, but different interface (2 active lines, not 4)*/
			//TODO - set up stators using manual, this seems to be correct based on the previous behaviour
			switch (pattern)
			{
				case 0x00 :
				step[which].phase = 6;
				break;
				case 0x01 :
				step[which].phase = 4;
				break;
				case 0x03 :
				step[which].phase = 2;
				break;
				case 0x02 :
				step[which].phase = 0;
				break;
			}
			break;
		}

		if ((step[which].type == BARCREST_48STEP_REEL) || (step[which].type == MPU3_48STEP_REEL))
		{
			steps = step[which].old_phase - step[which].phase;
		
			if (steps < -4)
			{
				steps = steps +8;
			}
			if (steps > 4)
			{
				steps = steps -8;
			}
		}
		step[which].old_phase = step[which].phase;
		step[which].old_pattern = step[which].pattern;

		int max = step[which].max_steps;
		pos = 0;

		if (max!=0)
		{
			if (step[which].reverse)
			{
				pos = (step[which].step_pos - steps + max) % max;
			}
			else
			{
				pos = (step[which].step_pos + steps + max) % max;
			}
		}
		else
		{
			logerror("step[%x].max_steps == 0\n",which);
		}

		if (pos != step[which].step_pos)
		{
			changed++;
		}

		step[which].step_pos = pos;
		update_optic(which);

	}
	return changed;
}
