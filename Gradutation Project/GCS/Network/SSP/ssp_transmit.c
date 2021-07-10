/*
transmitting process>>>>
to transmit any data you need to plug it into packet or frame (packet per frame)
constructing a transmitted structure to make it easy to treat with the frame packet
each structure contain that:
1-frame of data to be ready to be transmitted
2-crc0,crc1 values for each frame which already includede into the frame
3-length of the frame that will be transmitted
4-type of the frame which already included into it also
>>>that only to simplify processing on the frame but only the frame (array of data)
is tramnsmitted

*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#include "STD.h"
#include "ssp_interface.h"
#include "ssp_private.h"
#include"ssp_DL_SM.h"

/*
BUILD FRAME FUNCTION>>>
1-check data type
2-generate crc for each frame
3-construct the frame depend on it's type
4-stuffing it for more effecient framing !!>>due to some types
note that:>>>>>>>>>>>>
***if dataless ==1>>so there is no data nedded in the frame
***READ and WRITE are provided for this purpose. Small SSP implementations,
 e.g. on microcontrollers,might not support these requests. !!:)
***ID is not needed as it is only one slave and by default master know it's specifications and the capabilties!!
*** simple implementations might support only phase 0. for ID request !!
***Many processes will have only one port, e.g. because they reside on a device which
only has one SSP-capable interface,so routing requests as ADDRT,DELRT,GETRT are not needed !!

*/
void build_frame( frame_struct * FRAME,u8 *sent_data,u8 dataless,u8 datalength)
   {

switch (FRAME ->frame_type) //check first byte of data
 {
 //requestes
  case PING:
    {
        printf("\n ping transmitted");

        if(dataless==1) //no data
          {
              FRAME ->frame_length = min_data_len;
              build_std_frame(FRAME);
          }
        else
        {
          FRAME ->frame_length = min_data_len;
          build_other_frame(FRAME , sent_data  , FRAME->data_length);
        }
    }break;
  case INIT:
    {
        printf("\n init transmitted");
        //simple implementations might only support dataless INIT.
        if(dataless==1) //no data
          {
              FRAME ->frame_length = min_data_len;
              build_std_frame(FRAME);
          }
        else
        {
          FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
          FRAME ->data_length =datalength;
          build_other_frame(FRAME , sent_data  , FRAME->data_length);
        }
    }break;

  case GET_0:
    {
        /*data in GET are 2_bytes variables addresses
        ,addresses are 16 bit>>2 byte>>
        so 4 bytes data here,for only entierly variables
        */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
        FRAME ->data_length =datalength;
        //sending frame structure,data to be attached,data length=frame len-std len
        build_other_frame(FRAME , sent_data  , FRAME->data_length);

    }break;
    case GET_1:
    {
        /*data in GET are 2_bytes variables addresses
        ,addresses are 16 bit>>2 byte>>
        so 4 bytes data here or more,for certain monitoring varaibles
        */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
        FRAME ->data_length =datalength;
        //sending frame structure,data to be attached,data length=frame len-std len
        build_other_frame(FRAME , sent_data  , FRAME->data_length);

    }break;
 case PUT_0:
    {
        /*
        6 byte of data included,
        4 byte for a variable(4x8bit values,4 values >>2 values for one variable)
         and 2 byte for an address( 2x16bit address)
         totally  6 bytes(7+6)
         */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
        FRAME ->data_length =datalength;
        build_other_frame(FRAME ,sent_data ,FRAME->data_length);

    }break;
    case PUT_1:
    {
        /*
        6 byte of data included,
        4 byte for a variable(4x8bit values,4 values >>2 values for one variable)
         and 2 byte for an address( 2x16bit address)
         totally  6 bytes,or more than one sequence for space_1(7+6 or more)
         */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
          FRAME ->data_length =datalength;
        build_other_frame(FRAME ,sent_data ,FRAME->data_length);

    }break;
 case READ_0:
    {   /*4 bytes for address and 2 bytes for unsigned
         16-bit count of the number of bytes to be read from the memory area */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
          FRAME ->data_length =datalength;
        build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);

    }break;
    case READ_1:
    {
        /*4 bytes for address and 2 bytes for unsigned
         16-bit count of the number of bytes to be read from the memory area */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
          FRAME ->data_length =datalength;
        build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);

    }break;
    case READ_2:
    {
       /*4 bytes for address and 2 bytes for unsigned
         16-bit count of the number of bytes to be read from the memory area */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
          FRAME ->data_length =datalength;
        build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);

    }break;
    case READ_3:
    {
        /*4 bytes for address and 2 bytes for unsigned
         16-bit count of the number of bytes to be read from the memory area */
        FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
          FRAME ->data_length =datalength;
        build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);

    }break;
  case WRITE_0:
    {
       /*4 bytes for address and x bytes for
         data to be written */
    FRAME ->frame_length = min_data_len+datalength;    //7+4 of 4 byte data length for init with data to make re_start
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);

    }break;
    case WRITE_1:
    {
        /*4 bytes for address and x bytes for
         data to be written */
        FRAME ->frame_length = min_data_len+datalength;    //7+6 of 4 byte data length for init with data to make re_start
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
    }break;
    case WRITE_2:
    {
       /*4 bytes for address and x bytes for
        data to be written*/
         FRAME ->frame_length = min_data_len+datalength;    //7+6 of 6 byte data length for init with data to make re_start
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);

    }break;
    case WRITE_3:
    {
        /*4 bytes for address and x bytes for
         data to be written*/
    FRAME ->frame_length = min_data_len+datalength;    //7+6 of 6 byte data length for init with data to make re_start
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);


    }break;
  case ID_0:
    {
        /* an ID/0 request is dataless */
        FRAME ->frame_length =min_data_len;
        FRAME->data_length=0;
        build_std_frame(FRAME );

    }break;
     case ID_1:
    {
        /* an ID/1 request has one data byte */
        FRAME ->frame_length =min_data_len+1;
        FRAME->data_length=1;
        build_other_frame(FRAME ,sent_data  , FRAME->data_length);

    }break;
    case ADDRT:
    {
        /* an ADDRT request has 8 data byte(netw mask flgs port lif0 lif1 lif2 lif3) */
    FRAME ->frame_length = min_data_len+datalength;
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
    }break;
    case DELRT :
    {
       /* an DELRT request has 8 data byte(netw mask flgs port lif0 lif1 lif2 lif3) */
        FRAME ->frame_length = min_data_len+datalength;
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
    }break;
    case GETRT :
    {   /*dataless request*/
        FRAME ->frame_length =min_data_len;
        FRAME->data_length=0;
         build_std_frame(FRAME);

    }break;
     ///response !!
  case ACK_RESPONSE:
    {
        FRAME ->frame_length =min_data_len;
        build_std_frame(FRAME);
    }break;
  case ACK_DATA:
    {
        if(dataless==1)
        {
        printf("\n ACK/data \n");
        FRAME ->frame_length =min_data_len;
         build_std_frame(FRAME);
        }
         else
            {
        //n byte of data for ACK_YES type **for example:for indicate how long time will take to init
       FRAME ->frame_length = min_data_len+datalength;
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
            }
    }break;
  case NAK_UNKOWN:
    {    if(dataless==1)
        {
        printf("\n NACK/unknown \n");
        FRAME ->frame_length =min_data_len;
         build_std_frame(FRAME);
        }
         else
            {
    FRAME ->frame_length = min_data_len+datalength;
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
            }
    }break;
  case NAK_INCORRECT:
    {    if(dataless==1)
        {
        printf("\n NACK/incorrect \n");
        FRAME ->frame_length =min_data_len;
         build_std_frame(FRAME);
        }
         else
            {
    FRAME ->frame_length = min_data_len+datalength;
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
            }
    }break;
  case NAK_FAILED:
    {    if(dataless==1)
        {
        printf("\n NACK/failled \n");
        FRAME ->frame_length =min_data_len;
         build_std_frame(FRAME);
        }
         else
            {
    FRAME ->frame_length = min_data_len+datalength;
    FRAME ->data_length =datalength;
    build_other_frame(FRAME ,data_to_be_sent  , FRAME->data_length);
            }
    }break;

 } ///end of switch

 frame_stuffing(FRAME);

} ///end of the function


