#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
#include "CRC_interface.h"
#include "TIMER_general.h"

#include "DATALINK_interface.h"

void CCSDS_void_Start_Timer(u8 VCID)
{
    static u8 Timer_VCID[64];
    Timer_VCID[VCID]=VCID;
    /*starting timer section*/
    VirtualChannels[VCID].timer = start_timer(VirtualChannels[VCID].T1Initial,CCSDS_void_time_handler, TIMER_SINGLE_SHOT,&Timer_VCID[VCID]);
    if (CCSDS_FOP_DEBUG)
        printf("starting timer \n");
}
void CCSDS_void_cancel_Timer(u8 VCID)
{
    /*canceling timer section*/
    stop_timer(VirtualChannels[VCID].timer);
    if (CCSDS_FOP_DEBUG)
        printf("stopping timer \n");
}

void CCSDS_void_time_handler(size_t timer_id, void* user_data)
{
    u8* P_VCID ;
    P_VCID = (u8*)user_data;
    u8 VCID =0;
    VCID =(u8 )*P_VCID;
    stop_timer(VirtualChannels[VCID].timer);
    if (CCSDS_FOP_DEBUG)
        printf("Timer in VCID %d expire\n",VCID);
    CCSDS_void_FOP(CCSDS_FOP_TIMER_EXPIRE,VCID, 0, 0, 0);
}

