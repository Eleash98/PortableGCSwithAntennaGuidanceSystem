/*
 * DL_program.c
 *
 *  Created on: Jan 26, 2021
 *      Author: MohammedGamalEleish
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "AX25_interface.h"
//#include "PH_interface.h"
//#include "PH_flags.h"
#include "Timer_interface.h"
#include "Timer_flags.h"

#include "PRINT_interface.h"

#include "DL_interface.h"
#include "DL_private.h"
#include "DL_flags.h"
#include "Test.h"
//#include "UART.h"

#include "MDL_flags.h"
#include "MDL_interface.h"

#include "DATALINK_interface.h"

/*
char MYAddress[7] = "3leash";
u8 MySSID;
char DestAddress[7] = "Eslam ";
u8 DestSSID;

*/

//Rec
/*****************functions*****************/

void DL_voidSetControlBits(u8 ControlBits){

    if(ControlBits == 8 || ControlBits != 16){
        AX25_voidSetControlModulo(AX25_8BITS);
        K = 4;
    }
    else{
        AX25_voidSetControlModulo(AX25_16BITS);
        K = 32;
    }

}

void Transmit(u8 *Frame, u32 FrameLength){

    u8* F = (u8*)malloc(sizeof(u8)*FrameLength);
    u32 Len = FrameLength;
    memcpy(F,Frame,Len);
    TransmitFrame(F,Len,0,0);
    /*
    static int i=0;
    static u8 first=255;
    printf("Transmitting: ");
    frame_t* f = (frame_t*)malloc(sizeof(frame_t));
    f->PtrtoFrame = (u8*)malloc(sizeof(u8)*FrameLength);
    memcpy(f->PtrtoFrame,Frame,FrameLength);
    f->FrameLength = FrameLength;
    AX25_u8DeConstructFrame(f);
    PrintFrameType(f);
    if (f->FrameType == AX25_I_FRAME){
        printf(" %d\n",i++);
        if (first != f->Info[2]){
        printf("new Info\n");
        first = f->Info[0];
        }
    }
    AX25_voidFreeFrame(f);*/
    //UART_voidWriteArray(Frame,FrameLength);
    //usleep(FrameLength*1000);
}


void DL_voidInit(void)
{
	//IQueue = (frame_t**)malloc(sizeof(frame_t*));
	NofRepeaters = 0;
	DL_Internal.T1V = AX25Timer_u32GetTime(AX25_TIMER_T1);
	AX25Timer_voidSetTime(AX25_TIMER_T1,DL_Internal.T1V);
	DL_Internal.ACKPending = DL_FLAG_CLEAR;
	DL_Internal.Layer3Init = DL_FLAG_CLEAR;
	DL_Internal.OwnReceiverBusy = DL_FLAG_CLEAR;
	DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
	DL_Internal.SRT = DL_Internal.T1V;
	SREJ_Enable = DL_FLAG_SET;
	SeizeConfirm = DL_FLAG_CLEAR;
	Layer3Protocol = No_layer_3_protocol_implemented;
    //DL_Internal.
}

void DL_voidPushRequest(){
    s32 i = 0;
    for (i=0;i<NofSegments;i++){
        frame_t* f = AX25_frame_tPtrCreatIFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 0,Layer3Protocol,SegmentertoDL[i],SegmentsLengths[i]);
        DL_voidPushIQueue(f);
        //free(SegmentertoDL[i]);
        //memcpy(SegmentertoDL,SegmentertoDL+1,(SegmentertoDL_Len-1)*sizeof(frame_t*));
        //SegmentertoDL_Len--;
    }
    free(SegmentertoDL);
    free(SegmentsLengths);
    NofSegments = 0;

}

void DL_voidPushIQueue(frame_t*Frame)
{
    if(IQueueCounter == 0)
    {
        IQueue = (frame_t**)malloc(sizeof(frame_t*));
        IQueueCounter++;
    }
    else
    {
        IQueueCounter++;
        IQueue = (frame_t**)realloc(IQueue,sizeof(frame_t*)*IQueueCounter);
    }
	IQueue[IQueueCounter-1] = Frame;
	//IQueue = (frame_t**)realloc(IQueue,sizeof(frame_t*)*IQueueCounter);
}

frame_t* DL_PopSentFramefromNR(u8 Copy_u8NR){
    for(int i=0;i<SentQueueCounter;i++)
    {
        u8 FrameNR = SentQueue[i]->NR;
        if (FrameNR == Copy_u8NR)
            return SentQueue[i];
    }
    printf("////////////////////////////////////\nNR Not Found\n//////////////////////////\n");
    return 0;
    //return SentFrames[Copy_u8NR-1].Frame;
}


void DL_voidPushSentQueue(frame_t* IFrame){
    if(SentQueueCounter == 0)
    {
        SentQueue = (frame_t**)malloc(sizeof(frame_t*));
        SentQueueCounter++;
    }
    else
    {
        SentQueueCounter++;
        SentQueue = (frame_t**)realloc(SentQueue,sizeof(frame_t*)*SentQueueCounter);
    }
    SentQueue[SentQueueCounter-1] = IFrame;
    //SentQueue[SentQueueCounter-1].ACKState = DL_IFRAME_NOT_ACK;
}

void DL_FreeSentQueue(u8 FrameNR){

    u8 AckFlag = 0;
    u32 i = 0;
    for (i = 0;i < SentQueueCounter; i++ ){
        if(SentQueue[i]->NR >= FrameNR)
            break;
        AckFlag = 1;
        AX25_voidFreeFrame(SentQueue[i]);
    }

    if(AckFlag == 1)
    {
        SentQueueCounter -= i;
        memcpy(SentQueue,SentQueue+i,sizeof(frame_t*)*SentQueueCounter);
        SentQueue = (frame_t**)realloc(SentQueue,SentQueueCounter);
    }
    /*else
    {
        for (i = 0;i < SentQueueCounter; i++ ){
        if(SentQueue[i]->NR == FrameNR-1){
            AckFlag = 2;
            AX25_voidFreeFrame(SentQueue[i]);
            break;
            }
        }
        if(AckFlag == 2){
        SentQueueCounter--;
        memcpy(SentQueue+i,SentQueue+i+1,(SentQueueCounter-i)*sizeof(frame_t);
        SentQueue = (frame_t**)realloc(SentQueue,SentQueueCounter);
        }
    }

*/
}


frame_t* DL_frame_tPtrPopIQueue(void)
{
	frame_t* F = IQueue[0];
	ShiftIQueue();
	return F;
}

void DL_voidPushReceivedFrame(frame_t* Frame){

    if(ReceivedQueueCounter == 0){
        ReceivedQueue = (frame_t**)malloc(sizeof(frame_t*));
        ReceivedQueueCounter++;
    }
    else
    {
        ReceivedQueueCounter++;
        ReceivedQueue = (frame_t**)realloc(ReceivedQueue,sizeof(frame_t*));
    }
    ReceivedQueue[ReceivedQueueCounter-1] = Frame;

}

frame_t* DL_GetReceivedFrameFromNS(u8 FrameNS){
    frame_t* f = NULL;
    for(int i = 0; i < ReceivedQueueCounter;i++)
    {
        if(ReceivedQueue[i]->NS == FrameNS)
        {
            f = ReceivedQueue[i];
            ReceivedQueueCounter--;
            memcpy(ReceivedQueue+i,ReceivedQueue+i+1,ReceivedQueueCounter-i);
        }

    }
    return f;


}

/*
void ShiftReceivedQueue(s32 Shift){
    AX25_voidFreeFrame(ReceivedFrames[Shift]);
    memcpy(ReceivedFrames+Shift,ReceivedFrames+Shift+1,(ReceivedFramesCounter-1)*sizeof(*ReceivedFrames));
    ReceivedFramesCounter--;
    ReceivedFrames = (frame_t**)realloc(ReceivedFrames,sizeof(*ReceivedFrames));
}

*/
void ShiftIQueue(void)
{
	memcpy(IQueue,IQueue+1,(IQueueCounter-1)*sizeof(frame_t*));
	IQueueCounter--;
	IQueue = (frame_t**)realloc(IQueue,sizeof(frame_t*)*IQueueCounter);
}

void DL_voidDiscardQueue (frame_t** Q,s32 *Length)
{
#if DEBUGGING == 1
printf("Discarding I Queue...\n");
#endif // DEBUGGING
    for (u32 i = 0; i < *Length;i++){
        AX25_voidFreeFrame(Q[i]);
    }
    *Length = 0;
    free(Q);
}

void DL_voidExit(void){
    for (int i = 0; i< IQueueCounter; i++){
        AX25_voidFreeFrame(IQueue[i]);
    }
    free(IQueue);
    for (int i = 0; i < SentQueueCounter; i++){
        AX25_voidFreeFrame(SentQueue[i]);
    }
    free(SentQueue);
    for (int i = 0; i < ReceivedQueueCounter; i++){
        AX25_voidFreeFrame(ReceivedQueue[i]);
    }
    free(ReceivedQueue);

}

void DL_SendUnitData(void){
    for(u32 i = 0;i<NofSegments;i++){
        frame_t*UIFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 0, Unnumbered_Information, SegmentertoDL[i], SegmentsLengths[i]);
        free(UIFrame);
        free(SegmentertoDL[i]);
        //DL_voidDiscardQueue(SegmentertoDL,&SegmentertoDL_Len);
        //////////////////////////////////////////////////////////
        //DL_oPhysical = UIFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        Transmit(UIFrame->PtrtoFrame,UIFrame->FrameLength);
        AX25_voidFreeFrame(UIFrame);
		}
    free(SegmentertoDL);
    free(SegmentsLengths);
    NofSegments = 0;
}

void DL_FreeSegmenterQueue(void)
{
    for (u32 i=0;i<NofSegments;i++){
        free(SegmentertoDL[i]);
    }
    free(SegmentertoDL);
    free(SegmentsLengths);
    NofSegments = 0;
}

u8 FirstConnection = 0;

/*****************Main function***************************/
void SM_voidDataLinkLayer(void)
{
#if DEBUGGING == 1
printf("In Data Link Main Function\n");
printf("++++++++++++++++External Requests+++++++++++++++++++\n");
char *Flag;
Flag = DL_Request.ConnectRequest == DL_FLAG_SET? "SET":"CLEARD";
printf("Connect Request is %s (Data Link)\n",Flag);
Flag = DL_Request.DisconnectRequest == DL_FLAG_SET? "SET":"CLEARD";
printf("Disconnect Request is %s (Data Link)\n",Flag);
Flag = DL_Request.DataRequest == DL_FLAG_SET? "SET":"CLEARD";
printf("Data Request is %s (Data Link)\n",Flag);
Flag = DL_Request.UnitDataRequest == DL_FLAG_SET? "SET":"CLEARD";
printf("Unit Data Request is %s (Data Link)\n",Flag);
Flag = DL_Request.FlowoffRequest == DL_FLAG_SET? "SET":"CLEARD";
printf("Flow Off Request is %s (Data Link)\n",Flag);
Flag = DL_Request.FlowonRequest == DL_FLAG_SET? "SET":"CLEARD";
printf("Flow On Request is %s (Data Link)\n",Flag);
printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
printf("----------------Internal Flags------------------------\n");
Flag = DL_Internal.ACKPending == DL_FLAG_SET? "SET":"CLEARD";
printf("ACK Pending is %s (Data Link)\n",Flag);
Flag = DL_Internal.Layer3Init == DL_FLAG_SET? "SET":"CLEARD";
printf("Layer 3 Initiated is %s (Data Link)\n",Flag);
Flag = DL_Internal.OwnReceiverBusy == DL_FLAG_SET? "SET":"CLEARD";
printf("Own Receiver Busy is %s (Data Link)\n",Flag);
Flag = DL_Internal.PeerReceiverBusy== DL_FLAG_SET? "SET":"CLEARD";
printf("Peer Receiver Ready is %s (Data Link)\n",Flag);
Flag = DL_Internal.RejectException == DL_FLAG_SET? "SET":"CLEARD";
printf("RejectException is %s (Data Link)\n",Flag);
printf("-------------------------------------------------------\n\n");
#endif
/*
    if(PH_Indication.BusyIndication == PH_FLAG_SET){
        //Suspend All Link Layer Timing
        AX25Timer_voidStop(AX25_TIMER_T1);
        AX25Timer_voidStop(AX25_TIMER_T2);
        AX25Timer_voidStop(AX25_TIMER_T3);
        PH_Indication.BusyIndication = PH_FLAG_CLEAR;
        return;
    }
    else if (PH_Indication.QuietIndication == PH_FLAG_SET)
    {
        AX25Timer_voidStart(AX25_TIMER_T1);
        AX25Timer_voidStart(AX25_TIMER_T2);
        AX25Timer_voidStart(AX25_TIMER_T3);
        PH_Indication.QuietIndication = PH_FLAG_CLEAR;
        return;
    }
*/
    static u8 Local_Current = SM_DL_STATE_DISCONNECTED;

    if(Local_Current != DL_CurrentState){
        printf("Preveous ");
        PrintState(DATA_LINK,Local_Current);
        printf("Next ");
        PrintState(DATA_LINK,DL_CurrentState);
        Local_Current = DL_CurrentState;
    }

    switch(DL_CurrentState)
    {
        case SM_DL_STATE_AWAITING_CONNECTION:
            if (DL_AwaitingConnectionVersion == DL_AWAITING_CONNECTION_2_2)
                DL_AwaitingConnection2_2();
            else
                DL_AwaitingConnectionState();
            break;
        case SM_DL_STATE_AWAITING_RELEASE:
            DL_AwaitingReleaseState();
            break;
        case SM_DL_STATE_CONNECTED:
            DL_ConnectedState();
            break;
        case SM_DL_STATE_DISCONNECTED:
            DL_DisconnectState();
            break;
        case SM_DL_STATE_TIMER_RECOVERY:
            DL_TimerRecoveryState();
            break;
        default:
            break;
    }
#if DEBUGGING == 1
PrintState(DATA_LINK,DL_CurrentState);
#endif //
}




