#ifndef CCSDS_FOP_PRIVATE_H
#define CCSDS_FOP_PRIVATE_H
#include "STD_TYPES.h"
#include "CCSDS_FOP_INTERFACE.h"
/*_______________________________________________________________________________________*/
/*FOP states*/
#define CCSDS_ACTIVE_STATE                  0
#define CCSDS_RETRANSMIT_WITHOUT_WAIT       1
#define CCSDS_RETRANSMIT_WITH_WAIT          2
#define CCSDS_INIT_WITHOUT_BC_FRAME         3
#define CCSDS_INIT_WITH_BC_FRAME            4
#define CCSDS_INITIAL                       5
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
#define VER                     0
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP Notifications*/
#define CCSDS_ALERT_CLCW                                0
#define CCSDS_ALERT_SYNCH                               1
#define CCSDS_ALERT_LIMIT                               2
#define CCSDS_ALERT_NNR                                 3
#define CCSDS_ALERT_T1                                  4
#define CCSDS_ALERT_LLIF                                5
#define CCSDS_ALERT_TERM                                6
#define CCSDS_ALERT_LOCKOUT                             7
#define CCSDS_SUSPEND                                   8
#define CCSDS_ACCEPT_RESPONSETO_DIRECTIVE               9
#define CCSDS_ACCEPT_RESPONSETO_TRANSFARFDU             10
#define CCSDS_REJECT_RESPONSETO_DIRECTIVE               11
#define CCSDS_REJECT_RESPONSETO_TRANSFARFDU             12
#define CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE     13
#define CCSDS_POSITIVE_CONFIRM_RESPONSETO_TRANSFARFDU   14
#define CCSDS_NEGATIVE_CONFIRM_RESPONSE                 15
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
#define CCSDS_READY                         1
#define CCSDS_NOT_READY                     0
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
typedef struct
{
    /*data transmitted within frame */
    u8* first_byte_pointer_TC_Data;
    u16 length_TC_Data;
    u8 TC_VCID ;
    u8 BypassFlag;
    u8 ControlCommandFlag;
    u8 N_of_S;
    /*TC frame*/
    u8* frame;
    u32 frame_length;
    /*received contents*/
    /*SPACCECRAFT ID*/
    u16 SCID;
    /*VIRTUAL CHANNEL ID*/
    u8 TM_VCID;
    /*Operation control field flag*/
    u8 OCF_flag;
    /*Master channel Frame Counter*/
    u8 MCFC;
    /*Virtual Channel frame Counter*/
    u8 VCFC;

    /*Transfer frame secondary header flag*/
    u8 TFSH_flag;
    u8 Synch_flag;
    u8 PacketOrder_flag;

    /*segment length ID*/
    u8 SLI;

    /*data received within TM frame*/
    u8* first_byte_pointer_TM_Data;
    u32 length_TM_Data;
    u8 TFSH_length;
    u8* SecondaryHeaderData;

    /*Received CLCW Contents*/
    u8 StatusField;     //3bits
    u8 COPInEffect;
    u8 Rec_TC_VCID;     //6bits
    u8 Flags ;          //5bits
    u8 FarmBCounter;    //2bits
    u8 ReportValue;     //8bit
}CCSDS_TC;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
//u8  FOP_TMVCID;
u16 FOP_SCID;
/*enable disable the REED SOLOMON channel coding*/
/*enable by default*/
//u8  CCSDS_FOP_RS_ON;
/*enable disable the RANDOMIZER channel coding*/
/*enable by default*/
//u8  CCSDS_FOP_RANDOMIZER_ON;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP functions*/
void CCSDS_void_Receive_Request_to_Transfer_FDU(u8 VCID,u8 service_type,u8* FDU_PTR,u16 FDU_length,CCSDS_TC* TC_frame);
void CCSDS_void_ReceiveResponseFromLowerLayer(u8 VCID,u8 Response,CCSDS_TC* TC);
void CCSDS_void_ReceiveDirectiveFromManagementFunction(u8 VCID,u8 DirectiveType,CCSDS_TC TC,u32 DirectiveVar);
void CCSDS_void_FOPTIMEREXP(u8 VCID,CCSDS_TC* TC);
void CCSDS_void_CLCWarrives(u8 VCID,u8 COPeffect,u8 RecVCID,u8 flags ,u8 RecN_of_R,CCSDS_TC* TC);
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*FOP actions functions*/
void CCSDS_void_Start_Timer(u8 VCID);
void CCSDS_void_cancel_Timer(u8 VCID);
void CCSDS_void_lookfordirective(u8 VCID);
void CCSDS_void_initiateADretransmission(u8 VCID);
void CCSDS_void_initiateBCretransmission(u8 VCID);
void CCSDS_void_alert(u8 reason_code,u8 VCID);
void CCSDS_void_purging_the_Sent_Queue(u8 VCID);
void CCSDS_void_purging_the_Wait_Queue(u8 VCID);
void CCSDS_void_Suspend();
void CCSDS_void_REJECT(u8 Reject_Reason);
void CCSDS_void_ACCEPT(u8 Accept_Reason);
void CCSDS_void_INITIALIZE(u8 VCID);
void CCSDS_void_CONFIRM(u8 Confirm_Reason);
void CCSDS_void_ReleaseCopyOfTypeBCFrame(u8 VCID);
void CCSDS_void_RemoveFrameFromSentQueue(u8 Frame,u8 VCID);
void CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(u8 RecNofR,u8 VCID);
void CCSDS_void_AddFrametoSentQueue(u8* Frame,u32 FrameLength,u8 FrameType,u8 VCID);
void CCSDS_void_TRANSMITTBC(CCSDS_TC* TC,u8 VCID);
void CCSDS_void_TRANSMITTAD(CCSDS_TC* TC,u8 VCID);
void CCSDS_void_TRANSMITTBD(CCSDS_TC* TC,u8 VCID);
void CCSDS_void_lookforFDU(CCSDS_TC* TC,u8 VCID);
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*Extra functions used by FOP*/
u8 CCSDS_u8_TM_DEFRAME(u8* Frame,u32 Frame_Length,CCSDS_TC* TC_data);
void CCSDS_void_TC_Framing(CCSDS_TC* TC_frame);
void CCSDS_FOP_void_packet_receiption(u8* packet,u32 packet_len,u8* sec_data,u32 sec_data_len,u8 VCID);
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/**timer handler function**/
void CCSDS_void_time_handler(size_t timer_id, void * user_data);
#endif
