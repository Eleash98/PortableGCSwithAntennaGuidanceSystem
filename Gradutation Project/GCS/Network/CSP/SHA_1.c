#include "CSP_interface.h"
/* ********************************************************************************************************/
void SHA_1_func(u8 *message, u64 message_length , SHA_1_t *SHA_1_struc)
 {
    //Successful memory allocation?
    if(SHA_1_struc != NULL)
    {
       //Initialize the SHA-256
       SHA_1_initalize(SHA_1_struc,message_length);
       //preprocessing(update and parsing) the message
       SHA_1_preprocessor(message,message_length,SHA_1_struc);
       //compute the SHA-256 message digest
       SHA_1_Compute(SHA_1_struc);
       /*get digest message*/
 /*
 1- H1 || H2 || H3 and so on >>>>|| symble for concatinate two arrays
 2-Since this implementation uses little endian byte ordering and SHA uses big endian,
 3- reverse all the bytes when copying the final state to the output hash.
 4-get output on sha_256_struc->digest[using casting]
 */
 u16 temp[16] ={0};
 for(int i=0;i<8;i++)
   {
   for(int j=1;j>=0;j--)
     temp[j+2*i]  = (u16)((SHA_1_struc->H_value[i]>>(1-j)*16) & 0xffff);

   }
   for(int k=0;k<16;k++)
    for(int l=0;l<2 ;l++)
        {
            SHA_1_struc->digest[l+2*k]=(u8)((temp[k]>>l*8) & 0xff);
        }
  //finally ,swapping
  u8 temp3=0;
  for(int i=0;i<32;i=i+2)
    {
    temp3=SHA_1_struc->digest[i];
    SHA_1_struc->digest[i]=SHA_1_struc->digest[i+1];
    SHA_1_struc->digest[i+1]=temp3;
    }
    /*printf("\n.........................................result is........................................ \n");
    for(int i=0;i<20;i++)
      printf("%x",SHA_1_struc->digest[i]);
     */
    printf("\n.........................................result SHA1 is........................................ \n");
    for(int i=0;i<5;i++)
      printf("%x",SHA_1_struc->H_value[i]);
    }
    else
    {
       printf("\n Failed to allocate memory\n");
       return;
    }
 }


void SHA_1_initalize(SHA_1_t *SHA_1_struc,u64 message_length)
 {
    SHA_1_struc->K1_constant = 0x5a827999UL;  //0=<t<=19
    SHA_1_struc->K2_constant = 0x6ed9eba1UL;  //20=<t<=39
    SHA_1_struc->K3_constant = 0x8f1bbcdcUL;  //40=<t<=59
    SHA_1_struc->K4_constant = 0xca62c1d6UL;  //60=<t<=79

    SHA_1_struc->M_length= 0;
	SHA_1_struc->M_bitlength= 0;

    //Set initial hash value
    SHA_1_struc->H_value[0]  = 0x67452301UL;
    SHA_1_struc->H_value[1]  = 0xefcdab89UL;
    SHA_1_struc->H_value[2]  = 0x98badcfeUL;
    SHA_1_struc->H_value[3]  = 0x10325476UL;
    SHA_1_struc->H_value[4]  = 0xc3d2e1f0UL;

 }
void SHA_1_padding(u8 *Message, u64 Message_len,SHA_1_t *SHA_1_struc)
{
    u64 i;

	i = SHA_1_struc->M_length;

    if(SHA_1_struc->M_length < 56)
    {
    //padding one
      SHA_1_struc->M_padd_message[i++] =0x80;
    //padding zeros with number of padded size
    while (i < 56)
       SHA_1_struc->M_padd_message[i++] = 0x00;
    }
    else
    {
     //padding one
      SHA_1_struc->M_padd_message[i++] =0x80;
    //padding zeros with number of padded size
    while (i < 64)
       SHA_1_struc->M_padd_message[i++] = 0x00;
    SHA_1_Compute(SHA_1_struc);
    memset(SHA_1_struc->M_padd_message,0,56);

    }
//get length of data
     SHA_1_struc->M_bitlength = SHA_1_struc->M_length * 8;
    // STORE64H(SHA_1_struc->M_length, SHA_1_struc->M_padd_message + 56);
    SHA_1_struc->M_padd_message[63] = SHA_1_struc->M_bitlength;
	SHA_1_struc->M_padd_message[62] =(SHA_1_struc->M_bitlength) >> 8;
	SHA_1_struc->M_padd_message[61] =(SHA_1_struc->M_bitlength) >> 16;
	SHA_1_struc->M_padd_message[60] =(SHA_1_struc->M_bitlength) >> 24;
	SHA_1_struc->M_padd_message[59] =(SHA_1_struc->M_bitlength) >> 32;
	SHA_1_struc->M_padd_message[58] =(SHA_1_struc->M_bitlength) >> 40;
	SHA_1_struc->M_padd_message[57] =(SHA_1_struc->M_bitlength) >> 48;
	SHA_1_struc->M_padd_message[56] =(SHA_1_struc->M_bitlength) >> 56;
	printf("\n");
	for(int i=0;i<64 ;i++)
        printf("%x",SHA_1_struc->M_padd_message[i]);
    printf("//////////////////////////////////////\n");

}