/**********************STATES functions*********************/
void DL_DisconnectState(void)
{

    FirstConnection = 0;
#if DEBUGGING == 1
    printf("In Disconnected State (Data Link)\n");
#endif
	u8 ControlModulo = AX25_u8ControlModulo();

	if(DL_Request.DisconnectRequest == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Disconnect Request Received from Layer 3 (Data Link)\n");
#endif

		DL_Indication.DisConnectConfirm = DL_FLAG_SET;
		DL_CurrentState = SM_DL_STATE_DISCONNECTED;
	}
	else if(DL_Request.UnitDataRequest == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Unit Data Request Received from Layer 3 (Data Link)\n");
#endif

		/////////////////////////////////////////////////////////
		DL_SendUnitData();
		DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		DL_Request.UnitDataRequest = DL_FLAG_CLEAR;
	}
	else if(DL_Request.ConnectRequest == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Connect Request Received from Layer 3 (Data Link)\n");
#endif
		DL_Internal.SRT =	DefaultSRT;
		DL_Internal.T1V = 2 * DL_Internal.SRT;
		DL_EstablishDataLink();
		DL_Internal.Layer3Init = DL_FLAG_SET;
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		DL_Request.ConnectRequest = DL_FLAG_CLEAR;
	}
	else if((DL_Request.DataRequest == DL_FLAG_SET) ||(DL_Request.FlowoffRequest == DL_FLAG_SET)
				|| (DL_Request.FlowonRequest == DL_FLAG_SET))
	{
#if DEBUGGING == 1
    printf("Other Request Received from Layer 3 (Data Link)\n");
#endif\

        if(DL_Request.DataRequest == DL_FLAG_SET)
            DL_FreeSegmenterQueue();
            //DL_voidDiscardQueue (SegmentertoDL,&SegmentertoDL_Len);

		DL_CurrentState = SM_DL_STATE_DISCONNECTED;

		DL_Request.DataRequest = DL_FLAG_CLEAR;
		DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
        DL_Request.FlowonRequest = DL_FLAG_CLEAR;
	}
	else if (Hardware.Frame == PH_FLAG_SET)
	{

#if DEBUGGING == 1
    printf("Data Indication Received from Physical (Data Link)\n");
#endif
		frame_t* Frame = HardwaretoDL;
		u8 Error = DL_u8CheckErrors(ControlModulo, Frame);

		/***************/
		if(Error == 0)
            PrintFrameType(Frame);
		/*******************/

		if (Error != 0)
        {
            //AX25_voidFreeFrame(Frame);
            if(Error == 255)
            {
#if DEBUGGING == 1
    printf("CRC Error (Data Link)\n");
#endif
                /*CRC error, Discard frame*/

            }
            else if(Error ==1 || Error == 2)
            {
#if DEBUGGING == 1
    printf("Control Field Error (Data Link)\n");
#endif
                DL_Indication.ErrorIndication = L;
            }
            else if(Error == 3 || Error == 4)
            {
#if DEBUGGING == 1
    printf("Info Length is not Permitted (Data Link)\n");
#endif
                DL_Indication.ErrorIndication = M;
            }
            else if(Error == 5 || Error == 6)
            {
#if DEBUGGING == 1
    printf("Missing S or U frame Info (Data Link)\n");
#endif
                DL_Indication.ErrorIndication = N;
            }
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
		else if(Frame->FrameType == AX25_U_FRAME && Frame->ControlFieldType == Unnumbered_Acknowledge)
		{
#if DEBUGGING == 1
    printf("U frame, Unnumbered ACK (Data Link)\n");
#endif
			DL_Indication.ErrorIndication = (C<<8)||(D);
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}
		else if(Frame->FrameType == AX25_U_FRAME && Frame->ControlFieldType == Disconnect_Mode)
		{
#if DEBUGGING == 1
    printf("U frame, Disconnect Mode (Data Link)\n");
#endif
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}
		else if(Frame->FrameType == AX25_U_FRAME && Frame->ControlFieldType == Unnumbered_Information)
		{
#if DEBUGGING == 1
    printf("U frame, Unnumbered Information (Data Link)\n");
#endif
			DL_UICheck(Frame);
			if(Frame->PollFinal == 1)
			{
				frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 1, Disconnect_Mode, 0, 0);
				Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
				AX25_voidFreeFrame(DMFrame);

				//DLtoPhysical = DMFrame;
				//PH_Request.DataRequest = PH_FLAG_SET;
			}
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}
		else if(Frame->FrameType == AX25_U_FRAME && Frame->ControlFieldType == Disconnect)
		{
#if DEBUGGING == 1
    printf("U frame, Disconnect (Data Link)\n");
#endif
			frame_t *DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Disconnect_Mode, 0,0);
			Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
			//DLtoPhysical = DMFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}

		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
        {
            SM_voidManagmentDataLink();
        }

		else if((Frame->FrameType == AX25_U_FRAME) && ((Frame->ControlFieldType == Set_Async_Balanced_mode)
												||(Frame->ControlFieldType == Set_Async_Balanced_mode_Extended)))
		{
#if DEBUGGING == 1
    printf("U frame, SABME or SABM (Data Link)\n");
#endif
            /**||||||||||||||||||||||||||||||||||||**/
            /**||||||||||||||||||||||||||||||||||||**/
            /**||||||||||||||||||||||||||||||||||||**/
            /**||||||||||||||||||||||||||||||||||||**/
			if (0)///Not Able To Establish
			{
#if DEBUGGING == 1
    printf("Not Able to establish (Data Link)\n");
#endif
				frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Disconnect_Mode, 0, 0);
				Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
				AX25_voidFreeFrame(DMFrame);
				//DLtoPhysical = DMFrame;
				//PH_Request.DataRequest = PH_FLAG_SET;
				DL_CurrentState = SM_DL_STATE_DISCONNECTED;
			}
			else
			{
#if DEBUGGING == 1
    printf("Able to Establish (Data Link)\n");
#endif
				if(Frame->ControlFieldType == Set_Async_Balanced_mode)
					DL_SetVersion2_0();
				else
					DL_SetVersion2_2();
				frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Unnumbered_Acknowledge, 0, 0);
				Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
				AX25_voidFreeFrame(UAFrame);
				//DLtoPhysical = UAFrame;
				//PH_Request.DataRequest = PH_FLAG_SET;
				DL_ClearExceptionConditions();
				VS = 0;
				VA = 0;
				VR = 0;
				DL_Indication.ConnectIndication = DL_FLAG_SET;
				DL_Internal.SRT =	DefaultSRT;
				DL_Internal.T1V = 2 * DL_Internal.SRT;
				AX25Timer_voidStart(AX25_TIMER_T3);
				DL_CurrentState = SM_DL_STATE_CONNECTED;

				//Test_voidExit();
				//PH_Indication.DataIndication = PH_FLAG_CLEAR;
			}
		}
		else if(((Frame->FrameType == AX25_U_FRAME ) &&((Frame->ControlFieldType == Test)
				||(Frame->ControlFieldType == Exchange_Identification)
				|| (Frame->ControlFieldType == Frame_Reject))) || (Frame->FrameType == AX25_S_FRAME))
		{
#if DEBUGGING == 1
    printf("U frame, Other Type (Data Link)\n");
#endif
			frame_t *DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Disconnect_Mode, 0,0);
			Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
			//DLtoPhysical = DMFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}
		AX25_voidFreeFrame(Frame);
		Hardware.Frame = PH_FLAG_CLEAR;
	}
	/*else if(PH_Indication.BusyIndication == PH_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Busy Indication Received from Physical (Data Link)\n");
#endif
		AX25Timer_voidStop(AX25_TIMER_T1);
		AX25Timer_voidStop(AX25_TIMER_T3);
		DL_CurrentState = SM_DL_STATE_DISCONNECTED;
	}
	else if(PH_Indication.QuietIndication == PH_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Quiet Indication Received from Physical (Data Link)\n");
#endif
		AX25Timer_voidStart(AX25_TIMER_T1);
		AX25Timer_voidStart(AX25_TIMER_T3);
		DL_CurrentState = SM_DL_STATE_DISCONNECTED;
	}*/
	/*else if(PH_Indication.SiezeConfirm == PH_FLAG_SET || PH_Indication.DataIndication == PH_FLAG_SET)
	{
	    PH_Indication.SiezeConfirm = PH_FLAG_CLEAR;
	    PH_Indication.DataIndication = PH_FLAG_CLEAR;
#if DEBUGGING == 1
    printf("Seize Confirm Received from Physical (Data Link)\n");
#endif
		DL_CurrentState = SM_DL_STATE_DISCONNECTED;
	}*/
	//DL_voidClearRequests();
	return;
}

