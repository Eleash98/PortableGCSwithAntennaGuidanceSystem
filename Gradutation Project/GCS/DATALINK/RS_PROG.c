#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "RS_PRIVATE.h"
#include "RS_INTERFACE.h"
//************************************************************************//
//************************************************************************//
//**ALPHA matrix**//
unsigned char CCSDS_alpha_to[] = {
0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x87,0x89,0x95,0xad,0xdd,0x3d,0x7a,0xf4,
0x6f,0xde,0x3b,0x76,0xec,0x5f,0xbe,0xfb,0x71,0xe2,0x43,0x86,0x8b,0x91,0xa5,0xcd,
0x1d,0x3a,0x74,0xe8,0x57,0xae,0xdb,0x31,0x62,0xc4,0x0f,0x1e,0x3c,0x78,0xf0,0x67,
0xce,0x1b,0x36,0x6c,0xd8,0x37,0x6e,0xdc,0x3f,0x7e,0xfc,0x7f,0xfe,0x7b,0xf6,0x6b,
0xd6,0x2b,0x56,0xac,0xdf,0x39,0x72,0xe4,0x4f,0x9e,0xbb,0xf1,0x65,0xca,0x13,0x26,
0x4c,0x98,0xb7,0xe9,0x55,0xaa,0xd3,0x21,0x42,0x84,0x8f,0x99,0xb5,0xed,0x5d,0xba,
0xf3,0x61,0xc2,0x03,0x06,0x0c,0x18,0x30,0x60,0xc0,0x07,0x0e,0x1c,0x38,0x70,0xe0,
0x47,0x8e,0x9b,0xb1,0xe5,0x4d,0x9a,0xb3,0xe1,0x45,0x8a,0x93,0xa1,0xc5,0x0d,0x1a,
0x34,0x68,0xd0,0x27,0x4e,0x9c,0xbf,0xf9,0x75,0xea,0x53,0xa6,0xcb,0x11,0x22,0x44,
0x88,0x97,0xa9,0xd5,0x2d,0x5a,0xb4,0xef,0x59,0xb2,0xe3,0x41,0x82,0x83,0x81,0x85,
0x8d,0x9d,0xbd,0xfd,0x7d,0xfa,0x73,0xe6,0x4b,0x96,0xab,0xd1,0x25,0x4a,0x94,0xaf,
0xd9,0x35,0x6a,0xd4,0x2f,0x5e,0xbc,0xff,0x79,0xf2,0x63,0xc6,0x0b,0x16,0x2c,0x58,
0xb0,0xe7,0x49,0x92,0xa3,0xc1,0x05,0x0a,0x14,0x28,0x50,0xa0,0xc7,0x09,0x12,0x24,
0x48,0x90,0xa7,0xc9,0x15,0x2a,0x54,0xa8,0xd7,0x29,0x52,0xa4,0xcf,0x19,0x32,0x64,
0xc8,0x17,0x2e,0x5c,0xb8,0xf7,0x69,0xd2,0x23,0x46,0x8c,0x9f,0xb9,0xf5,0x6d,0xda,
0x33,0x66,0xcc,0x1f,0x3e,0x7c,0xf8,0x77,0xee,0x5b,0xb6,0xeb,0x51,0xa2,0xc3,0x00,
};

