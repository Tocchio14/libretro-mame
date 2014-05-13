/***************************************************************************

	Amiga Zorro Cards

***************************************************************************/

#include "cards.h"

SLOT_INTERFACE_START( a1000_expansion_cards )
SLOT_INTERFACE_END

SLOT_INTERFACE_START( a500_expansion_cards )
	SLOT_INTERFACE("ar1", ACTION_REPLAY_MK1)
	SLOT_INTERFACE("ar2", ACTION_REPLAY_MK2)
	SLOT_INTERFACE("ar3", ACTION_REPLAY_MK3)
	SLOT_INTERFACE("a590", A590)
SLOT_INTERFACE_END

SLOT_INTERFACE_START( a2000_expansion_cards )
	SLOT_INTERFACE("ar1", ACTION_REPLAY_MK1)
	SLOT_INTERFACE("ar2", ACTION_REPLAY_MK2)
	SLOT_INTERFACE("ar3", ACTION_REPLAY_MK3)
SLOT_INTERFACE_END

SLOT_INTERFACE_START( zorro2_cards )
	SLOT_INTERFACE("a2091", A2091)
SLOT_INTERFACE_END

SLOT_INTERFACE_START( zorro3_cards )
	SLOT_INTERFACE("a2091", A2091)
SLOT_INTERFACE_END