void DL_AwaitingConnectionState(void)
{
#if DEBUGGING == 1
    printf("In Awaiting Connection State (Data Link)\n");
#endif
    /*DL_Connect_Request*/
    if(DL_Request.ConnectRequest == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Connect Request Received from Layer 3 (Data Link)\n");
#endif
		/*Discard Queue*/
        DL_voidDiscardQueue(IQueue,&IQueueCounter);

		DL_Internal.Layer3Init = DL_FLAG_SET;
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		DL_Request.ConnectRequest = DL_FLAG_CLEAR;
	}
	/*DL_Disconnect_Request*/
    else if(DL_Request.DisconnectRequest == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Disconnect Request Received from Layer 3 (Data Link)\n");
#endif
printf("shit\n");
		/*Requeue*/
		/*Cuboid mean save a signal until a new state is reached*/
		/*I think that we can't clear flags in this state*/
		DL_Request.DisconnectRequest = DL_FLAG_CLEAR;
	}
    /*DL_Data_Request and Frame Pop off queue*/
    else if((DL_Request.DataRequest == DL_FLAG_SET )) //I don't know how to check on frame pop off queue
    {
#if DEBUGGING == 1
    printf("Data Request Received from Layer 3 (Data Link)\n");
#endif
	   if(DL_Internal.Layer3Init == DL_FLAG_CLEAR)
	   {
		   /*Push Frame on Queue*/
		   DL_voidPushRequest();
	   }
	   DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
	   DL_Request.DataRequest = DL_FLAG_CLEAR;
    }
    /*DL_Unit_Data_Request*/
    else if(DL_Request.UnitDataRequest == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Unit Data Request Received from Layer 3 (Data Link)\n");
#endif

		DL_SendUnitData();
		//DLtoPhysical = UIFrame;
		//PH_Request.DataRequest = PH_FLAG_SET;
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		DL_Request.UnitDataRequest = DL_FLAG_CLEAR;
	}
    /*All Other Primitives "Requests"*/
	else if((DL_Request.FlowoffRequest == DL_FLAG_SET) || (DL_Request.FlowonRequest == DL_FLAG_SET))
	{
#if DEBUGGING == 1
    printf("Other Request Received from Layer 3 (Data Link)\n");
#endif
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
		DL_Request.FlowonRequest = DL_FLAG_SET;
	}
    /*Timer T1 Expire*/
	else if(Timer.T1_EXPIRE == DL_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Timer 1 Expired (Data Link)\n");
#endif
		if(RC == N2)
		{
			/*Discard Frame Queue*/
			DL_voidDiscardQueue(IQueue,&IQueueCounter);

			/*DL_Indication.ErrorIndication = "I don't know g";*/
			DL_Indication.ErrorIndication = I;
			DL_Indication.DisConnectIndication = DL_FLAG_SET;
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}
		else
		{
			RC = RC+1;
			frame_t* SABMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Set_Async_Balanced_mode, 0,0);
			Transmit(SABMFrame->PtrtoFrame,SABMFrame->FrameLength);
            AX25_voidFreeFrame(SABMFrame);
			//DLtoPhysical = SABMFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_SelectT1Value();
			AX25Timer_voidStart(AX25_TIMER_T1);
			DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		}
		Timer.T1_EXPIRE = DL_FLAG_CLEAR;
	}
    else if(Hardware.Frame == PH_FLAG_SET)
    {

#if DEBUGGING == 1
    printf("Data Indication Received From Physical (Data Link)\n");
#endif
    	frame_t* Frame = HardwaretoDL;
    	u8 ControlModulo = AX25_u8ControlModulo();
    	u8 Error = DL_u8CheckErrors(ControlModulo, Frame);
    	/***************/
		if(Error == 0){
            PrintFrameType(Frame);
            printf("In Awaiting Connection\n");
            }
		/*******************/
		if(Error != 0)
        {
            //AX25_voidFreeFrame(Frame);
            if(Error == 255)
            {
#if DEBUGGING == 1
    printf("CRC Error (Data Link)\n");
#endif
                /*CRC error, Discard frame*/

            }
            else if(Error ==1 || Error == 2)
            {
#if DEBUGGING == 1
    printf("Control Field Error (Data Link)\n");
#endif
                DL_Indication.ErrorIndication = L;
            }
            else if(Error == 3 || Error == 4)
            {
#if DEBUGGING == 1
    printf("Info Length is not Permitted (Data Link)\n");
#endif
                DL_Indication.ErrorIndication = M;
            }
            else if(Error == 5 || Error == 6)
            {
#if DEBUGGING == 1
    printf("Missing S or U frame Info (Data Link)\n");
#endif
                DL_Indication.ErrorIndication = N;
            }
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
		/*SABM "Set Asynchronous Balanced Mode"*/
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Set_Async_Balanced_mode))
		{
#if DEBUGGING == 1
    printf("U frame SABM (Data Link)\n");
#endif
			frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Unnumbered_Acknowledge, 0, 0);
			Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
            AX25_voidFreeFrame(UAFrame);
			//DLtoPhysical = UAFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		}
		/*DISC "Disconnect"*/
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect))
		{
#if DEBUGGING == 1
    printf("U frame Disconnect (Data Link)\n");
#endif
			frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal,Disconnect_Mode, 0,0);
			Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
			//DLtoPhysical = DMFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		}
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
        {
            SM_voidManagmentDataLink();
        }

		/*UI "Unnumbered_Information"*/
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Information))
		{
#if DEBUGGING == 1
    printf("U frame Unnumbered Information (Data Link)\n");
#endif
			DL_UICheck(Frame);
			if(Frame->PollFinal == DL_FLAG_SET)
			{
				frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 1, Disconnect_Mode, 0, 0);
				Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
				AX25_voidFreeFrame(DMFrame);
				//DLtoPhysical = DMFrame;
				//PH_Request.DataRequest = PH_FLAG_SET;
			}
			DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		}
		/*DM "Disconnect Mode"*/
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect_Mode))
		{
#if DEBUGGING == 1
    printf("U frame Disconnect Mode (Data Link)\n");
#endif
			if(Frame->PollFinal == DL_FLAG_SET)
			{
				/*Discard Frame Queue*/
				DL_Indication.DisConnectIndication = DL_FLAG_SET;
				AX25Timer_voidStop(AX25_TIMER_T1);
				DL_CurrentState = SM_DL_STATE_DISCONNECTED;
			}
			else
			{
				DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
			}
		}
		/*UA "Unnumbered_Acknowledge"*/
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Acknowledge))
		{
#if DEBUGGING == 1
    printf("U frame Unnumbered ACK (Data Link)\n");
#endif
			if(Frame->PollFinal == 1)
			{
				if(DL_Internal.Layer3Init == DL_FLAG_SET)
				{
					DL_Indication.ConnectConfirm = DL_FLAG_SET;
				}
				else
				{
					if(VS != VA)
					{
						/*Discard Frame Queue*/
						DL_Indication.ConnectIndication = DL_FLAG_SET;
					}
				}
				AX25Timer_voidStop(AX25_TIMER_T1);
				AX25Timer_voidStop(AX25_TIMER_T3);
				VS = 0;
				VA = 0;
				VR = 0;
				DL_SelectT1Value();
				DL_CurrentState = SM_DL_STATE_CONNECTED;
				if(FirstConnection == 0){
				MDL_NegotiateRequest.ControlModulo = AX25_XID_8BIT_MODULO;
				MDL_NegotiateRequest.K = 4;
				MDL_NegotiateRequest.N1 = 256;
				MDL_NegotiateRequest.RejectType = AX25_XID_SELECTIVE_REJECT;
				MDL_NegotiateRequest.Retry = 10;
				MDL_NegotiateRequest.T1 = 3000;
				MDL_NegotiateRequest.TransmissionMode = AX25_XID_HALFDUPLEX;

				MDL_Request.NegotiateRequest = MDL_FLAG_SET;
				SM_voidManagmentDataLink();
				FirstConnection = 1;
				}
			}
			else
			{
				DL_Indication.ErrorIndication = D;
				DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
			}

		}
		/*SABME "Set Asynchronous Balanced Mode Extended"*/
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Set_Async_Balanced_mode_Extended))
		{
#if DEBUGGING == 1
    printf("U frame SABME (Data Link)\n");
#endif
			frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Disconnect_Mode, 0,0);
			Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
			//DLtoPhysical = DMFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_AwaitingConnectionVersion = DL_AWAITING_CONNECTION_2_2;
			DL_AwaitingConnection2_2();
			AX25_voidFreeFrame(Frame);
			return;
		}
		else{
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
		}
		Hardware.Frame= PH_FLAG_CLEAR;
		AX25_voidFreeFrame(Frame);
    }
    /*All Other Primitives "From Physical"*/
    /*else if(PH_Indication.BusyIndication == PH_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Busy Indication From Physical (Data Link)\n");
#endif
		AX25Timer_voidStop(AX25_TIMER_T1);
		AX25Timer_voidStop(AX25_TIMER_T3);
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
	}
    else if(PH_Indication.QuietIndication == PH_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Quiet Indication From Physical (Data Link)\n");
#endif
		AX25Timer_voidStart(AX25_TIMER_T1);
		AX25Timer_voidStart(AX25_TIMER_T3);
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
	}
    else if(PH_Indication.SiezeConfirm == PH_FLAG_SET)
	{
#if DEBUGGING == 1
    printf("Seize Confirm From Physical (Data Link)\n");
#endif
		DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
	}*/

		///////////////////////////////////////////////////////////////////
	/*else if(IQueueCounter > 0)
	{
#if DEBUGGING == 1
    printf("Frame Pop Off Queue (Data Link)\n");
#endif
		frame_t* F = DL_frame_tPtrPopIQueue();
		if(DL_Internal.Layer3Init == DL_FLAG_CLEAR)
		   {
			   Push Frame on Queue
			   DL_voidPushIQueue(F);
		   }
		   DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
	}
	////////////////////////////////////////////////////////////////////
	*/
    DL_voidClearRequests();

}

