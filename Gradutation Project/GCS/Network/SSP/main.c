#include <stdio.h>
#include <stdlib.h>

#include"ssp_private.h"
#include"ssp_interface.h"
#include"ssp_DL_SM.h"
#include"STD.h"
#include"BIT_MATH.h"
void start()
{
    //select if you want to receive or sent??
    printf("*************************** note that ...ENTER 'S' TO SEND AND 'R' TO RECEVE :) *************************\n");
    u8 value=getchar();
    if(value=='S')
    {
//fill RX buffer for test response
RX_FRAME_buffer->frame_type=ACK_RESPONSE;
RX_FRAME_buffer->frame_length=min_data_len;
build_std_frame(RX_FRAME_buffer);

TX_FRAME_buffer->frame_type=READ_0;
TX_FRAME_buffer->frame=data_to_be_sent;
TX_FRAME_buffer->frame_length=4+min_data_len;
TX_FRAME_buffer->dataless=0;
build_other_frame(TX_FRAME_buffer,data_to_be_sent,4+min_data_len);
printf("\n TX_buffer ....");
print_frame(TX_FRAME_buffer->frame,TX_FRAME_buffer->frame_length);
Current_State=S_IDLE;
    }
    else if(value=='R')
    {
//fill RX_buffer for test receive
RX_FRAME_buffer->frame_type=GET_0;
RX_FRAME_buffer->frame=data_to_be_sent;
RX_FRAME_buffer->frame_length =4+min_data_len;
RX_FRAME_buffer->dataless=1;
build_other_frame(RX_FRAME_buffer,data_to_be_sent,4+min_data_len);
printf("\n RX_buffer ....");
print_frame(RX_FRAME_buffer->frame,RX_FRAME_buffer->frame_length);
Current_State=R_IDLE;
    }
    else
       {
           printf("\n invalid !! ");
           Current_State=17;
       }

}

int main()
{
    /*
    note that>>>
    it should every time TX_buffer will be filled by the process if  SM_EVENT.TX_BUFFER_FULL==1 or not
    to ignore this data and continue processing on the previous data or this data will be processed and sent !!
    filling TX_buffer will occur using interrupt may be?!
    //that check event will be also in  this interrupt routine ....keep it in mind :)
    //TX_BUFFER_FULL will be 1 only when i still need the previous buffer data on waitresponse for the previous session...
    */
    /*same as rx_buffer_busy*/
    charptr=0;
    CLEAR_EVENT();
    start();
    while(1)
        {

   switch(Current_State)
   {
     case S_IDLE:
       {
        printf("\n IDLE state");
        S_IDLE_state();

       }break;

       case S_WAIT_RESPONSE:
       {
        //printf("\n WAITACK state");
        S_WAIT_RESPONSE_state();

       }break;
       case S_TRANSMIT:
       {
          // printf("\n  state");
           S_TRANSMIT_state();

       }break;
       case S_RETRANSMIT:
       {
           //printf("\n NOACK state");
           S_RETRANSMIT_state();

       }break;
       case R_IDLE:
        {
            R_IDLE_state();
        }break;
        case R_START_RECEIVE:
        {
            R_START_RECEIVE_state();
        }break;
        case R_END_RECEIVE:
        {
            R_END_RECEIVE_state();
        }break;
        default:
            printf("\n invalid");
            break;

   }
}
}
