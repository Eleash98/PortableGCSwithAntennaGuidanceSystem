/*
 * MDL_private.h
 *
 *  Created on: Jan 21, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef MDL_PRIVATE_H_
#define MDL_PRIVATE_H_

/*************************STATES*********************/
#define SM_MDL_STATE_READY              0
#define SM_MDL_STATE_NEGOTIATING        1



/***********************Internal Flags****************/

typedef struct
{
    u32 RC;
    u32 NM201; /*Maximum number of retries of the XID command*/
}MDL_InternalFlag_t;

/********************Variables****************/
u8  MDL_CurrentState = SM_MDL_STATE_READY;



u8 AcceptTransmission   = MDL_HALF_DUPLEX;
u8 AcceptReject         = MDL_SEL_REJ;
u8 AcceptModulo         = MDL_MOD_8;
u32 AcceptRXLen         = 256;
u32 AcceptTXLen         = 256;
u32 AcceptRXWindow      = 7;
u32 AcceptTXWindow      = 7;
u32 AcceptACKTimer      = 3000;
u32 AcceptRetries       = 10;


/******************SubRoutines*****************/
/*void MDL_Initiate_N1_Notification(void);
void MDL_N1_NotificationResponse(void);
void Complete_N1_Notification(void);
void InitiateWindowNotification(void);
void WindowNotificationResponse(void);
void CompleteWindowNotification(void);
void Initiate_T1_Negotiation(void);
void T1_NegotiationResponse(void);
void Complete_T1_Negotiation(void);
void InitiateRetryNegotiation(void);
void RetryNegotiationResponse(void);
void CompleteRetryNegotiation(void);
void InitiateOptionalFunctionsNegotiation(void);
void OptionalFunctionsNegotiationResponse(void);
void CompleteOptionalFunctionsNegotiation(void);
void InitiateClasses_Of_ProceduresNegotiation(void);
void Classes_Of_ProceduresNegotiationResponse(void);
void CompleteClasses_Of_ProceduresNegotiation(void);
*/
/******************STATES FUNCTIONS**************/
void MDL_ReadyState(void);
void MDL_NegotiatingState(void);


/*****************Flags*********************/
MDL_InternalFlag_t      MDL_Internal;

frame_t* TransmittedFrame = NULL;
#endif /* MDL_PRIVATE_H_ */