void DL_AwaitingReleaseState(void)
{
    #if DEBUGGING == 1
    printf("In Awaiting Release State (DataLink)\n");
    #endif // DEBUGGING

	if(DL_Request.DisconnectRequest == DL_FLAG_SET)
	{
    #if DEBUGGING == 1
    printf("Disconnect Request Set (Data Link)\n");
    #endif // DEBUGGING


		frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Disconnect_Mode, 0, 0);
        Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
        AX25_voidFreeFrame(DMFrame);
		//DLtoPhysical = DMFrame;
		//PH_Request.ExpeditedDataRequest = PH_FLAG_SET;
		AX25Timer_voidStop(AX25_TIMER_T1);
		DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		DL_Request.DisconnectRequest = DL_FLAG_CLEAR;
	}
	else if(DL_Request.UnitDataRequest == DL_FLAG_SET)
	{
    #if DEBUGGING == 1
    printf("Unit Data Request Set (Data Link)\n");
    #endif // DEBUGGING


		DL_SendUnitData();
		//DLtoPhysical = UIFrame;
		//PH_Request.DataRequest = PH_FLAG_SET;
		AX25Timer_voidStop(AX25_TIMER_T1);
		DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		DL_Request.UnitDataRequest = DL_FLAG_CLEAR;
	}
	else if((DL_Request.ConnectRequest == DL_FLAG_SET) || (DL_Request.DataRequest == DL_FLAG_SET)
				|| (DL_Request.FlowoffRequest == DL_FLAG_SET) ||(DL_Request.FlowonRequest == DL_FLAG_SET))
	{
    #if DEBUGGING == 1
    printf("Connect /Data /Flow off/ Flow on Request (Data Link)\n");
    #endif // DEBUGGING

		DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		DL_Request.ConnectRequest = DL_FLAG_CLEAR;
		DL_Request.DataRequest = DL_FLAG_CLEAR;
		DL_FreeSegmenterQueue();
		//DL_voidDiscardQueue(SegmentertoDL,&SegmentertoDL_Len);
		//AX25_voidFreeFrame(SegmentertoDL[0]);
        //memcpy(SegmentertoDL,SegmentertoDL+1,SegmentertoDL_Len-1);
		//SegmentertoDL_Len--;
		DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
		DL_Request.FlowonRequest = DL_FLAG_CLEAR;
	}

	if(Timer.T1_EXPIRE == DL_FLAG_SET)
	{
    #if DEBUGGING == 1
    printf("Timer T1 Expired (Data Link)\n");
    #endif // DEBUGGING

		if(RC == N2)
		{

			DL_Indication.ErrorIndication = I;

			DL_Indication.DisConnectConfirm = DL_FLAG_SET;
			DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		}
		else
		{
			RC++;
			frame_t* DISCFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Disconnect, 0, 0);
			Transmit(DISCFrame->PtrtoFrame,DISCFrame->FrameLength);
            AX25_voidFreeFrame(DISCFrame);
			//DLtoPhysical = DISCFrame;
			//PH_Request.DataRequest = PH_FLAG_SET;
			DL_SelectT1Value();
			AX25Timer_voidStart(AX25_TIMER_T1);
			DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		}
		Timer.T1_EXPIRE = DL_FLAG_CLEAR;
	}
	else if(Hardware.Frame == PH_FLAG_SET)
	{

    #if DEBUGGING == 1
    printf("Data Indication from Physical (Data Link)\n");
    #endif // DEBUGGING

		u8 ControlModulo = AX25_u8ControlModulo();
		frame_t* Frame = HardwaretoDL;
		if(DL_u8CheckErrors(ControlModulo, Frame) != 0 )
			return;
        /***************/
        PrintFrameType(Frame);
		/*******************/
        if((Frame->FrameType == AX25_U_FRAME) && Frame->ControlFieldType == Set_Async_Balanced_mode)
		{
		    #if DEBUGGING == 1
            printf("U Frame SABM (Data Link)\n");
            #endif // DEBUGGING
			frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Disconnect_Mode, 0, 0);
			Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
			//DLtoPhysical = DMFrame;
			//PH_Request.ExpeditedDataRequest = PH_FLAG_SET;
			AX25Timer_voidStop(AX25_TIMER_T1);
			DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		}

        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
        {
            SM_voidManagmentDataLink();
        }
		else if((Frame->FrameType == AX25_U_FRAME) && Frame->ControlFieldType == Disconnect)
		{
		    #if DEBUGGING == 1
            printf("U Frame Disconnect (Data Link)\n");
            #endif // DEBUGGING
			frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Unnumbered_Acknowledge, 0, 0);
			Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
            AX25_voidFreeFrame(UAFrame);
			//DLtoPhysical = UAFrame;
			//PH_Request.ExpeditedDataRequest = PH_FLAG_SET;
			AX25Timer_voidStop(AX25_TIMER_T1);
			DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		}

		else if(Frame->FrameType != AX25_U_FRAME)
		{
            #if DEBUGGING == 1
            printf("Not U Frame (Data Link)\n");
            #endif // DEBUGGING
			if (Frame->PollFinal == 1)
			{
				frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 1, Disconnect_Mode, 0, 0);
				Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
				AX25_voidFreeFrame(DMFrame);
				//DLtoPhysical = DMFrame;
				//PH_Request.DataRequest = PH_FLAG_SET;
			}
			DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		}
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Information))
		{
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Information (Data Link)\n");
            #endif // DEBUGGING
			DL_UICheck(Frame);
			if (Frame->PollFinal == 1)
			{
				frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 1, Disconnect_Mode, 0, 0);
				Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
				AX25_voidFreeFrame(DMFrame);
				//DLtoPhysical = DMFrame;
				//PH_Request.DataRequest = PH_FLAG_SET;
			}
			DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
		}
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Acknowledge))
		{
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Ack (Data Link)\n");
            #endif // DEBUGGING
			if(Frame->PollFinal == 1)
			{
				DL_Indication.DisConnectConfirm = DL_FLAG_SET;
				AX25Timer_voidStop(AX25_TIMER_T1);
				DL_CurrentState = SM_DL_STATE_DISCONNECTED;
			}
			else
			{
				DL_Indication.ErrorIndication = D;
				DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
			}
		}
		else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect_Mode))
		{
            #if DEBUGGING == 1
            printf("U Frame Disconnect Mode (Data Link)\n");
            #endif // DEBUGGING
			if(Frame->PollFinal == 1)
			{
				AX25Timer_voidStop(AX25_TIMER_T1);

				DL_Indication.DisConnectConfirm = DL_FLAG_SET;

				DL_CurrentState = SM_DL_STATE_DISCONNECTED;
			}
			else
			{
				DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
			}
		}
		AX25_voidFreeFrame(Frame);
        Hardware.Frame = PH_FLAG_CLEAR;
	}
	/*
	else if(PH_Indication.BusyIndication == PH_FLAG_SET)
	{
        #if DEBUGGING == 1
        printf("Busy Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
		AX25Timer_voidStop(AX25_TIMER_T1);
		AX25Timer_voidStop(AX25_TIMER_T3);
		DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;

	}
	else if(PH_Indication.QuietIndication == PH_FLAG_SET)
	{
	    #if DEBUGGING == 1
        printf("Quiet Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
		AX25Timer_voidStart(AX25_TIMER_T1);
		AX25Timer_voidStart(AX25_TIMER_T3);
		DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
	}
	else if(PH_Indication.SiezeConfirm == PH_FLAG_SET)
	{
	    #if DEBUGGING == 1
        printf("Sieze confirm from Physical (Data Link)\n");
        #endif // DEBUGGING
		DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
	}*/
	DL_voidClearRequests();
}
void DL_ConnectedState(void)
{
    #if DEBUGGING == 1
    printf("In Connected State Data Link\n");
    #endif // DEBUGGING
    if(DL_Request.ConnectRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Connect Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_voidDiscardQueue(IQueue,&IQueueCounter);
        DL_EstablishDataLink();
        DL_Internal.Layer3Init = DL_FLAG_SET;
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        DL_Request.ConnectRequest = DL_FLAG_CLEAR;

    }
    else if(DL_Request.DisconnectRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Disconnect Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_voidDiscardQueue(IQueue,&IQueueCounter);
        RC = 0;
        frame_t* DISFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1,Disconnect,0,0);
        Transmit(DISFrame->PtrtoFrame,DISFrame->FrameLength);
        AX25_voidFreeFrame(DISFrame);
        //DLtoPhysical = DISFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        AX25Timer_voidStop(AX25_TIMER_T3);
        AX25Timer_voidStart(AX25_TIMER_T1);
        DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
        DL_Request.DisconnectRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.FlowoffRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Flow off Request Set (Data Link)\n");
        #endif // DEBUGGING
        if(DL_Internal.OwnReceiverBusy == DL_FLAG_CLEAR)
        {
            DL_Internal.OwnReceiverBusy = DL_FLAG_SET;
            frame_t* RNRFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 0,ReceiveNotReady, 0,0);
            Transmit(RNRFrame->PtrtoFrame,RNRFrame->FrameLength);
            AX25_voidFreeFrame(RNRFrame);
            //DLtoPhysical = RNRFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_Internal.ACKPending = DL_FLAG_CLEAR;
        }
        DL_CurrentState = SM_DL_STATE_CONNECTED;
        DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.FlowonRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Flow On Request Set (Data Link)\n");
        #endif // DEBUGGING
        if(DL_Internal.OwnReceiverBusy == DL_FLAG_SET)
        {
            DL_Internal.OwnReceiverBusy = DL_FLAG_CLEAR;
            frame_t* RRFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1,ReceiveReady, 0,0);
            Transmit(RRFrame->PtrtoFrame,RRFrame->FrameLength);
            AX25_voidFreeFrame(RRFrame);
            //DLtoPhysical = RRFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_Internal.ACKPending = DL_FLAG_CLEAR;
            if(Timer.T1_EXPIRE == PH_FLAG_SET)
            {
                AX25Timer_voidStop(AX25_TIMER_T3);
                AX25Timer_voidStart(AX25_TIMER_T1);
            }
        }
        DL_CurrentState = SM_DL_STATE_CONNECTED;
        DL_Request.FlowonRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.UnitDataRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Unit Data Request Set (Data Link)\n");
        #endif // DEBUGGING

        DL_SendUnitData();
        //DLtoPhysical = UIFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        DL_CurrentState = SM_DL_STATE_CONNECTED;
        DL_Request.UnitDataRequest = DL_FLAG_CLEAR;
    }

    else if((Timer.T1_EXPIRE == PH_FLAG_SET) || (Timer.T3_EXPIRE == PH_FLAG_SET))
    {

        #if DEBUGGING == 1
        printf("Timer T1/T3 Expired (Data Link)\n");
        #endif // DEBUGGING
        if(Timer.T1_EXPIRE)
        {
            Timer.T1_EXPIRE = DL_FLAG_CLEAR;
            RC =1;
        }
        else
        {
            Timer.T3_EXPIRE = DL_FLAG_CLEAR;
            RC =0;
        }
        DL_TransmitEnquiry();
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
    }
    else if(DL_Request.DataRequest == DL_FLAG_SET)
    {
        DL_voidPushRequest();
        DL_Request.DataRequest = DL_FLAG_CLEAR;
        DL_CurrentState = SM_DL_STATE_CONNECTED;
    }
    else if(Hardware.Frame == PH_FLAG_SET)
    {

        #if DEBUGGING == 1
        printf("Data Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        frame_t* Frame = HardwaretoDL;
        u8 ControlModulo = AX25_u8ControlModulo();
        u8 Error = DL_u8CheckErrors(ControlModulo,Frame);
        /***************/
		if(Error == 0)
            PrintFrameType(Frame);
		/*******************/
        if(Error != 0)
        {
        	if((Error == 1) || (Error == 2))
            {
                //Control Error
                DL_Indication.ErrorIndication = L;
            }
            else if ((Error ==3) || (Error ==4))
            {
                //Info Length error
                DL_Indication.ErrorIndication = M;
            }
            else
            {
                //U Frame or S Frame
                DL_Indication.ErrorIndication = N;
            }
            DL_voidDiscardQueue(IQueue,&IQueueCounter);
            DL_EstablishDataLink();
            DL_Internal.Layer3Init = DL_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
        {
            SM_voidManagmentDataLink();
        }
        else if((Frame->FrameType == AX25_U_FRAME) && ((Frame->ControlFieldType == Set_Async_Balanced_mode)
            || (Frame->ControlFieldType == Set_Async_Balanced_mode_Extended)))
        {
            #if DEBUGGING == 1
            printf("U Frame SABM/SABME (Data Link)\n");
            #endif // DEBUGGING
            frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Unnumbered_Acknowledge,0,0);
            Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
				AX25_voidFreeFrame(UAFrame);
            //DLtoPhysical = UAFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_ClearExceptionConditions();
            DL_Indication.ErrorIndication = F;
            if(VS != VA)
            {
                DL_voidDiscardQueue(IQueue,&IQueueCounter);
                DL_Indication.ConnectIndication = DL_FLAG_SET;
            }
            AX25Timer_voidStop(AX25_TIMER_T1);
            AX25Timer_voidStart(AX25_TIMER_T3);
            VS =0;
            VA =0;
            VR =0;
            DL_CurrentState = SM_DL_STATE_CONNECTED;
        }
        else if ((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect))
        {
            #if DEBUGGING == 1
            printf("U Frame Disconnect (Data Link)\n");
            #endif // DEBUGGING
            DL_voidDiscardQueue(IQueue,&IQueueCounter);
            frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal,Unnumbered_Acknowledge, 0,0);
            Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
				AX25_voidFreeFrame(UAFrame);
            //DLtoPhysical = UAFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_Indication.DisConnectIndication = DL_FLAG_SET;
            AX25Timer_voidStop(AX25_TIMER_T1);
            AX25Timer_voidStop(AX25_TIMER_T3);
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
        else if ((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Acknowledge))
        {
            #if DEBUGGING == 1
            printf("U Frame Unnumbered ACK (Data Link)\n");
            #endif // DEBUGGING
            DL_Indication.ErrorIndication = C;
            DL_EstablishDataLink();
            DL_Internal.Layer3Init = DL_FLAG_CLEAR;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect_Mode))
        {
            #if DEBUGGING == 1
            printf("U Frame Disconnect Mode (Data Link)\n");
            #endif // DEBUGGING
            DL_Indication.ErrorIndication = E;
            DL_Indication.DisConnectIndication = DL_FLAG_SET;
            DL_voidDiscardQueue(IQueue,&IQueueCounter);
            AX25Timer_voidStop(AX25_TIMER_T1);
            AX25Timer_voidStop(AX25_TIMER_T3);
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
        //else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Frame_Reject))
        //{
        //    #if DEBUGGING == 1
        //
        //   #endif // DEBUGGING
        //	///////////////////////////////////////////
        //    //DL_Indication.ErrorIndication = K;
        //    ////////////////////////////////////////////
        //    DL_EstablishDataLink();
        //    DL_Internal.Layer3Init = DL_FLAG_CLEAR;
        //    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        //}
        else if ((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Information))
        {
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Information (Data Link)\n");
            #endif // DEBUGGING
            DL_UICheck(Frame);
            if(Frame->PollFinal == 1)
            {
                DL_ResponseEnquiry(1);
            }
            DL_CurrentState = SM_DL_STATE_CONNECTED;
        }
        else if ((Frame->FrameType == AX25_S_FRAME) && ((Frame->ControlFieldType == ReceiveReady)
                    || (Frame->ControlFieldType == ReceiveNotReady)))
        {
            #if DEBUGGING == 1
            printf("S Frame Receive Ready/Not Ready (Data Link)\n");
            #endif // DEBUGGING
            if(Frame->ControlFieldType == ReceiveReady)
            {
                DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
            }
            else
            {
                DL_Internal.PeerReceiverBusy = DL_FLAG_SET;
            }
            DL_CheckNeedForResponse(Frame);
            u8 FrameNR = AX25_u8ExtractSequenceNumber(Frame->FrameType,Frame->ControlField,AX25_NR);
            if((VA <= FrameNR) && (FrameNR <= VS))
            {
                DL_CheckIFrameACK(FrameNR);
                DL_CurrentState = SM_DL_STATE_CONNECTED;
            }
            else
            {
                DL_NRErrorRecovery();
                DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
            }
        }
        else if((Frame->FrameType == AX25_S_FRAME) && ((Frame->ControlFieldType == SelectiveReject)))
        {
            #if DEBUGGING == 1
            printf("S Frame Selective Reject (Data Link)\n");
            #endif // DEBUGGING
            DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
            DL_CheckNeedForResponse(Frame);
            u8 FrameNR = AX25_u8ExtractSequenceNumber(Frame->FrameType,Frame->ControlField,AX25_NR);
            if((VA <= FrameNR) && (FrameNR <= VS))
            {
                if(Frame->PollFinal == 1)
                {
                    VA = FrameNR;
                }
                AX25Timer_voidStop(AX25_TIMER_T1);
                AX25Timer_voidStart(AX25_TIMER_T3);
                DL_SelectT1Value();
                DL_voidPushIQueue(DL_PopSentFramefromNR(FrameNR));
                DL_CurrentState = SM_DL_STATE_CONNECTED;
            }
            else
            {
                DL_NRErrorRecovery();
                DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
            }
        }
        else if((Frame->FrameType == AX25_S_FRAME) && (Frame->ControlFieldType == Reject))
        {
            #if DEBUGGING == 1
            printf("S Frame Reject (Data Link)\n");
            #endif // DEBUGGING
            DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
            DL_CheckNeedForResponse(Frame);
            u8 FrameNR = AX25_u8ExtractSequenceNumber(Frame->FrameType,Frame->ControlField,AX25_NR);
            if((VA <= FrameNR) && (FrameNR <= VS))
            {
                VA = FrameNR;
                AX25Timer_voidStop(AX25_TIMER_T1);
                AX25Timer_voidStop(AX25_TIMER_T3);
                DL_SelectT1Value();
                DL_InvokeReTransmission(FrameNR);
                DL_CurrentState = SM_DL_STATE_CONNECTED;
            }
            else
            {
                DL_NRErrorRecovery();
                DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
            }
        }
        else if(Frame->FrameType == AX25_I_FRAME)
        {
            #if DEBUGGING == 1
            printf("I Frame (Data Link)\n");
            #endif // DEBUGGING
            DL_IFrameReceived(Frame);
        }


    Hardware.Frame = PH_FLAG_CLEAR;
    }

        else if(SeizeConfirm == DL_FLAG_SET)
        {
            #if DEBUGGING == 1
            printf("Seize Confirm from Physical (Data Link)\n");
            #endif // DEBUGGING
            if(DL_Internal.ACKPending == DL_FLAG_SET)
            {
                DL_Internal.ACKPending = DL_FLAG_CLEAR;
                DL_ResponseEnquiry(0);
            }
            SeizeConfirm = DL_FLAG_CLEAR;
            DL_CurrentState = SM_DL_STATE_CONNECTED;
        }
        //IFrame pops of Queue
    else if(IQueueCounter >0)
    {
        frame_t* Frame = DL_frame_tPtrPopIQueue();
        if((DL_Internal.PeerReceiverBusy == DL_FLAG_SET) ||(VS == VA+K))
        {
            DL_voidPushIQueue(Frame);
        }
        else
        {
            NS = VS;
            NR = VR;
            frame_t* IFrame = AX25_frame_tPtrCreatIFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, Frame->PollFinal,Frame->PID, Frame->Info,Frame->InfoLength);
            DL_voidPushSentQueue(IFrame);
            Transmit(IFrame->PtrtoFrame,IFrame->FrameLength);
            //AX25_voidFreeFrame(IFrame);
            //DLtoPhysical = IFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            VS++;
            if (AX25_u8ControlModulo() == AX25_8BITS)
                VS = VS%8;
            else if(AX25_u8ControlModulo() == AX25_16BITS)
                VS = VS%128;

            DL_Internal.ACKPending = DL_FLAG_CLEAR;
            if(Timer.T1_EXPIRE == PH_FLAG_SET)
            {
                AX25Timer_voidStop(AX25_TIMER_T3);
                AX25Timer_voidStart(AX25_TIMER_T1);
            }
        }
        DL_CurrentState = SM_DL_STATE_CONNECTED;
    }
    DL_voidClearRequests();
    return;
}




