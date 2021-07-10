#ifndef CCSDS_FOP_INTERFACE_H
#define CCSDS_FOP_INTERFACE_H
#include "STD_TYPES.h"
#include <stdlib.h>
#include <stdio.h>
//#include "CCSDS_FOP_INTERFACE.h"
/*_______________________________________________________________________________________*/
//#define USED_SCID               0b0111001011
//#define TMVCID                  0b110
#define SET                     1
#define CLEAR                   0
#define CCSDS_FOP_DEBUG         1
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP directives*/
#define CCSDS_INITIATE_AD_WITHOUT_CLCW      0
#define CCSDS_INITIATE_AD_WITH_CLCW         1
#define CCSDS_INITIATE_AD_WITH_UNLOCK       2
#define CCSDS_INITIATE_AD_WITH_SET_VR       3
#define CCSDS_TERMINATE_AD_SERVICE          4
#define CCSDS_RESUME_AD_SERVICE             5
#define CCSDS_SET_VS                        6
#define CCSDS_SET_FOP_SILDING_WINDOW        7
#define CCSDS_SET_T1_INITIAL                8
#define CCSDS_SET_TRANSMISSION_LIMIT        9
#define CCSDS_SET_TIMEOUT_TYPE              10
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP services*/
#define CCSDS_FOP_AD_Service                0
#define CCSDS_FOP_BD_Service                1
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP parts*/
#define CCSDS_FOP_CLCW                                       0
#define CCSDS_FOP_TIMER_EXPIRE                               1
#define CCSDS_FOP_RECIVE_REQUESTTO_TRANSFER_FDU              2
#define CCSDS_FOP_RECEIVE_DIRCTIVEFROM_MANGEMENT_FUNCTION    3
#define CCSDS_FOP_RECEIVE_RESPONSEFROM_LOWER_PROCEDURE       4
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*Response from lower layer*/
#define CCSDS_FOP_AD_ACCEPT                 0
#define CCSDS_FOP_AD_REJECT                 1
#define CCSDS_FOP_BC_ACCEPT                 2
#define CCSDS_FOP_BC_REJECT                 3
#define CCSDS_FOP_BD_ACCEPT                 4
#define CCSDS_FOP_BD_REJECT                 5
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
typedef struct
{
    u8 State;
    u8 V_of_S;         //Transmitter frame sequence number
    u8 N_of_R;
    /*Wait Queue*/
    u8* WaitQueue;
    u32 wait_queue_FDU_length;
    /*Sent Queue*/
    u8* SentQueue;
    u8  sent_queue_counter;
    u32* sent_queue_frame_lengths;
    /*to be changed to u32 instead of u16*/
    u32 SentQueue_Length;
    /*1 for BC frame and 0 for AD frame*/
    u32 sent_queue_frame_type;
    u32 ToBeRetramittedFlag;
    u8 ADOutFlag;
    u8 BDOutFlag;
    u8 BCOutFlag;
    u8 NNR;              //Expected Acknowledgment Frame Sequence number
    /**Timer parameters**/
    u32 T1Initial;
    u8 TT;              //TimeoutType
    size_t timer;
    /******************/
    u32 TrasmissionLimit;
    u32 TrasmissionCount;
    u8 K;               //FOP Sliding window width
    u8 SS;              //Suspend state
    u8 flags;
} FOPVariables_t;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
typedef struct{
    u8* data;
    u32* data_len;
    u8* sec_data;
    u32* sec_data_len;
    u8* VCID;
    u32 data_Used_size;
    u32 sec_data_Used_size;
    u16 Queue_Counter;

}FOPreceivedqueue;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
FOPVariables_t VirtualChannels[64];
FOPreceivedqueue FOP_Received_Queue;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
u16 CCSDS_Notifications;
u8 CCSDS_Abort;
u8* CCSDS_copy_frame_tolowerleyer;
u32 CCSDS_lengthof_copy_frame_tolowerleyer;
/**0 if no frame to be transmittted
   1 if the transmitted frame is an AD
   2 if the transmitted frame is a BD
   3 if the transmitted frame is a BC*/
u8  CCSDS_transmitte_frame_tybe;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP for GCS*/
void CCSDS_void_FOP(u8 procedure,u8 VCID ,void*PTR1,void*PTR2,void*PTR3);
/*FOP initialization function*/
void CCSDS_void_FOPINIT();
/*FOP configuration function*/
void CCSDS_void_FOPconfig(u16 SCID);
/*To get a data from the received queue*/
u8* CCSDS_u8pointer_GetDataFromReceivedAueue(u32* data_length,u32* sec_data_lengyh,u8* VCID);
/*To free all the memory used by the FOP*/
void CCSDS_void_FOP_free();
#endif