//**index matrix**//
unsigned char CCSDS_index_of[] = {
255,  0,  1, 99,  2,198,100,106,  3,205,199,188,101,126,107, 42,
  4,141,206, 78,200,212,189,225,102,221,127, 49,108, 32, 43,243,
  5, 87,142,232,207,172, 79,131,201,217,213, 65,190,148,226,180,
103, 39,222,240,128,177, 50, 53,109, 69, 33, 18, 44, 13,244, 56,
  6,155, 88, 26,143,121,233,112,208,194,173,168, 80,117,132, 72,
202,252,218,138,214, 84, 66, 36,191,152,149,249,227, 94,181, 21,
104, 97, 40,186,223, 76,241, 47,129,230,178, 63, 51,238, 54, 16,
110, 24, 70,166, 34,136, 19,247, 45,184, 14, 61,245,164, 57, 59,
  7,158,156,157, 89,159, 27,  8,144,  9,122, 28,234,160,113, 90,
209, 29,195,123,174, 10,169,145, 81, 91,118,114,133,161, 73,235,
203,124,253,196,219, 30,139,210,215,146, 85,170, 67, 11, 37,175,
192,115,153,119,150, 92,250, 82,228,236, 95, 74,182,162, 22,134,
105,197, 98,254, 41,125,187,204,224,211, 77,140,242, 31, 48,220,
130,171,231, 86,179,147, 64,216, 52,176,239, 38, 55, 12, 17, 68,
111,120, 25,154, 71,116,167,193, 35, 83,137,251, 20, 93,248,151,
 46, 75,185, 96, 15,237, 62,229,246,135,165, 23, 58,163, 60,183,
};

//**Generator Polynomial matrix**//
unsigned char g[]=
{
1,149,63,158,151,242,60,185,
229,153,181,171,55,177,254,13,
130,133,13,84,242,76,232,172,197,
251,179,219,205,119,135,182,59};

