/*
 * Timer_flags.h
 *
 *  Created on: Jan 23, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef TIMER_FLAGS_H_
#define TIMER_FLAGS_H_

#define TIMER_FLAG_SET			1
#define TIMER_FLAG_CLEAR		0

typedef struct
{
u8 T1_EXPIRE	;		
u8 T2_EXPIRE	;		
u8 T3_EXPIRE	;		
u8 T100_EXPIRE	;		
u8 T101_EXPIRE	;		
u8 T102_EXPIRE	;		
u8 T103_EXPIRE	;		
u8 T104_EXPIRE	;		
u8 T105_EXPIRE	;		
u8 T106_EXPIRE	;		
u8 T107_EXPIRE	;		
u8 T108_EXPIRE	;		
u8 TR210_EXPIRE	;		
}Timer_Expire_t;

Timer_Expire_t Timer;

#endif /* TIMER_FLAGS_H_ */