void CCSDS_void_lookfordirective(u8 VCID)
{
    u8 i=0;
    u32 len=0;
    if (VirtualChannels[VCID].BCOutFlag==CCSDS_READY)
    {
        if (VirtualChannels[VCID].ToBeRetramittedFlag)
        {
            for (i=0;i<VirtualChannels[VCID].sent_queue_counter;i++)
            {
                if ((GET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i)))
                {
                    if (GET_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i))
                    {
                        /*Set BC out flag to not ready*/
                        VirtualChannels[VCID].BCOutFlag=CCSDS_NOT_READY;
                        u32 FLen = VirtualChannels[VCID].sent_queue_frame_lengths[i];
                        u8* F = (u8*)malloc(sizeof(u8)*FLen);
                        memcpy(F,VirtualChannels[VCID].SentQueue+len,FLen);
                        TransmitFrame(F,FLen,3,VCID);
                        /*CCSDS_lengthof_copy_frame_tolowerleyer=VirtualChannels[VCID].sent_queue_frame_lengths[i];
                        CCSDS_copy_frame_tolowerleyer=(u8*)malloc(CCSDS_lengthof_copy_frame_tolowerleyer);
                        memcpy(CCSDS_copy_frame_tolowerleyer,VirtualChannels[VCID].SentQueue+len,CCSDS_lengthof_copy_frame_tolowerleyer);
                        *0 if no frame to be transmittted
                           1 if the transmitted frame is an AD
                           2 if the transmitted frame is a BD
                           3 if the transmitted frame is a BC*
                        CCSDS_transmitte_frame_tybe=3;
                        /a copy of the frame is passed to the lower procedure to be retransmitting*/
                        //CCSDS_void_transmitting(u8* frame,u16 frame_length,u8 FrameType);
                        CLR_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i);
                        break;
                    }
                }
                len+=VirtualChannels[VCID].sent_queue_frame_lengths[i];
            }
        }
    }
}
void CCSDS_void_lookforFDU(CCSDS_TC* TC,u8 VCID)
{
    u8 i=0;
    u8 flag=0;
    u32 len=0;
    if (VirtualChannels[VCID].ADOutFlag==CCSDS_READY)
    {
        if (VirtualChannels[VCID].ToBeRetramittedFlag)
        {
            for (i=0;i<VirtualChannels[VCID].sent_queue_counter;i++)
            {
                if (!(GET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i)))
                {
                    if (GET_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i))
                    {
                        /*Set AD out flag to not ready*/
                        VirtualChannels[VCID].ADOutFlag=CCSDS_NOT_READY;
                        /*a copy of the frame is passed to the lower procedure to be retransmitting*/
                        //CCSDS_void_transmitting(u8* frame,u16 frame_length);
                        //CCSDS_lengthof_copy_frame_tolowerleyer=VirtualChannels[VCID].sent_queue_frame_lengths[i];
                        //CCSDS_copy_frame_tolowerleyer=(u8*)malloc(CCSDS_lengthof_copy_frame_tolowerleyer);
                        //memcpy(CCSDS_copy_frame_tolowerleyer,VirtualChannels[VCID].SentQueue+len,CCSDS_lengthof_copy_frame_tolowerleyer);

                        u32 FLen = VirtualChannels[VCID].sent_queue_frame_lengths[i];
                        u8* F = (u8*)malloc(sizeof(u8)*FLen);
                        memcpy(F,VirtualChannels[VCID].SentQueue+len,FLen);
                        TransmitFrame(F,FLen,1,VCID);
                        /**0 if no frame to be transmittted
                           1 if the transmitted frame is an AD
                           2 if the transmitted frame is a BD
                           3 if the transmitted frame is a BC*/
                        //CCSDS_transmitte_frame_tybe=1;
                        CLR_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i);
                        flag=1;
                        break;
                    }
                }
                len+=VirtualChannels[VCID].sent_queue_frame_lengths[i];
            }
        }
        if (!flag)
        {
           if (VirtualChannels[VCID].V_of_S<VirtualChannels[VCID].NNR+VirtualChannels[VCID].K)
           {
               if (VirtualChannels[VCID].wait_queue_FDU_length)
               {
                   CCSDS_void_TRANSMITTAD(TC,VCID);
                   /*The FDU is removed from the Wait_Queue*/
                   free(VirtualChannels[VCID].WaitQueue);
                   VirtualChannels[VCID].wait_queue_FDU_length=0;
                   /*"Accept Response to Request to Transfer FDU" is passed to the Higher Layer*/
                   SET_BIT(CCSDS_Notifications,CCSDS_ACCEPT_RESPONSETO_TRANSFARFDU);
                   CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_TRANSFARFDU);
               }
           }
        }
    }
}
void CCSDS_void_initiateADretransmission(u8 VCID)
{
    u8 i=0;
    /*send Abort request to lower procedure*/
    ////////////////////////////////////////////
    /*function receives requests from fop*/
    CCSDS_Abort=1;
    ////////////////////////////////////////////
    /*increment counter*/
    VirtualChannels[VCID].TrasmissionCount++;
    /*starting timer*/
    CCSDS_void_Start_Timer(VCID);
    /*mark all AD frame to be retransmitted on sent queue*/
    for (i=0;i<VirtualChannels[VCID].sent_queue_counter;i++)
    {
        if (!GET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i))
        {
            SET_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i);
        }
    }
}
void CCSDS_void_initiateBCretransmission(u8 VCID)
{
    u8 i=0;
    /*send Abort request to lower procedure*/
    ////////////////////////////////////////////
    /*function receives requests from fop*/
    CCSDS_Abort=1;
    ////////////////////////////////////////////
    /*increment counter*/
    VirtualChannels[VCID].TrasmissionCount++;
    /*starting timer*/
    CCSDS_void_Start_Timer(VCID);
    /*mark all BC frame to be retransmitted on sent queue*/
    for (i=0;i<VirtualChannels[VCID].sent_queue_counter;i++)
    {
        if (GET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i))
        {
            SET_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i);
        }
    }
}
void CCSDS_void_alert(u8 reason_code,u8 VCID)
{
    /*canceling the timer*/
    CCSDS_void_cancel_Timer(VCID);
    /*purging the Sent Queue*/
    CCSDS_void_purging_the_Sent_Queue(VCID);
    /*purging the Wait Queue*/
    CCSDS_void_purging_the_Wait_Queue(VCID);

    /*completing the processing of any "Initiate AD Service" Directive by generating a
    Negative Confirm Response to Directive’ for the Directive;*/
    if (CCSDS_FOP_DEBUG)
        printf("CCSDS NEGATIVE CONFIRM RESPONSE \n");
    SET_BIT(CCSDS_Notifications,CCSDS_NEGATIVE_CONFIRM_RESPONSE);
    /*generating an ‘Alert’ notification with the Reason Code specified in square brackets
    in the State Table.*/
    SET_BIT(CCSDS_Notifications,reason_code);
    if (CCSDS_FOP_DEBUG)
    {
        switch (reason_code)
        {
            case CCSDS_ALERT_CLCW:
                printf("CCSDS Alert CLCW \n");
                break;
            case CCSDS_ALERT_SYNCH:
                printf("CCSDS Alert synchronization \n");
                break;
            case CCSDS_ALERT_LIMIT:
                printf("CCSDS Alert transmission limit \n");
                break;
            case CCSDS_ALERT_NNR:
                printf("CCSDS Alert NN(R) \n");
                break;
            case CCSDS_ALERT_T1:
                printf("CCSDS Alert Time out\n");
                break;
            case CCSDS_ALERT_LLIF:
                printf("CCSDS Alert LLIF\n");
                break;
            case CCSDS_ALERT_TERM:
                printf("CCSDS Alert TERM\n");
                break;
            case CCSDS_ALERT_LOCKOUT:
                printf("CCSDS Alert Lockout\n");
                break;
            default:
                printf("CCSDS Unknown Alert\n");
                break;
        }
    }

}
void CCSDS_void_purging_the_Sent_Queue(u8 VCID)
{
    if (VirtualChannels[VCID].sent_queue_counter)
    {
        free(VirtualChannels[VCID].sent_queue_frame_lengths);
        free(VirtualChannels[VCID].SentQueue);
        VirtualChannels[VCID].sent_queue_counter=0;
        VirtualChannels[VCID].SentQueue_Length=0;
    }
}
void CCSDS_void_purging_the_Wait_Queue(u8 VCID)
{
    if (VirtualChannels[VCID].wait_queue_FDU_length)
    {
    free(VirtualChannels[VCID].WaitQueue);
    VirtualChannels[VCID].wait_queue_FDU_length=0;
    }
}
void CCSDS_void_Suspend()
{
    /*generating a "Suspend" notification to the Higher Procedures.*/
    SET_BIT(CCSDS_Notifications,CCSDS_SUSPEND);
    if (CCSDS_FOP_DEBUG)
        printf("CCSDS Suspend notification \n");
}
void CCSDS_void_REJECT(u8 Reject_Reason)
{
    /*generating either a Reject Response to Request to Transfer FDU or a Reject
    Response to Directive, depending on the occasion.*/
    SET_BIT(CCSDS_Notifications,Reject_Reason);
    if (CCSDS_FOP_DEBUG)
    {
        if (Reject_Reason==CCSDS_REJECT_RESPONSETO_TRANSFARFDU)
            printf("CCSDS REJECT RESPONSE TO TRANSFER FDU \n");
        else if (Reject_Reason==CCSDS_REJECT_RESPONSETO_DIRECTIVE)
            printf("CCSDS REJECT RESPONSE TO DIRECTIVE \n");
        else
            printf("Unknown Reject type \n");
    }
}
void CCSDS_void_ACCEPT(u8 Accept_Reason)
{
    /*generating either an Accept Response to Request to Transfer FDU or an Accept
    Response to Directive, depending on the occasion*/
    SET_BIT(CCSDS_Notifications,Accept_Reason);
    if (CCSDS_FOP_DEBUG)
    {
        if (Accept_Reason==CCSDS_ACCEPT_RESPONSETO_TRANSFARFDU)
            printf("CCSDS ACCEPT RESPONSE TO TRANSFARFDU \n");
        else if (Accept_Reason==CCSDS_ACCEPT_RESPONSETO_DIRECTIVE)
            printf("CCSDS ACCEPT RESPONSE TO DIRECTIVE \n");
        else
            printf("CCSDS unknown accept type\n");
    }
}
void CCSDS_void_CONFIRM(u8 Confirm_Reason)
{
    /*generating either a "Positive Confirm Response to Request to Transfer FDU" or a
    "Positive Confirm Response to Directive", depending on the occasion*/
    SET_BIT(CCSDS_Notifications,Confirm_Reason);
    if (CCSDS_FOP_DEBUG)
    {
        if (Confirm_Reason==CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE)
            printf("CCSDS POSITIVE CONFIRM RESPONSE TO DIRECTIVE \n");
        else if (Confirm_Reason==CCSDS_POSITIVE_CONFIRM_RESPONSETO_TRANSFARFDU)
            printf("CCSDS POSITIVE CONFIRM RESPONSE TO TRANSFARFDU \n");
        else
            printf("CCSDS unknown confirm type\n");
    }
}
void CCSDS_void_INITIALIZE(u8 VCID)
{
    CCSDS_void_purging_the_Sent_Queue(VCID);
    CCSDS_void_purging_the_Wait_Queue(VCID);
    VirtualChannels[VCID].TrasmissionCount=1;
    VirtualChannels[VCID].SS=0;
}
void CCSDS_void_TRANSMITTBC(CCSDS_TC* TC,u8 VCID)
{
    TC->BypassFlag=1;
    TC->ControlCommandFlag=1;
    CCSDS_void_TC_Framing(TC);
    free(TC->first_byte_pointer_TC_Data);
    /*Adding a copy of the frame to the sent queue*/
    CCSDS_void_AddFrametoSentQueue(TC->frame,TC->frame_length,1,VCID);
    CCSDS_void_Start_Timer(VCID);
    VirtualChannels[VCID].BCOutFlag=CCSDS_NOT_READY;
    ////////////////////////////////////////////////////////////////////

    u32 FLen = TC->frame_length;
    u8* F = (u8*)malloc(sizeof(u8)*FLen);
    memcpy(F,TC->frame,FLen);
    TransmitFrame(F,FLen,3,VCID);

    //CCSDS_lengthof_copy_frame_tolowerleyer=TC->frame_length;
    //CCSDS_copy_frame_tolowerleyer=(u8*)malloc(CCSDS_lengthof_copy_frame_tolowerleyer);
    //memcpy(CCSDS_copy_frame_tolowerleyer,TC->frame,TC->frame_length);
    /**0 if no frame to be transmittted
       1 if the transmitted frame is an AD
       2 if the transmitted frame is a BD
       3 if the transmitted frame is a BC*/
    //CCSDS_transmitte_frame_tybe=3;
    /*a copy of the frame is passed to the lower procedure to be transmitted*/
    //CCSDS_void_transmitting(u8* frame,u16 frame_length)
    free(TC->frame);
}
void CCSDS_void_ReleaseCopyOfTypeBCFrame(u8 VCID)
{
    u8 i=0;
    for (i=0;i<VirtualChannels[VCID].sent_queue_counter;i++)
    {
        if (GET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i))
        {
            if (CCSDS_FOP_DEBUG)
                printf("BC frame has been released\n");
            CCSDS_void_RemoveFrameFromSentQueue(i,VCID);
            break;
        }
    }

}
void CCSDS_void_RemoveFrameFromSentQueue(u8 Frame_number,u8 VCID)
{
    u8 i=0;
    u32 Len=0;
    for (i=0;i<Frame_number;i++)
    {
        Len+=VirtualChannels[VCID].sent_queue_frame_lengths[i];
    }
    if (VirtualChannels[VCID].sent_queue_counter>1)
    {
        //queue_copy=(u8*)malloc(VirtualChannels[VCID].SentQueue_Length);
        //memset(queue_copy,0,VirtualChannels[VCID].SentQueue_Length);
        /*Save a copy of the sent queue*/
        //memcpy(queue_copy,VirtualChannels[VCID].SentQueue,VirtualChannels[VCID].SentQueue_Length);
        //*decrease the queue length by the length of the removed frame*//
        VirtualChannels[VCID].SentQueue_Length-=
            VirtualChannels[VCID].sent_queue_frame_lengths[Frame_number];
        //*realloc the sent queue by the new length*//
        //free(VirtualChannels[VCID].SentQueue);
        //VirtualChannels[VCID].SentQueue=(u8*)malloc(VirtualChannels[VCID].SentQueue_Length);
        //*remove the frame copy from the queue*//
        /*if (Len)
            memcpy(VirtualChannels[VCID].SentQueue,queue_copy,Len);*/
        memcpy(VirtualChannels[VCID].SentQueue+Len,VirtualChannels[VCID].SentQueue+Len+
               VirtualChannels[VCID].sent_queue_frame_lengths[Frame_number],
               VirtualChannels[VCID].SentQueue_Length-Len);

        VirtualChannels[VCID].SentQueue=(u8*)realloc(VirtualChannels[VCID].SentQueue,
                                                     VirtualChannels[VCID].SentQueue_Length);
        //*remove the length of the removed frame from the lengths of the queue*/
        memcpy(VirtualChannels[VCID].sent_queue_frame_lengths+Frame_number,
            VirtualChannels[VCID].sent_queue_frame_lengths+Frame_number+1,
            (VirtualChannels[VCID].sent_queue_counter-Frame_number-1)*sizeof(u32));
        //*realloc the lengths of the frames in the queue*//
        VirtualChannels[VCID].sent_queue_frame_lengths=
            (u32*)realloc(VirtualChannels[VCID].sent_queue_frame_lengths,
            (VirtualChannels[VCID].sent_queue_counter-1)*sizeof(u32));
        //*decrease the number of the frames in the queue by one*//
        VirtualChannels[VCID].sent_queue_counter--;
        for (i=Frame_number;i<VirtualChannels[VCID].sent_queue_counter;i++)
        {
            if (GET_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,(i+1)))
            {
                SET_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i);
            }
            else
            {
                CLR_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,i);
            }
        }
        for (i=Frame_number;i<VirtualChannels[VCID].sent_queue_counter;i++)
        {
            if (GET_BIT((VirtualChannels[VCID].sent_queue_frame_type),(i+1)))
            {
                SET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i);
            }
            else
            {
                CLR_BIT(VirtualChannels[VCID].sent_queue_frame_type,i);
            }
        }
        //free(queue_copy);
    }
    else
    {
        free(VirtualChannels[VCID].SentQueue);
        free(VirtualChannels[VCID].sent_queue_frame_lengths);
        //VirtualChannels[VCID].sent_queue_frame_lengths=(u32*)malloc(sizeof(u32));
        //VirtualChannels[VCID].sent_queue_frame_lengths[0]=0;
        VirtualChannels[VCID].sent_queue_counter=0;
        VirtualChannels[VCID].SentQueue_Length=0;
        VirtualChannels[VCID].sent_queue_frame_type=0;
        VirtualChannels[VCID].ToBeRetramittedFlag=0;
    }
}
void CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(u8 RecNofR,u8 VCID)
{
    u8 i=0;
    for (i=0;i<VirtualChannels[VCID].sent_queue_counter;i++)
    {
        if (!(GET_BIT(VirtualChannels[VCID].sent_queue_frame_type,i)))
        {
            if (CCSDS_FOP_DEBUG)
                printf("Acknowledged AD Frame has been removed from sent queue\n");
            CCSDS_void_RemoveFrameFromSentQueue(i,VCID);
            VirtualChannels[VCID].NNR++;
            if (VirtualChannels[VCID].NNR==RecNofR)
            {
                break;
            }
            i--;
        }
    }
    VirtualChannels[VCID].TrasmissionCount=1;
}
void CCSDS_void_TRANSMITTAD(CCSDS_TC* TC,u8 VCID)
{
    TC->N_of_S=VirtualChannels[VCID].V_of_S;
    VirtualChannels[VCID].V_of_S++;
    TC->BypassFlag=0;
    TC->ControlCommandFlag=0;
    TC->first_byte_pointer_TC_Data=(u8*)malloc(VirtualChannels[VCID].wait_queue_FDU_length);
    memcpy(TC->first_byte_pointer_TC_Data,VirtualChannels[VCID].WaitQueue,VirtualChannels[VCID].wait_queue_FDU_length);
    TC->length_TC_Data=VirtualChannels[VCID].wait_queue_FDU_length;
    CCSDS_void_TC_Framing(TC);
    /*add a copy of the frame to sent queue*/
    CCSDS_void_AddFrametoSentQueue(TC->frame,TC->frame_length,0,VCID);
    CCSDS_void_Start_Timer(VCID);
    VirtualChannels[VCID].ADOutFlag=CCSDS_NOT_READY;
    ////////////////////////////////////////////////////////
    u32 FLen = TC->frame_length;
    u8* F = (u8*)malloc(sizeof(u8)*FLen);
    memcpy(F,TC->frame,FLen);
    TransmitFrame(F,FLen,1,VCID);


    //CCSDS_lengthof_copy_frame_tolowerleyer=TC->frame_length;
    //CCSDS_copy_frame_tolowerleyer=(u8*)malloc(CCSDS_lengthof_copy_frame_tolowerleyer);
    //memcpy(CCSDS_copy_frame_tolowerleyer,TC->frame,TC->frame_length);
    /**0 if no frame to be transmittted
       1 if the transmitted frame is an AD
       2 if the transmitted frame is a BD
       3 if the transmitted frame is a BC*/
    //CCSDS_transmitte_frame_tybe=1;
    /*a copy of the frame is passed to the lower procedure to be transmitted*/
    //CCSDS_void_transmitting(u8* frame,u32 frame_length)
    free(TC->first_byte_pointer_TC_Data);
    free(TC->frame);
}
void CCSDS_void_AddFrametoSentQueue(u8* Frame,u32 FrameLength,u8 FrameType,u8 VCID)
{
    if (VirtualChannels[VCID].sent_queue_counter<=0)
    {
        VirtualChannels[VCID].SentQueue=(u8*)malloc(FrameLength);
        memcpy(VirtualChannels[VCID].SentQueue,Frame,FrameLength);
        VirtualChannels[VCID].sent_queue_counter=1;
        VirtualChannels[VCID].sent_queue_frame_lengths=(u32*)malloc(sizeof(u32));
        VirtualChannels[VCID].sent_queue_frame_lengths[0]=FrameLength;
        CLR_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,0);
        if (FrameType)
            SET_BIT(VirtualChannels[VCID].sent_queue_frame_type,0);
        else
            CLR_BIT(VirtualChannels[VCID].sent_queue_frame_type,0);
        VirtualChannels[VCID].sent_queue_frame_type|=FrameType;
        VirtualChannels[VCID].SentQueue_Length=FrameLength;
        VirtualChannels[VCID].TrasmissionCount=1;
    }
    else
    {
        VirtualChannels[VCID].SentQueue_Length+=FrameLength;
        VirtualChannels[VCID].SentQueue=(u8*)realloc(VirtualChannels[VCID].SentQueue,VirtualChannels[VCID].SentQueue_Length*sizeof(u8));
        memcpy(VirtualChannels[VCID].SentQueue+(VirtualChannels[VCID].SentQueue_Length-FrameLength),Frame,FrameLength);
        VirtualChannels[VCID].sent_queue_counter++;
        VirtualChannels[VCID].sent_queue_frame_lengths=(u32*)realloc(VirtualChannels[VCID].sent_queue_frame_lengths,VirtualChannels[VCID].sent_queue_counter*sizeof(u32));
        VirtualChannels[VCID].sent_queue_frame_lengths[VirtualChannels[VCID].sent_queue_counter-1]=FrameLength;
        CLR_BIT(VirtualChannels[VCID].ToBeRetramittedFlag,(VirtualChannels[VCID].sent_queue_counter-1));
        if (FrameType)
        {
            SET_BIT(VirtualChannels[VCID].sent_queue_frame_type,(VirtualChannels[VCID].sent_queue_counter-1));
        }
        else
        {
            CLR_BIT(VirtualChannels[VCID].sent_queue_frame_type,(VirtualChannels[VCID].sent_queue_counter-1));
        }
    }
}
void CCSDS_void_TRANSMITTBD(CCSDS_TC* TC,u8 VCID)
{
    TC->BypassFlag=1;
    TC->ControlCommandFlag=0;
    /*set BD out flag to not ready*/
    VirtualChannels[VCID].BDOutFlag=0;
    CCSDS_void_TC_Framing(TC);

    u32 FLen = TC->frame_length;
    u8* F = (u8*)malloc(sizeof(u8)*FLen);
    memcpy(F,TC->frame,FLen);
    TransmitFrame(F,FLen,2,VCID);

    //CCSDS_lengthof_copy_frame_tolowerleyer=TC->frame_length;
    //CCSDS_copy_frame_tolowerleyer=(u8*)malloc(CCSDS_lengthof_copy_frame_tolowerleyer);
    //memcpy(CCSDS_copy_frame_tolowerleyer,TC->frame,TC->frame_length);
    /**0 if no frame to be transmittted
       1 if the transmitted frame is an AD
       2 if the transmitted frame is a BD
       3 if the transmitted frame is a BC*/
    //CCSDS_transmitte_frame_tybe=2;
    /*passing a copy of the transfer frame to the lower procedure*/
    //CCSDS_void_transmitting(u8* frame,u32 frame_length);
}


