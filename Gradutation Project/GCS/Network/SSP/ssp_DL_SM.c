
#include <stdio.h>
#include <stdlib.h>
#include"ssp_private.h"
#include"ssp_interface.h"
#include"STD.h"
#include"BIT_MATH.h"
#include"ssp_DL_SM.h"

/*.**************************************************************************************************/
frame_struct testSframe,testRframe,testframe;
frame_struct*FRAME=&testframe;
frame_struct*TX_FRAME_buffer=&testSframe;        //TX_BUFFER
frame_struct*RX_FRAME_buffer=&testRframe;        //RX_BUFFER
SM_EVENT_t  SM_EVENT;
u8 data_to_be_sent[4]={'d','d','d','d'};
u8 SM_data_type=0;
u8 SM_dataless=1;
u8 SM_datalength=0;
u8 *SM_response_data =data_to_be_sent;
u8 charptr=0;
u8 Current_State =R_IDLE;
u8 NACK_RCVD_ITERATION=5;
//////////////////////////////////////////
u8 WAITRESPONSE_TIMER  =0;             //timer value
u8 WAITRESPONSE_TIMEOUT =6;            //6 msec for test    //value of timeoout interval
u8 WAITRESPONSE_TIMEOUT_ITERATIONS=5;
//////////////////////////////////////////
u8 SENDDATA_TIMER=0;                   //timer value
u8 SENDDATA_TIMEOUT=6;                  //value of timeout interval
//////////////////////////////////////////
u8 RECEIVEDATA_TIMER=0;
u8 RECEIVEDATA_TIMEOUT=6;
/////////////////////////////////////////////

 /*.**********************************S_FSM_STATES function********************************************************/
