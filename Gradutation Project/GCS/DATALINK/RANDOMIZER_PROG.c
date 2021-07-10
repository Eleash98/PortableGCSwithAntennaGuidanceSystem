#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "BIT_MATH.h"
#include "STD_TYPES.h"
#include "RANDOMIZER.h"
u8* Sequence;
u8 CCSDS_polynomial_X0=1;
u8 CCSDS_polynomial_X1=0;
u8 CCSDS_polynomial_X2=0;
u8 CCSDS_polynomial_X3=1;
u8 CCSDS_polynomial_X4=0;
u8 CCSDS_polynomial_X5=1;
u8 CCSDS_polynomial_X6=0;
u8 CCSDS_polynomial_X7=1;
void CCSCS_void_Sequence_Generaeor(u32 length)
{
    Sequence = (u8*)malloc(length);
    /*initiate Sequence to be all zeros*/
    memset(Sequence, 0, length);
    u32 i=0;
    u8 shift_register=0xFF;
    u8 trmporary_register;
    for (i=0;i<length*8;i++)
    {
        Sequence[i/8]|=((shift_register)&1)<<(7-i%8);
        trmporary_register=shift_register>>1;
        trmporary_register=    (((((shift_register&1)&CCSDS_polynomial_X0)
                            ^(((shift_register>>1)&1)&CCSDS_polynomial_X1)
                            ^(((shift_register>>2)&1)&CCSDS_polynomial_X2)
                            ^(((shift_register>>3)&1)&CCSDS_polynomial_X3)
                            ^(((shift_register>>4)&1)&CCSDS_polynomial_X4)
                            ^(((shift_register>>5)&1)&CCSDS_polynomial_X5)
                            ^(((shift_register>>6)&1)&CCSDS_polynomial_X6)
                            ^(((shift_register>>7)&1)&CCSDS_polynomial_X7))<<7)&0x80)|(trmporary_register&0x7f);
        shift_register=trmporary_register;
    }
}
void CCSDS_void_Randomizer(u8* Frame,u32 length)
{
    CCSCS_void_Sequence_Generaeor(length);
    u32 i=0;
    for (i=0;i<length;i++)
    {
        Frame[i]^=Sequence[i];
    }
    free(Sequence);
}
void CCSDS_void_Set_polynomial(u8 polynomial)
{
    CCSDS_polynomial_X0=GET_BIT(polynomial,0);
    CCSDS_polynomial_X1=GET_BIT(polynomial,1);
    CCSDS_polynomial_X2=GET_BIT(polynomial,2);
    CCSDS_polynomial_X3=GET_BIT(polynomial,3);
    CCSDS_polynomial_X4=GET_BIT(polynomial,4);
    CCSDS_polynomial_X5=GET_BIT(polynomial,5);
    CCSDS_polynomial_X6=GET_BIT(polynomial,6);
    CCSDS_polynomial_X7=GET_BIT(polynomial,7);
}
