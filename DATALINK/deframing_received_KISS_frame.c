/************************************************/
/*	Author		:	Ahmed Mohy			        */
/*	Date		:	18 DEC 2020					*/
/************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include"BIT_MATH.h"
#include "STD_TYPES.h"
#include "KISS_private.h"
#include "KISS_interface.h"
/*******************************************************************************/
/* Deframing the second part */
/* function that extract the command and the data from received frame */
u8* deframing_function (u8 *receivedframe,u32*data_len)
{
    u16 CRC_RECEIVED=0;
    u16 CRC_calc=0;
    Kiss_frame_t RXframe;
    RXframe.FrameLength=*data_len;
    RXframe.PtrtoFrame=(u8*)malloc(*data_len);
    memcpy(RXframe.PtrtoFrame,receivedframe,*data_len);
    //RXframe.PtrtoFrame=receivedframe;
    /* initialize the destuffing bytes to zero*/
    RXframe.destuffingbytes=0;
    /*a function to calculate the unknown frame length */
    //get_frame_length(&RXframe);

    /*checking for FEND flag (C0)*/
   if (RXframe.PtrtoFrame[0]==FEND && RXframe.PtrtoFrame[RXframe.FrameLength-1]==FEND)
    {
        /* it is a function to delete FEND flag (C0) from the frame */
        FEND_destuffing(&RXframe);
        /* put the first byte of the data to command type byte */
        RXframe.commandtypebyte=RXframe.PtrtoFrame[0];
        /* it is a function to extract FEND and FESC from the data */
        extract_FEND_FESC(&RXframe);
        /*extract CRC received*/
        CRC_RECEIVED |= (RXframe.PtrtoFrame[RXframe.FrameLength-2]) & 0xff;
        CRC_RECEIVED |= (RXframe.PtrtoFrame[RXframe.FrameLength-1] << 8) & 0xff00;
        RXframe.FrameLength-=2;
        /*check the CRC16 */
        CRC_calc=CRC16_u16CCITTFalse(RXframe.PtrtoFrame, RXframe.FrameLength);
        printf("calculated CRC =%X ",CRC_calc);
         printf("....RECEIVED CRC =%X ",CRC_RECEIVED);
        if(CRC_RECEIVED==CRC_calc)
        {
            printf("So CRC check is right \n");
            printf("***************************\n");
        }
        else
        {
            printf("So CRC check is wrong\n");
            printf("***************************\n");
        }

    }

   else
   {
        printf("NACK    NACK    NACK    NACK    NACK    NACK\n");
        printf("***************************\n");
   }

    *data_len=RXframe.FrameLength;
    receivedframe=(u8*)realloc(receivedframe,*data_len);
    memcpy(receivedframe,RXframe.PtrtoFrame,*data_len);
    free(RXframe.PtrtoFrame);
    //free(TXframe);
    //free(data);
   return receivedframe;
}

/* it is a function to delete FEND flag (C0) from the frame */
void FEND_destuffing(Kiss_frame_t *RXframe)
{
    /* shifting left the frame one byte to delete start C0*/
    memcpy(RXframe->PtrtoFrame,RXframe->PtrtoFrame+1,RXframe->FrameLength-1);
    /* using realloc to shrink the frame array with two bytes so that the C0 at the end will deleted*/
    RXframe->PtrtoFrame=(u8*)realloc((RXframe->PtrtoFrame),(RXframe->FrameLength-2)*sizeof(u8));
    /* decrement the frame length by 2 */
    RXframe->FrameLength-=2;
    /* increment destuffing bytes by 2 for destuffing two bytes of C0 at the end end start frame */
    RXframe->destuffingbytes+=2;
}
/* it is a function to extract FEND and FESC from the data  replace (FESC,TFEND)
 flags with FEND flag and (FESC,TFEND) flags with FESC flag */
void extract_FEND_FESC(Kiss_frame_t *RXframe)
{
     for(u32 i=0; i<(RXframe->FrameLength); i++)
    {
        /* searching for frame escaped flags (FESC=DB && TFESC=DD) to replace it with flag (FESC=DB)  */
        if(RXframe->PtrtoFrame[i]==FESC && RXframe->PtrtoFrame[i+1]==TFESC)
          {
               FESC_flag_recovery(RXframe,i);
          }
    }

    /* searching for frame escape flags (FESC=DB && TFEND=DC) to replace it with flag  (FEND=C0)  */
    for(u32 i=0; i<RXframe->FrameLength; i++)
    {

        if(RXframe->PtrtoFrame[i]==FESC && RXframe->PtrtoFrame[i+1]==TFEND)
               FEND_flag_recovery(RXframe,i);
    }
}
/* a function to recover FESC flag*/
void FESC_flag_recovery(Kiss_frame_t *RXframe,u32 i)
{
    /* shift right the frame array after destuffing one byte */
    memcpy(RXframe->PtrtoFrame+i+1,RXframe->PtrtoFrame+i+2,RXframe->FrameLength-i-2);
    /* recover FESC flag*/
    RXframe->PtrtoFrame[i]=FESC;
    /* using realloc function to shrink the array frame
     size because we will replace two bytes with one byte */
    RXframe->PtrtoFrame=(u8*)realloc((RXframe->PtrtoFrame),(RXframe->FrameLength-1)*sizeof(u8));
    /* decrement the frame length by one */
    RXframe->FrameLength--;
    /* increment destuffing bytes by one for destuffing one byte*/
    RXframe->destuffingbytes++;
}

/* a function to recover FEND flag*/
void FEND_flag_recovery(Kiss_frame_t *RXframe,u32 i)
{
    /* shift left the frame array after destuffing one byte */
    memcpy(RXframe->PtrtoFrame+i+1,RXframe->PtrtoFrame+i+2,RXframe->FrameLength-i-2);
    /* recover FEND flag */
    RXframe->PtrtoFrame[i]=FEND;
    /* using realloc function to shrink the array frame
     size because we will replace two bytes with one byte */
    RXframe->PtrtoFrame=(u8*)realloc((RXframe->PtrtoFrame),(RXframe->FrameLength-1)*sizeof(u8));
    /* decrement the frame length by one */
    RXframe->FrameLength--;
    /* increment destuffing bytes by one for destuffing one byte*/
    RXframe->destuffingbytes++;
}
/*a function to calculate the unknown frame length */
void get_frame_length(Kiss_frame_t *RXframe)
{

    RXframe->FrameLength=1;
    while (RXframe->PtrtoFrame[RXframe->FrameLength] !=FEND)
    {
        RXframe->FrameLength++;
    }
    RXframe->FrameLength++;
}
