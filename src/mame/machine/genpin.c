/*********************************************************************************

This is for common pinball machine coding.

**********************************************************************************/



#include "genpin.h"

MACHINE_CONFIG_FRAGMENT( genpin_audio )
	MCFG_SPEAKER_STANDARD_MONO("mechmono")
	MCFG_SOUND_ADD("samples", SAMPLES, 0)
	MCFG_SAMPLES_CHANNELS(6)
	MCFG_SAMPLES_NAMES(genpin_sample_names)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mechmono", 1.0)
MACHINE_CONFIG_END