/*
function to built or construct the frame
as it a standard frame type like:
PING,INIT,ACK,NAK >>frames without data
*/
void build_std_frame(frame_struct *FRAME)
{
    //construct the array for the packet
    FRAME ->frame =(u8*) malloc(FRAME->frame_length);
    FRAME->frame[0]=FEND;
    FRAME->frame[1]=ADD1;      //but here destination  address which want to send to
    FRAME->frame[2]=MY_ADD;    //but here your address as a
    FRAME->frame[3]=FRAME->frame_type;
    //generate crc for frame without fends
    u16 crc_res = crc_generate(FRAME->frame+1,FRAME->frame_length-4);
    FRAME->frame[4]=crc_res;                //CRC low
    FRAME->frame[5]=crc_res>>8;             //CRC high
    FRAME->frame[6]=FEND;

}

/*
function to stuffing all the constructed frame to be sure
 that all the frame is correct
*/
void frame_stuffing(frame_struct *FRAME)
{
 for(u8 i=1;i<FRAME->frame_length-1;i++)
 {
     if(FRAME->frame[i]==FEND)
     {
         shift_right(FRAME,i+1,1);
         FRAME->frame[i]=FESC;
         FRAME->frame[i+1]=TFEND;
         i=i+1;
         FRAME->frame_length++;
     }
     else if(FRAME->frame[i]==FESC)
     {
         shift_right(FRAME,i+1,1);
         FRAME->frame[i]=FESC;
         FRAME->frame[i+1]=TFESC;
         i=i+1;
         FRAME->frame_length++;
     }
 }
}
/*
function to shift all the frame to right by no of moves from the location of pos

*/
void shift_right(frame_struct *FRAME, u8 pos,u8 moves)
{
  u8 new_len= FRAME->frame_length+moves;
  FRAME->frame = (u8*) realloc(FRAME->frame, new_len);
  for(u8 i=new_len-1;i>=pos;i--)
  {
  FRAME->frame[i]=FRAME->frame[i-1];
  }
}
/*
function to construct a frame which isn't a standarad one>>
there is a data with unlimited number of bytes
*/

void build_other_frame(frame_struct*FRAME ,u8 *data  , u8 data_len)
{
    FRAME ->frame =(u8*) malloc(FRAME->frame_length);
    u8 len=FRAME->frame_length;

    FRAME->frame[0]=FEND;
    FRAME->frame[1]=ADD1;         //put here the destination address which you want to send to
    FRAME->frame[2]=MY_ADD;        //put here your address
    FRAME->frame[3]=FRAME->frame_type;
    for(u8 i=4;i< data_len+4;i++)
    {
        FRAME->frame[i]=*(data);
        data++;
    }
    u16 crc_res=crc_generate(FRAME->frame+1,FRAME->frame_length-4);
    FRAME->frame[len-3]=crc_res;
    FRAME->frame[len-2]=crc_res>>8;
    FRAME->frame[len-1]=FEND;
}




