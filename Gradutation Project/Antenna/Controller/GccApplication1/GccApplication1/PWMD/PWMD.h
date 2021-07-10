/*
 * PWMD.h
 *
 * Created: 5/29/2021 8:00:40 PM
 *  Author: ahmed
 */ 


#ifndef PWMD_H_
#define PWMD_H_


typedef enum {MOTOR_AZ, MOTOR_EL} MotorxType;

void PWMD_voidInit     ();
void PWMD_voidRun      (MotorxType motorx);
void PWMD_voidStop     (MotorxType motorx);
void PWMD_voidSetAmp   (MotorxType motorx, u8 freq);
void PWMD_voidChAmp	   (MotorxType motorx, u8 delta);
void PWMD_voidAdvance  ();


#endif /* PWMD_H_ */