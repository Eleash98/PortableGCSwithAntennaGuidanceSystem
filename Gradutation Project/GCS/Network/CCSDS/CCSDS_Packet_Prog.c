#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_PACKET_PRIVATE.h"
/*________________________________GENERAL________________________________*/
/*_______________________________________________________________________*/
void CCSDS_PACKET_void_init()
{
    CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter=0;
    CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size=0;
    CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter=0;
    CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size=0;
    packet_seq_count_TC=0;
    packet_seq_count_TM=0;
}
void CCSDS_PACKET_void_free()
{
    if (CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter)
    {
        free(CCSDS_TRANSMIT_PACKET_QUEUE.APIDs);
        free(CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths);
        free(CCSDS_TRANSMIT_PACKET_QUEUE.Queue);
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter=0;
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size=0;
    }
    if (CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter)
    {
        free(CCSDS_RECEIVE_PACKET_QUEUE.APIDs);
        free(CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths);
        free(CCSDS_RECEIVE_PACKET_QUEUE.Queue);
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter=0;
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size=0;
    }
}
u8* CCSDS_PACKET_u8pointer_packetTOSubnetwork(u32* packet_length)
{
    u8* packet;
    if (CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter<=0)
    {
        *packet_length=0;
        if (CCSDS_PACKET_DEPUG)
            printf("No packet in the TRANSMIT_PACKET_QUEUE\n");
    }
    else if (CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter==1)
    {
        /*get the length of the first packet*/
        *packet_length=CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths[0];
        free(CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths);
        /*set the packet counter to zero*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter=0;
        /*set the queue used size to zero*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size=0;
        /*copy the first packet*/
        packet=(u8*)malloc(*packet_length);
        memcpy(packet,CCSDS_TRANSMIT_PACKET_QUEUE.Queue,*packet_length);
        free(CCSDS_TRANSMIT_PACKET_QUEUE.Queue);
        free(CCSDS_TRANSMIT_PACKET_QUEUE.APIDs);
    }
    else
    {
        /*decrease the queue counter by one*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter--;
        /*get the length of the first packet*/
        *packet_length=CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths[0];
        memcpy(CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths,CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths+1,
            CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter);
        CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths=(u32*)realloc(CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths,
            CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter*sizeof(u32));
        /*decrease the queue used size by the coped packet length*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size-=*packet_length;
        /*copy the first packet in the queue*/
        packet=(u8*)malloc(*packet_length);
        memcpy(packet,CCSDS_TRANSMIT_PACKET_QUEUE.Queue,*packet_length);
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue=(u8*)realloc(CCSDS_TRANSMIT_PACKET_QUEUE.Queue,
            CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size);
    }
    return packet;
}
void CCSDS_PACKET_u8pointer_packetFROMSubnetwork(u8* packet,u32 packet_length)
{
    if (CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter<=0)
    {
        /*set the queue counter to one*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter=1;
        /*set the used queue size*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size=packet_length;
        /*free all pointers in the queue*/
        free(CCSDS_RECEIVE_PACKET_QUEUE.Queue);
        free(CCSDS_RECEIVE_PACKET_QUEUE.APIDs);
        free(CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths);
        /*extract the APID from the received packet*/
        CCSDS_RECEIVE_PACKET_QUEUE.APIDs=(u16*)malloc(sizeof(u16));
        CCSDS_RECEIVE_PACKET_QUEUE.APIDs[0]=(u16)((packet[0]&0x07)<<8)|packet[1];
        /*add the packet to the queue*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue=(u8*)malloc(packet_length);
        memcpy(CCSDS_RECEIVE_PACKET_QUEUE.Queue,packet,packet_length);
        /*add the length of the packet to the queue packet lengths*/
        CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths=(u32*)malloc(sizeof(u32));
        CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths[0]=packet_length;

    }
    else
    {
        /*increase the queue counter by one*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter++;
        /*increase the used queue size*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size+=packet_length;
        /*extract the APID from the received packet*/
        CCSDS_RECEIVE_PACKET_QUEUE.APIDs=(u16*)realloc(CCSDS_RECEIVE_PACKET_QUEUE.APIDs,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter*sizeof(u16));
        CCSDS_RECEIVE_PACKET_QUEUE.APIDs[CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter-1]=(u16)((packet[0]&0x07)<<8)|packet[1];
        /*add the packet to the queue*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue=(u8*)realloc(CCSDS_RECEIVE_PACKET_QUEUE.Queue,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size);
        memcpy(CCSDS_RECEIVE_PACKET_QUEUE.Queue+CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size-packet_length,
            packet,packet_length);
        /*add the length of the packet to the queue packet lengths*/
        CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths=(u32*)realloc(CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter*sizeof(u32));
        CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths[CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter-1]=packet_length;
    }
}
/*________________________________PACKET________________________________*/
/*______________________________________________________________________*/
void CCSDS_PACKET_void_PACKET_REQUEST(u16 APID,u32 packet_length,u8* packet)
{
    if (CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter<=0)
    {
        /*the number of the packet in the queue is 1*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter=1;
        /*increase the used size by the new packet size*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size=packet_length;
        /*put the APID in it place*/
        CCSDS_TRANSMIT_PACKET_QUEUE.APIDs=(u16*)malloc(sizeof(u16));
        CCSDS_TRANSMIT_PACKET_QUEUE.APIDs[0]=APID;
        /*add the packet to the queue*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue=(u8*)malloc(packet_length);
        memcpy(CCSDS_TRANSMIT_PACKET_QUEUE.Queue,packet,packet_length);
        /*add the length of the packet to the packet lengths*/
        CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths=(u32*)malloc(sizeof(u32));
        CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths[0]=packet_length;
    }
    else
    {
        /*increase the number of the packet*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter++;
        /*increase the used size by the new packet size*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size+=packet_length;
        /*put the APID in it place*/
        CCSDS_TRANSMIT_PACKET_QUEUE.APIDs=(u16*)realloc(CCSDS_TRANSMIT_PACKET_QUEUE.APIDs,
            CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter*sizeof(u16));
        CCSDS_TRANSMIT_PACKET_QUEUE.APIDs[CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter-1]=APID;
        /*add the packet to the queue*/
        CCSDS_TRANSMIT_PACKET_QUEUE.Queue=(u8*)realloc(CCSDS_TRANSMIT_PACKET_QUEUE.Queue,
            CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size);
        memcpy(CCSDS_TRANSMIT_PACKET_QUEUE.Queue+CCSDS_TRANSMIT_PACKET_QUEUE.Queue_used_size-packet_length
            ,packet,packet_length);
        /*add the length of the packet to the packet lengths*/
        CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths=(u32*)realloc(CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths,
            CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter*sizeof(u32));
        CCSDS_TRANSMIT_PACKET_QUEUE.packet_lengths[CCSDS_TRANSMIT_PACKET_QUEUE.Queue_counter-1]=packet_length;
    }
}
u8* CCSDS_PACKET_u8pointer_PACKET_INDICATION(u16* APID,u32* packet_length)
{
    u8 a=0;
    u8* packet=&a;
    if (CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter<1)
    {
        *packet_length=0;
        if (CCSDS_PACKET_DEPUG)
            printf("No packets in the RECEIVE_PACKET_QUEUE\n");
    }
    else if (CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter==1)
    {
        /*set the packet counter to zero*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter=0;
        /*set the queue used size to zero*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size=0;
        /*get the packet length and free the queue packet lengths*/
        *packet_length=CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths[0];
        free(CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths);
        /*copy the packet from the queue and free the queue*/
        packet=(u8*)malloc(*packet_length);
        memcpy(packet,CCSDS_RECEIVE_PACKET_QUEUE.Queue,*packet_length);
        free(CCSDS_RECEIVE_PACKET_QUEUE.Queue);
        /*get the packet APID and free the queue APIDs*/
        *APID=CCSDS_RECEIVE_PACKET_QUEUE.APIDs[0];
        free(CCSDS_RECEIVE_PACKET_QUEUE.APIDs);
    }
    else
    {
        /*decrease the packet counter by one*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter--;
        /*get the first packet length*/
        *packet_length=CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths[0];
        memcpy(CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths,CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths+1,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter);
        CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths=(u32*)realloc(CCSDS_RECEIVE_PACKET_QUEUE.packet_lengths,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter*sizeof(u32));
        /*decrease the queue used size by the packet length*/
        CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size-=*packet_length;
        /*get the packet APID*/
        *APID=CCSDS_RECEIVE_PACKET_QUEUE.APIDs[0];
        memcpy(CCSDS_RECEIVE_PACKET_QUEUE.APIDs,CCSDS_RECEIVE_PACKET_QUEUE.APIDs+1,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter);
        CCSDS_RECEIVE_PACKET_QUEUE.APIDs=(u16*)realloc(CCSDS_RECEIVE_PACKET_QUEUE.APIDs,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_counter*sizeof(u16));
        /*get the packet from the queue*/
        packet=(u8*)malloc(*packet_length);
        memcpy(packet,CCSDS_RECEIVE_PACKET_QUEUE.Queue,*packet_length);
        memcpy(CCSDS_RECEIVE_PACKET_QUEUE.Queue,CCSDS_RECEIVE_PACKET_QUEUE.Queue+*packet_length,
            CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size);
        CCSDS_RECEIVE_PACKET_QUEUE.Queue=(u8*)realloc(CCSDS_RECEIVE_PACKET_QUEUE.Queue,CCSDS_RECEIVE_PACKET_QUEUE.Queue_used_size);
    }
    return packet;
}
/*________________________________OCTET_STRING________________________________*/
/*____________________________________________________________________________*/
void CCSDS_PACKET_void_OCTET_STRING_REQUEST(u16 APID,u8* OCTET_STRING,
    u32 OCTET_STRING_length,u8 packet_type,u8 sec_head_flag,u8* Time_code_field,
    u16 time_code_length,u8* ancillary_data,u16 ancillary_length)
{
    u8*data=(u8*)malloc(OCTET_STRING_length);
    memcpy(data,OCTET_STRING,OCTET_STRING_length);
    /*packet assembly*/
    data=CCSDS_u32_PACKETASSEMBLY(APID,data,packet_type,sec_head_flag,&OCTET_STRING_length,
        Time_code_field,time_code_length,ancillary_data,ancillary_length);
    free(Time_code_field);
    free(ancillary_data);
    /*add the packet to the queue*/
    CCSDS_PACKET_void_PACKET_REQUEST(APID,OCTET_STRING_length,data);
    free(data);
}
u8* CCSDS_PACKET_u8pointer_OCTET_STRING_INDICATION(u16* APID,u32* OCTET_STRING_length,u8* Secondary_Header_Indicator)
{
    u8* OCTET_STRING;
    OCTET_STRING=CCSDS_PACKET_u8pointer_PACKET_INDICATION(APID,OCTET_STRING_length);
    OCTET_STRING=CCSDS_u32_PACKETDEASSEMBLY(OCTET_STRING,OCTET_STRING_length,APID,Secondary_Header_Indicator);
    return OCTET_STRING;
}
/*==================================ADDITIONAL==================================*/
/*==============================================================================*/
u8* CCSDS_u32_PACKETASSEMBLY(u16 APID,u8* SDU,u8 packet_type,u8 sec_head_flag,
    u32* packet_data_length,u8* Time_code_field,u16 time_code_length,u8* ancillary_data,
    u16 ancillary_length/**,u16 packet_seq_count**/)
{
    u16 packet_seq_count;
    if (packet_type)
    {
        packet_seq_count_TC=(packet_seq_count_TC+1)%16384;
        packet_seq_count=packet_seq_count_TC;
    }
    else
    {
        packet_seq_count_TM=(packet_seq_count_TM+1)%16384;
        packet_seq_count=packet_seq_count_TM;
    }
    u32 packet_length=*packet_data_length+time_code_length+ancillary_length+6;
    u8* data_cpy = (u8*)malloc(*packet_data_length);
    memcpy(data_cpy,SDU,*packet_data_length);
    SDU=(u8*)realloc(SDU,packet_length);
    SDU[0]=0;
    SDU[0]|=(packet_type&1)<<4;
    SDU[0]|=(sec_head_flag&1)<<3;
    SDU[0]|=(APID>>8)&0x07;
    SDU[1]=APID&0xff;
    SDU[2]=0;
    /*we use unsegmented data*/
    SDU[2]|=0x03<<6;
    SDU[2]|=(packet_seq_count>>8)&0x3f;
    SDU[3]= packet_seq_count&0xff;
    (*packet_data_length)--;
    SDU[4]=(*packet_data_length>>8)&0xff;
    SDU[5]=(*packet_data_length)&0xff;
    if (sec_head_flag==1)
    {
        /*secondary header is present*/
        if (time_code_length>0)
        {
            /*copy time data to secondary header*/
            memcpy(SDU+6+time_code_length,Time_code_field,time_code_length);
        }
        if (ancillary_length>0)
        {
            /*copy ancillary data to secondary header*/
            memcpy(SDU+6+time_code_length,ancillary_data,ancillary_length);
        }
    }
    free(data_cpy);
    *packet_data_length=packet_length;
    return SDU;
}
u8* CCSDS_u32_PACKETDEASSEMBLY(u8* packet,u32* packet_length,u16*APID,u8* Secondary_Header_Indicator)
{
    if (*packet_length)
    {
        *Secondary_Header_Indicator=((packet[0]&0x10)>>1);
        *APID=(u16)((packet[0]&0x07)<<8)|packet[1];
        memcpy(packet,packet+6,*packet_length-6);
        packet=(u8*)realloc(packet,*packet_length-6);
        *packet_length-=6;
    }

    return packet;
}
