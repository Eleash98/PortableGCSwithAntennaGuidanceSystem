#ifndef GOLAY_ENCODING_H_INCLUDED
#define GOLAY_ENCODING_H_INCLUDED

#include "STD_TYPES.h"
#include"BIT_MATH.h"

#define GOLAY_POLY1  0XAE3;
#define GOLAY_POLY2  0XC75;

void set_polynomial(u16 poly);
u16* Golay_voidDivide(u8 * arr, u32 len, u32* GolayArrayLen );
u8* Golay_u8pointerEncode(u8* data,u32*dataLength);
u32 calc_golay_codeword(u16 data);
u32 reverseBits(u32 num,u8 NO_OF_BITS);
void print_number_bits(u32 num, u16 bitsnum);
u8 codeword_weight(u32 cw);
u8 parity_bit(u32 cw);


#endif // GOLAY_ENCODING_H_INCLUDED
