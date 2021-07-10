#include "CSP_interface.h"
/* ********************************************************************************************************/
   const u32 H_0_values[8]=
 {
 0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
 0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
 };
 //padded message
 u8 padd_message1[64] =
 {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
 };
  u32 K_256_224_constant[64]=
 {
    0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
 };
///////////////////////////////////////////////////////////////////////
void SHA_256_224(u8 *message, u64 message_length , SHA_256_224_t *SHA_256_224_struc,u8 sha_type)
 {
    //Successful memory allocation?
    if(SHA_256_224_struc != NULL)
    {
       //Initialize the SHA-256
       SHA_256_224_initalize(SHA_256_224_struc,message_length,sha_type);
       //preprocessing(update and parsing) the message
       SHA_256_224_preprocessor(message,message_length,SHA_256_224_struc);
       //compute the SHA-256 message digest
       SHA_256_224_Compute(SHA_256_224_struc);
       /*get digest message*/
 /*
 1- H1 || H2 || H3 and so on >>>>|| symble for concatinate two arrays
 2-Since this implementation uses little endian byte ordering and SHA uses big endian,
 3- reverse all the bytes when copying the final state to the output hash.
 4-get output on sha_256_struc->digest[using casting]
 */
 u8 maxH,maxsize=0;
  if(sha_type ==1)
    {maxH=8; maxsize=32; }
  else
   {maxH=7; maxsize=28;}
 u16 temp[16] ={0};
 for(int i=0;i<maxH;i++)
   {
   for(int j=1;j>=0;j--)
     temp[j+2*i]  = (u16)((SHA_256_224_struc->H_value[i]>>(1-j)*16) & 0xffff);

   }
   for(int k=0;k<16;k++)
    for(int l=0;l<2 ;l++)
        {SHA_256_224_struc->digest[l+2*k]=(u8)((temp[k]>>l*8) & 0xff);
        }
  //finally ,swapping
  u8 temp3=0;
  for(int i=0;i<maxsize;i=i+2)
    {
    temp3=SHA_256_224_struc->digest[i];
    SHA_256_224_struc->digest[i]=SHA_256_224_struc->digest[i+1];
    SHA_256_224_struc->digest[i+1]=temp3;
    }
    printf("\n.........................................result is........................................ \n");
    for(int i=0;i<maxsize;i++)
      printf("%x",SHA_256_224_struc->digest[i]);

    printf("\n.........................................result is........................................ \n");
    for(int i=0;i<maxH;i++)
      printf("%x",SHA_256_224_struc->H_value[i]);
    }
    else
    {
       printf("\n Failed to allocate memory\n");
       return;
    }
 }
void SHA_256_224_initalize(SHA_256_224_t *SHA_256_224_struc,u64 message_length,u8 sha_type)
 {
    SHA_256_224_struc->M_padd_message[0] = padd_message1[0];
    SHA_256_224_struc->K_constant = K_256_224_constant;
    SHA_256_224_struc->M_length= 0;
	SHA_256_224_struc->M_bitlength= 0;

    //Set initial hash value
    if(sha_type ==1)
   {SHA_256_224_struc->H_value[0]  = 0x6A09E667;
    SHA_256_224_struc->H_value[1]  = 0xBB67AE85;
    SHA_256_224_struc->H_value[2]  = 0x3C6EF372;
    SHA_256_224_struc->H_value[3]  = 0xA54FF53A;
    SHA_256_224_struc->H_value[4]  = 0x510E527F;
    SHA_256_224_struc->H_value[5]  = 0x9B05688C;
    SHA_256_224_struc->H_value[6]  = 0x1F83D9AB;
    SHA_256_224_struc->H_value[7]  = 0x5BE0CD19;
    }
    else{
    SHA_256_224_struc->H_value[0]  = 0xc1059ed8;
    SHA_256_224_struc->H_value[1]  = 0x367cd507;
    SHA_256_224_struc->H_value[2]  = 0x3070dd17;
    SHA_256_224_struc->H_value[3]  = 0xf70e5939;
    SHA_256_224_struc->H_value[4]  = 0xffc00b31;
    SHA_256_224_struc->H_value[5]  = 0x68581511;
    SHA_256_224_struc->H_value[6]  = 0x64f98fa7;
    SHA_256_224_struc->H_value[7]  = 0xbefa4fa4;
    }
 }
void SHA_256_224_padding(u8 *Message, u64 Message_len,SHA_256_224_t *SHA_256_224_struc)
{
    u64 i;

	i = SHA_256_224_struc->M_length;

    if(SHA_256_224_struc->M_length < 56)
    {
    //padding one
      SHA_256_224_struc->M_padd_message[i++] =0x80;
    //padding zeros with number of padded size
    while (i < 56)
       SHA_256_224_struc->M_padd_message[i++] = 0x00;
    }
    else
    {
//padding one
      SHA_256_224_struc->M_padd_message[i++] =0x80;
    //padding zeros with number of padded size
    while (i < 64)
       SHA_256_224_struc->M_padd_message[i++] = 0x00;
    SHA_256_224_Compute(SHA_256_224_struc);
    memset(SHA_256_224_struc->M_padd_message,0,56);

    }
//get length of data
    SHA_256_224_struc->M_bitlength += SHA_256_224_struc->M_length * 8;
    SHA_256_224_struc->M_padd_message[63] = SHA_256_224_struc->M_bitlength;
	SHA_256_224_struc->M_padd_message[62] =(SHA_256_224_struc->M_bitlength) >> 8;
	SHA_256_224_struc->M_padd_message[61] =(SHA_256_224_struc->M_bitlength) >> 16;
	SHA_256_224_struc->M_padd_message[60] =(SHA_256_224_struc->M_bitlength) >> 24;
	SHA_256_224_struc->M_padd_message[59] =(SHA_256_224_struc->M_bitlength) >> 32;
	SHA_256_224_struc->M_padd_message[58] =(SHA_256_224_struc->M_bitlength) >> 40;
	SHA_256_224_struc->M_padd_message[57] =(SHA_256_224_struc->M_bitlength) >> 48;
	SHA_256_224_struc->M_padd_message[56] =(SHA_256_224_struc->M_bitlength) >> 56;

}


