#include "CSP_interface.h"
/*HMAC file.c*/
extern u8 HMAC_opad = 0x5c;
extern u8 HMAC_ipad = 0x36;
extern u8 HMAC_secret_KEY[];

void HMAC_process(u8 *message ,u64 key_length,u8 *key , SHA_256_224_t *SHA_256_224_struc,SHA_1_t *SHA_1_struc,u64 datalength,u8 sha_type )
{
    u8 hash_size=0;
   //detect SHA algorithm used
    if(sha_type ==1)       //for SHA_256
        hash_size=32;
    else if(sha_type==2)  //for SHA-224
        hash_size=28;
    else if(sha_type==3)  //for SHA-1
        hash_size=20;

    //get key_dash with size =64 byte
    u8 key_dash[512]; u8 H_1[512] ; u8 HMAC_ipad_key[512]; u8 HMAC_opad_key[512]  ;
    //for default
    memset(key_dash,0,64);
    memset(H_1,0,64);
    memset(HMAC_ipad_key,0,64);
     memset(HMAC_opad_key,0,64);

    memcpy(key_dash ,key,key_length);
    if(key_length > HMAC_block_LENGTH)
     {
     //shorten it untill 256 bit(32 byte) using hash
     if(sha_type==3)
       {SHA_1_func(key, key_length,SHA_1_struc);
       //update key length with degist length
       key_length=hash_size;
       memcpy(key_dash,SHA_256_224_struc->digest,hash_size);}
     else
       {SHA_256_224(key, key_length,SHA_256_224_struc,sha_type);
       //update key length with degist length
        key_length=hash_size;
        memcpy(key_dash,SHA_256_224_struc->digest,hash_size);}
     }
    /*then :
    1-padding it untill 64 byte
    2-XOR (bitwise exclusive-OR) the B byte string computed in step (1) with ipad*/
    for(u8 i=0;i<key_length;i++)
     HMAC_ipad_key[i]= key_dash[i] ^ HMAC_ipad;
   for(u8 i=key_length;i<64;i++)
     HMAC_ipad_key[i]= 0 ^ (HMAC_ipad);
    /*
     3- append the stream of data 'text' to the 64 byte string resulting from step (2)
     4- apply H to the stream generated in step (3)
     */
   for(int j=0; j<datalength;j++)
      HMAC_ipad_key[64+j]=message[j];

   //perform hashing  the new HMAC_opad_key
   if(sha_type==3)
   {SHA_1_func(HMAC_ipad_key ,64+datalength,SHA_1_struc);
   for(int i=0;i<hash_size;i++)
     H_1[i] = SHA_1_struc->digest[i];
   }
   else
   {
    SHA_256_224(HMAC_ipad_key ,64+datalength,SHA_256_224_struc,sha_type);
   for(int i=0;i<hash_size;i++)
     H_1[i] = SHA_256_224_struc->digest[i];
   }
  /*
    5-XOR (bitwise exclusive-OR) the B byte string computed in step (1) with opad
     * NOTE: The "key_dash" variable is reused.
  */
   for(u8 i=0;i<key_length;i++)
     HMAC_opad_key[i] = key_dash[i] ^ HMAC_opad;
   for(u8 j=key_length;j<64;j++)
       HMAC_opad_key[j]= 0 ^ HMAC_opad;
   /*
   6-append the H result from step (4) to the 64 byte string resulting from step (5)
   7-apply H to the stream generated in step (6) and output the result
    */
  for(int j=0; j<strlen(H_1);j++)
      HMAC_opad_key[64+j]=H_1[j];
   //hashing final result
   if(sha_type ==3)
   {
   SHA_1_func(HMAC_opad_key ,64+hash_size,SHA_1_struc);
   printf("HMAC_RESULT\n");
    for(int i=0;i<hash_size;i++)
        printf("%x",SHA_1_struc->digest[i]);
      printf("\n");
      printf("*************************************************************************************\n");
   }
   else
   {
     SHA_256_224(HMAC_opad_key ,64+hash_size,SHA_256_224_struc,sha_type);
   printf("HMAC_RESULT\n");
    for(int i=0;i<hash_size;i++)
        printf("%x",SHA_256_224_struc->digest[i]);
      printf("\n");
      printf("*************************************************************************************\n");
   }
}
//////////////////////////////////////////////////////////
