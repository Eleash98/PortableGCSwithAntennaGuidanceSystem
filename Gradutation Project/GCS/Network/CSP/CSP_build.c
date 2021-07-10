
#include "CSP_interface.h"
void CSP_VOID_TRANSMISSION(CSP_STRUCTURE* CSP_FRAME,u8 hmac_key_length   ,u8* hmac_key   ,SHA_256_224_t* sha_struct  ,SHA_1_t *sha_1_struct, u8 SHA_type,u8 num_rounds,u32* XTEA_key,u8 CSP_priority)
{
    printf("print data to be sent:\n");
    for(u8 i=0;i<CSP_FRAME->data_length;i++)
        printf("%X ",CSP_FRAME->data_ptr[i]);
    printf("\n");
    printf("*************************************************************************************\n");
    /*fill CRC,xtea,hmac,reserved bits*/
     for (u8 i=0; i<4;i++)
    {
        if(GET_Bit(CSP_FRAME->flags,i))
            SET_Bit(CSP_FRAME->CSP_packet[0],i);
        else
            CLEAR_Bit(CSP_FRAME->CSP_packet[0],i);
    }
    /*the 4 MSB bits are reserved*/
    CSP_FRAME->CSP_packet[0] &=0x0f;
    /*fill the source port six bits*/
    for (u8 i=0; i<6;i++)
    {
        if (GET_Bit(CSP_FRAME->SOURCE_PORT,i))
            SET_Bit(CSP_FRAME->CSP_packet[1],i);
        else
            CLEAR_Bit(CSP_FRAME->CSP_packet[1],i);
    }
     /*fill the destination port six bits*/
     u8 j=0; u8 n=6; u8 w=0;
     for (u8 i=0; i<6;i++)
    {
        if (i==2)
        {
             j=1; w=2;
        }
        if (i>=2)
            n=0;
        if (GET_Bit(CSP_FRAME->DESTINATION_PORT,i))
            SET_Bit(CSP_FRAME->CSP_packet[1+j],i+n-w);
        else
            CLEAR_Bit(CSP_FRAME->CSP_packet[1+j],i+n-w);
    }

     /*fill the destination ID five bits*/
      j=0;  n=4;  w=0;
     for (u8 i=0; i<5;i++)
    {

        if (i==4)
        {
             j=1; w=4;
        }
        if (i>=4)
            n=0;
        if (GET_Bit(CSP_FRAME->DESTINATION_ID,i))
            SET_Bit(CSP_FRAME->CSP_packet[2+j],i+n-w);
        else
            CLEAR_Bit(CSP_FRAME->CSP_packet[2+j],i+n-w);
    }
     /*fill the source ID five bits*/
     for (u8 i=0; i<5;i++)
    {
        if (GET_Bit(CSP_FRAME->SOURCE_ID,i))
            SET_Bit(CSP_FRAME->CSP_packet[3],i+1);
        else
            CLEAR_Bit(CSP_FRAME->CSP_packet[3],i+1);
    }
     /*fill the priority two bits*/
     for (u8 i=0; i<2;i++)
    {
        if(GET_Bit(CSP_FRAME->PRIORITY,i))
            SET_Bit(CSP_FRAME->CSP_packet[3],i+6);
        else
          CLEAR_Bit(CSP_FRAME->CSP_packet[3],i+6);
    }
    printf("print CSP Header:\n");
    for(u8 i=0;i<4;i++)
        printf("%X ",CSP_FRAME->CSP_packet[i]);
    printf("\n");
    printf("*************************************************************************************\n");

    if (GET_Bit(CSP_FRAME->flags,0))
        {
            /*there is CRC*/
            CSP_FRAME->CRC=CSP_CRC32(CSP_FRAME->data_ptr,CSP_FRAME->data_length);
            /*put crc after data*/
            for (u8 i=0; i<4; i++)
                CSP_FRAME->CSP_packet[CSP_FRAME->data_length+4+i]= (CSP_FRAME->CRC >>(3-i)*8) & 0xff;
        }
    else {/* there is no CRC */}

    if (GET_Bit(CSP_FRAME->flags,3))
        {
            /*There is HMAC*/
            HMAC_process(CSP_FRAME->data_ptr, hmac_key_length,hmac_key,sha_struct,sha_1_struct,CSP_FRAME->data_length,SHA_type);
            /*put HMAC after CRC*/
            memcpy(CSP_FRAME->CSP_packet + CSP_FRAME->data_length+4+4, sha_struct->digest,32);
        }
    else {/* there is no HMAC */}

    if (GET_Bit(CSP_FRAME->flags,2))
        {
            /*data will be encrypted*/
            CSP_VOID_XTEA_ENCRYPTION(num_rounds, CSP_FRAME->data_ptr ,XTEA_key,CSP_FRAME->data_length);
            memcpy(CSP_FRAME->CSP_packet+4,CSP_FRAME->data_ptr ,CSP_FRAME->data_length);
        }
    else
        {
                /* there is no encryption for data */
                memcpy(CSP_FRAME->CSP_packet+4,CSP_FRAME->data_ptr ,CSP_FRAME->data_length);
        }



    //CSP_routing_table(CSP_FRAME);

    printf("Transmitted CSP header.......\n") ;
    printf("T_SOURCE_ID = %X\n",CSP_FRAME->SOURCE_ID);
    printf("T_DESTINATION_ID = %X\n",CSP_FRAME->DESTINATION_ID);
    printf("T_SOURCE_PORT = %X\n",CSP_FRAME->SOURCE_PORT);
    printf("T_DESTINATION_PORT = %X\n",CSP_FRAME->DESTINATION_PORT);
    printf("T_PRIORITY = %X\n",CSP_FRAME->PRIORITY);
    printf("T_FLAGS = %X\n",CSP_FRAME->flags);
    printf("*************************************************************************\n");

}