void S_IDLE_state()
{

    printf("\n .....S_IDLE_STATE.....");
    printf("\n checking TX buffer..");
    check_TX_BUFFER();
    //check if TX_buffer is full and there is a data needs to be sent
    if(SM_EVENT.S_DATARDY==SM_SET)
    {
        printf("\n sending data .....");
        Current_State=S_TRANSMIT;
        SM_EVENT.TX_BUFFER_BUSY=SM_SET;          //till now i'm not sure that it should be activated here??!!
        RE_START_TIMER(SENDDATA_TIMER);   //start senddata timer
        return;
    }
    //if no data in the buffer
    else
    {
        //printf("\n no data needs to be sent here:)");
        Current_State=S_IDLE;
        CLEAR_EVENT();
        return;
    }

    //CLEAR_EVENT();
}
/*.******************************************************************************************/
void S_TRANSMIT_state()
{
    printf("\n S_TRANSMIT_state.. ");
      //check for transmit timer
      if(SENDDATA_TIMER == SENDDATA_TIMEOUT)
       {
        //send data timeout occur
        printf("\n sending for long time!!...flush the buffer and go to idle state");
        CLEAR_TX_BUFFER();
        SM_EVENT.TX_BUFFER_BUSY=SM_CLEAR;
        STOP_TIMER(SENDDATA_TIMER);
        Current_State=S_IDLE;
       }
       else   //no timeout ...send data
       {
     //send data now
    S_SEND_DATA();
    Current_State=S_WAIT_RESPONSE;
    //start ack timer
    RE_START_TIMER(WAITRESPONSE_TIMER);


       }
}
/*.******************************************************************************************/
void S_WAIT_RESPONSE_state()
{
    printf("\n wait reaponse state");
    //recieve frame needed to be ack?!
    //module to receive ack frame  frame
    //SM_EVENT.S_ACK_Rcvd=SM_SET;
    check_RX_BUFFER();
    while(WAITRESPONSE_TIMER != WAITRESPONSE_TIMEOUT)
    {
       if(SM_EVENT.S_ACK_response_Rcvd==SM_SET )
        {
        printf("\n received ack...");
        print_frame(RX_FRAME_buffer->frame,RX_FRAME_buffer->frame_length);
        printf("\n stop ack timer");
        printf("\n reset ack timout iteration");
        printf("\n flush TX buffer now");
        printf("\n go to idle state");
        STOP_TIMER(WAITRESPONSE_TIMER);
        WAITRESPONSE_TIMEOUT_ITERATIONS=5;   //set
        CLEAR_TX_BUFFER();
        Current_State=S_IDLE;
        //clear timout
        STOP_TIMER(WAITRESPONSE_TIMER);
        SM_EVENT.TX_BUFFER_BUSY=SM_CLEAR; //busy tx bufffer
         return;       //out of state
        }
        else if(SM_EVENT.S_NAK_response_Rcvd==SM_SET)
        {
            //nak recieved
            //data needs to be sent again
            printf("\n sent data lost!!you need to retransmit it :) ");
            printf("\n go to retransmit state");
            //SM_EVENT.S_ACK_TIMERTIMOUT=SM_SET;
            Current_State=S_RETRANSMIT;
            //clear timout
            STOP_TIMER(WAITRESPONSE_TIMER);
            SM_EVENT.TX_BUFFER_BUSY=SM_SET; //busy tx bufffer
            return;
        }
      else if(SM_EVENT.R_EMSG==SM_SET)
       {
     //no data response received
     printf("\n invalid response in RX buffer");
     printf("\n still waiting until ACK or NAK or timeout occur.. check R_buffer again..");
     SM_EVENT.TX_BUFFER_BUSY=SM_SET; //busy tx bufffer
     check_RX_BUFFER();
       }
    }//end of while
    //routine performed when ack timeout occur
    //ack timeout event fired
    //decrease no of timeout iteration
    printf("\n ack timeout occur");
    printf("\n go to retransmit state");
    SM_EVENT.S_WAITRESPONSE_TIMERTIMOUT=SM_SET;
    //clear timer
    STOP_TIMER(WAITRESPONSE_TIMER);
    SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
    SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
    Current_State=S_RETRANSMIT;
}
/*.******************************************************************************************/
void S_RETRANSMIT_state()
{    //check if retransmit due to timeout response
    if(SM_EVENT.S_WAITRESPONSE_TIMERTIMOUT==SM_SET)
    {
        if(WAITRESPONSE_TIMEOUT_ITERATIONS >0)
        {
        //decrease no of iteration
        //send again
        WAITRESPONSE_TIMEOUT_ITERATIONS--;
        //retransmit again
        //clear timout
       RE_START_TIMER(WAITRESPONSE_TIMER);
       S_SEND_DATA();
       Current_State=S_WAIT_RESPONSE;
       return;

        }
        else if(WAITRESPONSE_TIMEOUT_ITERATIONS==0)
        {
            //loss session ..
            //fire loss session event
            //go to ideal state
            printf("\n loss this session");
            SM_EVENT.LOSS_SESSION=SM_SET;
            SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
            SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
            Current_State=S_IDLE;
            WAITRESPONSE_TIMEOUT_ITERATIONS=5;
            return;
        }
    }
    else if(SM_EVENT.S_NAK_response_Rcvd==SM_SET)
    {
        if( NACK_RCVD_ITERATION >0)
        {
        //nak received.....need to retransmit again
        //sent data again
        S_SEND_DATA();
        //start waitresponse timer
       RE_START_TIMER(WAITRESPONSE_TIMER);
       Current_State=S_WAIT_RESPONSE;
       NACK_RCVD_ITERATION--;
       }
       else if( NACK_RCVD_ITERATION==0)
       {
           printf("\n invalid session");
           printf("\n request synchronization");
           //put ping in tx_biffer
           //send it
           TX_FRAME_buffer->frame_type=PING;
           S_SEND_DATA();

           printf("\n go to idle state");
           NACK_RCVD_ITERATION=5;
           WAITRESPONSE_TIMEOUT_ITERATIONS=5;
           STOP_TIMER(WAITRESPONSE_TIMER);
           STOP_TIMER(SENDDATA_TIMER);
           CLEAR_EVENT();
           CLEAR_TX_BUFFER();
           Current_State=S_IDLE;
       }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/*.**********************************R_FSM_STATE function******************************************/
void R_IDLE_state()
{
    printf("\n .....R_IDLE_STATE.....");
    printf("\n checking RX buffer..");
    R_check_RX_BUFFER();
    //check if RX_buffer is full and there is received data(start with fend or invalid?)
    if(SM_EVENT.R_DATARDY==SM_SET)
    {
        printf("\n receiving data .....");
        Current_State=R_START_RECEIVE;
        RE_START_TIMER(RECEIVEDATA_TIMER);   //start receivedata timer
        return;
    }
    //if no data in the buffer
    else if(SM_EVENT.R_EMSG==SM_SET)
    {
        printf("\n ignore this data :)");
        Current_State=R_IDLE;
        CLEAR_EVENT();
        return;
    }
    else //no data here
    {
        printf("\n  there is no  data to receive :)");
        Current_State=R_IDLE;
        CLEAR_EVENT();
        return;

    }

}
/*.***************************************************************************************/
void R_START_RECEIVE_state()
{
 //check valid recieved data
 //recieving data until timout receiving data fires
 printf("\n R_start_receive_state.. ");
      //check for transmit timer
      /*if(RECEIVEDATA_TIMER == RECEIVEDATA_TIMEOUT)
       {
        //receive data timeout occur
        printf("\n receiving for long time!!...flush the buffer ... go to idle state");
        CLEAR_RX_BUFFER();
        SM_EVENT.RX_BUFFER_BUSY=SM_CLEAR;
        STOP_TIMER(RECEIVEDATA_TIMER);
        Current_State=R_IDLE;
        return;
       }*/
       //else   //no timeouth...receive data
       //{
         SM_EVENT.RX_BUFFER_BUSY=SM_SET;
         R_DEAL_DATA();   //it will generate if data received is valid or not
         if(SM_EVENT.R_EMSG==SM_SET)
          {
              //invalid frame packet recieved
              //flush recieved buffer
              //go to ideal state
              printf("\n invalid received packet frame ..ignore it ");
              CLEAR_RX_BUFFER();
              Current_State=R_IDLE;  //if error frame will be ignored
              CLEAR_EVENT();
              return;

          }
         else if(SM_EVENT.R_VALID==SM_SET)
          {
              //valid received data
              //transmit ack response
              //go to end received to send the response
              printf("\n valid received data packet ");
              Current_State=R_END_RECEIVE;
              SM_EVENT.R_EMSG=SM_CLEAR;
              SM_EVENT.R_InvalidData=SM_CLEAR;
              SM_EVENT.R_wrong_direction=SM_CLEAR;
              return;
          }
          else if(SM_EVENT.R_wrong_direction==SM_SET)
          {
              //it will be responed with NAK_failed and ignore this packet
              printf("\n wrong direction ... response with NAK_FAILED..");
              Current_State=R_END_RECEIVE;
              SM_EVENT.R_EMSG=SM_CLEAR;
              SM_EVENT.R_InvalidData=SM_CLEAR;
              SM_EVENT.R_VALID=SM_CLEAR;
              return;
          }
          //if wrong data recieved
          else if(SM_EVENT.R_InvalidData==SM_SET)
          {
               //it will be responed with NAK_failed and ignore this packet
              printf("\n invalid data... response with NAK_FAILED..");
              Current_State=R_END_RECEIVE;
              SM_EVENT.R_EMSG=SM_CLEAR;
              SM_EVENT.R_VALID=SM_CLEAR;
              SM_EVENT.R_wrong_direction=SM_CLEAR;
              return;
          }
       //}

}
void R_END_RECEIVE_state()
{
    u8 valid=2;
    //only you will enter it for valid received frames
    if(SM_EVENT.R_InvalidData==SM_SET)
        valid=0;
    else if(SM_EVENT.R_wrong_direction==SM_SET)
        valid=1;
    //here it will fill TX_buffer with response suitable for received request
    get_response(RX_FRAME_buffer,valid);
    //now transmit this reponse
    //here frame  length is the length of data
    transmit(TX_FRAME_buffer->frame_type ,data_to_be_sent,TX_FRAME_buffer->dataless,TX_FRAME_buffer->data_length);
    printf("\n session ended..go to idle state");
    CLEAR_EVENT();
    CLEAR_RX_BUFFER();
    CLEAR_TX_BUFFER();
    STOP_TIMER(RECEIVEDATA_TIMER);
    Current_State=R_IDLE;
    SM_EVENT.RX_BUFFER_BUSY=SM_CLEAR;
}
/*...********************************S_FSM subroutines /actions functions************************/
void S_DEAL_DATA()
{
    printf("\n deal data event\n");
    /*process on frame received frame in received buffer*/
  //firstly check for valid length
  //reset length of rx_frame for every check length
  RX_FRAME_buffer->frame_length=0;
   for(u8 i=1;RX_FRAME_buffer->frame[i]!=FEND;i++)
   {
       RX_FRAME_buffer->frame_length++;
       if(RX_FRAME_buffer->frame_length > max_data_len-2 )
       {
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
         SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
         return;
       }
   }
   RX_FRAME_buffer->frame_length+=2;
   if(RX_FRAME_buffer->frame_length <min_data_len ) //check for lenght is smaller than min_length
     {
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
         SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
         return;
     }
     //destuffing the frame
    destuffing_frame(RX_FRAME_buffer);
    //check for valid source and destenition
    if(RX_FRAME_buffer->frame[2]==MY_ADD)   //check dest is my address? here i will inverse it only for checking:)
    {
     if( RX_FRAME_buffer->frame[1]!=0)
     {
         if(RX_FRAME_buffer->frame[1] < ADD1 && RX_FRAME_buffer->frame[1] > ADD7)  //if source is not in our source addresses
         {
         //error
         printf("\n invalid source");
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
         SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
         return;
         }
         else
         {
             printf("\n valid source and destination frame packet ");
         }
     }
     else  //for zer0 source address invalid ...
     {
       //error
       printf("\n wrong source");
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
         SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
         return;
     }
    }
    else //wrong destination
    {
       //error,ignore
       printf("\n wrong destination");
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
         SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
         return;
    }
  //check for valid response type
   if(RX_FRAME_buffer->frame[3]==ACK_RESPONSE  )//for valid pktype
   {
       //check for valid crc
   u16 crcnew=crc_generate(RX_FRAME_buffer->frame,RX_FRAME_buffer->frame_length);
   u8 checkcrc=crc_check(RX_FRAME_buffer,crcnew);
   if(checkcrc==1)
   {
       SM_EVENT.R_EMSG=SM_SET;
       SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
       SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
        return;
   }
       printf("\n ack recieved");
       SM_EVENT.S_ACK_response_Rcvd=SM_SET;
        SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
        return;
   }
   else if(RX_FRAME_buffer->frame[3]==NAK_FAILED)
   {
      //check for valid crc
   u16 crcnew=crc_generate(RX_FRAME_buffer->frame,RX_FRAME_buffer->frame_length);
   u8 checkcrc=crc_check(RX_FRAME_buffer,crcnew);
   if(checkcrc==1)
   {
       SM_EVENT.R_EMSG=SM_SET;
       SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
       SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
        return;
   }
   printf("\n nak received");
    SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
    SM_EVENT.S_NAK_response_Rcvd=SM_SET;
        return;
   }

}
/*.******************************************************************************************/
void S_SEND_DATA()
{
   printf("\n send data event\n");
   ///putting a frame  in sent buffer(till now it will print it)
  transmit(TX_FRAME_buffer->frame_type , TX_FRAME_buffer->frame ,TX_FRAME_buffer->dataless,TX_FRAME_buffer->frame_length-min_data_len);


}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*.******************************************R_FSM subroutines /actions function************************************************/
void R_DEAL_DATA()
{
    printf("\n deal data event\n");
    /*process on frame received frame in received buffer*/
  //firstly check for valid length
  //reset length of rx_frame for every check length
  RX_FRAME_buffer->frame_length=0;
   for(u8 i=1;RX_FRAME_buffer->frame[i]!=FEND;i++)
   {
       RX_FRAME_buffer->frame_length++;
       //here timeout is of how large received packet is
       if(RECEIVEDATA_TIMER==RECEIVEDATA_TIMEOUT)
       {
           printf("\n too long received data...ignore this");
           Current_State=R_IDLE;
           CLEAR_EVENT();
           STOP_TIMER(RECEIVEDATA_TIMER);
           return;
       }
        if(RX_FRAME_buffer->frame_length > max_data_len-2 ) //check for lenght is smaller than min_length
        {
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.R_VALID=SM_CLEAR;
         return;
        }
   }
   //once you finished storing data frame ...receiving timeout should stop
   STOP_TIMER(RECEIVEDATA_TIMER);
   //now processing this received frame :)
   RX_FRAME_buffer->frame_length +=2;
   if(RX_FRAME_buffer->frame_length <min_data_len ) //check for lenght is smaller than min_length
     {
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.R_VALID=SM_CLEAR;
         return;
     }
     //destuffing the frame
    destuffing_frame(RX_FRAME_buffer);
    //check for valid source and destenition
     if(RX_FRAME_buffer->frame[2]==MY_ADD)   //check dest is my address? here i will inverse it only for checking:)
    {
     if( RX_FRAME_buffer->frame[1]!=0)
     {
         if(RX_FRAME_buffer->frame[1] < ADD1 && RX_FRAME_buffer->frame[1] > ADD7)  //if source is not in our source addresses
         {
         //error
         printf("\n invalid source");
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.R_VALID=SM_CLEAR;
         return;
         }
         else
         {
             printf("\n valid source and destination frame packet ");
         }
     }
     else  //for zer0 source address invalid ...
     {
       //error
       printf("\n wrong source");
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.R_VALID=SM_CLEAR;
         return;
     }
    }
    else //wrong destination
    {
       //error,ignore
       printf("\n wrong destination");
         SM_EVENT.R_EMSG=SM_SET;
         SM_EVENT.R_VALID=SM_CLEAR;
         return;
    }
  //check for valid type
   if((RX_FRAME_buffer->frame_type & 0x3F) < 64 )//for valid pktype
   {
       if((RX_FRAME_buffer->frame[3] & 0x3F)==0 || (RX_FRAME_buffer->frame[3] & 0x3F)==1 ) //for ping ,init
          {
              if((RX_FRAME_buffer->frame[3] & 0xC0)!=0)  //ss =0
                  {
                    SM_EVENT.R_EMSG=SM_SET;
                    SM_EVENT.R_VALID=SM_CLEAR;
                    return;
                  }
          }
       else if(((RX_FRAME_buffer->frame[3] & 0x3F)==2) || ((RX_FRAME_buffer->frame[3] & 0x3F)==8)|| ((RX_FRAME_buffer->frame[3] & 0x3F)==4) ||((RX_FRAME_buffer->frame[3] & 0x3F)==5) ) //for ackyes or ackno,id,get,put
           {if(((RX_FRAME_buffer->frame[3] & 0xC0)>>6)>1)           //ss=0,1
              {
             SM_EVENT.R_EMSG=SM_SET;
             SM_EVENT.R_VALID=SM_CLEAR;
              return;
              }
           }
       else if(((RX_FRAME_buffer->frame[3] & 0x3F)==3)&& ((RX_FRAME_buffer->frame[3] & 0xC0)>>6)>2)  //for nackfailed,nackunkown,nackincorrect
             {
              SM_EVENT.R_EMSG=SM_SET;
              SM_EVENT.R_VALID=SM_CLEAR;
               return;
               }
       else if(((RX_FRAME_buffer->frame[3] & 0x3F)==6) || ((RX_FRAME_buffer->frame[3] & 0x3F)==7)) //read,write
                { /*ss=0,1,2,3 valid*/}
           //correct type detected

        if(RX_FRAME_buffer->frame[3]==NAK_FAILED || RX_FRAME_buffer->frame[3]==ACK_RESPONSE || RX_FRAME_buffer->frame[3]==NAK_INCORRECT || RX_FRAME_buffer->frame[3]==NAK_UNKOWN  )
            {
                  //wrong direction ....it require response with
                  SM_EVENT.R_EMSG=SM_CLEAR;
                  SM_EVENT.R_VALID=SM_CLEAR;
                  SM_EVENT.R_wrong_direction=SM_SET;
                  return;

            }
        else   //valid request frame received
        {
                 SM_EVENT.R_EMSG=SM_CLEAR;
                 SM_EVENT.R_VALID=SM_SET;
        }
   }
   else
   {   //wrong type (request was not understod)
       printf("\n wrong type received");
       SM_EVENT.R_EMSG=SM_SET;
       SM_EVENT.R_VALID=SM_CLEAR;
        return;
   }
  //check for valid crc
   u16 crcnew=crc_generate(RX_FRAME_buffer->frame,RX_FRAME_buffer->frame_length);
   u8 checkcrc=crc_check(RX_FRAME_buffer,crcnew);
   if(checkcrc==1)
   {
       SM_EVENT.R_EMSG=SM_SET;
       SM_EVENT.R_VALID=SM_CLEAR;
        return;
   }
   //check for valid data for certain types
  R_check_valid_data();
  if(SM_EVENT.R_InvalidData==SM_SET)
  {
  SM_EVENT.R_VALID=SM_CLEAR;
  SM_EVENT.R_EMSG=SM_CLEAR;
  return;

  }
  //give notification (event) for an valid frame
  SM_EVENT.R_VALID=SM_SET;
  SM_EVENT.R_EMSG=SM_CLEAR;
}
/*.******************************************************************************************/
void R_check_valid_data()
{
    switch(RX_FRAME_buffer->frame_type)
    {
    case PUT_0:
        {
        //for address space0
        //check for invalid addresses
        //if invalid?>>>>fire invalid data
        }break;
    case PUT_1:
        {

        }break;
    case GET_0:
        {

        }break;
    case GET_1:
        {

        }break;
    case READ_0:
        {

        }break;
    case READ_1:
        {

        }break;
    case READ_2:
        {

        }break;
    case READ_3:
        {

        }break;
    case WRITE_0:
        {

        }break;
    case WRITE_1:
        {

        }break;
    case WRITE_2:
        {

        }break;
    case WRITE_3:
        {

        }break;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*.*************************************general subroutines functions****************************************************************/
void CLEAR_EVENT()
{
    SM_EVENT.R_EMSG=SM_CLEAR;
    SM_EVENT.LOSS_SESSION=SM_CLEAR;
    SM_EVENT.R_EMSG_N=SM_CLEAR;
    SM_EVENT.R_DATARDY=SM_CLEAR;
    SM_EVENT.R_InvalidData=SM_CLEAR;
    SM_EVENT.R_wrong_direction=SM_CLEAR;
    SM_EVENT.R_VALID=SM_CLEAR;
    SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
    SM_EVENT.S_DATARDY=SM_CLEAR;
    SM_EVENT.S_M_Rcvd=SM_CLEAR;
    SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
    SM_EVENT.S_TX_timout=SM_CLEAR;
    SM_EVENT.S_WAITRESPONSE_TIMERTIMOUT=SM_CLEAR;
    SM_EVENT.S_WAITRESPONSE_TIMERTIMOUT_N=SM_CLEAR;
    SM_EVENT.TX_BUFFER_BUSY=SM_CLEAR;
}
/*.******************************************************************************************/
void CLEAR_TX_BUFFER()
{
    printf("\n flushing TX_buffer\n");
    for(u8 i=0;i<5;i++)
    {
        TX_FRAME_buffer->frame[i]=0;
    }
    SM_EVENT.TX_BUFFER_BUSY=0;  //empty buffer
}
/*.******************************************************************************************/
void CLEAR_RX_BUFFER()
{
    printf("\n flushing RX_buffer\n");

   for(u8 i=0;i<5;i++)
    {
        RX_FRAME_buffer->frame[i]=0;
    }
}
/*.******************************************************************************************/
void check_RX_BUFFER()  ///modify it don't forget ya fatma
{
    u8 NO_of_ZEROS=0;
    printf("\n checking RX buffer\n");
    //check foe sent buffer if there is data needs to be sent
    //check if it 's flushed or not?
    //check for first 5 bytes(min data frame length) if zero?>>so it's flusshed
    for(u8 i=0;i<min_data_len;i++)
    {
        if(RX_FRAME_buffer->frame[i]==0)
        {
          NO_of_ZEROS++;

        }
    }
    if(NO_of_ZEROS==5)
        {
            RX_FRAME_buffer->flushed_frame=1;   //frame is empty

        }
    //check if flushed
    if(RX_FRAME_buffer->flushed_frame ==0 )  //buffer has data>>not flushed
        {
            if(RX_FRAME_buffer->frame[0]==FEND)
             {
            printf("\n RX buffer has data !!");
            S_DEAL_DATA();
            return;
             }
            else if(TX_FRAME_buffer->frame[0]!=FEND)
            {
            SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
            SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
            printf("\n invalid data received\n");
            return;
            }
        }
        else
        {
            SM_EVENT.S_ACK_response_Rcvd=SM_CLEAR;
            SM_EVENT.S_NAK_response_Rcvd=SM_CLEAR;
            printf("\n no data received");
            return;
        }

}
/*.******************************************************************************************/
void check_TX_BUFFER()
{
    u8 NO_of_ZEROS=0;
    printf("\n checking TX buffer\n");
    //check foe sent buffer if there is data needs to be sent
    //check if it 's flushed or not?
    //check for first 5 bytes(min data frame length) if zero?>>so it's flusshed
    for(u8 i=0;i<min_data_len;i++)
    {
        if(TX_FRAME_buffer->frame[i]==0)
        {
          NO_of_ZEROS++;

        }
    }
    if(NO_of_ZEROS==5)
        {TX_FRAME_buffer->flushed_frame=1;
        SM_EVENT.S_DATARDY=SM_CLEAR;

        }
    //check if flushed
    if(TX_FRAME_buffer->flushed_frame ==0 )  //buffer has data>>not flushed
        {
            if(TX_FRAME_buffer->frame[0]==FEND)
             {
            printf("\n TX buffer is ready !!");
            SM_EVENT.S_DATARDY=SM_SET;
            SM_EVENT.LOSS_SESSION=SM_CLEAR;
             }
            else if(TX_FRAME_buffer->frame[0]!=FEND)
            {
            SM_EVENT.S_DATARDY=SM_CLEAR;
            printf("\n invalid data to be sent\n");
            return;

            }
        }
        else
        {
            SM_EVENT.S_DATARDY=SM_CLEAR;
            printf("\n no data needs to be send");
            return;
        }
}
void R_check_RX_BUFFER()
{
    u8 NO_of_ZEROS=0;
    printf("\n checking RX buffer\n");
    //check foe sent buffer if there is data needs to be sent
    //check if it 's flushed or not?
    //check for first 5 bytes(min data frame length) if zero?>>so it's flusshed
    for(u8 i=0;i<min_data_len;i++)
    {
        if(RX_FRAME_buffer->frame[i]==0)
        {
          NO_of_ZEROS++;

        }
    }
    if(NO_of_ZEROS==5)
        {
            RX_FRAME_buffer->flushed_frame=1;   //frame is empty

        }
    //check if flushed
    if(RX_FRAME_buffer->flushed_frame ==0 )  //buffer has data>>not flushed
        {
            if(RX_FRAME_buffer->frame[0]==FEND)
             {
            printf("\n RX buffer has data !!");
            //fire datardy flag
            SM_EVENT.R_DATARDY=SM_SET;
            return;
             }
            else if(RX_FRAME_buffer->frame[0]!=FEND)
            {
            SM_EVENT.R_DATARDY=SM_CLEAR;
            SM_EVENT.R_EMSG=SM_SET;
            printf("\n invalid data received\n");
            return;
            }
        }
        else
        {  //if no data here
            SM_EVENT.R_DATARDY=SM_CLEAR;
            printf("\n no data received");
            return;
        }
}
void R_check_TX_BUFFER()
{
    //i do't need it anyway!!

}

void RE_START_TIMER(u8 SM_TIMER)
{

}
void STOP_TIMER(u8 SM_TIMER)
{

}
