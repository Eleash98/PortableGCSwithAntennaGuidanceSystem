#ifndef CSP_XTEA_H_INCLUDED
#define CSP_XTEA_H_INCLUDED
#include "STD.h"
 #define  NUM_ROUNDS  32


void CSP_VOID_XTEA_ENCRYPTION(u8 num_rounds, u8* data,u32* key, u16 frame_length);

void CSP_VOID_XTEA_DECRYPTION(u8 num_rounds, u8* data,u32* key, u16 frame_length);


#endif // CSP_XTEA_H_INCLUDED
