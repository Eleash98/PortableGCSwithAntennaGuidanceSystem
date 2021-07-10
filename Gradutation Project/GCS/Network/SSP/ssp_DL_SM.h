#ifndef SSP_DL_SM_H_INCLUDED
#define SSP_DL_SM_H_INCLUDED
/*.****************************events*********************************************/

typedef struct
{
   u8 R_DATARDY;               //RX_buffer has data now
   u8 R_wrong_direction;       //receiving  a response while it may be request
   u8 R_InvalidData;           //invalid data addresses or values or variables!!
   u8 R_EMSG;                  //receiving error message
   u8 R_EMSG_N;                //receiving error message for (N)10 times
   u8 R_VALID;                 //receiving valid frame

   u8 S_DATARDY ;                       //data packet needs to be sent
   u8 S_WAITRESPONSE_TIMERTIMOUT;       //ack timer timeout
   u8 S_WAITRESPONSE_TIMERTIMOUT_N;     //ack timer timeout for (N)10 times

   u8 S_ACK_response_Rcvd;     //ack packet received
   u8 S_NAK_response_Rcvd;     //ack packet received

   u8 S_TX_timout;             //timeout for sending data
   u8 TX_BUFFER_BUSY;          //indicate TX buffer is full now...busy TX buffer
   u8 RX_BUFFER_BUSY;          ////indicate RX buffer is full now...busy RX buffer


   u8 LOSS_SESSION;            //to indicate session is lost
   u8 S_M_Rcvd;

}SM_EVENT_t;

/*.************************************************************************/
#define SM_SET    1
#define SM_CLEAR  0
/*.********************************....*****************************************/

extern u8 Current_State;
extern frame_struct *TX_FRAME_buffer;     //send buffer*/
extern frame_struct *RX_FRAME_buffer;     //RCV buffer*/
extern u8 data_to_be_sent[4];            //data to be sent as a response*/
extern u8 SM_data_type;         //data  type to be sent as a response*/
extern u8 SM_dataless;
extern u8 SM_datalength;
extern u8 *SM_response_data;
extern u8 NACK_RCVD_ITERATION;
extern SM_EVENT_t  SM_EVENT;
extern u8 charptr;
/////////////////////////////////////////////////////
extern u8 WAITRESPONSE_TIMER;
extern u8 WAITRESPONSE_TIMEOUT;
extern u8 WAITRESPONSE_TIMEOUT_ITERATIONS;
////////////////////////////////////////////////
extern u8 SENDDATA_TIMER;                  //timer for sending data
extern u8 SENDDATA_TIMEOUT;
//extern u8 SENDDATA_TIMEOUT_COUNTER;
//////////////////////////////////////////////
extern u8 RECEIVEDATA_TIMER;
extern u8 RECEIVEDATA_TIMEOUT;
//////////////////////////////////////////////

/*.******************************states*************************************/
#define S_IDLE             0         //The initial state for transimitter
#define S_WAIT_RESPONSE    1         //wait state for S_FSM
#define S_TRANSMIT         2         //transmit state for S_FSM
#define S_RETRANSMIT       3         //retransmit state for S_FSM


#define R_IDLE             4        //idle state for R_FSM
#define R_START_RECEIVE    5        //start receive state for
#define R_END_RECEIVE      6        //END receive state

/*.***************************timers**********************************************/
//#define WAITRESPONSE_TIMER      0   //timers flags
//#define TRANSMIT_TIMER          1
//#define TIMER_H                 2

/*.****************************S_FSM states functions**************************************************/
void S_IDLE_state();
void S_TRANSMIT_state();
void S_WAIT_RESPONSE_state();
void S_RETRANSMIT_state();
/*.***************************R_FSM states function*************************************************************************/
void R_IDLE_state();
void R_START_RECEIVE_state();
void R_END_RECEIVE_state();


/*.*******************************subroutines/actions*************************************************/
void Rcv_NEXT_char(frame_struct *FRAME);//receive next char
void R_DEAL_DATA();                    //deal with data packet
void R_check_valid_data();
void SEND_M();                       //send management packet
void SEND_ACK();                     //send ack packet
void SEND_NAK_FAILED();              //send nak failed
/*.***********************************************************************************/
void S_DEAL_DATA();
void S_SEND_DATA();                    //send data packet
/*.*******************deal with buffers functions************************************/
void CLEAR_RX_BUFFER();
void CLEAR_TX_BUFFER();

void check_RX_BUFFER();
void check_TX_BUFFER();

void R_check_RX_BUFFER();
void R_check_TX_BUFFER();
/*.******************timers function************************************************/
void RE_START_TIMER(u8);
void STOP_TIMER(u8 );
/*.*************************************************************************************/
void CLEAR_EVENT();
#endif // SSP_DL_SM_H_INCLUDED
