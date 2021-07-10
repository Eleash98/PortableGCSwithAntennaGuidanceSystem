/*
 * BIT_MATH.h
 *
 * Created: 11/3/2020 6:56:19 AM
 *  Author: ahmed
 */ 


#ifndef _BIT_MATH_H_
#define _BIT_MATH_H_

#define BIT_SET(x,p) ((x)|=1<<(p))
#define BIT_CLR(x,p) ((x)&=~(1<<(p)))
#define BIT_TGL(x,p) ((x)^=1<<(p))
#define BIT_GET(x,p) ((u8)(((x)&(1<<(p)))!=0))

#endif /* BIT_MATH_H_ */