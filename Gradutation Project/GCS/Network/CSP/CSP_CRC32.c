#include <stdio.h>
#include <stdlib.h>
#include "CSP_CRC32.h"
#include "STD.h"

u32 CSP_CRC32(u8* dataptr,u16 datalength)
{
    if (datalength==0)
        return 0;
    /*put the initial crc register value equal zero*/
    u32 crc_register=0x00000000;
    /*crc32 polynomial*/
    #define CRC32_POLY 0X04C11DB7

    for (u16 i=0; i<datalength; i++)
    {
        /*put the first byte in the in the MSB of crc register*/
        crc_register ^=dataptr[i]<<24;
        for (u16 j=0; j<8; j++)
        {
            /*check if the MSB =1*/
            if (crc_register & 0x80000000)

                crc_register = (crc_register<<1) ^ CRC32_POLY;
            crc_register <<=1;
        }
    }

    printf("CRC32= ");
    printf("%X\n",crc_register);
    printf("*************************************************************************************\n");
    return crc_register;
}