void DL_TimerRecoveryState(void)
{
    #if DEBUGGING == 1
    printf("In Timer Recovery State Data Link\n");
    #endif // DEBUGGING
    if(DL_Request.ConnectRequest == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Connect Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_voidDiscardQueue(IQueue,&IQueueCounter);
        DL_EstablishDataLink();
        DL_Internal.Layer3Init = DL_FLAG_SET;
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        DL_Request.ConnectRequest = PH_FLAG_SET;
    }
    else if(DL_Request.DisconnectRequest == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Disconnect Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_voidDiscardQueue(IQueue,&IQueueCounter);
        RC = 0;
        frame_t* DISCFrame = AX25_frame_tPtrCreatUFrame(DestAddress,DestSSID,MYAddress,MySSID,AX25_COMMAND_FRAME,1,Disconnect,0,0);
        Transmit(DISCFrame->PtrtoFrame,DISCFrame->FrameLength);
        AX25_voidFreeFrame(DISCFrame);
        //DLtoPhysical = DISCFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        AX25Timer_voidStop(AX25_TIMER_T3);
        AX25Timer_voidStart(AX25_TIMER_T1);
        DL_CurrentState = SM_DL_STATE_AWAITING_RELEASE;
        DL_Request.DisconnectRequest = PH_FLAG_CLEAR;
    }
    else if(DL_Request.DataRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Data Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_voidPushRequest();
        //DL_voidPushIQueue(SegmentertoDL[0]);
        //memcpy(SegmentertoDL,SegmentertoDL+1,SegmentertoDL_Len-1);
		//SegmentertoDL_Len--;
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        DL_Request.DataRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.UnitDataRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Unit Data Request Set (Data Link)\n");
        #endif // DEBUGGING

        DL_SendUnitData();
        //DLtoPhysical = UIFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        DL_Request.UnitDataRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.FlowoffRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Flow off Request Set (Data Link)\n");
        #endif // DEBUGGING
        if(DL_Internal.OwnReceiverBusy == DL_FLAG_CLEAR)
        {
            DL_Internal.OwnReceiverBusy = DL_FLAG_SET;
            frame_t* RNRFrame = AX25_frame_tPtrCreatSFrame(DestAddress,DestSSID,MYAddress,MySSID,AX25_RESPONSE_FRAME,0,ReceiveNotReady,0,0);
            Transmit(RNRFrame->PtrtoFrame,RNRFrame->FrameLength);
            AX25_voidFreeFrame(RNRFrame);
            //DLtoPhysical = RNRFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
        }
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.FlowonRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Flow on Request Set (Data Link)\n");
        #endif // DEBUGGING
        if(DL_Internal.OwnReceiverBusy == DL_FLAG_SET)
        {
            DL_Internal.OwnReceiverBusy = DL_FLAG_CLEAR;
            frame_t* RRFrame = AX25_frame_tPtrCreatSFrame(DestAddress,DestSSID,MYAddress,MySSID,AX25_COMMAND_FRAME,1,ReceiveReady,0,0);
            Transmit(RRFrame->PtrtoFrame,RRFrame->FrameLength);
            AX25_voidFreeFrame(RRFrame);
            //DLtoPhysical = RRFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_Internal.ACKPending = DL_FLAG_CLEAR;
            if(Timer.T1_EXPIRE == DL_FLAG_SET)
            {
               AX25Timer_voidStop(AX25_TIMER_T3);
                AX25Timer_voidStart(AX25_TIMER_T1);
            }
        }
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        DL_Request.FlowonRequest = DL_FLAG_CLEAR;
    }
    else if(Timer.T1_EXPIRE == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Timre T1 Expired (Data Link)\n");
        #endif // DEBUGGING
        if(RC == N2)
        {
            if(VA == VS)
            {
                if(DL_Internal.PeerReceiverBusy == DL_FLAG_SET)
                {
                    DL_Indication.ErrorIndication = U;
                }
                else
                {
                    DL_Indication.ErrorIndication = T;
                }
            }
            else
            {
                DL_Indication.ErrorIndication = I;
            }

            DL_Indication.DisConnectIndication = DL_FLAG_SET;

            DL_voidDiscardQueue(IQueue,&IQueueCounter);

            frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress,DestSSID,MYAddress,MySSID,AX25_RESPONSE_FRAME,1,Disconnect_Mode,0,0);
            Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
            //DLtoPhysical = DMFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
        else
        {
            RC++;
            DL_TransmitEnquiry();
            DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        }
        Timer.T1_EXPIRE = DL_FLAG_CLEAR;
    }

    else if(SeizeConfirm == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Seize Confirm from Physical (Data Link)\n");
        #endif // DEBUGGING
        if(DL_Internal.ACKPending == DL_FLAG_SET)
        {
            DL_Internal.ACKPending = DL_FLAG_CLEAR;
            DL_ResponseEnquiry(0);
        }
        SeizeConfirm = DL_FLAG_CLEAR;
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
    }
    /*else if(PH_Indication.BusyIndication == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Busy Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        AX25Timer_voidStop(AX25_TIMER_T1);
        AX25Timer_voidStop(AX25_TIMER_T3);
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
    }
    else if(PH_Indication.QuietIndication == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Quiet Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        AX25Timer_voidStart(AX25_TIMER_T1);
        AX25Timer_voidStart(AX25_TIMER_T3);
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
    }*/
    else if(Hardware.Frame == PH_FLAG_SET)
    {

        #if DEBUGGING == 1
        printf("Data Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        u8 ControlModulo = AX25_u8ControlModulo();
        frame_t* Frame = HardwaretoDL;
        u8 Error = DL_u8CheckErrors(ControlModulo,Frame);
        /***************/
		if(Error == 0)
            PrintFrameType(Frame);
		/*******************/
        if(Error != 0)
        {
            //AX25_voidFreeFrame(Frame);
            if(Error == 255)
            {
                //CRC error
            }
            else if(Error == 1 || Error == 2)
            {
                //Control Error
                DL_Indication.ErrorIndication = L;
            }
            else if(Error == 3 || Error == 4)
            {
                //Info Length error
                DL_Indication.ErrorIndication = M;
            }
            else if(Error == 5 || Error == 6)
            {
                //U Frame or S Frame length error
                DL_Indication.ErrorIndication = N;
            }
            DL_voidDiscardQueue(IQueue,&IQueueCounter);
            DL_EstablishDataLink();
            DL_Internal.Layer3Init = DL_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
        else if((Frame->FrameType == AX25_U_FRAME)&& ((Frame->ControlFieldType == Set_Async_Balanced_mode)
                                                      || (Frame->ControlFieldType == Set_Async_Balanced_mode_Extended)))
        {

            if(Frame->ControlFieldType == Set_Async_Balanced_mode)
            {
                #if DEBUGGING == 1
                printf("U Frame SABM (Data Link)\n");
                #endif // DEBUGGING
                DL_SetVersion2_0();
            }
            else
            {
                #if DEBUGGING == 1
                printf("U Frame SABME (Data Link)\n");
                #endif // DEBUGGING
                DL_SetVersion2_2();
            }
            frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress,DestSSID,MYAddress,MySSID,AX25_RESPONSE_FRAME,Frame->PollFinal,Unnumbered_Acknowledge,0,0);
            Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
            AX25_voidFreeFrame(UAFrame);
            //DLtoPhysical = UAFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_ClearExceptionConditions();
            DL_Indication.ErrorIndication = F;
            if (VS != VA)
            {
                DL_voidDiscardQueue(IQueue,&IQueueCounter);
                DL_Indication.ConnectIndication = DL_FLAG_SET;
            }
            AX25Timer_voidStop(AX25_TIMER_T1);
            AX25Timer_voidStart(AX25_TIMER_T3);
            VA = 0;
            VS = 0;

            VR = 0;

            DL_CurrentState = SM_DL_STATE_CONNECTED;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
        {
            SM_voidManagmentDataLink();
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect))
        {
            #if DEBUGGING == 1
            printf("U Frame Disconnect (Data Link)\n");
            #endif // DEBUGGING
            DL_voidDiscardQueue(IQueue,&IQueueCounter);
            frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress,DestSSID,MYAddress,MySSID,AX25_RESPONSE_FRAME,Frame->PollFinal,Unnumbered_Acknowledge,0,0);
            Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
            AX25_voidFreeFrame(UAFrame);
            //DLtoPhysical = UAFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_Indication.DisConnectIndication = DL_FLAG_SET;
            AX25Timer_voidStop(AX25_TIMER_T1);
            AX25Timer_voidStop(AX25_TIMER_T3);
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Acknowledge))
        {
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Ack (Data Link)\n");
            #endif // DEBUGGING
            DL_Indication.ErrorIndication = C;
            DL_EstablishDataLink();
            DL_Internal.ACKPending = DL_FLAG_CLEAR;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Information))
        {
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Information (Data Link)\n");
            #endif // DEBUGGING
            DL_UICheck(Frame);
            if(Frame->PollFinal == 1)
            {
                DL_ResponseEnquiry(1);
            }
            DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect_Mode))
        {
            #if DEBUGGING == 1
            printf("U Frame Disconnect Mode (Data Link)\n");
            #endif // DEBUGGING
            DL_Indication.ErrorIndication = E;
            DL_Indication.DisConnectIndication = DL_FLAG_SET;
            DL_voidDiscardQueue(IQueue,&IQueueCounter);
            AX25Timer_voidStop(AX25_TIMER_T1);
            AX25Timer_voidStop(AX25_TIMER_T3);
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
        /*else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Frame_Reject))
        {

            //////////////////////////////////////
            //DL_Indication.ErrorIndication = K;//
            //////////////////////////////////////
            DL_EstablishDataLink();
            DL_Internal.Layer3Init = DL_FLAG_CLEAR;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }*/
        else if((Frame->FrameType == AX25_S_FRAME)&& ((Frame->ControlFieldType == ReceiveReady)
                                                      || (Frame->ControlFieldType == ReceiveNotReady)))
        {
            #if DEBUGGING == 1
            printf("S Frame Receive Ready/Not Ready (Data Link)\n");
            #endif // DEBUGGING
            u8 FrameNR = AX25_u8ExtractSequenceNumber(AX25_S_FRAME,Frame->ControlField,AX25_NR);
            if(Frame->ControlFieldType == ReceiveReady)
            {
                DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
            }
            else
            {
                DL_Internal.PeerReceiverBusy = DL_FLAG_SET;
            }
            if((Frame->CMDRES == AX25_RESPONSE_FRAME)&&(Frame->PollFinal == 1))
            {
                AX25Timer_voidStop(AX25_TIMER_T1);
                DL_SelectT1Value();
                if((VA<=FrameNR)&&(FrameNR<=VS))
                {
                    VA = FrameNR;
                    if(VS == VA)
                    {
                        AX25Timer_voidStart(AX25_TIMER_T3);
                        DL_CurrentState = SM_DL_STATE_CONNECTED;
                    }
                    else
                    {
                        DL_InvokeReTransmission(FrameNR);
                        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                    }
                }
                else
                {
                    DL_NRErrorRecovery();
                    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
                }
            }
            else
            {
                if((Frame->CMDRES == AX25_COMMAND_FRAME)&&(Frame->PollFinal == 1))
                {
                    DL_ResponseEnquiry(1);
                }
                if((VA <= FrameNR)&&(FrameNR <= VS))
                {
                    VA = FrameNR;
                    DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                }
                else
                {
                    DL_NRErrorRecovery();
                    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
                }
            }
        }
        else if((Frame->FrameType == AX25_S_FRAME)&&(Frame->ControlFieldType == Reject))
        {
            #if DEBUGGING == 1
            printf("S Frame Reject (Data Link)\n");
            #endif // DEBUGGING

            DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
            u8 FrameNR = AX25_u8ExtractSequenceNumber(AX25_S_FRAME,Frame->ControlField,AX25_NR);
            if((Frame->CMDRES == AX25_RESPONSE_FRAME) && (Frame->PollFinal == 1))
            {
                AX25Timer_voidStop(AX25_TIMER_T1);
                DL_SelectT1Value();
                if((VA<=FrameNR)&&(FrameNR<=VS))
                {
                    VA = FrameNR;
                    if(VS == VA)
                    {
                        AX25Timer_voidStart(AX25_TIMER_T3);
                        DL_CurrentState = SM_DL_STATE_CONNECTED;
                    }
                    else
                    {
                        DL_InvokeReTransmission(FrameNR);
                        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                    }
                }
                else
                {
                    DL_NRErrorRecovery();
                    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
                }
            }
            else
            {
                if((Frame->CMDRES == AX25_COMMAND_FRAME) && (Frame->PollFinal == 1))
                {
                    DL_ResponseEnquiry(1);
                }
                if((VA<=FrameNR)&&(FrameNR<=VS))
                {
                    VA = FrameNR;
                    if(VS != VA)
                    {
                        DL_InvokeReTransmission(FrameNR);
                    }
                    DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                }
                else
                {
                    DL_NRErrorRecovery();
                    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
                }
            }
        }
        else if((Frame->FrameType == AX25_S_FRAME)&&(Frame->ControlFieldType == SelectiveReject))
        {
            #if DEBUGGING == 1
            printf("S Frame Selective Reject (Data Link)\n");
            #endif // DEBUGGING
            u8 FrameNR = AX25_u8ExtractSequenceNumber(AX25_S_FRAME,Frame->ControlField,AX25_NR);
            DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
            if(Frame->CMDRES == AX25_RESPONSE_FRAME)
            {
                AX25Timer_voidStop(AX25_TIMER_T1);
                DL_SelectT1Value();

                if((VA <= FrameNR) && (FrameNR <= VS))
                {
                    if(Frame->PollFinal)
                    {
                        VA = FrameNR;
                    }
                    if(VA == VS)
                    {
                        AX25Timer_voidStart(AX25_TIMER_T1);
                        DL_CurrentState = SM_DL_STATE_CONNECTED;
                    }
                    else
                    {
                        /*Push NR Frame on Queue*/
                        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                    }
                }
                else
                {
                    DL_NRErrorRecovery();
                    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
                }

            }
            else
            {
                if((VA <= FrameNR) && (FrameNR <= VS))
                {
                    if(Frame->PollFinal)
                    {
                        VA = FrameNR;
                    }
                    if(VA == VS)
                    {
                        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                    }
                    else
                    {
                        //////////////////////////////////////////
                        ////////*Push NR Frame on Queue*//////////
                        //////////////////////////////////////////
                        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
                    }
                }
                else
                {
                    DL_NRErrorRecovery();
                    DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
                }
            }

        }
        else if(Frame->FrameType == AX25_I_FRAME)
        {
            #if DEBUGGING == 1
            printf("I Frame (Data Link)\n");
            #endif // DEBUGGING

            DL_IFrameReceived(Frame);
        }

    }
    ////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////
    else if (IQueueCounter >1)
    {
        /*POP I Frame*/
        frame_t* Frame = DL_frame_tPtrPopIQueue();
        if((DL_Internal.PeerReceiverBusy == DL_FLAG_SET) || (VS == VA+K))
        {
            /*Push I Frame*/
            DL_voidPushIQueue(Frame);
        }
        else
        {
            NS = VS;
            NR = VR;
            frame_t* IFrame = AX25_frame_tPtrCreatIFrame (DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 0,No_layer_3_protocol_implemented, Frame->Info, Frame->InfoLength);
            Transmit(IFrame->PtrtoFrame,IFrame->FrameLength);
            AX25_voidFreeFrame(IFrame);
            //DLtoPhysical = IFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            VS++;
            DL_Internal.ACKPending = DL_FLAG_CLEAR;
            if(Timer.T1_EXPIRE == PH_FLAG_SET)
            {
                AX25Timer_voidStop(AX25_TIMER_T3);
                AX25Timer_voidStart(AX25_TIMER_T1);
            }
        }
        DL_CurrentState = SM_DL_STATE_TIMER_RECOVERY;
        Hardware.Frame = PH_FLAG_CLEAR;
    }
    ///////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////
    //DL_voidClearRequests();
}

