#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
//#include "CRC.h"
#include "TIMER_general.h"

void CCSDS_void_FOP(u8 procedure,u8 VCID ,void*PTR1,void*PTR2,void*PTR3)
{
    CCSDS_TC TC;
    CCSDS_TC* TC_frame=&TC;
    TC.BypassFlag=0;
    TC.ControlCommandFlag=0;
    TC.COPInEffect=0;
    TC.FarmBCounter=0;
    TC.Flags=0;
    TC.frame_length=0;
    TC.length_TC_Data=0;
    TC.length_TM_Data=0;
    TC.MCFC=0;
    TC.N_of_S=0;
    TC.OCF_flag=0;
    TC.PacketOrder_flag=0;
    TC.Rec_TC_VCID=0;
    TC.ReportValue=0;
    TC.SCID=0;
    TC.SLI=0;
    TC.VCFC=0;
    TC.TM_VCID=0;
    TC.TFSH_length=0;
    TC.TC_VCID=0;
    TC.Synch_flag=0;
    TC.StatusField=0;
    u8 frame,first_byte_pointer_TC_Data;
    u8 first_byte_pointer_TM_Data,SecondaryHeaderData;
    TC.first_byte_pointer_TC_Data=&first_byte_pointer_TC_Data;
    TC.frame=&frame;
    TC.first_byte_pointer_TM_Data=&first_byte_pointer_TM_Data;
    TC.SecondaryHeaderData=&SecondaryHeaderData;
    u8 CRC_error=0;
    u8 pac;
    u8* packet=&pac;
    u32 packet_length=0;
    TC.TC_VCID=VCID;
    switch (procedure)
    {
        case CCSDS_FOP_CLCW:
            /**PTR1 is the frame (u8*)**/
            /**PTR2 the frame length (u32)**/
            /**PTR3 unused**/
            /**CRC check**/
            CRC_error=CCSDS_u8_TM_DEFRAME((u8*)PTR1,*(u32*)PTR2,&TC);
            /**check the CRC and check the OCF flag**/
            if ((TC_frame->OCF_flag)&&(!CRC_error))
            {
                CCSDS_void_CLCWarrives(VCID,TC_frame->COPInEffect,TC_frame->Rec_TC_VCID,TC_frame->Flags,TC.ReportValue,&TC);
            }
            else if(CRC_error)
            {
                if(CCSDS_FOP_DEBUG)
                    printf("CRC error\n");
            }
            else
            {
                if (CCSDS_FOP_DEBUG)
                    printf("NO CLCW found \n");
            }
            break;
        case CCSDS_FOP_TIMER_EXPIRE:
            /**PTR1 unused**/
            /**PTR2 unused**/
            /**PTR3 unused**/
            CCSDS_void_FOPTIMEREXP(VCID,&TC);
            break;
        case CCSDS_FOP_RECIVE_REQUESTTO_TRANSFER_FDU:
            /**PTR1 service type (u8)**/
            /**PTR2 SDU (u8*)**/
            /**PTR3 SDU length (u32)**/
            packet_length=(u32)(*((u32*)PTR3));
            packet=(u8*)PTR2;
            CCSDS_void_Receive_Request_to_Transfer_FDU(VCID,*(u8*)PTR1,packet,packet_length,&TC);
            break;
        case CCSDS_FOP_RECEIVE_DIRCTIVEFROM_MANGEMENT_FUNCTION:
            /**PTR1 the directive type (u8)**/
            /**PTR2 the directive var (u32)**/
            /**PTR3 unused**/
            CCSDS_void_ReceiveDirectiveFromManagementFunction(VCID,*(u8*)PTR1,TC,*(u32*)PTR2);
            break;
        case CCSDS_FOP_RECEIVE_RESPONSEFROM_LOWER_PROCEDURE:
            /**PTR1 Response (u8)**/
            /**PTR2 unused**/
            /**PTR3 unused**/
            CCSDS_void_ReceiveResponseFromLowerLayer(VCID,*(u8*)PTR1,&TC);
            break;
        default:
            if (CCSDS_FOP_DEBUG)
                printf("Unknown procedure \n");
            break;
    }
}
void CCSDS_FOP_void_packet_receiption(u8* packet,u32 packet_len,u8* sec_data,u32 sec_data_len,u8 VCID)
{
    if (FOP_Received_Queue.Queue_Counter<1)
    {
        /*set the number of the packets in the queue to 1*/
        FOP_Received_Queue.Queue_Counter=1;
        /*copy the packet to the queue*/
        FOP_Received_Queue.data=(u8*)malloc(packet_len);
        memcpy(FOP_Received_Queue.data,packet,packet_len);
        FOP_Received_Queue.data_len=(u32*)malloc(1*sizeof(u32));
        FOP_Received_Queue.data_len[0]=packet_len;
        FOP_Received_Queue.data_Used_size=packet_len;
        /*copy the sec data to the queue*/
        if(sec_data_len>0)
        {
            FOP_Received_Queue.sec_data=(u8*)malloc(sec_data_len);
            memcpy(FOP_Received_Queue.sec_data,sec_data,sec_data_len);
            FOP_Received_Queue.sec_data_len=(u32*)malloc(sizeof(u32));
            FOP_Received_Queue.sec_data_len[0]=sec_data_len;
            FOP_Received_Queue.sec_data_Used_size=sec_data_len;
        }
        /*add the VCID to the queue*/
        FOP_Received_Queue.VCID=(u8*)malloc(sizeof(u8));
        FOP_Received_Queue.VCID[0]=VCID;
    }
    else
    {
        /*increase the number of the packets in the queue by 1*/
        FOP_Received_Queue.Queue_Counter+=1;
        /*copy the packet to the queue*/
        FOP_Received_Queue.data_Used_size+=packet_len;
        FOP_Received_Queue.data=(u8*)realloc(FOP_Received_Queue.data,FOP_Received_Queue.data_Used_size);
        memcpy(FOP_Received_Queue.data+FOP_Received_Queue.data_Used_size-packet_len,packet,packet_len);

        FOP_Received_Queue.data_len=(u32*)realloc(FOP_Received_Queue.data_len,FOP_Received_Queue.Queue_Counter*sizeof(u32));
        FOP_Received_Queue.data_len[FOP_Received_Queue.Queue_Counter-1]=packet_len;
        /*copy the sec data to the queue*/
        FOP_Received_Queue.sec_data_Used_size+=sec_data_len;
        FOP_Received_Queue.sec_data=(u8*)realloc(FOP_Received_Queue.sec_data,FOP_Received_Queue.sec_data_Used_size);
        memcpy(FOP_Received_Queue.sec_data+FOP_Received_Queue.sec_data_Used_size-sec_data_len,sec_data,sec_data_len);

        FOP_Received_Queue.sec_data_len=(u32*)realloc(FOP_Received_Queue.sec_data_len,FOP_Received_Queue.Queue_Counter*sizeof(u32));
        FOP_Received_Queue.sec_data_len[FOP_Received_Queue.Queue_Counter-1]=sec_data_len;
        /*add the VCID to the queue*/
        FOP_Received_Queue.VCID=(u8*)realloc(FOP_Received_Queue.VCID,FOP_Received_Queue.Queue_Counter*sizeof(u8));
        FOP_Received_Queue.VCID[FOP_Received_Queue.Queue_Counter-1]=VCID;
    }
}
void CCSDS_void_FOP_free()
{
    u8 i=0;
    for (i=0;i<64;i++)
    {
        CCSDS_void_purging_the_Sent_Queue(i);
        CCSDS_void_purging_the_Wait_Queue(i);
    }
    if (FOP_Received_Queue.Queue_Counter)
    {
        free(FOP_Received_Queue.data);
        free(FOP_Received_Queue.data_len);
        free(FOP_Received_Queue.sec_data);
        free(FOP_Received_Queue.sec_data_len);
        free(FOP_Received_Queue.VCID);
        FOP_Received_Queue.Queue_Counter=0;
        FOP_Received_Queue.data_Used_size=0;
    }
}
u8* CCSDS_u8pointer_GetDataFromReceivedAueue(u32* data_length,u32* sec_data_lengyh,u8* VCID)
{
    u8 a=0;
    u8* data=&a;
    if (FOP_Received_Queue.Queue_Counter<=0)
    {
        *data_length=0;
        *sec_data_lengyh=0;
        *VCID=0;
    }
    else if (FOP_Received_Queue.Queue_Counter==1)
    {
        /**Stet the queue counter to zero**/
        FOP_Received_Queue.Queue_Counter=0;

        /**Get the length of the data**/
        *data_length=FOP_Received_Queue.data_len[0];

        /**Get the length of the secondary data**/
        *sec_data_lengyh=FOP_Received_Queue.sec_data_len[0];

        /**Get the VCID**/
        *VCID=FOP_Received_Queue.VCID[0];

        /**Free the unused memory**/
        free(FOP_Received_Queue.VCID);
        free(FOP_Received_Queue.data_len);
        free(FOP_Received_Queue.sec_data_len);

        /**Get a copy of the data**/
        data=(u8*)malloc(*data_length+*sec_data_lengyh);
        memcpy(data,FOP_Received_Queue.data,*data_length);
        free(FOP_Received_Queue.data);

        /**Get a copy of the secondary data**/
        memcpy(data+*data_length,FOP_Received_Queue.sec_data,*sec_data_lengyh);
        free(FOP_Received_Queue.sec_data);

        /**Set the used size of the queue**/
        FOP_Received_Queue.sec_data_Used_size=0;
        FOP_Received_Queue.data_Used_size=0;
    }
    else
    {
        /**Decrement the queue counter**/
        FOP_Received_Queue.Queue_Counter--;

        /**Get the length of the data and length of the secondary data and the VCID**/
        *data_length=FOP_Received_Queue.data_len[0];
        *sec_data_lengyh=FOP_Received_Queue.sec_data_len[0];
        *VCID=FOP_Received_Queue.VCID[0];

        /**shift the data in the queue**/
        memcpy(FOP_Received_Queue.data_len,FOP_Received_Queue.data_len+1,FOP_Received_Queue.Queue_Counter);
        memcpy(FOP_Received_Queue.sec_data_len,FOP_Received_Queue.sec_data_len+1,FOP_Received_Queue.Queue_Counter);
        memcpy(FOP_Received_Queue.VCID,FOP_Received_Queue.VCID+1,FOP_Received_Queue.Queue_Counter);

        /**Allocate a memory for the data the memory of the queue**/
        FOP_Received_Queue.VCID=(u8*)realloc(FOP_Received_Queue.VCID,FOP_Received_Queue.Queue_Counter);
        FOP_Received_Queue.data_len=(u32*)realloc(FOP_Received_Queue.data_len,FOP_Received_Queue.Queue_Counter);
        FOP_Received_Queue.sec_data_len=(u32*)realloc(FOP_Received_Queue.sec_data_len,FOP_Received_Queue.Queue_Counter);

        /**Reset the size of the used memory**/
        FOP_Received_Queue.sec_data_Used_size-=*sec_data_lengyh;
        FOP_Received_Queue.data_Used_size-=*data_length;

        /**Get a copy of the data from the queue**/
        data=(u8*)malloc(*data_length+*sec_data_lengyh);
        memcpy(data,FOP_Received_Queue.data,*data_length);
        memcpy(FOP_Received_Queue.data,FOP_Received_Queue.data+*data_length,FOP_Received_Queue.data_Used_size);
        FOP_Received_Queue.data=(u8*)realloc(FOP_Received_Queue.data,FOP_Received_Queue.data_Used_size);

        /**Get a copy of the secondary data from the queue**/
        memcpy(data+*data_length,FOP_Received_Queue.sec_data,*sec_data_lengyh);
        memcpy(FOP_Received_Queue.sec_data,FOP_Received_Queue.sec_data+*sec_data_lengyh,FOP_Received_Queue.sec_data_Used_size);
        FOP_Received_Queue.sec_data=(u8*)realloc(FOP_Received_Queue.sec_data,FOP_Received_Queue.sec_data_Used_size);
    }
    return data;
}
void CCSDS_void_FOPINIT()
{
    u8 i=0;
    CCSDS_lengthof_copy_frame_tolowerleyer=0;
    CCSDS_transmitte_frame_tybe=0;
    timer_initialize();
    for (i=0;i<64;i++)
    {
        VirtualChannels[i].ADOutFlag=1;
        VirtualChannels[i].BCOutFlag=1;
        VirtualChannels[i].BDOutFlag=1;
        VirtualChannels[i].flags=0;
        VirtualChannels[i].K=4;
        VirtualChannels[i].NNR=0;
        VirtualChannels[i].N_of_R=0;
        VirtualChannels[i].SS=0;
        VirtualChannels[i].State=CCSDS_INITIAL;
        VirtualChannels[i].T1Initial=5000;
        VirtualChannels[i].ToBeRetramittedFlag=0;
        VirtualChannels[i].TrasmissionCount=0;
        VirtualChannels[i].TrasmissionLimit=5;
        VirtualChannels[i].TT=1;
        VirtualChannels[i].V_of_S=0;
        VirtualChannels[i].wait_queue_FDU_length=0;
        VirtualChannels[i].sent_queue_counter=0;
        VirtualChannels[i].SentQueue_Length=0;
    }
    CCSDS_Abort=0;
    CCSDS_Notifications=0;
    //CCSDS_FOP_RS_ON = 1;
    //CCSDS_FOP_RANDOMIZER_ON = 1;
    FOP_Received_Queue.data_Used_size=0;
    FOP_Received_Queue.sec_data_Used_size=0;
    FOP_Received_Queue.Queue_Counter=0;
}
void CCSDS_void_FOPconfig(u16 SCID)
{
    FOP_SCID = SCID;
}
