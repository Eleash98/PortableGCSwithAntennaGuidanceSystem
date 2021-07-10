/*
 * PWMD.c
 *
 * Created: 5/29/2021 8:01:00 PM
 *  Author: ahmed
 */ 
#include "std-types.h"
#include "PWMD.h"
#include "DIO_INT.h"
#include "TIM_INT.h"
#include "INT_INT.h"
static u8 az_amp=0;
static u8 el_amp=0;

static u8 az_msk=0;
static u8 el_msk=0;

void PWMD_voidInit     ()
{
	IO_voidPinMode(PE,PIN3,OUTPUT);
	IO_voidPinMode(PE,PIN4,OUTPUT);
	IO_voidPinMode(PE,PIN5,OUTPUT);
	IO_voidPinMode(PH,PIN3,OUTPUT);
	IO_voidPinMode(PH,PIN4,OUTPUT);
	IO_voidPinMode(PH,PIN5,OUTPUT);

	PWMD_voidSetAmp(MOTOR_AZ, 50);
	PWMD_voidSetAmp(MOTOR_EL, 50);
	TIM_voidWriteCounter(TIM3,0);
	TIM_voidWriteCounter(TIM4,0);
	TIM_voidTIMxSetCallback(TIM3,TIM_INT_OVF,&PWMD_voidAdvance);
	TIM_voidTIMxIntUnmask(TIM3,TIM_INT_OVF);
	INT_voidGlobEnable();
}
void PWMD_voidRun      (MotorxType motorx)
{
	if (motorx==MOTOR_AZ)
		az_msk=1;
	else if (motorx==MOTOR_EL)
		el_msk=1;
}
void PWMD_voidStop     (MotorxType motorx)
{
	if (motorx==MOTOR_AZ)
		az_msk=0;
	else if (motorx==MOTOR_EL)
		el_msk=0;
}
void PWMD_voidSetAmp   (MotorxType motorx, u8 amp)
{
	if (motorx==MOTOR_AZ)
		az_amp=amp;
	else if (motorx==MOTOR_EL)
		el_amp=amp;
}
void PWMD_voidChAmp	   (MotorxType motorx, u8 delta)
{
	if (motorx==MOTOR_AZ)
		az_amp=az_amp+delta;
	else if (motorx==MOTOR_EL)
		el_amp=el_amp+delta;
}
void PWMD_voidAdvance  ()
{
	static u8 phase=0;
	static u8 az_buf;
	static u8 el_buf;
	
	static u8 sina[]={0,  79, 150, 207, 243,   0, 243, 207, 150,  79,   0, 177, 106,
		49,  13,   0,  13,  49, 106, 177};
	static u8 sinb[]={221, 171, 104,  26, 203, 129,  66,  23,   2,   6,  35,  85, 152,
		230,  53, 127, 190, 233, 254, 250};
	static u8 sinc[]={35,   6,   2,  23,  66, 129, 203,  26, 104, 171, 221, 250, 254,
		233, 190, 127,  53, 230, 152,  85};
		
	if (!phase)
	{
		az_buf=az_amp;
		el_buf=el_amp;
	}
	TIM_voidSetCompare(TIM3,OCHA,sina[phase]*(u16)az_buf/100*az_msk);
	TIM_voidSetCompare(TIM3,OCHB,sinb[phase]*(u16)az_buf/100*az_msk);
	TIM_voidSetCompare(TIM3,OCHC,sinc[phase]*(u16)az_buf/100*az_msk);
	TIM_voidSetCompare(TIM4,OCHA,sina[phase]*(u16)el_buf/100*el_msk);
	TIM_voidSetCompare(TIM4,OCHB,sinb[phase]*(u16)el_buf/100*el_msk);
	TIM_voidSetCompare(TIM4,OCHC,sinc[phase]*(u16)el_buf/100*el_msk);
	
	phase=phase+1;
	phase%=20;
	
}
