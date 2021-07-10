/*
 * DL_interface.h
 *
 *  Created on: Jan 26, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef DL_INTERFACE_H_
#define DL_INTERFACE_H_


#define PH_FLAG_SET				1
#define PH_FLAG_CLEAR			0

void SetCallBack(u8 CallBackType, void (*Ptr)(void));

#define PH_CALLBACK_TURN_ON_TRANSMITTER					0
#define PH_CALLBACK_TURN_OFF_TRANSMITTER				1

#define PH_CALLBACK_TRANSMIT							2
#define PH_CALLBACK_RECEIVE								3


void Transmit(u8 *Frame, u32 FrameLength);
void DL_voidSetControlBits(u8 ControlBits);
typedef struct
{
	u8 AcquisitionOfSignal;
	u8 Frame;
	u8 LossOfSignal;
}HD_t;

HD_t 				Hardware;

char MYAddress[7];
u8 MySSID;
char DestAddress[7];
u8 DestSSID;

u8 SREJ_Enable;

u8 RejectType;

u32 N1;			/*Maximum number of octets in info field of a frame*/
u32 N2;			/*Maximum number of retries permitted*/

void DL_voidInit(void);
void SM_voidDataLinkLayer(void);
u8 DL_u8CheckErrors(u8 ModuloType,frame_t* Frame);

void DL_voidExit(void);

u32 RC;			/*Retries count*/
u8 K;           /*Window Size*/
void Transmit(u8*,u32);

u8* DLtoAssembler;
u32 DLtoAssemblerLength;


u8** SegmentertoDL;
s32* SegmentsLengths;
s32  NofSegments;

frame_t* HardwaretoDL;

u8 Layer3Protocol;

u32 NofRepeaters;
#endif /* DL_INTERFACE_H_ */
