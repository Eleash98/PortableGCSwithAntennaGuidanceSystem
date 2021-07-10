#ifndef BIT_MATH_H_INCLUDED
#define BIT_MATH_H_INCLUDED


#define SET_Bit(port,n)     port|=(1<<n)        //make bit number n =1
#define CLEAR_Bit(port,n)   port&=~(1<<n)    //make bit number n =0
#define TOGGLE_Bit(port,n)  port^=(1<<n)    //toggle bit number n
#define GET_Bit(port,n)    (port >>(n))&1        //get state of bit number n
#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   (x >> n) | (x << (32 - n))
#define ROTL(x, n)   (x << n) | (x >> (32 - n))
#define ROL(x,y)	(((x) << (y)) | ((x) >> (32-y)))



#endif // BIT_MATH_H_INCLUDED
