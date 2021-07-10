/*
 * Timer_interface.h
 *
 *  Created on: Jan 21, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef TIMER_INTERFACE_H_
#define TIMER_INTERFACE_H_

#define AX25_TIMER_T1				0
#define AX25_TIMER_T2				1
#define AX25_TIMER_T3				2
#define AX25_TIMER_TR210			3
/*#define AX25_TIMER_T100				3
#define AX25_TIMER_T101				4
#define AX25_TIMER_T102				5
#define AX25_TIMER_T103				6
#define AX25_TIMER_T104				7
#define AX25_TIMER_T105				8
#define AX25_TIMER_T106				9
#define AX25_TIMER_T107				10
#define AX25_TIMER_T108				11

*/
#define TIMER_STATUS_RUN        1
#define TIMER_STATUS_STOP       0


typedef struct
{
	u32 T1;			//Acknowledgment timer
	u32 T2;			//Response delay timer
	u32 T3;			//Inactive link timer
	u32 T100;		//Repeater Hang timer (AXHANG)
	u32 T101;		//Priority Window timer (PRIACK)
	u32 T102;		//Slot time timer (P-Persistence)
	u32 T103;		//Transmitter startup timer (TXDELAY)
	u32 T104;		//Repeater Startup timer (AXDELAY)
	u32 T105;		//Remote Receiver Synch Timer
	u32 T106;		//10 minute Transmission limit timer
	u32 T107;		//Anti-Hogging limit timer
	u32 T108;		//Receiver startup Timer
	u32 TR210;		//Next segment timer

}Timer_t;

u32 T1Value;
//Timer_t AX25_Timer = {0};

void AX25Timer_voidInit(void);
void AX25Timer_voidExit(void);

void AX25Timer_voidSetTime(u8 TimerID,u32 TimerValue);
void AX25Timer_voidSetExpireTime(u8 TimerID,u32 ExpireValue);
void AX25Timer_voidStop(u8 TimerID);
void AX25Timer_voidStart(u8 TimerID);
void AX25Timer_voidClear(u8 TimerID);

u8 AX25Timer_u8GetStatus(u8 TimerID);

u32 AX25Timer_u32GetTime(u8 TimerID);
u32 AX25Timer_u32GetExpireTime(u8 TimerID);



#endif /* TIMER_INTERFACE_H_ */
