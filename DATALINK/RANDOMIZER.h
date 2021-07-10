#ifndef RANDOMIZER_H
#define RANDOMIZER_H
#include "STD_TYPES.h"

/*the terms of the polynomial to calculate the randomization sequence*/
void CCSCS_void_Sequence_Generaeor(u32 length);
void CCSDS_void_Randomizer(u8* Frame,u32 length);
void CCSDS_void_Set_polynomial(u8 polynomial);
#endif
