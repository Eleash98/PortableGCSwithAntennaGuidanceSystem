/*
 * DL_private.h
 *
 *  Created on: Jan 26, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef DL_PRIVATE_H_
#define DL_PRIVATE_H_

/*********************STATES****************************/
#define SM_DL_STATE_DISCONNECTED						0
#define SM_DL_STATE_AWAITING_CONNECTION					1
#define SM_DL_STATE_AWAITING_RELEASE					2
#define SM_DL_STATE_CONNECTED							3
#define SM_DL_STATE_TIMER_RECOVERY						4
#define SM_DL_STATE_AWAITING_CONNECTION_2_2             5

/********************Macros******************************/
#define DL_AWAITING_CONNECTION_2_2              0
#define DL_AWAITING_CONNECTION                  1


#define DL_IFRAME_NOT_ACK                       0
#define DL_IFRAME_ACK                           1



/*******************Internal Flags***********************/

void (*TurnOnTransmitter)(void) = 0;
void (*TurnOffTransmitter)(void) = 0;

void Transmit(u8 *Frame, u32 FrameLength);
frame_t*(*Receive)(void) = 0;



typedef struct
{
	u8 Layer3Init;
	u8 PeerReceiverBusy;
	u8 OwnReceiverBusy;
	s16 RejectException;
	s16 SelectiveRejectException;
	u8 ACKPending;
	f32 SRT;				/*Smooth round trip time*/
	u32 T1V;				/*next Value for T1*/
	//u32 N1;					/*Maximum number of octets in info field of a frame*/
	//u32 N2;					/*Maximum number of retries permitted*/
	//u8 K;					/*Window Size*/
}DL_Internalflag_t;
/*T1V default initial value is SRT*/

/*
typedef struct{
    u8 ACKState;
    frame_t* Frame;
}SentQueue_t;
*/


/*******************Queues******************/
frame_t** IQueue        = 0;
s32       IQueueCounter = 0;

frame_t** ReceivedQueue        = 0;
s32       ReceivedQueueCounter = 0;

frame_t** SentQueue       = 0;
s32      SentQueueCounter = 0;



/***************Variables******************/
u8 DL_CurrentState = SM_DL_STATE_DISCONNECTED;

u8 SeizeConfirm ;

//u8 RejectType		;
u8 TransmissionMode ;

f32 DefaultSRT	=	1000;

u8 DL_AwaitingConnectionVersion = DL_AWAITING_CONNECTION;

/*****************functions*****************/

void DL_FreeSentQueue(u8 FrameNR);

void DL_voidPushRequest();

void DL_voidPushIQueue(frame_t*Frame);
frame_t* DL_frame_tPtrPopIQueue(void);

void DL_voidPushSentQueue(frame_t* IFrame);
frame_t* DL_PopSentFramefromNR(u8 Copy_u8NR);

void ShiftReceivedQueue(s32 Shift);
void DL_voidPushReceivedFrame(frame_t* Frame);

void ShiftIQueue(void);
void DL_voidClearRequests(void);
void DL_voidDiscardQueue (frame_t** Q,s32 *Length);

void DL_IFrameReceived(frame_t* Frame);



/****************SubRoutines****************/
void DL_NRErrorRecovery(void);
void DL_EstablishDataLink(void);
void DL_ClearExceptionConditions(void);
void DL_TransmitEnquiry(void);
void DL_ResponseEnquiry(u8 Final);
void DL_InvokeReTransmission(u8 Copy_u8NR);
void DL_CheckIFrameACK(u8 FrameNR);
void DL_CheckNeedForResponse(frame_t* Frame);
void DL_UICheck(frame_t* Frame);
void DL_SelectT1Value(void);
void DL_EstablishExtendedDataLink(void);
void DL_SetVersion2_0(void);
void DL_SetVersion2_2(void);


/*************STATES Functions*********/
void DL_DisconnectState(void);
void DL_AwaitingConnectionState(void);
void DL_AwaitingReleaseState(void);
void DL_ConnectedState(void);
void DL_TimerRecoveryState(void);
void DL_AwaitingConnection2_2(void);


/****************Flags****************/
DL_Internalflag_t	DL_Internal;


#endif /* DL_PRIVATE_H_ */
