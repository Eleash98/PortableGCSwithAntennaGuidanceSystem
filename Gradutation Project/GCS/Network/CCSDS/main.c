#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_PACKET_INTERFACE.h"

int main()
{
    u16 APID=100;
    srand(80);
    u32 FDU_len=100;
    u8* FDU = (u8*)malloc(FDU_len);
    u8 i=0;
    for (i=0;i<FDU_len;i++)
        FDU[i]=(u8)rand();
    CCSDS_PACKET_void_init();
    CCSDS_PACKET_void_PACKET_REQUEST(APID,FDU_len,FDU);
    CCSDS_PACKET_void_PACKET_REQUEST(APID,FDU_len,FDU);
    //char a;
    //scanf("%c",a);
    /*for (i=0;i<CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size;i++)
        printf("%d %d  ",CCSDS_TRANSMIT_PACKET_QUEUE.Queue[i] ,i);*/
    CCSDS_PACKET_u8pointer_packetFROMSubnetwork(FDU,FDU_len);
    CCSDS_PACKET_u8pointer_packetFROMSubnetwork(FDU,FDU_len);
    CCSDS_PACKET_u8pointer_packetFROMSubnetwork(FDU,FDU_len);
    free(FDU);
    //FDU=CCSDS_PACKET_u8pointer_PACKET_INDICATION(&APID,&FDU_len);
    //free(FDU);
    //FDU=CCSDS_PACKET_u8pointer_PACKET_INDICATION(&APID,&FDU_len);
    //free(FDU);
    //FDU=CCSDS_PACKET_u8pointer_PACKET_INDICATION(&APID,&FDU_len);
    //free(FDU);
    //FDU=CCSDS_PACKET_u8pointer_PACKET_INDICATION(&APID,&FDU_len);
    //free(FDU);
    //FDU=CCSDS_PACKET_u8pointer_PACKET_INDICATION(&APID,&FDU_len);
    //free(FDU);
    FDU=CCSDS_PACKET_u8pointer_packetTOSubnetwork(&FDU_len);
    free(FDU);
    FDU=CCSDS_PACKET_u8pointer_packetTOSubnetwork(&FDU_len);
    free(FDU);
    FDU=CCSDS_PACKET_u8pointer_packetTOSubnetwork(&FDU_len);
    FDU=CCSDS_PACKET_u8pointer_packetTOSubnetwork(&FDU_len);

    CCSDS_PACKET_void_OCTET_STRING_REQUEST(APID,FDU,FDU_len,1,0,0,0,0,0);
    //free(FDU);
    u8 a;
    //free(FDU);
    FDU =CCSDS_PACKET_u8pointer_OCTET_STRING_INDICATION(&APID,&FDU_len,&a);
    free(FDU);
    FDU =CCSDS_PACKET_u8pointer_OCTET_STRING_INDICATION(&APID,&FDU_len,&a);
    free(FDU);
    FDU =CCSDS_PACKET_u8pointer_OCTET_STRING_INDICATION(&APID,&FDU_len,&a);
    free(FDU);
    FDU =CCSDS_PACKET_u8pointer_OCTET_STRING_INDICATION(&APID,&FDU_len,&a);
    //free(FDU);
    CCSDS_PACKET_void_free();
    return 0;
}
