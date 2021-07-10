/*
recieving process>>>>
to recieve any data you need to plug it out of the packet or frame (packet per frame)
constructing a recieved structure to make it easy to treat with the frame packet
each structure contain that:
1-frame of data which is already recieved
2-crc0,crc1 values for each frame which already includede into the frame
3-length of the frame that recieved>>you can detect it before destuffing it
4-type of the frame which already included into it also
>>>that only to simplify processing on the frame but only the frame (array of data)
is recieved

*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "STD.h"
#include "ssp_interface.h"
#include "ssp_private.h"
#include"ssp_DL_SM.h"



//static u8 RX_frame_length=0;          ///to include recieve frame length
/*
function to recieve a frame andmake an action depend on these data
1-detect length of frame that you recieved but befor destuufing it
>>it's easy to get it's length now as it's already stuufed before!!
2-construct recieved structure to make it easy to treat with that frame
>>store it;s frame before destuffing
>>store it's length before destuffing
3-destuff the frame without two flags bit
>>it generate new length and update the frame length in it's structure
4-generate crc0,crc1 again
5-check that correct crc >>if correct >>take an action>>if not>>transmit NAK frame
*/
 u8 recieve_frame(u8 *RX_frame )
 {
    /*create structure for ssp recieved frame */
    frame_struct test_frame_2;
    frame_struct *RX_FRAME =&test_frame_2;
    RX_FRAME->frame=RX_frame;
    RX_FRAME->frame_length=0;
    /* check for valid or not recieved frame */
      u8 valid = check_valid(RX_FRAME);
      if(valid==0)
        {
        free(RX_FRAME->frame);
        return 0 ;
        }
      /* get actual frame length */
      RX_FRAME->frame_length+= 2;
      /*save rx_frame in recieved structure*/
      RX_FRAME ->frame =(u8*) malloc(RX_FRAME->frame_length);
      for(u8 i=0;i<RX_FRAME->frame_length;i++)
       {
        RX_FRAME->frame[i]=RX_frame[i];
       }
    // destuffing the recieved frame
    destuffing_frame(RX_FRAME);

    /// generate crc of the recieved frame
    u16 crc_new = crc_generate(RX_FRAME->frame+1,RX_FRAME->frame_length-2);
    // check that 's correct crc >>frame is valid
     u8 crc_valid = crc_check(RX_FRAME,crc_new);
    // determine if i will use that frame to take an action or not?(valid or not)
    if(crc_valid)
    {
      //print destuffed recieved frame
      printf("destuffed recieved data\n");
      print_frame(RX_FRAME->frame,RX_FRAME->frame_length);
      printf("\n");
      // now take an action according to the recieved frame>>(type and data)
      get_response(RX_FRAME,2);

    }  //end if
    else
      printf("invalid frame packet\n");
   // free the allocated array
  free(RX_FRAME->frame);
  return 1;

} ///end function
/*
destuffing function to get the original frame
1-check for FESC...if there?
>>check if followed by TFEND ...if there?
 >>>>>replace FESC TFEND by FEND ,,shrink the array,,,update frame length
>>check if followed by TFESC ...if there?
>>>>>>replace FESC TFESC by FESC ,,,shrink the array,,update the frame length
if not followed by one of them>>>no destuffing is required:)

*/

void destuffing_frame(frame_struct * RX_FRAME)
{
    /*destuffing all bytes except two flags*/
   for(u8 i=1;i<RX_FRAME->frame_length-1;i++)
   {
       if(RX_FRAME->frame[i]==FESC)
       {
         if(RX_FRAME->frame[i+1]==TFEND)
         {
             RX_FRAME->frame[i]=FEND;
             shift_left(RX_FRAME,i+1,1);
         }
         else if(RX_FRAME->frame[i+1]==TFESC)
         {
             RX_FRAME->frame[i]=FESC;
             shift_left(RX_FRAME,i+1,1);
         }

       } ///if end
   } ///for end
   RX_FRAME->frame_type=RX_FRAME->frame[3];
} ///function end

