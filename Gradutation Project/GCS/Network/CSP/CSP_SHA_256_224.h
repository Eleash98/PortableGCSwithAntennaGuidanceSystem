#ifndef CSP_SHA_256_H_INCLUDED
#define CSP_SHA_256_H_INCLUDED

#include "STD.h"
 /* ***************************************************************************/
 #define CSP_SHA_256_BLOCKSIZE	512
/* The SHA1 digest (hash) size in bytes */
#define CSP_SHA_256_DIGESTSIZE	256
/////////////////////////////////////
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
////////////////////////////////////
 const u32  H_0_values[8];
 u8 padd_message1[64];
 u32 parsed_message1[16];
 u32 K_256_224_constant[64];
/* create type struct */
typedef struct
{
 /*hash values*/
 u32 H_value[8];
 /*padded message*/
  u8 M_padd_message[64];
 u32 M_block[16];
 /* SHA_256 constatnt*/
 u32 *K_constant;
 /*message schedule,W*/
 u32 W_schedule[64];
 /*original message length*/
 u64 M_length;
 unsigned long long M_bitlength;
 /* digest message */
 u8 digest[32];
 /*working variables*/
 u32 a;
 u32 b;
 u32 c;
 u32 d;
 u32 e;
 u32 f;
 u32 g;
 u32 h;
 /*T variables*/
 u32 T1;
 u32 T2;
}SHA_256_224_t;


void SHA_256_224(u8 *message, u64 message_length, SHA_256_224_t *SHA_256_224_struc,u8 sha_type);
void SHA_256_224_initalize(SHA_256_224_t *SHA_256_224_struc,u64 message_length,u8 sha_type);
void SHA_256_224_padding(u8 *Message, u64 Message_len,SHA_256_224_t *SHA_256_224_struc);
void SHA_256_224_preprocessor(u8 *message, u64 message_length,SHA_256_224_t *SHA_256_224_struc);
void SHA_256_224_Compute(SHA_256_224_t *SHA_256_224_struc);
u32 segma(u32 x,u8 n);
u32 SUMM(u32 x,u8 n);


#endif // CSP_SHA_256_H_INCLUDED