void SHA_256_224_preprocessor(u8 *message, u64 message_length,SHA_256_224_t *SHA_256_224_struc)
{
  for(u64 i = 0; i< message_length; ++i){ //here size in bytes
       SHA_256_224_struc->M_padd_message[SHA_256_224_struc->M_length] = message[i];
		SHA_256_224_struc->M_length++;
		//for multible block
		if (SHA_256_224_struc->M_length == 64) {
			SHA_256_224_Compute(SHA_256_224_struc);
			SHA_256_224_struc->M_bitlength += 512;
			SHA_256_224_struc->M_length = 0;
		}
	}
  SHA_256_224_padding(message,  message_length,SHA_256_224_struc);
}

void SHA_256_224_Compute(SHA_256_224_t *SHA_256_224_struc)
{
  //compute W parameters
     int t,j;
    for (t = 0, j = 0; t < 16; ++t, j += 4)
         SHA_256_224_struc->W_schedule[t]= (SHA_256_224_struc->M_padd_message[j] << 24) | (SHA_256_224_struc->M_padd_message[j+1] << 16) | (SHA_256_224_struc->M_padd_message[j+2] << 8) | (SHA_256_224_struc->M_padd_message[j+3]);

     for ( ; t < 64; ++t)
         SHA_256_224_struc->W_schedule[t] = segma(SHA_256_224_struc->W_schedule[t-2],1)+SHA_256_224_struc->W_schedule[t-7]+segma(SHA_256_224_struc->W_schedule[t-15],0)+SHA_256_224_struc->W_schedule[t-16];

 /*get working values*/
 SHA_256_224_struc->a = SHA_256_224_struc->H_value[0];
 SHA_256_224_struc->b = SHA_256_224_struc->H_value[1];
 SHA_256_224_struc->c = SHA_256_224_struc->H_value[2];
 SHA_256_224_struc->d = SHA_256_224_struc->H_value[3];
 SHA_256_224_struc->e = SHA_256_224_struc->H_value[4];
 SHA_256_224_struc->f = SHA_256_224_struc->H_value[5];
 SHA_256_224_struc->g = SHA_256_224_struc->H_value[6];
 SHA_256_224_struc->h = SHA_256_224_struc->H_value[7];

 /*compute T variables*/
 for(int t=0 ; t<64 ; ++t) //64
  {
 SHA_256_224_struc->T1 = SHA_256_224_struc->h+SUMM(SHA_256_224_struc->e,1)+CH(SHA_256_224_struc->e, SHA_256_224_struc->f, SHA_256_224_struc->g)+SHA_256_224_struc->K_constant[t]+SHA_256_224_struc->W_schedule[t];
  SHA_256_224_struc->T2=SUMM(SHA_256_224_struc->a,0)+MAJ(SHA_256_224_struc->a,SHA_256_224_struc->b,SHA_256_224_struc->c);
  SHA_256_224_struc->h=SHA_256_224_struc->g;
  SHA_256_224_struc->g=SHA_256_224_struc->f;
  SHA_256_224_struc->f=SHA_256_224_struc->e;
  SHA_256_224_struc->e=(SHA_256_224_struc->d+SHA_256_224_struc->T1);
  SHA_256_224_struc->d=SHA_256_224_struc->c;
  SHA_256_224_struc->c=SHA_256_224_struc->b;
  SHA_256_224_struc->b=SHA_256_224_struc->a;
  SHA_256_224_struc->a=SHA_256_224_struc->T1 + SHA_256_224_struc->T2;
  }
  //update working values
  SHA_256_224_struc->H_value[0]+=SHA_256_224_struc->a;
  SHA_256_224_struc->H_value[1]+=SHA_256_224_struc->b;
  SHA_256_224_struc->H_value[2]+=SHA_256_224_struc->c;
  SHA_256_224_struc->H_value[3]+=SHA_256_224_struc->d;
  SHA_256_224_struc->H_value[4]+=SHA_256_224_struc->e;
  SHA_256_224_struc->H_value[5]+=SHA_256_224_struc->f;
  SHA_256_224_struc->H_value[6]+=SHA_256_224_struc->g;
  SHA_256_224_struc->H_value[7]+=SHA_256_224_struc->h;

}///end of function


u32 segma(u32 x,u8 n)
{
    u32 y1,y2,y3,y4;
    if(n==0)
    {
    y1 = (ROTR(x, 7));
    y2= (ROTR(x, 18)) ;
    y3= (SHFR(x, 3));
    y4=y1^y2^y3;
    }
    if(n==1)
    {
    y1= (ROTR(x, 17));
     y2= (ROTR(x, 19));
     y3=  (SHFR(x, 10));
     y4=y1^y2^y3;
    }
    return y4;
}
u32 SUMM(u32 x,u8 n)
{
    u32 y1,y2,y3,y4;
    if(n==0)
    {
     y1= (ROTR(x, 2) );
     y2= (ROTR(x, 13));
     y3=( ROTR(x, 22));
     y4 = y1^y2^y3;
    }
    if(n==1)
    {
    y1=(ROTR(x, 6));
     y2= ROTR(x, 11);
    y3=(ROTR(x, 25));
    y4=y1^y2^y3;
    }
    return y4;
}


