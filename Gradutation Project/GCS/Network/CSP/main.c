#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CSP_interface.h"


u8 message1[] = {0XAB,0XCA,0XAB,0X2D,0X1C,0X9E,0X5F,0XEF,0XAD};
//u8 message[]={"fatmajdkjkelfjojghjhewijo;wekf;lkjewbfgewtrfgewuhroewklfjhdsbfvehwfrywqrhipeqi867358237756732yrihewkjfeklmfkenwkjhfgjksadhfkhewjkfgnbdshfbhheygfjwhbfkkkkhkjjkkkkkkkkkkkkkkkkkfhgewyjhgfuqewhyiu573265872985894uotljrlkgnfdghfhjghjgjhkkkkkkkkkkyyyyyyyyyyyyyyyyyyyy"};
u8 message[]={"fatma"};
static SHA_256_224_t SHA_256_224_struc_ptr;
static SHA_1_t       SHA_1_struc_ptr;
static SHA_256_224_t SHA_256_224_struc_ptr2;
static SHA_1_t       SHA_1_struc_ptr2;
static CSP_STRUCTURE CSP_frame_ptr;
u8 *digestresult;

int main()
{
 u8 testpacket[8192]={0x44,0xdd,0xee,0xff,0x22};
 u8 i=0;
//SHA_256_struc_ptr.M_padd_message[0] = padd_message1[0];
SHA_256_224_struc_ptr.K_constant = K_256_224_constant;
////////////////////////////////////////////
SHA_256_224_struc_ptr2.M_padd_message[0] = padd_message1[0];
SHA_256_224_struc_ptr2.K_constant = K_256_224_constant;
//fill csp struct
CSP_frame_ptr.data_ptr = message1;
CSP_frame_ptr.data_length =strlen(CSP_frame_ptr.data_ptr);
CSP_frame_ptr.CSP_packet= testpacket;
CSP_frame_ptr.SOURCE_ID=0x01;
CSP_frame_ptr.DESTINATION_ID=1;
CSP_frame_ptr.SOURCE_PORT=0x2f;
CSP_frame_ptr.DESTINATION_PORT=0x1c;
CSP_frame_ptr.PRIORITY=0x01;
CSP_frame_ptr.flags=0x0B;
/////////////////////////////////////////////
 u32 XTEA_SECRET_KEY[4]={0x0123,0x4567,0x8901,0x2345};
 u8 HMAC_secret_KEY[ ] ={"80070713463e7749b90c2dc24911e275gghvjhlkjlojjkghdfaedxthfvkujhlihghydsrgcjhmdhfgjksdghkjdshgukerkuheehgjkhfjkhn"};
 //printf("%d",strlen(message));
//SHA_256_224(message, strlen(message),&SHA_256_224_struc_ptr,1);
//SHA_1_func(message, strlen(message),&SHA_1_struc_ptr);
HMAC_process(message , strlen(HMAC_secret_KEY), HMAC_secret_KEY, &SHA_256_224_struc_ptr,&SHA_1_struc_ptr,strlen(message),SHA_224 );

CSP_VOID_TRANSMISSION( &CSP_frame_ptr,strlen(HMAC_secret_KEY),HMAC_secret_KEY, &SHA_256_224_struc_ptr,&SHA_1_struc_ptr,SHA_224,NUM_ROUNDS,XTEA_SECRET_KEY,CSP_PRIO_LOW);

    printf("the  constructed frame is  :  \n");
    //print the constructed frame
    for (i=0; i<CSP_frame_ptr.data_length+4+4+32; i++)
    {
        printf("%X",testpacket[i]);
        printf(" ");
    }
    printf("\n");
    printf("*************************************************************************************\n");
CSP_VOID_RECEIVING( &CSP_frame_ptr, CSP_frame_ptr.CSP_packet,strlen(HMAC_secret_KEY),HMAC_secret_KEY, &SHA_256_224_struc_ptr2  ,&SHA_1_struc_ptr2, SHA_224 ,NUM_ROUNDS, XTEA_SECRET_KEY);

    return 0;
}
