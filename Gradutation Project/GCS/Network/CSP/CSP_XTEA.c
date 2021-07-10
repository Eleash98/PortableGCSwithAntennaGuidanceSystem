#include "CSP_XTEA.h"
#include "STD.h"


void CSP_VOID_XTEA_ENCRYPTION(u8 num_rounds, u8* data,u32* key, u16 frame_length)
{
    u32 n=0;  u32 i=0;  u32 j=0;  u8 v0; u8 v1;
    u32 delta=0x9E3779B9;
    for (j=0; j<frame_length/2; j++)
    {

        v0=data[n]; v1=data[n+1];  u32 sum=0;
        for (i=0; i < num_rounds; i++)
        {
            v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
            sum += delta;
            v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
        }
        data[n]=v0;    data[n+1]=v1;
        n+=2;
    }
    printf("print encrypted data:\n");
    for(u8 i=0;i<strlen(data);i++)
        printf("%x ",data[i]);
    printf("\n");
    printf("*************************************************************************************\n");
}

void CSP_VOID_XTEA_DECRYPTION(u8 num_rounds, u8* data,u32* key, u16 frame_length)
{
    u32 n=0;  u32 i=0;  u32 j=0; u8 v0; u8 v1;
    u32 delta=0x9E3779B9;
    for (j=0; j<frame_length/2; j++)
    {
        v0=data[n];  v1=data[n+1];  u32 sum=delta*num_rounds;
        for (i=0; i < num_rounds; i++)
        {
              v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
              sum -= delta;
              v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
        }
        data[n]=v0;    data[n+1]=v1;
        n+=2;
    }
    printf("print decrypted data:\n");
    for(u8 i=0;i<frame_length;i++)
        printf("%x ",data[i]);
    printf("\n");
    printf("*************************************************************************************\n");
}
