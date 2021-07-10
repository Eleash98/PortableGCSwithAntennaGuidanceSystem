/*
 * VFD.h
 *
 * Created: 4/27/2021 12:11:35 AM
 *  Author: ahmed
 */ 


#ifndef VFD_H_
#define VFD_H_

typedef enum {MOTOR_AZ, MOTOR_EL} MotorxType;

void VFD_voidInit     ();
void VFD_voidRun      (MotorxType motorx);
void VFD_voidStop     (MotorxType motorx);
void VFD_voidSetFreq  (MotorxType motorx, u8 freq);

#endif /* VFD_H_ */