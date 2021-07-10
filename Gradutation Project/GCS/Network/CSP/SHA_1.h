#ifndef SHA_1_H_INCLUDED
#define SHA_1_H_INCLUDED
#include "STD.h"
#include "BIT_MATH.h"

#define FF_0(a, b, c, d, e, i) do {e = (ROL(a, 5) + F0(b,c,d) + e + SHA_1_struc->W_schedule[i] + 0x5a827999UL); b = ROL(b, 30);} while (0)
#define FF_1(a, b, c, d, e, i) do {e = (ROL(a, 5) + F1(b,c,d) + e + SHA_1_struc->W_schedule[i] + 0x6ed9eba1UL); b = ROL(b, 30);} while (0)
#define FF_2(a, b, c, d, e, i) do {e = (ROL(a, 5) + F2(b,c,d) + e + SHA_1_struc->W_schedule[i] + 0x8f1bbcdcUL); b = ROL(b, 30);} while (0)
#define FF_3(a, b, c, d, e, i) do {e = (ROL(a, 5) + F3(b,c,d) + e + SHA_1_struc->W_schedule[i] + 0xca62c1d6UL); b = ROL(b, 30);} while (0)
 #define LOAD32H(x, y)  do { (x) = ((u32)((y)[0] & 0xff) << 24) | \
							   	  ((u32)((y)[1] & 0xff) << 16) | \
							   	  ((u32)((y)[2] & 0xff) <<  8) | \
							   	  ((u32)((y)[3] & 0xff) <<  0); } while (0)


#define STORE32H(x, y) do { (y)[0] = (u8)(((x) >> 24) & 0xff); \
							(y)[1] = (u8)(((x) >> 16) & 0xff); \
							(y)[2] = (u8)(((x) >>  8) & 0xff); \
							(y)[3] = (u8)(((x) >>  0) & 0xff); } while (0)

#define STORE64H(x, y) do {	(y)[0] = (u8)(((x) >> 56) & 0xff); \
							(y)[1] = (u8)(((x) >> 48) & 0xff); \
							(y)[2] = (u8)(((x) >> 40) & 0xff); \
							(y)[3] = (u8)(((x) >> 32) & 0xff); \
							(y)[4] = (u8)(((x) >> 24) & 0xff); \
							(y)[5] = (u8)(((x) >> 16) & 0xff); \
							(y)[6] = (u8)(((x) >>  8) & 0xff); \
							(y)[7] = (u8)(((x) >>  0) & 0xff); } while (0)
 /* ***************************************************************************/
 #define CSP_SHA_1_BLOCKSIZE	512
/* The SHA1 digest (hash) size in bytes */
#define CSP_SHA_1_DIGESTSIZE	160
/* ****************************************************************************/
#define CH(x,y,z )    (z ^ (x & (y ^ z)))
#define parity(x,y,z) (x^y^z)                       //f2,f4(20<t<39,60<t<79)
#define MAJ(x,y,z )   ((x & y) | (z & (x | y)))
/* ****************************************************************************/
#define F0(x,y,z)  (z ^ (x & (y ^ z)))
#define F1(x,y,z)  (x ^ y ^ z)
#define F2(x,y,z)  ((x & y) | (z & (x | y)))
#define F3(x,y,z)  (x ^ y ^ z)
/* ****************************************************************************/
/* create type struct */
typedef struct
{
 /*hash values*/
 u32 H_value[8];
 /*padded message*/
 u8 M_padd_message[64];
 /* SHA_1 constatnt*/
 u32 K1_constant;
 u32 K2_constant;
 u32 K3_constant;
 u32 K4_constant;
 /*message schedule,W*/
 u32 W_schedule[80];
 /*original message length*/
 u64 M_length;
 unsigned long long M_bitlength;
 /* digest message */
 u8 digest[20];
 /*working variables*/
 u32 a;
 u32 b;
 u32 c;
 u32 d;
 u32 e;
 /*T variables*/
 u32 T;

}SHA_1_t;

void SHA_1_func(u8 *message, u64 message_length, SHA_1_t *SHA_1_struc);
void SHA_1_initalize(SHA_1_t *SHA_1_struc,u64 message_length);
void SHA_1_padding(u8 *Message, u64 Message_len,SHA_1_t *SHA_1_struc);
void SHA_1_preprocessor(u8 *message, u64 message_length,SHA_1_t *SHA_1_struc);
void SHA_1_Compute(SHA_1_t *SHA_1_struc);




#endif // SHA_1_H_INCLUDED