void DL_AwaitingConnection2_2(void)
{
    #if DEBUGGING == 1
    printf("In Awaiting Connection 2.2 Data Link\n");
    #endif // DEBUGGING

    if(DL_Request.UnitDataRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Unit Data Request Set (Data Link)\n");
        #endif // DEBUGGING


        DL_SendUnitData();
        //DLtoPhysical = UIFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        DL_Request.UnitDataRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.ConnectRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Connect Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_voidDiscardQueue(IQueue,&IQueueCounter);
        DL_Internal.Layer3Init = DL_FLAG_SET;
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        DL_Request.ConnectRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.DisconnectRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Disconnect Request Set (Data Link)\n");
        #endif // DEBUGGING
        /*ReQueue*/
        printf("shit2.2\n");
        DL_Request.DisconnectRequest = DL_FLAG_CLEAR;
    }
    else if(DL_Request.DataRequest == DL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Data Request Set (Data Link)\n");
        #endif // DEBUGGING
        if(DL_Internal.Layer3Init == DL_FLAG_CLEAR)
        {
            //DL_voidPushIQueue(SegmentertoDL[0]);
            DL_voidPushRequest();
        }
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        DL_Request.DataRequest = DL_FLAG_CLEAR;
    }
    else if((DL_Request.FlowoffRequest == DL_FLAG_SET) || (DL_Request.FlowonRequest == DL_FLAG_SET))
    {
        #if DEBUGGING == 1
        printf("Flow Off/On Request Set (Data Link)\n");
        #endif // DEBUGGING
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
        DL_Request.FlowonRequest = DL_FLAG_SET;
    }
    else if(Timer.T1_EXPIRE == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Timer T1 Expired (Data Link)\n");
        #endif // DEBUGGING
        if(RC == N2)
        {
            DL_voidDiscardQueue(IQueue,&IQueueCounter);

            DL_Indication.ErrorIndication = I;

            DL_Indication.DisConnectIndication = DL_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_DISCONNECTED;
        }
        else
        {
            RC++;
            frame_t* SABMEFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Set_Async_Balanced_mode_Extended, 0,0);
            Transmit(SABMEFrame->PtrtoFrame,SABMEFrame->FrameLength);
            AX25_voidFreeFrame(SABMEFrame);
            //DLtoPhysical = SABMEFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_SelectT1Value();

            AX25Timer_voidStart(AX25_TIMER_T1);
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        }
        Timer.T1_EXPIRE = DL_FLAG_CLEAR;
    }
    else if(Hardware.Frame == PH_FLAG_SET)
    {

        #if DEBUGGING == 1
        printf("Data Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        frame_t* Frame = HardwaretoDL;
        u8 ControlModulo = AX25_u8ControlModulo();
        u8 Error = DL_u8CheckErrors(ControlModulo,Frame);
        if(Error !=0)
        {
            //AX25_voidFreeFrame(Frame);
            if(Error == 255)
            {
                /*CRC Error and Discard Frame*/

            }
            else if((Error == 1) || (Error ==2))
            {
                DL_Indication.ErrorIndication = L;
            }

            else if((Error == 3) || (Error ==4))
            {
                DL_Indication.ErrorIndication = M;
            }

            else if((Error == 5) || (Error ==6))
            {
                DL_Indication.ErrorIndication = N;
            }
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Information))
        {
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Information (Data Link)\n");
            #endif // DEBUGGING
            DL_UICheck(Frame);
            if(Frame->PollFinal == 1)
            {
                frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 1, Disconnect_Mode, 0,0);
                Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
				AX25_voidFreeFrame(DMFrame);
                //DLtoPhysical = DMFrame;
                //PH_Request.DataRequest = PH_FLAG_SET;
            }
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
        {
            SM_voidManagmentDataLink();
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Set_Async_Balanced_mode))
        {
            #if DEBUGGING == 1
            printf("U Frame SABM (Data Link)\n");
            #endif // DEBUGGING
            frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Unnumbered_Acknowledge, 0,0);
            Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
            AX25_voidFreeFrame(UAFrame);
            //DLtoPhysical = UAFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect))
        {
            #if DEBUGGING == 1
            printf("U Frame Disconnect (Data Link)\n");
            #endif // DEBUGGING
            frame_t* DMFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Disconnect_Mode, 0,0);
            Transmit(DMFrame->PtrtoFrame,DMFrame->FrameLength);
            AX25_voidFreeFrame(DMFrame);
            //DLtoPhysical = DMFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Disconnect_Mode))
        {
            #if DEBUGGING == 1
            printf("U Frame Disconnect Mode (Data Link)\n");
            #endif // DEBUGGING
            if(Frame->PollFinal == 1)
            {
                DL_voidDiscardQueue(IQueue,&IQueueCounter);
                DL_Indication.DisConnectIndication = DL_FLAG_SET;
                AX25Timer_voidStop(AX25_TIMER_T1);
                DL_CurrentState = SM_DL_STATE_DISCONNECTED;
            }
            else
            {
                DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
            }
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Set_Async_Balanced_mode_Extended))
        {
            #if DEBUGGING == 1
            printf("U Frame SABME (Data Link)\n");
            #endif // DEBUGGING
            frame_t* UAFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Frame->PollFinal, Unnumbered_Acknowledge, 0,0);
            Transmit(UAFrame->PtrtoFrame,UAFrame->FrameLength);
            AX25_voidFreeFrame(UAFrame);
            //DLtoPhysical = UAFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
        }
        else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Unnumbered_Acknowledge))
        {
            #if DEBUGGING == 1
            printf("U Frame Unnumbered Ack (Data Link)\n");
            #endif // DEBUGGING
            if(Frame->PollFinal == 1)
            {
                if(DL_Internal.Layer3Init == DL_FLAG_SET)
                {
                    DL_Indication.ConnectConfirm = DL_FLAG_SET;
                }
                else
                {
                    if(VS != VA)
                    {
                        DL_voidDiscardQueue(IQueue,&IQueueCounter);
                        DL_Indication.ConnectIndication = DL_FLAG_SET;
                    }
                }
                AX25Timer_voidStop(AX25_TIMER_T1);
                AX25Timer_voidStop(AX25_TIMER_T3);
                VS =0;
                VA =0;
                VR =0;
                DL_SelectT1Value();
                DL_CurrentState = SM_DL_STATE_CONNECTED;
            }
            else
            {
                DL_Indication.ErrorIndication = D;
                DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
            }
        }
        /*else if((Frame->FrameType == AX25_U_FRAME) &&(Frame->ControlFieldType == Frame_Reject))
        {
            #if DEBUGGING == 1
            printf(" (Data Link)\n");
            #endif // DEBUGGING
            DL_Internal.SRT = DefaultSRT;
            DL_Internal.T1V = 2 * DL_Internal.SRT;
            DL_EstablishDataLink();
            DL_Internal.Layer3Init = DL_FLAG_SET;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }*/
    Hardware.Frame = PH_FLAG_CLEAR;
    }
    /*
    else if(PH_Indication.BusyIndication == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Busy Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        AX25Timer_voidStop(AX25_TIMER_T1);
        AX25Timer_voidStop(AX25_TIMER_T3);
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
    }
    else if(PH_Indication.QuietIndication == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Quiet Indication from Physical (Data Link)\n");
        #endif // DEBUGGING
        AX25Timer_voidStart(AX25_TIMER_T1);
        AX25Timer_voidStart(AX25_TIMER_T3);
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
    }
    else if(PH_Indication.SiezeConfirm == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Seize Confirm from Physical (Data Link)\n");
        #endif // DEBUGGING
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
    }*/
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    else if(IQueueCounter >1)
    {
        /*POP I Frame*/
        if(DL_Internal.Layer3Init == DL_FLAG_CLEAR)
        {
            /*Push I Frame*/
        }
        DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION_2_2;
    }
    ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////
    DL_voidClearRequests();
}