/*
function to shift left array from location pos by no of steps = moves
*/
void shift_left(frame_struct *RX_FRAME, u8 pos,u8 moves)
{
     u8 len= RX_FRAME->frame_length;
  for(u8 i=pos;i<=len;i++)
  {
   RX_FRAME->frame[i]=RX_FRAME->frame[i+1];

  }
  ///shrink the frame after shifting it left after destuffing

  RX_FRAME->frame = (u8*) realloc(RX_FRAME->frame, len-moves);
  RX_FRAME->frame_length--;

}
/*
function to compare crc of incomming frame and new crc generated
if it's the same >>it's valid frame
if not>>it's invalid frame
*/
u8 crc_check(frame_struct *RX_FRAME,u16 crc_new)
{
    /*u8 len =RX_FRAME->frame_length;
    u16 crc0 =(u16)RX_FRAME->frame[len-3];
    u16 crc1 =(u16)RX_FRAME->frame[len-2];
    //if((crc_new & 0x00FF) == crc0  && (crc_new >>8) == crc1)*/
    if (crc_new==0)
       return 1;
    else
      return 0;

}
/*
function to response to the recieved frame and take an action depending on it
if you need any data you will only need to realloc the array and sent it
 each response depend on type of recieved request due to ssp version or
 protocol that will be met:)
*/
void get_response(frame_struct *FRAME,u8 valid)
{
    u8 data_length=0;
    u8*response_data =(u8*) malloc(4);
    for(u8 i=0;i<5;i++)
    {
        response_data[i]=5;
    }
   //response for invalid data
    if(valid==0)
        FRAME->frame_type= NAK_FAILED;
    //response for wrong direction
    else if(valid==1)
        FRAME->frame_type= NAK_INCORRECT;

    //case for every request to decied which response to send?
  switch(FRAME->frame_type)
  {
   //if bing recieved>>>send an ack answer

  case ACK_DATA:
    {

     printf(" response to ack_data is ACK/response for not busy>>>response data pushed if TX buffer \n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
    //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_RESPONSE;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame = response_data;
    /*
    SM_data_type=ACK_RESPONSE;
    SM_dataless=1;
    SM_datalength=data_length;
    SM_response_data=response_data;*/

    }break;
    case PING:
    {

     printf(" response to ping is ACK/response for not busy>>>response data pushed if TX buffer \n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
    //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_RESPONSE;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame = response_data;
    /*
    SM_data_type=ACK_RESPONSE;
    SM_dataless=1;
    SM_datalength=data_length;
    SM_response_data=response_data;*/

    }break;
    case INIT:
    {
    /* or response can include two butes of data include time will it take to initalize!! */
    printf(" response to INIT is ACK/DATA simply with time wil take to initialize>>>response data pushed if TX buffer \n");
    printf("\n fire M flag to get an action to sync ");
    SM_EVENT.S_M_Rcvd=SM_SET;
    data_length=2;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;
    }break;

  case GET_0:
    {
    /*  response can include 4 bytes of data for variables value needed!! */
    printf("\n response to GET is ACK/YES simply with values needed>>>response data pushed if TX buffer \n");
    data_length=4;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame = data_to_be_sent;

    }break;
    case GET_1:
    {
    /*  response can include 4 bytes of data for variables value needed!! */
    printf("\n response to GET is ACK/DATA simply with values needed>>>response data pushed if TX buffer \n");
    data_length=4;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=data_to_be_sent;

    }break;
 case PUT_0:
    {
        /*  response can include no data for variables value needed!! */
    printf("\n response to PUT is ACK/0 simply with no values needed>>>response sent frame is\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
    case PUT_1:
    {
        /*  response can include no data for variables value needed!! */
    printf("\n response to PUT is ACK/DATA simply with no values needed>>>response sent frame is\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
 case READ_0:
    {
/*A successful response to a READ is an ACK/0, with exactly the requested number of memory bytes as
data. Devices with a well-defined byte order in memory send bytes in that order. On word-addressed
machines with no official byte order, byte order and padding conventions are implementation-defined (for
example, 24-bit words might be sent as 3 bytes each, or might be sent as 32-bit values with the top byte
zero). The byte count is how many bytes should actually be sent, including all padding.
        */
    printf("\n response to READ_0 is ACK/DATA simply with  values needed>>>response sent frame is\n");
     data_length=4; //to get same length of data received
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=data_to_be_sent;


    }break;
    case READ_1:
    {
/*A successful response to a READ is an ACK/0, with exactly the requested number of memory bytes as
data. Devices with a well-defined byte order in memory send bytes in that order. On word-addressed
machines with no official byte order, byte order and padding conventions are implementation-defined (for
example, 24-bit words might be sent as 3 bytes each, or might be sent as 32-bit values with the top byte
zero). The byte count is how many bytes should actually be sent, including all padding.
        */
         printf("\n response to READ_1 is ACK/DATA simply with  values needed>>>response sent frame is\n");
     data_length=4;  //to get same length of data received
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=data_to_be_sent;


    }break;
    case READ_2:
    {
/*A successful response to a READ is an ACK/0, with exactly the requested number of memory bytes as
data. Devices with a well-defined byte order in memory send bytes in that order. On word-addressed
machines with no official byte order, byte order and padding conventions are implementation-defined (for
example, 24-bit words might be sent as 3 bytes each, or might be sent as 32-bit values with the top byte
zero). The byte count is how many bytes should actually be sent, including all padding.
        */
        printf("\n response to READ_2 is ACK/DATA simply with  values needed>>>response sent frame is\n");
     data_length = 4;  //to get same length of data received
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=data_to_be_sent;


    }break;
    case READ_3:
    {
/*A successful response to a READ is an ACK/0, with exactly the requested number of memory bytes as
data. Devices with a well-defined byte order in memory send bytes in that order. On word-addressed
machines with no official byte order, byte order and padding conventions are implementation-defined (for
example, 24-bit words might be sent as 3 bytes each, or might be sent as 32-bit values with the top byte
zero). The byte count is how many bytes should actually be sent, including all padding.
        */
         printf("\n response to READ_3 is ACK/DATA simply with  values needed>>>response sent frame is\n");
     data_length=FRAME->frame_length-min_data_len;  //to get same length of data received
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=0;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=data_to_be_sent;


    }break;
  case WRITE_0:
    {

         /*  response can include no data for variables value needed!! */
    printf("\n response to write_0 is ACK/0 simply with no values needed>>>response sent frame is\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
    case WRITE_1:
    {

         /*  response can include no data for variables value needed!! */
    printf("\n response to write_1 is ACK/0 simply with no values needed>>>response sent frame is\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
    case WRITE_2:
    {

         /*  response can include no data for variables value needed!! */
    printf("\n response to write_2 is ACK/0 simply with no values needed>>>response sent frame is\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
    case WRITE_3:
    {

         /*  response can include no data for variables value needed!! */
    printf("\n response to write_3 is ACK/0 simply with no values needed>>>response sent frame is\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->data_length=data_length;
    TX_FRAME_buffer->frame_length=data_length+min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
  case ID_0:
    {
    /*  response can include 4 bytes of data for variables value needed!!
    A successful response is an ACK/0 with exactly four data bytes:
    flgs bsiz ssiz impl */
    ///>>>implemented later
    printf("\n response to ID is ACK/DATA simply with 4 values needed>>>response sent frame is\n");
    data_length=4;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->frame_length=min_data_len+data_length;
    TX_FRAME_buffer->frame=response_data;

    }break;
    case ID_1:
    {
    /*  response can include 4 bytes of data for variables value needed!!
    A successful response is an ACK/0 with exactly four data bytes:
    flgs bsiz ssiz impl */
    ///>>>implemented later
    printf("\n response to ID_1 is ACK/DATA simply with 4 values needed>>>response sent frame is\n");
    data_length=4;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=ACK_DATA;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->frame_length=min_data_len+data_length;
    TX_FRAME_buffer->frame=response_data;

    }break;

    case ADDRT:
    {
        FRAME ->frame_length =min_data_len;

    }break;
    case DELRT :
    {
        FRAME ->frame_length =min_data_len;

    }break;
    case GETRT :
    {
        FRAME ->frame_length =min_data_len;

    }break;

  case NAK_UNKOWN:
    {
     printf("\n  NACK/unkown response \n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=NAK_UNKOWN;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->frame_length=min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;
  case NAK_INCORRECT:
    {
        printf("\n  NACK/incorrect\n");
     data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=NAK_INCORRECT;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->frame_length=min_data_len;
    TX_FRAME_buffer->frame=response_data;


    }break;
  case NAK_FAILED:
    {
        printf("\n  NACK/failled\n");
    data_length=0;
    response_data= (u8*) realloc(response_data, data_length);
     //filling TX_BUFFER
    TX_FRAME_buffer->frame_type=NAK_FAILED;
    TX_FRAME_buffer->dataless=1;
    TX_FRAME_buffer->frame_length=min_data_len;
    TX_FRAME_buffer->frame=response_data;

    }break;

   }
}

/*  */

 u8 check_valid(frame_struct*RX_frame)
 {
     /*
  check if it start with fend only and getting it's length>>start of the frame
  */
    if(RX_frame->frame[0]==FEND)
    {
        /*detect it's length before de_stufffing without two flags*/
    for(u8 i=1;RX_frame->frame[i]!=FEND;i++)
      {
        RX_frame->frame_length ++;
      }
        /*detect it's length before de_stufffing without two flags*/
      if(RX_frame->frame_length<5)
         {
          printf("invalid recieved frame, too short frame!!\n");
          return 0;
         }
    }
    else
    {

    return 1;
    }
    return 0;

}

