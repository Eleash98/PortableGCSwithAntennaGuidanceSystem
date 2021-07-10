#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "STD_TYPES.h"
#include "KISS_interface.h"
#include "KISS_private.h"
int main()
{
   u8 *x=(u8*)malloc(16);
    x[0]=FullDuplex;    //first byte is used for the command
    x[1]=0xc0;
    x[2]=0xdb;
    x[3]=0xdb;
    x[4]=0xc0;
    x[5]=0xCD;
    x[6]=0xc0;
    x[7]=0xdb;
    x[8]=0xAB;
    x[9]=0xc0;
    x[10]=0xAB;
    x[11]=0xdb;
    x[12]=0xdb;
    x[13]=0xfa;
    x[14]=0xc0;
    x[15]=0xc0;
    Kiss_frame_t f;
    f.PtrtoFrame=(u8*)malloc(16);
    memcpy(f.PtrtoFrame,x,16);
    //f.PtrtoFrame=x;
    f.FrameLength=16;
    u32 i=0;
    /* print the input data*/
    printf("the main input data is : \n");
     for (i=0; i<f.FrameLength; i++)
    {
        printf("%X",(x[i]>>4)&0xf);
        printf("%X",(x[i])&0xf);
        printf(" ");
    }
    printf("\n");
    printf("***************************\n");
    /*************************/
    /* construct the frame to send it */
    f.PtrtoFrame=construct_frame(f.PtrtoFrame,&f.FrameLength);
    printf("the construct frame is  :  \n");
    /*print the constructed frame*/
    for (i=0; i<f.FrameLength; i++)
    {
        printf("%X",(f.PtrtoFrame[i]>>4)&0xf);
        printf("%X",(f.PtrtoFrame[i])&0xf);
        printf(" ");
    }
    printf("\n");
    printf("***************************\n");
    /********************************************/
    /* deframing the received frame that has FEND flag*/
    Kiss_frame_t g;
    g.PtrtoFrame=deframing_function (f.PtrtoFrame,&f.FrameLength);
    g.FrameLength=f.FrameLength;
    printf("deframing the frame is : \n");
    for (i=0; i<g.FrameLength; i++)
    {
        printf("%X",(g.PtrtoFrame[i]>>4)&0xf);
        printf("%X",(g.PtrtoFrame[i])&0xf);
        printf(" ");
    }
    printf("\n");
    printf("***************************\n");
     /* test if the received frame doesn't have FEND flag before construct it*/
    //deframing_function(x);
    //printf("stuffingbytes number is :  %d\n",f.stuffingbytes);
    //printf("***************************\n");
    //printf("destuffingbytes number is :  %d\n",g.destuffingbytes);
    //printf("***************************\n");
    //printf("command type byte is :  %d\n",g.commandtypebyte);
    //printf("***************************\n");
    /*********************************************/
    /*When the program no longer needs the dynamic array,
     it must eventually call free to return the memory it
     occupies to the free store:*/
    free(x);
    free(g.PtrtoFrame);
    return 0;
}