/****************SubRoutines****************/
void DL_NRErrorRecovery(void)
{
    printf("In NR Error Recovery SubRoutine\n");
    #if DEBUGGING == 1
    printf("In NR Error Recovery SubRoutine\n");
    #endif // DEBUGGING
	DL_Indication.ErrorIndication = J;
	DL_EstablishDataLink();
	DL_Internal.Layer3Init = DL_FLAG_CLEAR;
}
void DL_EstablishDataLink(void)
{
    printf("In Establish Data Link SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Establish Data Link SubRoutine\n");
    #endif // DEBUGGING
	DL_ClearExceptionConditions();
	RC = 0;
	frame_t *Frame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Set_Async_Balanced_mode, 0, 0);
	Transmit(Frame->PtrtoFrame,Frame->FrameLength);
    AX25_voidFreeFrame(Frame);
	//DLtoPhysical = Frame;
	//PH_Request.DataRequest = PH_FLAG_SET;

	AX25Timer_voidStop(AX25_TIMER_T3);
	AX25Timer_voidClear(AX25_TIMER_T1);
	AX25Timer_voidStart(AX25_TIMER_T1);
}
void DL_ClearExceptionConditions(void)
{
printf("In Clear Exception Conditions SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Clear Exception Conditions SubRoutine\n");
    #endif // DEBUGGING
	DL_Internal.PeerReceiverBusy = DL_FLAG_CLEAR;
	DL_Internal.RejectException = DL_FLAG_CLEAR;
	DL_Internal.OwnReceiverBusy = DL_FLAG_CLEAR;
	DL_Internal.ACKPending = DL_FLAG_CLEAR;
}
void DL_TransmitEnquiry(void)
{
printf("In Transmit Enquiry SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Transmit Enquiry SubRoutine\n");
    #endif // DEBUGGING
	NR = VR;
	frame_t* Frame;
	if(DL_Internal.OwnReceiverBusy == DL_FLAG_SET)
	{
		Frame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, ReceiveNotReady, 0, 0);
	}
	else
	{
		Frame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, ReceiveReady, 0, 0);
	}
    Transmit(Frame->PtrtoFrame,Frame->FrameLength);
    AX25_voidFreeFrame(Frame);
	//DLtoPhysical = Frame;
	//PH_Request.DataRequest = PH_FLAG_SET;

	DL_Internal.ACKPending = DL_FLAG_CLEAR;

	AX25Timer_voidStart(AX25_TIMER_T1);
}
void DL_ResponseEnquiry(u8 Final)
{
    printf("In Response Enquiry SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Response Enquiry SubRoutine\n");
    #endif // DEBUGGING
	NR = VR;
	frame_t* Frame;
	if(DL_Internal.OwnReceiverBusy == DL_FLAG_SET)
	{
		Frame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Final, ReceiveNotReady, 0, 0);
	}
	else
	{
		Frame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, Final, ReceiveReady, 0, 0);
	}
    Transmit(Frame->PtrtoFrame,Frame->FrameLength);
    AX25_voidFreeFrame(Frame);
	//DLtoPhysical = Frame;
	//PH_Request.DataRequest = PH_FLAG_SET;

	DL_Internal.ACKPending = DL_FLAG_CLEAR;
}

void DL_InvokeReTransmission(u8 Copy_u8NR)
{
printf("In Invoke Retransmission SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Invoke Retransmission SubRoutine\n");
    #endif // DEBUGGING
	u8 X = VS;
	VS = Copy_u8NR;
	do{

		DL_voidPushIQueue(DL_PopSentFramefromNR(Copy_u8NR));

		VS++;
		VS = AX25_u8ControlModulo() == AX25_8BITS?  VS%8:VS%128;
	}while(VS != X);
}

void DL_CheckIFrameACK(u8 FrameNR)
{
printf("In Check I Frame Ack SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Check I Frame Ack SubRoutine\n");
    #endif // DEBUGGING
	if(DL_Internal.PeerReceiverBusy == DL_FLAG_SET)
	{
		VA = FrameNR;
		DL_FreeSentQueue(FrameNR);
		AX25Timer_voidStart(AX25_TIMER_T3);
		if(AX25Timer_u8GetStatus(AX25_TIMER_T1) == TIMER_STATUS_STOP)
		{
			AX25Timer_voidStart(AX25_TIMER_T1);
		}
	}
	else
	{
		if (FrameNR == VS)
		{
            DL_FreeSentQueue(FrameNR);
			VA = FrameNR;
			AX25Timer_voidStop(AX25_TIMER_T1);
			AX25Timer_voidStart(AX25_TIMER_T3);
			DL_SelectT1Value();
		}
		else
		{
			/*Not All frames ACKED*/
			if(FrameNR != VA)
			{
				VA = FrameNR;
				DL_FreeSentQueue(FrameNR);
				AX25Timer_voidClear(AX25_TIMER_T1);
				AX25Timer_voidStart(AX25_TIMER_T1);
			}
		}
	}
}
void DL_CheckNeedForResponse(frame_t* Frame)
{

printf("In Check Need for Response SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Check Need for Response SubRoutine\n");
    #endif // DEBUGGING
	if(Frame->CMDRES == AX25_COMMAND_FRAME && Frame->PollFinal == 1)
	{
		DL_ResponseEnquiry(1);
	}
	else if (Frame->CMDRES == AX25_RESPONSE_FRAME && Frame->PollFinal == 1)
	{
		DL_Indication.ErrorIndication = A;
	}
}
void DL_UICheck(frame_t* Frame)
{
printf("In UI Check SubRoutine\n");
    #if DEBUGGING == 1
    printf("In UI Check SubRoutine\n");
    #endif // DEBUGGING
	if(Frame->CMDRES == AX25_COMMAND_FRAME)
	{
		if(Frame->InfoLength <= N1)
		{
			DLtoAssembler = (u8*)malloc(sizeof(u8)*Frame->InfoLength);
			DLtoAssemblerLength = Frame->InfoLength;
			memcpy(DLtoAssembler,Frame->Info,Frame->InfoLength);
			AX25_voidFreeFrame(Frame);
			DL_Indication.UnitDataIndication = DL_FLAG_SET;
            while(DL_Indication.UnitDataIndication == DL_FLAG_SET) usleep(1);

		}
		else
		{
			//DL_Indication.ErrorIndication = K;
		}
	}
	else
	{
		DL_Indication.ErrorIndication = Q;
	}
}
void DL_SelectT1Value(void)
{
printf("In Select T1 Value SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Select T1 Value SubRoutine\n");
    #endif // DEBUGGING
	if(RC == 0)
	{
		 DL_Internal.SRT = (7.0/8.0) * DL_Internal.SRT + 0.125 * DL_Internal.T1V
		 	 	 	 	 	 - 0.125 * AX25Timer_u32GetTime(AX25_TIMER_T1);
		 DL_Internal.T1V = 2 * DL_Internal.SRT;
		 /////////////////////////////////////////
		 ////////////////////////////////////////
		 //AX25Timer_voidSetTime(AX25_TIMER_T1,DL_Internal.T1V);
	}
	else
	{
		if (AX25Timer_u8GetStatus(AX25_TIMER_T1) == TIMER_STATUS_STOP)
		{
			DL_Internal.T1V = pow(2,RC+1)*DL_Internal.SRT;
		}
	}
	//printf("New T1 Time: %lu\n",DL_Internal.T1V);
	AX25Timer_voidSetTime(AX25_TIMER_T1,DL_Internal.T1V);
}
void DL_EstablishExtendedDataLink(void)
{

printf("In Establish Extended Data Link SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Establish Extended Data Link SubRoutine\n");
    #endif // DEBUGGING
	DL_ClearExceptionConditions();
	RC = 0;
	frame_t* Frame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Set_Async_Balanced_mode_Extended, 0, 0);
	Transmit(Frame->PtrtoFrame,Frame->FrameLength);
    AX25_voidFreeFrame(Frame);

	//DLtoPhysical = Frame;
	//PH_Request.DataRequest = PH_FLAG_SET;
	AX25Timer_voidStop(AX25_TIMER_T3);
	AX25Timer_voidStop(AX25_TIMER_T1);
	AX25Timer_voidStart(AX25_TIMER_T1);
}
void DL_SetVersion2_0(void)
{
printf("In Set Version 2.0 SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Set Version 2.0 SubRoutine\n");
    #endif // DEBUGGING
	TransmissionMode = AX25_XID_HALFDUPLEX;
	RejectType = AX25_XID_IMPLICIT_REJECT;
	AX25_voidSetControlModulo(AX25_8BITS);
	N1 = 256;
	K = 4;
	 /////////////////////////////////////////
		 ////////////////////////////////////////
	AX25Timer_voidSetTime(AX25_TIMER_T2, 3000);
	N2 = 10;
}
void DL_SetVersion2_2(void)
{
printf("In Set Version 2.2 SubRoutine\n");
    #if DEBUGGING == 1
    printf("In Set Version 2.2 SubRoutine\n");
    #endif // DEBUGGING
	TransmissionMode = AX25_XID_HALFDUPLEX;
	RejectType = AX25_XID_SELECTIVE_REJECT;
	AX25_voidSetControlModulo(AX25_16BITS);
	N1 = 256;
	K = 32;
	 /////////////////////////////////////////
		 ////////////////////////////////////////
	AX25Timer_voidSetTime(AX25_TIMER_T2, 3000);
	N2 = 10;
}



void DL_voidClearRequests(void)
{
    #if DEBUGGING == 1
    printf("Clearing All Data Link Requests\n");
    #endif // DEBUGGING
	//DL_Request.ConnectRequest = DL_FLAG_CLEAR;
	//DL_Request.DataRequest = DL_FLAG_CLEAR;
	//DL_Request.DisconnectRequest = DL_FLAG_CLEAR;
	//DL_Request.FlowoffRequest = DL_FLAG_CLEAR;
	//DL_Request.FlowonRequest = DL_FLAG_CLEAR;
	//DL_Request.UnitDataRequest = DL_FLAG_CLEAR;

}


