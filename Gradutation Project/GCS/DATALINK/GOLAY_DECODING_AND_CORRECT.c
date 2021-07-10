#include <stdio.h>
#include <stdlib.h>
#include "STD_TYPES.h"
#include"BIT_MATH.h"
#include"GOLAY_DECODING_AND_CORRECT.h"
#include"GOLAY_ENCODING.h"

void GOLAY_DECODING_FUNCTION(u32* decode_codeword_array,u32* syndrome_array,u32 length,u16 errs)
{

   for (u32 i=0;i<length;i++)
    {
        //printf("parity_bit(decode_codeword_array[i])&0x7fffff)= %d \n",parity_bit((decode_codeword_array[i])&(0x7fffff)));
        //printf("GET_Bit(decode_codeword_array[i],23) = %d \n",GET_Bit(decode_codeword_array[i],23));
        if ((parity_bit((decode_codeword_array[i])&(0x7fffff)))==(GET_BIT(decode_codeword_array[i],23)))
        {
            if (syndrome_array[i]==0)
            {
                printf(" no error detected in codeword %lu\n",i);
                printf("*************************************************************\n");
            }

            else if (syndrome_array[i]!=0)
            {
                printf(" error detected in codeword %lu\n",i);
                decode_codeword_array[i]=(correct(decode_codeword_array[i],&errs)) &0xffffff;
                if(syndrome((decode_codeword_array[i])& 0xffffff) ==0)
                    {

                        printf(" %d",errs);
                        printf(" errors corrected in codeword %lu\n",i);
                        syndrome_array[i]=syndrome(decode_codeword_array[i]);
                        printf("*************************************************************\n");
                    }
                else
                    {
                        printf("error can't be corrected in codeword %lu\n",i);
                    syndrome_array[i]=syndrome(decode_codeword_array[i]);
                    }
            }
        }
        else
           printf("neglect codeword because parity bit error exist in codeword %lu \n",i);

    }
}
u32 correct(u32 cw, u16* errs)
{
    /* This function corrects Golay[23,12] codeword cw, returning the corrected codeword.
    This function will produce the corrected codeword for three or fewer errors.
    It will produce some other valid Golay codeword for four or more errors,
    possibly not the intended one. *errs is set to the number of bit errors corrected. */

    u8 w;                       /* current syndrome limit weight, 2 or 3 */
    u32 mask;                   /* mask for bit flipping */
    s16 i,j;                    /* index */
    u32 s;                      /* calculated syndrome */
    u32 cwsaver;                /* saves initial value of cw */
    cwsaver=cw;                 /* save */
    w=3;                        /*initial syndrome weight threshold*/
    j=-1;
    *errs=0;                      /* -1 = no trial bit flipping on first pass */
    mask=1;
    while (j<23)             /* flip each trial bit */
    {
        if (j != -1)         /* toggle a trial bit */
        {
            if (j>0)             /* restore last trial bit */
            {
                cw=cwsaver ^ mask;
                mask+=mask;         /* point to next bit */
            }

            cw=cwsaver ^ mask;      /* flip next trial bit */
            w=2;                    /* lower the threshold while bit diddling */

        }
        s=syndrome(cw);      /* look for errors */
        if (s)               /* errors exist */
        {
            for (i=0; i<23; i++)                     /* check syndrome of each cyclic shift */
            {
                if ((*errs=codeword_weight(s)) <= w)         /* syndrome matches error pattern */
                {
                    cw=cw ^ s;                     /* remove errors */
                    cw=ROTR(cw,i);                 /* unrotate data */
                    if (j >= 0)                    /* count toggled bit (per Steve Duncan) */
                        *errs=*errs+1;
                    return(s=cw);
                }
                else
                {
                    cw=ROTL(cw,1);                 /* rotate to next pattern */
                    s=syndrome(cw);                /* calc new syndrome */
                }
            }
            j++;                                     /* toggle next trial bit */
        }
        else
            return(cw);                           /* return corrected codeword */
    }
    return(cwsaver);                         /* return original if no corrections */
}
