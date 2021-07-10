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
/* Framing the first part */
/*it is a function to build and construct all frame*/
u8* construct_frame (u8* data,u32 *data_len/*frame_t *TXframe*/)
{
    /*initialize stuffing bytes to zero*/
    u8*frame;
    Kiss_frame_t* TXframe=(Kiss_frame_t*)malloc(sizeof(Kiss_frame_t));
    TXframe->stuffingbytes=0;
    TXframe->FrameLength=*data_len;
    TXframe->PtrtoFrame=(u8*)malloc(*data_len);
    memcpy(TXframe->PtrtoFrame,data,*data_len);
    TXframe->destuffingbytes=0;
    TXframe->commandtypebyte=0;

    u16 CRC=CRC16_u16CCITTFalse(TXframe->PtrtoFrame, TXframe->FrameLength);
    TXframe->PtrtoFrame=(u8*)realloc((TXframe->PtrtoFrame),(TXframe->FrameLength+2)*sizeof(u8));
    TXframe->PtrtoFrame[TXframe->FrameLength]=(CRC & 0xff);
    TXframe->PtrtoFrame[TXframe->FrameLength+1]=(CRC>>8 & 0xff);
    TXframe->FrameLength+=2;


    /*search for FEND or FESC in the data to replace it respectivily with FESC, TFEND and FESC, TFESC. */
    for(u32 i=0;i<TXframe->FrameLength;i++)
    {
        /* searching for frame escaped flag (FESC=DB) to replace it with two flags (FESC=DB && TFESC=DD)  */
        if(TXframe->PtrtoFrame[i]==FESC)
          {
               replace_FESC_flag(TXframe,i);
               /*we will replace one byte with two bytes so we increment i by one to skip the two bytes*/
               i++;
          }
    }
    for(u32 i=0;i<TXframe->FrameLength;i++)
    {
         /* searching for frame end flag (FEND=C0) to replace it with two flags (FESC=DB && TFEND=DC)  */
        if(TXframe->PtrtoFrame[i]==FEND)
            replace_FEND_flag(TXframe,i);
    }

    add_FEND_flag(TXframe);

    *data_len=TXframe->FrameLength;
    frame=(u8*)malloc(*data_len);
    memcpy(frame,TXframe->PtrtoFrame,*data_len);
    free(TXframe->PtrtoFrame);
    free(TXframe);
    free(data);
    return frame;
}

/*a function to add C0 flag at the last start byte of the frame */
void add_FEND_flag(Kiss_frame_t *TXframe)
{
    /* add two bytes for C0 at the begining and at the end of the frame*/
    TXframe->PtrtoFrame=(u8*)realloc((TXframe->PtrtoFrame),(TXframe->FrameLength+2)*sizeof(u8));
    /*increment frame length by two for adding two bytes*/
    TXframe->FrameLength+=2;
    /*increment stuffing bytes for adding two bytes*/
    TXframe->stuffingbytes+=2;
    /* shift right the array one byte*/
    //shift_frame(TXframe,1,'R');
    memcpy(TXframe->PtrtoFrame+1,TXframe->PtrtoFrame,TXframe->FrameLength-2);
    /* put C0 at the frame end*/
    TXframe->PtrtoFrame[(TXframe->FrameLength)-1]=FEND;
   /* put C0 at the frame start*/
    TXframe->PtrtoFrame[0]=FEND;
}
/*a function replace frame end flag (FEND=C0)
 with two flags (FESC=DB && TFEND=DC) */
void replace_FEND_flag(Kiss_frame_t *TXframe,u32 i)
{
    /* using realloc function to increase the array frame
     size because we will replace one byte with two bytes*/
    TXframe->PtrtoFrame=(u8*)realloc((TXframe->PtrtoFrame),(TXframe->FrameLength+1)*sizeof(u8));
    /*increment frame length by one for adding one bytes*/
    TXframe->FrameLength++;
    /*increment stuffing bytes for adding one bytes*/
    TXframe->stuffingbytes++;
    /*a function to shift right the array by one byte
     for a position we want to insert an element */
    memcpy(TXframe->PtrtoFrame+i+1,TXframe->PtrtoFrame+i,TXframe->FrameLength-i-1);
    /*replace frame end flag (FEND=C0) with two flags (FESC=DB && TFEND=DC) */
    TXframe->PtrtoFrame[i]=FESC;
    TXframe->PtrtoFrame[i+1]=TFEND;

}
/*a function replace frame escaped flag (FESC=C0)
 with two flags (FESC=DB && TFESC=DD)  */
void replace_FESC_flag(Kiss_frame_t *TXframe,u32 i)
{
    /* using realloc function to increase the array frame
     size because we will replace one byte with two bytes*/
    TXframe->PtrtoFrame=(u8*)realloc((TXframe->PtrtoFrame),(TXframe->FrameLength+1)*sizeof(u8));
    /*increment frame length by one for adding one bytes*/
    TXframe->FrameLength++;
    /*increment stuffing bytes for adding two bytes*/
    TXframe->stuffingbytes++;
   /*a function to shift right the array by one byte
    for a position we want to insert an element */
    memcpy(TXframe->PtrtoFrame+i+1,TXframe->PtrtoFrame+i,TXframe->FrameLength-i-1);
    /*replace frame escaped flag (FESC=C0) with two flags (FESC=DB && TFESC=DD)  */
    TXframe->PtrtoFrame[i]=FESC;
    TXframe->PtrtoFrame[i+1]=TFESC;
}
