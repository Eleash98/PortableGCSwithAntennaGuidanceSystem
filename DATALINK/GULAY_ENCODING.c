#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "GOLAY_ENCODING.h"
#define GET_NIBBLE_FROM_ARR(ARR,NIBBLENUMBER)       (((ARR[NIBBLENUMBER/2])>>((!(NIBBLENUMBER%2))*4))&0xF)
u16 GOLAY_POLY;
u16* Golay_voidDivide(u8* arr, u32 len, u32* GolayArrayLen )
{


/**len dividable by 1.5**/

    *GolayArrayLen = len/1.5;

    unsigned char padding_bits = 0;
    float rem = remainder(len,1.5);
    if (rem !=0)
    {
        /**padding needed**/
        padding_bits = rem > 0? 12-(int)(rem*8):4 ;
        /*pad with zeros*/
        *GolayArrayLen += 1;
    }

    u16 *GolayArray = (u16*)malloc(sizeof(u16)*(*GolayArrayLen));
    for (u32 i =0;i<*GolayArrayLen;i++)
        GolayArray[i] = 0;

    int Ni=0;
    for (Ni = 0;Ni<len*2;Ni++)
    {


        GolayArray[Ni/3] = GolayArray[Ni/3] <<4 |GET_NIBBLE_FROM_ARR(arr,Ni);
    }
    GolayArray[*GolayArrayLen-1] <<= padding_bits;


    return GolayArray;
}


u8* Golay_u8pointerEncode(u8* data,u32*dataLength)
{
    u16* golaydataarray;
    u32 golaydataarraylength;
    golaydataarray=Golay_voidDivide(data,*(dataLength),&golaydataarraylength);
    u32* encodecodewordarray=(u32*)malloc(sizeof(u32)*golaydataarraylength);
    //u32* decodecodewordarray=(u32*)malloc(sizeof(u32)*golaydataarraylength);
    //u32* syndromearray=(u32*)malloc(1);

    /*printf(" golay input data is \n");
    for (u32 i=0;i< golaydataarraylength;i++)
        printf(" %X",golaydataarray[i]);

    printf("\n");
    printf("*************************************************************\n");*/

    for (u32 i=0;i<golaydataarraylength;i++)
    {
        encodecodewordarray[i]=calc_golay_codeword(golaydataarray[i]);

        if (parity_bit(encodecodewordarray[i]))
            /*so it is 1 odd parity*/
            SET_BIT(encodecodewordarray[i],23);
        else
            /* it is 0 even parity*/
            CLR_BIT(encodecodewordarray[i],23);
    }
   /* printf(" encoded codeword array is \n");
    for (u32 i=0;i<(golaydataarraylength);i++)
        printf(" %X",encodecodewordarray[i]);
    printf("\n");
    printf("*************************************************************\n");*/
    /* divide the code word to the array of char to be transmitted*/

    u8 *encoded_data=(u8*)malloc(1);
    u32 w=0;
    u32 shift=2;
    for (u32 i=0;i< golaydataarraylength*3;i++)
    {
        if (i%3==0)
            shift=2;
        if (i%3==0 && i!=0)
            w++;

        encoded_data[i]=(encodecodewordarray[w]>>shift*8) & (0xFF);
        encoded_data=(u8*)realloc((encoded_data),(i+2)*sizeof(u8));
        shift--;
    }
    /*printf(" data after encoded is \n");

    for (u32 i=0;i< golaydataarraylength*3;i++)
        printf(" %X",encoded_data[i]);

    printf("\n");
    printf("*************************************************************\n");*/

    return(encoded_data);
}

u32 calc_golay_codeword(u16 data)
{
   u16 i=0;         u16 data2;   u16 c;//save original codeword
   u32 cw=0;
   c=data;
   data2=reverseBits(data,12);

   cw =data2;
   cw=cw<<11;

   for(i=0; i<12; i++)
   {
       if(cw&0x400000)
           cw ^=(GOLAY_POLY<<11);
      cw<<=1;
   }

   data2=cw>>12 & 0xfff;
   data2=reverseBits(data2,11);
   cw=0;
   cw|=(data2<<12)|c;

   return (cw);
}

u32 reverseBits(u32 num,u8 NO_OF_BITS)
{
   // unsigned int  NO_OF_BITS = sizeof(num) * 8;
    u32 reverse_num = 0;
    u32 i;
    for (i = 0; i < NO_OF_BITS; i++)
    {
        if((num & (1 << i)))
           reverse_num |= 1 << ((NO_OF_BITS - 1) - i);
   }
    return reverse_num;
}

void print_number_bits(u32 num, u16 bitsnum)
{
    printf(" ");
    for(u16 i=0; i<bitsnum+1; i++)
        printf("%lu",GET_BIT(num,(bitsnum-i)));
    printf("\n");
}

u8 codeword_weight(u32 cw)
{
    u8 weight=0;
    for(u8 i=0; i<24; i++)
    {
        if (GET_BIT(cw,i))
            weight++;
    }
    return (weight);
}

u8 parity_bit(u32 cw)
{
    /* This function checks the overall parity of codeword cw.
    If parity is even, 0 is returned, else 1. */
    u8 p;
    /* XOR the bytes of the codeword */
    p=cw & 0xff;
    p^=cw>>8 &0xff;
    p^=cw>>16 & 0xff;
    /* XOR the halves of the intermediate result */
    p=p ^ (p>>4);
    p=p ^ (p>>2);
    p=p ^ (p>>1);
    /* return the parity result */
    return(p & 1);
}

u32 syndrome(u32 cw)
{
 /* This function calculates and returns the syndrome of a [23,12] Golay codeword. */
    u32 syndrome=0; u32 cw1=0;
    cw1=reverseBits(cw,23);
    u16 i;
    for (i=1; i<=12; i++)
    {
        if(cw1&0x400000)
            cw1 ^=(GOLAY_POLY<<11);
        cw1<<=1;
    }
    syndrome=reverseBits(cw1,23);
    syndrome<<=12;

    return(syndrome);
}

void set_polynomial(u16 poly)
{
    GOLAY_POLY=poly;
}