void CSP_VOID_RECEIVING(CSP_STRUCTURE* CSP_FRAME,u8 * CSP_packet,u8 hmac_key_length,u8* hmac_key,SHA_256_224_t* sha_struct,SHA_1_t *sha_1_struct,u8 SHA_type,u8 num_rounds,u32* XTEA_key)
{
    u8 data_ptr[512];

    printf("print recieved packet.............\n");
    for (u8 i=0; i<CSP_FRAME->data_length+4+4+32; i++)
    {
        printf("%X",CSP_packet[i]);
        printf(" ");
    }
    printf("\n");
    printf("*************************************************************************************\n");
    /* ****************get flag status*********************/
    for (u8 i=0; i<8;i++)
    {
        if(GET_Bit(CSP_packet[0],i))
            SET_Bit(CSP_FRAME->flags,i);
        else
            CLEAR_Bit(CSP_FRAME->flags,i);
        /*the 4 MSB bits are reserved*/
    }
    /*rest of bits are reserved*/
    //printf("flag=%X\n",CSP_FRAME->flags);
   // printf("*************************************************************************************\n");
    //here to recieve we should full structure
  /* ************************get source port*******************************************/
  for(int i=0;i<6;i++)
   {
    if(GET_Bit(CSP_packet[1],i))
        SET_Bit(CSP_FRAME->SOURCE_PORT,i);
    else
      CLEAR_Bit(CSP_FRAME->SOURCE_PORT,i);
   }
  CLEAR_Bit(CSP_FRAME->SOURCE_PORT,6);
  CLEAR_Bit(CSP_FRAME->SOURCE_PORT,7);
  /* ************************get destination port****************************************/
  for(u8 i=6;i<8;i++)
   {
       if(GET_Bit(CSP_packet[1],i))
        SET_Bit(CSP_FRAME->DESTINATION_PORT,0);
        else
          CLEAR_Bit(CSP_FRAME->DESTINATION_PORT,0);
   }

  for(int i=2;i<6;i++)
   {
        if(GET_Bit(CSP_packet[2],(i-2)))
            SET_Bit(CSP_FRAME->DESTINATION_PORT,i);
        else
          CLEAR_Bit(CSP_FRAME->DESTINATION_PORT,i);
   }
 CLEAR_Bit(CSP_FRAME->DESTINATION_PORT,6);
 CLEAR_Bit(CSP_FRAME->DESTINATION_PORT,7);
  /* **************************destination**************************************/
   for(int i=4;i<8;i++)
   {
        if(GET_Bit(CSP_packet[2],i))
            SET_Bit(CSP_FRAME->DESTINATION_ID,(i-4));
        else
          CLEAR_Bit(CSP_FRAME->DESTINATION_ID,(i-4));
    }
    if(GET_Bit(CSP_packet[3],0))
        SET_Bit(CSP_FRAME->DESTINATION_ID,4);
    else
        CLEAR_Bit(CSP_FRAME->DESTINATION_ID,4);
        CLEAR_Bit(CSP_FRAME->DESTINATION_ID,5);
        CLEAR_Bit(CSP_FRAME->DESTINATION_ID,6);
        CLEAR_Bit(CSP_FRAME->DESTINATION_ID,7);
  /* ***********************source***************************************/
  for(int i=1;i<6;i++)
   {
        if(GET_Bit(CSP_packet[3],i))
            SET_Bit(CSP_FRAME->SOURCE_ID,(i-1));
        else
          CLEAR_Bit(CSP_FRAME->SOURCE_ID,(i-1));
   }
  CLEAR_Bit(CSP_FRAME->SOURCE_ID,5);
  CLEAR_Bit(CSP_FRAME->SOURCE_ID,6);
  CLEAR_Bit(CSP_FRAME->SOURCE_ID,7);
  /* ***********************priority***************************/
  for(u8 i=6;i<8;i++)
   {
       if(GET_Bit(CSP_packet[3],i))
        SET_Bit(CSP_FRAME->PRIORITY,i-6);
       else
        CLEAR_Bit(CSP_FRAME->PRIORITY,i-6);
   }
   for(u8 i=2;i<8;i++)
      CLEAR_Bit(CSP_FRAME->PRIORITY,i);

   /* ******************now get the data recieved********************/
   CSP_FRAME->data_ptr = &CSP_packet[4];
    printf("print recieved data:\n");
 for(u8 i=0;i<strlen(CSP_FRAME->data_ptr);i++)
        printf("%X ",CSP_FRAME->data_ptr[i]);
    printf("\n");
    printf("*************************************************************************************\n");
   /* ******************decryption process****************/
   if(GET_Bit(CSP_FRAME->flags,2))
      CSP_VOID_XTEA_DECRYPTION( num_rounds, CSP_FRAME->data_ptr ,XTEA_key, CSP_FRAME->data_length);
   else
    {/*no decryption required*/}

    /* **********************get crc value******************************/
    if(GET_Bit(CSP_FRAME->flags,0))
      {
          u32 new_crc = CSP_CRC32(CSP_FRAME->data_ptr,CSP_FRAME->data_length);
          printf("calculated CRC32 = %X",new_crc);
          printf("\n");

           if(CSP_FRAME->CRC==new_crc)
            {/*right frame packet*/
                printf("right crc value........\n");
                printf("*************************************************************************************\n");
            }
            else
            {/*wrong frame packet*/
                printf("wrong crc value........\n");
                printf("*************************************************************************************\n");
            }
      }
   else
    {
        /*no crc check*/
    }
    /* ******************************HMAC VALUE*************************************/
      memcpy(data_ptr,CSP_FRAME->data_ptr,CSP_FRAME->data_length);
     if(GET_Bit(CSP_FRAME->flags,3))
      {
           HMAC_process( data_ptr , hmac_key_length , hmac_key ,sha_struct,sha_1_struct,CSP_FRAME->data_length,SHA_type );
           //check for right hmac
           u8*hmac_old =&CSP_FRAME->data_ptr[CSP_FRAME->data_length+4];//after data+crc
           u8 check = memcmp(sha_struct->digest, hmac_old,32);
       if(check ==0)
        {
            /*right hmac result*/
            printf("correct HMAC........\n");
            printf("*************************************************************************************\n");
        }
       else
       {
            /*wrong hmac result*/
            printf("wrong HMAC.........\n");
            printf("*************************************************************************************\n");
       }
      }
   else
    {/*no hmac exist*/}

    CSP_routing_table(CSP_FRAME);

    printf("Received CSP header.......\n") ;
    printf("R_SOURCE_ID = %X\n",CSP_FRAME->SOURCE_ID);
    printf("R_DESTINATION_ID = %X\n",CSP_FRAME->DESTINATION_ID);
    printf("R_SOURCE_PORT = %X\n",CSP_FRAME->SOURCE_PORT);
    printf("R_DESTINATION_PORT = %X\n",CSP_FRAME->DESTINATION_PORT);
    printf("T_PRIORITY = %X\n",CSP_FRAME->PRIORITY);
    printf("T_FLAGS = %X\n",CSP_FRAME->flags);
    printf("*************************************************************************\n");
}