unsigned char CCSDS_Initial_root=1;
//************************************************************************//
//************************************************************************//
void RS_polynomial_generator(unsigned int poly,unsigned char Initial_root)
{
    int i=0;
    unsigned char* poly1;
    unsigned char* poly2;
    unsigned char* poly3;
    poly1=(unsigned char*)malloc(33);
    poly2=(unsigned char*)malloc(33);
    poly3=(unsigned char*)malloc(33);
    memset(poly1,0,33);
    memset(poly2,0,33);
    memset(poly3,0,33);
    memset(g,0,33);
    memset(CCSDS_alpha_to,0,256);
    memset(CCSDS_index_of,0,256);
    CCSDS_Initial_root=Initial_root;
    CCSDS_alpha_to[0]=0x01;
    CCSDS_alpha_to[1]=0x02;
    CCSDS_alpha_to[2]=0x04;
    CCSDS_alpha_to[3]=0x08;
    CCSDS_alpha_to[4]=0x10;
    CCSDS_alpha_to[5]=0x20;
    CCSDS_alpha_to[6]=0x40;
    CCSDS_alpha_to[7]=0x80;
    poly=poly%256;
    CCSDS_alpha_to[8]=(unsigned char)poly;
    CCSDS_alpha_to[255]=0;
    for (i=9;i<255;i++)
    {
        CCSDS_alpha_to[i]=(CCSDS_alpha_to[i-1]&0x80)?(CCSDS_alpha_to[i-1]<<1)^CCSDS_alpha_to[8]:CCSDS_alpha_to[i-1]<<1;
    }
    for (i=0;i<256;i++)
    {
        CCSDS_index_of[CCSDS_alpha_to[i]]=i;
    }
    poly2[1]=1;
    poly2[0]=CCSDS_alpha_to[Initial_root%255];
    for (i=1;i<32;i++)
    {
        poly1[1]=1;
        poly1[0]=CCSDS_alpha_to[(i+Initial_root)%255];
        memcpy(poly3,poly2,33);
        free(poly2);
        poly2=RS_223_to_255_void_RS_mult_poly(poly1,poly3);
        memset(poly1,0,33);
    }
    for (i=0;i<33;i++)
        g[32-i]=poly2[i];
    free(poly1);
    free(poly2);
    free(poly3);
}
//************************************************************************//
//************************************************************************//
unsigned char* RS_223_255_encoder_driver_pointer_unsigned_char(unsigned char* data, unsigned long int* data_len)
{
    int number_of_blocks=0;
    int i=0;
    unsigned char* data_block=(unsigned char*)malloc(255);
    memset(data_block,0,255);
    number_of_blocks = (*data_len)/223;
    if ((*data_len)%223)
    {
        number_of_blocks++;
        data=(unsigned char*)realloc(data,number_of_blocks*223/**sizeof(unsigned char)**/);
        memset(data+(*data_len),0,223*number_of_blocks-(*data_len));
    }
    unsigned char* encoded_data=(unsigned char*)malloc((number_of_blocks*255)*sizeof(unsigned char));
    memset(encoded_data,0,(number_of_blocks*255)*sizeof(unsigned char));
    for (i=0;i<number_of_blocks;i++)
    {
        memcpy(data_block,data+i*223,223);
        data_block=RS_223_to_255_encoder_P_pointer_unsigned_char(data_block);
        if ((i==number_of_blocks-1)&&((*data_len)%223))
        {
            memcpy(encoded_data+i*255,data_block,*data_len%223);
            memcpy(encoded_data+i*255+(*data_len)%223,data_block+223,32);
        }
        else
            memcpy(encoded_data+i*255,data_block,255);
        memset(data_block,0,255);
    }
    *data_len+=32*number_of_blocks;
    free(data);
    free(data_block);
    return encoded_data;
}
//************************************************************************//
//************************************************************************//
unsigned char* RS_223_to_255_encoder_P_pointer_unsigned_char(unsigned char* data)
{
    unsigned char index=0;
    unsigned char* data_copy;
    unsigned char* temp1=(unsigned char*)malloc(33);
    unsigned char* temp2=(unsigned char*)malloc(33);
    data_copy=(unsigned char*)malloc(255);
    int i=0,j=0;
    memset(temp1,0,33);
    memset(temp2,0,33);
    memset(data_copy,0,255);
    memcpy(data_copy,data,223);
    memcpy(temp1,data_copy,33);
    for (i=0;i<223;i++)
    {
        temp1[32]=data_copy[i+32];
        for (j=0;j<33;j++)
        {
            index=(CCSDS_index_of[temp1[0]]+CCSDS_index_of[g[j]])%255;
            temp2[j]=(temp1[0]==0)?0:CCSDS_alpha_to[index];
        }
        for (j=0;j<33;j++)
        {
            temp1[j]=temp1[j]^temp2[j];
        }
        for (j=0;j<32;j++)
        {
            temp1[j]=temp1[j+1];
        }
    }
    data =(unsigned char*)realloc(data,255);
    memcpy(data+223,temp1,32);
    free(temp1);
    free(temp2);
    free(data_copy);
    return data;
}
//************************************************************************//
//************************************************************************//
unsigned char* RS_223_255_decoder_driver_pointer_unsigned_char(unsigned char* data, unsigned long int* data_len)
{
    int number_of_blocks=0;
    int i=0;
    unsigned char* data_block=(unsigned char*)malloc(255);
    memset(data_block,0,255);
    number_of_blocks = *data_len/255;
    if (*data_len%255)
    {
        number_of_blocks++;
    }
    unsigned char* decoded_data=(unsigned char*)malloc((number_of_blocks*223)*sizeof(unsigned char));
    memset(decoded_data,0,(number_of_blocks*223)*sizeof(unsigned char));
    for (i=0;i<number_of_blocks;i++)
    {
        if ((i==number_of_blocks-1)&&((*data_len)%223))
        {
            memcpy(data_block,data+i*255,(*data_len%255)-32);
            memcpy(data_block+223,data+(*data_len)-32,32);
            data_block=RS_223_to_255_decoder_P_pointer_unsigned_char(data_block);
            memcpy(decoded_data+i*223,data_block,(*data_len%255)-32);
        }
        else
        {
            memcpy(data_block,data+i*255,255);
            data_block=RS_223_to_255_decoder_P_pointer_unsigned_char(data_block);
            memcpy(decoded_data+i*223,data_block,223);
        }
        memset(data_block,0,255);
    }
    *data_len-=32*number_of_blocks;
    free(data);
    free(data_block);
    return decoded_data;
}
//************************************************************************//
//************************************************************************//
unsigned char* RS_223_to_255_decoder_P_pointer_unsigned_char(unsigned char* data)
{
    unsigned char s[32];
    unsigned char* q1=(unsigned char*)malloc(33);
    unsigned char*r1=(unsigned char*)malloc(33);
    unsigned char* r2=(unsigned char*)malloc(33);
    unsigned char temp[33];
    unsigned char omega[33];
    unsigned char index=0;
    unsigned char* lamda_n=(unsigned char*)malloc(33);
    unsigned char* lamda_n_1=(unsigned char*)malloc(33);
    unsigned char* lamda_n_2=(unsigned char*)malloc(33);
    unsigned char power_of_r2=32;
    unsigned char error=0;
    unsigned char error_count=0;
    unsigned char error_position[32];
    unsigned char error_value[32];
    memset(error_position,0,32);
    memset(error_value,0,32);
    memset(s,0,32);
    memset(r1,0,33);
    memset(q1,0,33);
    memset(r2,0,33);
    memset(temp,0,33);
    memset(omega,0,33);
    int i=0,j=0;
    /*Syndrome polynomial*/
    for (i=0;i<32;i++)
    {
        for (j=0;j<255;j++)
        {
            index=(CCSDS_index_of[data[j]]+(254-j)*CCSDS_index_of[CCSDS_alpha_to[i+CCSDS_Initial_root]])%255; /*add the Initial_root*/
            s[i]^=(data[j]==0)?0:CCSDS_alpha_to[index];
        }
        error|=s[i];
    }
    if (error)
    {
        for (i=0;i<32;i++)
            r2[i+1]=s[31-i];
        power_of_r2=31;
        for (i=1;i<32;i++)
        {
            if (r2[i])
                break;
            power_of_r2--;
        }
        r1[0]=1;
        memset(lamda_n,0,33);
        memset(lamda_n_1,0,33);
        memset(lamda_n_2,0,33);
        lamda_n_1[0]=1;
        do
        {
            memset(q1,0,33);
            RS_223_to_255_void_divide_poly(r1,r2,q1);
            for(i=0;i<33;i++)
                temp[32-i]=q1[i];
            for(i=0;i<33;i++)
                q1[i]=temp[i];
            free(lamda_n);
            lamda_n=RS_223_to_255_void_RS_mult_poly(q1,lamda_n_1);
            for (i=0;i<33;i++)
            {
                lamda_n[i]^=lamda_n_2[i];
                lamda_n_2[i]=lamda_n_1[i];
                lamda_n_1[i]=lamda_n[i];
                temp[i]=r1[i];
                r1[i]=r2[i];
                r2[i]=temp[i];
            }
            power_of_r2=32;
            for (i=0;i<32;i++)
            {
                if (r2[i])
                    break;
                power_of_r2--;
            }
        }while(power_of_r2>=16);
        temp[0]=lamda_n[0];
        for (i=0;i<33;i++)
            omega[32-i]=r2[i];
        /*Error magnitude polynomial*/
        for (i=0;i<33;i++)
        {
            index = (255+CCSDS_index_of[lamda_n[i]]-CCSDS_index_of[temp[0]])%255;
            lamda_n[i]=(lamda_n[i]==0)?0:CCSDS_alpha_to[index];
            index = (255+CCSDS_index_of[omega[i]]-CCSDS_index_of[temp[0]])%255;
            omega[i]=(omega[i]==0)?0:CCSDS_alpha_to[index];
        }
        error=0;
        index=0;
        error_count=0;
        /*Error locater polynomial*/
        for (i=0;i<255;i++)
        {
            for (j=0;j<33;j++)
            {
                index=(CCSDS_index_of[lamda_n[j]]+j*CCSDS_index_of[CCSDS_alpha_to[i]])%255;
                error^=(lamda_n[j]==0)?0:CCSDS_alpha_to[index];
            }
            if (error==0)
            {
                error_count++;
                error_position[error_count-1]=i;
            }
            error=0;
        }
        for (i=0;i<32;i+=2)
        {
            lamda_n[i]=lamda_n[i+1];
            lamda_n[i+1]=0;
        }
        memset(lamda_n_2,0,33);
        memset(error_value,0,32);
        /*to be modified by using Initial_root*/
        for (i=0;i<error_count;i++)
        {
            for (j=0;j<33;j++)
            {
                index=(CCSDS_index_of[omega[j]]+j*error_position[i])%255;
                error_value[i]^=(omega[j]==0)?0:CCSDS_alpha_to[index];
                index=(CCSDS_index_of[lamda_n[j]]+(j)*(error_position[i]))%255;
                lamda_n_2[i]^=(lamda_n[j]==0)?0:CCSDS_alpha_to[index];
            }
            index=((CCSDS_index_of[error_value[i]]-CCSDS_index_of[lamda_n_2[i]]+255+error_position[i]*(CCSDS_Initial_root-1+255))%255)%255;   /*add the Initial_root hear*/
            error_value[i]=CCSDS_alpha_to[index];
        }
        if (RS_DEBUG)
        {
            printf("\n");
            printf("number of error = %d ",error_count);
            printf("\n");
            for (i=0;i<error_count;i++)
            {
              printf("error no %d = %d   position = %d\n",i,error_value[i],error_position[i]-1);
            }
        }
        for (i=0;i<error_count;i++)
        {
            data[error_position[i]-1]^=error_value[i];
        }
    }
    else
    {
        if (RS_DEBUG)
            printf("no errors\n");
    }
    free(q1);
    free(r1);
    free(r2);
    free(lamda_n);
    free(lamda_n_1);
    free(lamda_n_2);
    return data;
}
//************************************************************************//
//************************************************************************//
unsigned char* RS_223_to_255_void_divide_poly(unsigned char* poly1,unsigned char* poly2,unsigned char*q1)
{
    unsigned char poly2_power=32;
    unsigned char index=0;
    unsigned char q_power=32;
    unsigned char temp[33];
    memset(q1,0,33);
    memset(temp,0,33);
    int i=0,j=0;
    for (i=0;i<33;i++)
    {
        if (poly2[i])
            break;
        poly2_power--;
    }
    q_power=32-poly2_power+1;
    for (i=0;i<33;i++)
    {
        q_power--;
        if (i>(32-poly2_power))
            return q1;
        if (poly1[i]==0)
            continue;
        index=(255+CCSDS_index_of[poly1[i]]-CCSDS_index_of[poly2[32-poly2_power]])%255;
        q1[32-q_power]=CCSDS_alpha_to[index];
        for (j=q_power;j<33;j++)
        {
            index=(CCSDS_index_of[q1[32-q_power]]+CCSDS_index_of[poly2[j]])%255;
            temp[j-q_power]=((q1[32-q_power]==0)||(poly2[j]==0))?0:CCSDS_alpha_to[index];
        }
        for (j=i;j<33;j++)
        {
            poly1[j]^= temp[j];
        }
    }
    return q1;
}
//****************************************************************************//
//****************************************************************************//
unsigned char* RS_223_to_255_void_RS_mult_poly(unsigned char*poly1,unsigned char*poly2)
{
    int index=0;
    unsigned char temp[33];
    unsigned char* res=(unsigned char*)malloc(33);
    unsigned char poly1_power=32;
    unsigned char poly2_power=32;
    int i=0;int j=0;
    for (i=32;i>=0;i--)
    {
        if (poly1[i]!=0)
            break;
        poly1_power--;
        if (i==0)
            poly1_power=0;
    }
    for (j=32;j>=0;j--)
    {
        if (poly2[j]!=0)
            break;
        poly2_power--;
        if (j==0)
            poly2_power=0;
    }
    memset(temp,0,33);
    memset(res,0,33);
    for (i=0;i<poly1_power+1;i++)
    {
        if (poly1[i]==0)
            continue;
        for (j=0;j<poly2_power+1;j++)
        {
            index=CCSDS_index_of[poly1[i]];
            index=(index+CCSDS_index_of[poly2[j]])%255;
            temp[j+i]=(poly2[j]==0)?0:CCSDS_alpha_to[index];
        }
        for(j=0;j<33;j++)
        {
            res[j]^=temp[j];
        }
        memset(temp,0,33);
    }
    return res;
}
