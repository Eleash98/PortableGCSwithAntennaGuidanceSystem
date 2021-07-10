#ifndef CSP_HMAC_H_INCLUDED
#define CSP_HMAC_H_INCLUDED
#include "STD.h"
#include "CSP_SHA_256_224.h"
#include "SHA_1.h"
/* ******************************************************************/
/* let key length =16*/
#define HMAC_KEY_LENGTH	    16
#define HMAC_block_LENGTH	64
#define SHA_256              1
#define SHA_224              2
#define SHA_1                3


/* *****************************************************************/
typedef struct
{
    u8 HMAC_opad ;
    u8 HMAC_ipad;
    u8 HMAC_KEY;
    u8 HMAC_opad_key;
    u8 HMAC_ipad_key;


}HMAC_t;
/* ***********************************************************************/
extern u8 HMAC_secret_KEY[] ;
extern u8 HMAC_opad ;
extern u8 HMAC_ipad ;
/* ***********************************************************************/
void HMAC_process(u8 *message ,u64 key_length,u8 *key , SHA_256_224_t *SHA_256_224_struc,SHA_1_t *SHA_1_struc,u64 datalength,u8 sha_type );

#endif // CSP_HMAC_H_INCLUDED