u8 DL_u8CheckErrors(u8 ControlModulo, frame_t* Frame)
{
	if (AX25_u8DeConstructFrame(Frame) != 0)
		return 255;
	u8 Flag = 0;
	if ((strcmp(Frame->FrameAddress.Destination,MYAddress) != 0) ||
        (Frame->FrameAddress.DestinationSSID != MySSID))
        return 50;
	/*check for Control Field error*/
	if(Frame->FrameType == AX25_S_FRAME && ControlModulo == AX25_16BITS)
	{
		u16 ControlField = Frame->ControlField && ~(0xC);
		if (ControlField != 1)
		{
			/*Control Field Error*/
			//DL_Indication.ErrorIndication = L;

			Flag = 1;
		}
	}
	else if (Frame->FrameType == AX25_U_FRAME)
	{
		u8 ControlField = Frame->ControlField;
		CLR_BIT(ControlField,4);
		if ((ControlField != Set_Async_Balanced_mode) && (ControlField != Set_Async_Balanced_mode_Extended)
				&& (ControlField != Disconnect) && (ControlField != Disconnect_Mode)
				&& (ControlField != Unnumbered_Acknowledge) && (ControlField != Frame_Reject)
				&& (ControlField != Exchange_Identification) && (ControlField != Test)
				&& (ControlField != Unnumbered_Information))
		{
			/*Control Field Error*/
			//DL_Indication.ErrorIndication = L;
			//DL_CurrentState = SM_DL_STATE_DISCONNECTED;
			Flag = 2;
		}
	}
	/*Info field check*/
	else if(Frame->FrameType == AX25_S_FRAME && (Frame->Info != 0 || Frame->InfoLength !=0))
	{
		//DL_Indication.ErrorIndication = M;
		//DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		Flag = 3;
	}
	else if(Frame->FrameType == AX25_U_FRAME)
	{
		if ((Frame->ControlFieldType != Unnumbered_Information) && (Frame->ControlFieldType != Exchange_Identification)
				&& (Frame->ControlFieldType != Test) &&(Frame->ControlFieldType != Frame_Reject))
		{
			if((Frame->Info != 0) || (Frame->InfoLength !=0))
			{
				//DL_Indication.ErrorIndication = M;
				//DL_CurrentState = SM_DL_STATE_DISCONNECTED;
				Flag = 4;
			}
		}
	}
	/*U frame or S frame length*/
	else if (Frame->FrameType == AX25_S_FRAME && Frame->FrameLength != 17 + ControlModulo)
	{
		//DL_Indication.ErrorIndication = N;
		//DL_CurrentState = SM_DL_STATE_DISCONNECTED;
		Flag = 5;
	}
	else if (Frame->FrameType == AX25_U_FRAME)
	{
		if ((Frame->ControlFieldType != Unnumbered_Information) && (Frame->ControlFieldType != Exchange_Identification)
					&& (Frame->ControlFieldType != Test) &&(Frame->ControlFieldType != Frame_Reject))
		{
			if(Frame->FrameLength != 17)
			{
				//DL_Indication.ErrorIndication = N;
				//DL_CurrentState = SM_DL_STATE_DISCONNECTED;
				Flag = 6;
			}
		}
	}
	//DL_voidClearRequests();
	return Flag;
}




void DL_IFrameReceived(frame_t* Frame)
{
    if(Frame->CMDRES == AX25_RESPONSE_FRAME)
    {
        DL_Indication.ErrorIndication = S;
        AX25_voidFreeFrame(Frame);
        //DL_CurrentState = DL_CurrentState;
    }
    else
    {
        if(Frame->InfoLength > N1)
        {
            DL_Indication.ErrorIndication = O;
            DL_EstablishDataLink();
            DL_Internal.Layer3Init = DL_FLAG_CLEAR;
            DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
        }
        else
        {
            u8 FrameNR = Frame->NR;
            if((VA <= FrameNR) && (FrameNR <= VS))
            {
                if(DL_CurrentState == SM_DL_STATE_CONNECTED)
                {
                    DL_CheckIFrameACK(FrameNR);
                }
                else if (DL_CurrentState == SM_DL_STATE_TIMER_RECOVERY)
                {
                    VA = FrameNR;
                }
                if(DL_Internal.OwnReceiverBusy == DL_FLAG_CLEAR)
                {
                    u8 FrameNS = Frame->NS;
                    if(FrameNS == VR)
                    {
                        VR++;
                        if (AX25_u8ControlModulo() == AX25_8BITS)
                            VR = VR % 8;
                        else if (AX25_u8ControlModulo() == AX25_16BITS)
                            VR = VR % 128;

                        DL_Internal.RejectException = DL_FLAG_CLEAR;
                        if (DL_Internal.SelectiveRejectException >0)
                            DL_Internal.SelectiveRejectException--;
                        DLtoAssembler = (u8*)malloc(sizeof(u8)*Frame->InfoLength);
                        DLtoAssemblerLength = Frame->InfoLength;
                        memcpy(DLtoAssembler,Frame->Info,DLtoAssemblerLength);
                        DL_Indication.DataIndication = DL_FLAG_SET;
                        while(DL_Indication.DataIndication == DL_FLAG_SET)usleep(1);
                        AX25_voidFreeFrame(Frame);
                        frame_t* f = DL_GetReceivedFrameFromNS(VR);
                        while(f != NULL)
                        {

                            DLtoAssembler = (u8*)malloc(sizeof(u8)*f->InfoLength);
                            DLtoAssemblerLength = f->InfoLength;
                            memcpy(DLtoAssembler,f->Info,f->InfoLength);
                            DL_Indication.DataIndication = DL_FLAG_SET;
                            while(DL_Indication.DataIndication == DL_FLAG_SET)usleep(1);
                            AX25_voidFreeFrame(f);
                            VR++;
                            VR = AX25_u8ControlModulo() == AX25_8BITS? VR%8:VR%128;
                            f = DL_GetReceivedFrameFromNS(VR);
                        }
                        /*while(1) VR I frame stored
                        {
                            Retrieve contents of VR I Frame
                            while(i < ReceivedFramesCounter){
                                FrameNS = AX25_u8ExtractSequenceNumber(ReceivedFrames[i]->FrameType, ReceivedFrames[i]->ControlField,AX25_NS);
                                if (FrameNS == VR)
                                {
                                    DLtoAssembler = ReceivedFrames[i];
                                    ShiftReceivedQueue(i);
                                    DL_Indication.DataIndication = DL_FLAG_SET;
                                    while(DL_Indication.DataIndication == DL_FLAG_SET);
                                    VR++;
                                    if (AX25_u8ControlModulo() == AX25_8BITS )
                                        VR = VR % 8;
                                    else if (AX25_u8ControlModulo() == AX25_16BITS)
                                        VR = VR%128;

                                    break;
                                }
                                i++;
                            }
                            if (i == ReceivedFramesCounter)
                                break;
                            i = 0;

                        }*/
                        if(Frame->PollFinal == 1)
                        {
                            NR = VR;
                            frame_t* RRFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,1,ReceiveReady,0,0);
                            Transmit(RRFrame->PtrtoFrame,RRFrame->FrameLength);
                            AX25_voidFreeFrame(RRFrame);
                            //DLtoPhysical = RRFrame;
                            //PH_Request.DataRequest = PH_FLAG_SET;
                            DL_Internal.ACKPending = DL_FLAG_CLEAR;
                            //DL_CurrentState = DL_CurrentState;
                        }
                        else
                        {
                            if(DL_Internal.ACKPending == DL_FLAG_CLEAR)
                            {
                                //PH_Request.SiezeRequest = PH_FLAG_SET;
                                SeizeConfirm = DL_FLAG_SET;
                                DL_Internal.ACKPending = DL_FLAG_SET;
                            }
                            //DL_CurrentState = DL_CurrentState;
                        }
                    }
                    else
                    {
                        if(DL_Internal.RejectException == DL_FLAG_SET)
                        {
                            free(Frame->Info);
                            if(Frame->PollFinal == 1)
                            {
                                NR = VR;
                                frame_t* RRFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,1,ReceiveReady,0,0);
                                Transmit(RRFrame->PtrtoFrame,RRFrame->FrameLength);
                                AX25_voidFreeFrame(RRFrame);
                                //DLtoPhysical = RRFrame;
                                //PH_Request.DataRequest = PH_FLAG_SET;
                                DL_Internal.ACKPending = DL_FLAG_CLEAR;
                            }
                            //DL_CurrentState = DL_CurrentState;
                        }
                        else
                        {
                           if(SREJ_Enable == DL_FLAG_SET)/*Selective Reject Enable?*/
                           {
                               DL_voidPushReceivedFrame(Frame);
                               u8 FrameNS = AX25_u8ExtractSequenceNumber(Frame->FrameType,Frame->ControlField,AX25_NS);
                               if(DL_Internal.SelectiveRejectException >0)
                               {
                                   NR = FrameNS;
                                   DL_Internal.SelectiveRejectException++;
                                   frame_t* SREJFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,0,SelectiveReject,0,0);
                                   Transmit(SREJFrame->PtrtoFrame,SREJFrame->FrameLength);
                                   AX25_voidFreeFrame(SREJFrame);

                                   //DLtoPhysical = SREJFrame;
                                   //PH_Request.DataRequest = PH_FLAG_SET;
                                   DL_Internal.ACKPending = DL_FLAG_CLEAR;
                                   //DL_CurrentState = DL_CurrentState;
                               }
                               else
                               {
                                   if(FrameNS > VR+1)
                                   {

                                       //free(Frame->Info);
                                       DL_Internal.RejectException = DL_FLAG_SET;
                                       NR = VR;
                                       frame_t* REJFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,Frame->PollFinal,Reject,0,0);
                                       Transmit(REJFrame->PtrtoFrame,REJFrame->FrameLength);
                                       AX25_voidFreeFrame(REJFrame);

                                       //DLtoPhysical = REJFrame;
                                       //PH_Request.DataRequest = PH_FLAG_SET;
                                       DL_Internal.ACKPending = DL_FLAG_CLEAR;
                                      //DL_CurrentState = DL_CurrentState;
                                   }
                                   else
                                   {
                                       NR = VR;
                                       DL_Internal.SelectiveRejectException++;
                                       frame_t* SREJFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,0,SelectiveReject,0,0);
                                       Transmit(SREJFrame->PtrtoFrame,SREJFrame->FrameLength);
                                        AX25_voidFreeFrame(SREJFrame);
                                       //DLtoPhysical = SREJFrame;
                                       //PH_Request.DataRequest = PH_FLAG_SET;
                                       DL_Internal.ACKPending = DL_FLAG_CLEAR;
                                       //DL_CurrentState = DL_CurrentState;
                                   }
                                    AX25_voidFreeFrame(Frame);
                               }
                           }
                           else /*Selective Reject Enable*/
                           {

                               //free(Frame->Info);
                               DL_Internal.RejectException = DL_FLAG_SET;
                               NR = VR;
                               frame_t* REJFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,Frame->PollFinal,Reject,0,0);
                               Transmit(REJFrame->PtrtoFrame,REJFrame->FrameLength);
                                AX25_voidFreeFrame(REJFrame);
                               //DLtoPhysical = REJFrame;
                               //PH_Request.DataRequest = PH_FLAG_SET;
                               DL_Internal.ACKPending = DL_FLAG_CLEAR;
                               //DL_CurrentState = DL_CurrentState;
                           }
                           //AX25_voidFreeFrame(Frame);
                        }
                    }
                }
                else
                {
                    free(Frame->Info);
                    if(Frame->PollFinal == 1)
                    {
                        NR = VR;
                        frame_t* RNRFrame = AX25_frame_tPtrCreatSFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME,1,ReceiveNotReady,0,0);
                        Transmit(RNRFrame->PtrtoFrame,RNRFrame->FrameLength);
                        AX25_voidFreeFrame(RNRFrame);
                        //DLtoPhysical = RNRFrame;
                        //PH_Request.DataRequest = PH_FLAG_SET;
                        DL_Internal.ACKPending = DL_FLAG_CLEAR;
                    }
                    //DL_CurrentState = DL_CurrentState;
                }
            }
            else
            {
                DL_NRErrorRecovery();
                DL_CurrentState = SM_DL_STATE_AWAITING_CONNECTION;
            }
        }
    }
}