void SHA_1_preprocessor(u8 *message, u64 message_length,SHA_1_t *SHA_1_struc)
{
  for(u64 i = 0; i< message_length; ++i){ //here size in bytes
        SHA_1_struc->M_padd_message[SHA_1_struc->M_length] = message[i];
		SHA_1_struc->M_length++;
		//for multible block
		if (SHA_1_struc->M_length == 64)
            {
			SHA_1_Compute(SHA_1_struc);
			SHA_1_struc->M_bitlength += 512;
			SHA_1_struc->M_length = 0;
		    }
	}

  SHA_1_padding(message,  message_length,SHA_1_struc);
}

void SHA_1_Compute(SHA_1_t *SHA_1_struc)
{
  //compute W parameters
     int j;
    for (int t = 0, j = 0; t < 16; t++, j += 4)
     //LOAD32H(SHA_1_struc->W_schedule[t], SHA_1_struc->M_padd_message + (4*t));
    SHA_1_struc->W_schedule[t]= (SHA_1_struc->M_padd_message[t*4] << 24) | (SHA_1_struc->M_padd_message[t*4+1] << 16) | (SHA_1_struc->M_padd_message[t*4+2] << 8) | (SHA_1_struc->M_padd_message[t*4 +3]);
    /*get working values*/
    SHA_1_struc->a = SHA_1_struc->H_value[0];
    SHA_1_struc->b = SHA_1_struc->H_value[1];
    SHA_1_struc->c = SHA_1_struc->H_value[2];
    SHA_1_struc->d = SHA_1_struc->H_value[3];
    SHA_1_struc->e = SHA_1_struc->H_value[4];
     for ( int t=16; t < 80; t++)
         SHA_1_struc->W_schedule[t] = ROL(((SHA_1_struc->W_schedule[t-3]) ^ (SHA_1_struc->W_schedule[t-8]) ^ (SHA_1_struc->W_schedule[t-14]) ^ (SHA_1_struc->W_schedule[t-16])),1);


 /*compute T variables*/
for(int t=0 ; t<20 ;) //first round
  {
     /*FF_0(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_0(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_0(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_0(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_0(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);*/
      SHA_1_struc->T =ROL(SHA_1_struc->a,5) + CH(SHA_1_struc->b ,SHA_1_struc->c,SHA_1_struc->d) + (SHA_1_struc->e) + SHA_1_struc->K1_constant + SHA_1_struc->W_schedule[t];
      SHA_1_struc->a =SHA_1_struc->d;
      SHA_1_struc->d =SHA_1_struc->c;
      SHA_1_struc->c =ROL(SHA_1_struc->b,30);
      SHA_1_struc->b =SHA_1_struc->a;
      SHA_1_struc->a =SHA_1_struc->T;
      t++;
  }
  for(int t=20 ; t<40 ;)  //second round
  {
      /*FF_1(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_1(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_1(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_1(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_1(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);*/
      SHA_1_struc->T =ROL(SHA_1_struc->a , 5) + parity(SHA_1_struc->b , SHA_1_struc->c, SHA_1_struc->d) + (SHA_1_struc->e) + SHA_1_struc->K2_constant + SHA_1_struc->W_schedule[t];
      SHA_1_struc->a =SHA_1_struc->d;
      SHA_1_struc->d =SHA_1_struc->c;
      SHA_1_struc->c =ROL(SHA_1_struc->b,30);
      SHA_1_struc->b =SHA_1_struc->a;
      SHA_1_struc->a =SHA_1_struc->T;
      t++;
  }
  for(int t=40 ; t<60 ; ) //64
  {
      /*FF_2(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_2(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_2(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_2(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_2(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);*/
      SHA_1_struc->T =ROL(SHA_1_struc->a,5) + MAJ(SHA_1_struc->b ,SHA_1_struc->c,SHA_1_struc->d) + (SHA_1_struc->e) + SHA_1_struc->K3_constant + SHA_1_struc->W_schedule[t];
      SHA_1_struc->a =SHA_1_struc->d;
      SHA_1_struc->d =SHA_1_struc->c;
      SHA_1_struc->c =ROL(SHA_1_struc->b,30);
      SHA_1_struc->b =SHA_1_struc->a;
      SHA_1_struc->a =SHA_1_struc->T;
      t++;

  }
  for(int t=60 ; t<80 ;t++ ) //64
   {
     /* FF_3(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_3(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_3(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
	  FF_3(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);
      FF_3(SHA_1_struc->a,SHA_1_struc->b, SHA_1_struc->c, SHA_1_struc->d, SHA_1_struc->e, t++);*/
      SHA_1_struc->T =ROL(SHA_1_struc->a,5) + parity(SHA_1_struc->b ,SHA_1_struc->c,SHA_1_struc->d) + (SHA_1_struc->e) + SHA_1_struc->K4_constant + SHA_1_struc->W_schedule[t];
      SHA_1_struc->a =SHA_1_struc->d;
      SHA_1_struc->d =SHA_1_struc->c;
      SHA_1_struc->c =ROL(SHA_1_struc->b,30);
      SHA_1_struc->b =SHA_1_struc->a;
      SHA_1_struc->a =SHA_1_struc->T;
      t++;
   }
  //update working values
  SHA_1_struc->H_value[0] +=SHA_1_struc->a;
  SHA_1_struc->H_value[1] +=SHA_1_struc->b;
  SHA_1_struc->H_value[2] +=SHA_1_struc->c;
  SHA_1_struc->H_value[3] +=SHA_1_struc->d;
  SHA_1_struc->H_value[4] +=SHA_1_struc->e;
}///end of function


