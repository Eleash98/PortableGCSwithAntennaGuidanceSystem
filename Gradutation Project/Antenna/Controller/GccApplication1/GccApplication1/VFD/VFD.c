/*
 * VFD.c
 *
 * Created: 4/27/2021 12:33:07 AM
 *  Author: ahmed
 */ 
#include "std-types.h"
#include "VFD.h"
#include "TIM_INT.h"
#include "DIO_INT.h"
#include "INT_INT.h"

void updateGatesAz(void)
{
	static const u8 states[] = {0b101, 0b100, 0b110, 0b010, 0b011, 0b001};
	static int i=0;
	IO_voidWritePin(PB,PIN0,(states[i]<<2)>>2);
	IO_voidWritePin(PB,PIN1,(states[i]<<1)>>2);
	IO_voidWritePin(PB,PIN2,states[i]>>2);
	i = (i+1) % 6;
	TIM_voidTIMxClrFlag(TIM1,TIM_INT_CMPA);
}

void updateGatesEl(void)
{
	static const u8 states[] = {0b101, 0b100, 0b110, 0b010, 0b011, 0b001};
	static int i=0;
	IO_voidWritePin(PB,PIN3,(states[i]<<2)>>2);
	IO_voidWritePin(PB,PIN4,(states[i]<<1)>>2);
	IO_voidWritePin(PB,PIN5,states[i]>>2);
	i = (i+1) % 6;
	TIM_voidTIMxClrFlag(TIM1,TIM_INT_CMPA);
}

void VFD_voidInit()
{
	IO_voidPinMode(PB,PIN0,OUTPUT);
	IO_voidPinMode(PB,PIN1,OUTPUT);
	IO_voidPinMode(PB,PIN2,OUTPUT);
	IO_voidPinMode(PB,PIN3,OUTPUT);
	IO_voidPinMode(PB,PIN4,OUTPUT);
	IO_voidPinMode(PB,PIN5,OUTPUT);	
	
	VFD_voidSetFreq(MOTOR_AZ, 50);
	VFD_voidSetFreq(MOTOR_EL, 50);
	
	TIM_voidTIMxSetCallback(TIM1,TIM_INT_CMPA,&updateGatesAz);
	TIM_voidTIMxSetCallback(TIM3,TIM_INT_CMPA,&updateGatesEl);
	
	TIM_voidTIMxIntUnmask(TIM1,TIM_INT_CMPA);
	TIM_voidTIMxIntUnmask(TIM3,TIM_INT_CMPA);
	
	INT_voidGlobEnable();
}

void VFD_voidSetFreq(MotorxType motorx, u8 freq)
{
	if (motorx==MOTOR_AZ)
	{
		TIM_voidSetCompare(TIM1,OCHA,1/TIM_doubleTIMxGetTimestep(TIM1)/freq/6);
	}

	else if (motorx==MOTOR_EL)
	{
		TIM_voidSetCompare(TIM3,OCHA,1/TIM_doubleTIMxGetTimestep(TIM3)/freq/6);
	}
}

void VFD_voidStop(MotorxType motorx)
{
	if (motorx==MOTOR_AZ)
	{
		IO_voidClearPin(PB,PIN0);
		IO_voidClearPin(PB,PIN1);
		IO_voidClearPin(PB,PIN2);
		TIM_voidTIMxIntMask(TIM1,TIM_INT_CMPA);
	}
	else if (motorx==MOTOR_EL)
	{
		IO_voidClearPin(PB,PIN3);
		IO_voidClearPin(PB,PIN4);
		IO_voidClearPin(PB,PIN5);
		TIM_voidTIMxIntMask(TIM3,TIM_INT_CMPA);
	}
}

void VFD_voidStart(MotorxType motorx)
{
	if (motorx==MOTOR_AZ)
	{
		TIM_voidTIMxIntUnmask(TIM1, TIM_INT_CMPA);
	}
	else if (motorx==MOTOR_EL)
	{
		TIM_voidTIMxIntUnmask(TIM3, TIM_INT_CMPA);
	}
}