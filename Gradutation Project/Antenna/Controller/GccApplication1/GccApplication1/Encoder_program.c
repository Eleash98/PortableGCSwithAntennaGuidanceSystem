/*
 * Encoder_program.c
 *
 * Created: 4/20/2021 10:00:24 PM
 *  Author: MohammedGamalEleish
 */ 

#define F_CPU		16000000

#include <util/delay.h>

#include "STD_TYPES.h"
#include "bit-math.h"

#include "UART_interface.h"

#include "Encoder_interface.h"
#include "Encoder_private.h"
#include "Encoder_config.h"

#include <string.h>
#include <stdio.h>

void HEncoder_voidSendCommand(Encoder_t* Encoder, u8 Copy_u8Command)
{
	//MUART_voidWriteStr(UART0,"Sending Command: ");
	//char S[10];
	//sprintf(S,"%X\n",(Copy_u8Command<<4)|(Encoder->Address));
	//MUART_voidWriteStr(UART0,S);
	u8 Command = (Copy_u8Command<<4)|(Encoder->Address);
	MUART_voidWriteData(Encoder->UARTID,Command);
}

void HEncoder_voidReadData(Encoder_t* Encoder,u8* Buffer, u8 Bytes)
{

	
	//Set the Buffer Size
	u8 i = 0;
	
	//Read the Position Value and return it
	u8 Encoder_echo = MUART_u16ReadData(Encoder->UARTID);
	while (i<Bytes)
	{
		Buffer[i++] = MUART_u16ReadData(Encoder->UARTID);

	}
}


void HEncoder_voidInit(Encoder_t* Encoder)
{
	MUART_voidInit(Encoder->UARTID,9600,UART_DATA_BITS_8,UART_STOP_BITS_1,UART_PARITY_DISABLED);
	
	//HEncoder_voidChangeResolution(Encoder);
	
	//Mode
	//HEncoder_voidChangeMode(Encoder);
	
	//Baud Rate
	//HEncoder_voidChangeBaudRate(Encoder,Encoder->BaudRate);
}


Reading_t HEncoder_ReadingTGetReadings(Encoder_t* Encoder, u8 Copy_u8Reading)
{
	Reading_t R = {{0,0,0,0},{0,0},0};
	switch(Copy_u8Reading)
	{
		case ENCODER_READING_POS:
			HEncoder_voidGetPosition(Encoder,&R);
			break;
		
		case ENCODER_READING_POS_STATUS:
			HEncoder_voidGetStatus(Encoder,&R);
			break;
			
		case ENCODER_READING_POS_TIME_STATUS:
			HEncoder_voidGetTime(Encoder, &R);
			break;
			
		default:
			return R;
	}
	return R;
}


void HEncoder_voidGetPosition(Encoder_t* Encoder, Reading_t* R)
{
	
	if (Encoder->ConvMode == ENCODER_CONVERSION_DIS)
	{
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_STROBE);
		_delay_ms(10);
	}
	
	//Send Get Position Command to the selected encoder
	HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_POS);
	//_delay_ms(100);
	//Set the Buffer Size
	u8 buffsize = 2;
	if (Encoder->Mode == ENCODER_MODE_MULTI_TURN)
		buffsize = 4;
	
	HEncoder_voidReadData(Encoder, R->Pos, buffsize);
	
	R->Angle=0;
	//Convert to Radians
	u8 sz=2;
	if (Encoder->Mode == ENCODER_MODE_MULTI_TURN) sz=4;
	for (u8 i=0; i<sz; i++)
	{
		R->Angle+=R->Angle*256+R->Pos[i];
	}
	R->Angle*=360.0/Encoder->Resolution;
	
	/////////////////////////////
	/////////////////////////////
	////////////////////////////
}

void HEncoder_voidGetStatus(Encoder_t* Encoder, Reading_t* R)
{
	
	if (Encoder->ConvMode == ENCODER_CONVERSION_DIS)
	{
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_STROBE);
		_delay_ms(10);
	}
	
	//Send Get Time Command to the selected encoder
	HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_POS_1_STATUS);
	
	u8 buffsize = 3;
	if (Encoder->Mode == ENCODER_MODE_MULTI_TURN)
		buffsize = 5;
	
	u8 buffer [buffsize] ;
	
	HEncoder_voidReadData(Encoder, buffer, buffsize);
	
	memcpy(R->Pos,buffer,buffsize-2);
	R->Status = buffer[buffsize-1];
	
	HEncoder_u8GetErrorState(ENCODER_COMMAND_POS_1_STATUS,buffer,buffsize);
}

void HEncoder_voidGetTime(Encoder_t* Encoder, Reading_t* R)
{
	if (Encoder->ConvMode == ENCODER_CONVERSION_DIS)
	{
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_STROBE);
		_delay_ms(10);
	}
	
	//Send Get Time Command to the selected encoder
	HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_POS_2_TIME_STATUS);
	
	u8 buffsize = 5;
	if (Encoder->Mode == ENCODER_MODE_MULTI_TURN)
	buffsize = 7;
	
	u8 buffer [buffsize];
	
	HEncoder_voidReadData(Encoder, buffer, buffsize);
	
	memcpy(R->Pos,buffer,buffsize-4);
	memcpy(R->Time,buffer+buffsize-4,2);
	R->Status = buffer[buffsize-1];
	
	HEncoder_u8GetErrorState(ENCODER_COMMAND_POS_2_TIME_STATUS,buffer,buffsize);
}

u8 HEncoder_u8GetErrorState(u8 RequestSend, u8* DataReceived, u8 DataSize)
{
	u8 i = 0, R = RequestSend;
	while(i<DataSize-1)
	{
		R ^= DataReceived[i++];
	}
	if((R&0xF) == (DataReceived[DataSize-1] & 0xF))
		return 1;
	return 0;
}


void HEncoder_voidSleep(Encoder_t* Encoder, u8 MultiEncoder)
{
	u8 TempAddress = Encoder->Address;
	if (MultiEncoder == ENCODER_SET){
		Encoder->Address = 0xF;
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_SLEEP);
		Encoder->Address = TempAddress;
	}
	else
	{
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_SLEEP);
	}
	
}

void HEncoder_voidWakeup(Encoder_t* Encoder, u8 MultiEncoder)
{
	u8 TempAddress = Encoder->Address;
	if (MultiEncoder == ENCODER_SET){
		Encoder->Address = 0xF;
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_WAKEUP);
		Encoder->Address = TempAddress;
	}
	else
	{
		HEncoder_voidSendCommand(Encoder,ENCODER_COMMAND_WAKEUP);
	}
	_delay_ms(10);
}


void HEncoder_voidSendMultiByteCommand(Encoder_t* Encoder, u8* Command, u8 CommandLength)
{
	//HEncoder_voidSendCommand(Encoder,0xF0);
	MUART_voidWriteData(Encoder->UARTID,Command[0]);
	
	_delay_ms(7);
	
	u8 i = 1;
	while(i<CommandLength)
		MUART_voidWriteData(Encoder->UARTID, Command[i++]);
	
}

void HEncoder_voidSetOrigin(Encoder_t* Encoder)
{
	u8 Command[2] = {Encoder->Address + 0xF0, 1};
	
	HEncoder_voidSendMultiByteCommand(Encoder, Command, 2);
	
	u8 CheckSum = 0;
	HEncoder_voidReadData(Encoder, &CheckSum, 1);
	
	HEncoder_u8CheckSum(CheckSum,Command,2,0,0);
	
	if (Encoder->Mode == ENCODER_MODE_SINGLE_TURN)
		HEncoder_voidReset(Encoder);
}


void HEncoder_voidSetAbolutePosition(Encoder_t* Encoder, u8* Pos, u8 PosLength)
{
	u8 Command[PosLength+2];
	Command[0] = Encoder->Address+0xF0;
	Command[1] = 2;
	memcpy(Command+2,Pos,PosLength);
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,PosLength+2);
	
	u8 CheckSum = 0;
	HEncoder_voidReadData(Encoder, &CheckSum, 1);
	
	HEncoder_u8CheckSum(CheckSum,Command,PosLength+2,0,0);
	
	if (Encoder->Mode == ENCODER_MODE_SINGLE_TURN)
		HEncoder_voidReset(Encoder);
}

void HEncoder_voidReadSerialNumber(Encoder_t* Encoder, EncoderInfo_t* EnIn)
{
	u8 Command[2] = {Encoder->Address+0xF0,0x03};
	HEncoder_voidSendMultiByteCommand(Encoder,Command,2);
	
	u8 Buffer[5] = {0};
	HEncoder_voidReadData(Encoder, Buffer, 5);
	
	HEncoder_u8CheckSum(Buffer[4],Command,2,Buffer,4);
	
	memcpy(EnIn->SerialNumber,Buffer,4);
	
}

u8 HEncoder_u8CheckSerialNumber(Encoder_t* Encoder, EncoderInfo_t* EnIn)
{
	u8 Command[10] ,FF[4] = {0xFF,0xFF,0xFF,0xFF};
	Command[0] = Encoder->Address+0xF0;
	Command[1] = 0x4;
	memcpy(Command+2,EnIn->SerialNumber,4);
	memcpy(Command+6,FF,4);
	
	HEncoder_voidSendMultiByteCommand(Encoder, Command, 10);
	
	
	u16 i = 65535;
	while(i--)
	{
		if (HEncoder_u8ReadBusyLine(Encoder) == 0)
		{
			//Not Match
			return 0;
		}
	}
	HEncoder_voidSendCommand(Encoder,0);
	return 1;
}

u8 HEncoder_u8FailSerialNumber(Encoder_t* Encoder, EncoderInfo_t* EnIn)
{
	u8 Command[10], FF[4] = {0xFF,0xFF,0xFF,0xFF};
	Command[0] = Encoder->Address+0xF0;
	Command[1] = 0x4;
	memcpy(Command+2,EnIn->SerialNumber,4);
	memcpy(Command+6,FF,4);
	
	HEncoder_voidSendMultiByteCommand(Encoder, Command, 10);
	
	
	u16 i = 65535;
	while(i--)
	{
		if (HEncoder_u8ReadBusyLine(Encoder) == 0)
		{
			//Match
			return 1;
		}
	}
	HEncoder_voidSendCommand(Encoder,0);
	return 0;
}

u8 HEncoder_u8GetAddress(Encoder_t* Encoder, EncoderInfo_t* EnIn)
{
	u8 Command[6];
	Command[0] = Encoder->Address+0xF0;
	Command[1] = 0x06;
	memcpy(Command+2,EnIn->SerialNumber,4);
	
	HEncoder_voidSendMultiByteCommand(Encoder, Command, 6);
	
	u8 Buffer[2] = {0};
	
	HEncoder_voidReadData(Encoder,Buffer,2);
	
	HEncoder_u8CheckSum(Buffer[1],Command,6,Buffer,1);
	
	return Buffer[0];
}


void HEncoder_voidSetAddress(Encoder_t* Encoder, u8 Address, EncoderInfo_t * EnInfo){
	
	u8 Command[7];
	Command[0] = Encoder->Address+0xF0;
	Command[1] = 0x07;
	memcpy(Command+2,EnInfo->SerialNumber,4);
	Command[6] = Address & 0xF;
	
	HEncoder_voidSendMultiByteCommand(Encoder, Command, 7);
	
	u8 CheckSum = 0;
	
	HEncoder_voidReadData(Encoder, &CheckSum, 1);
	
	HEncoder_u8CheckSum(CheckSum,Command,7,0,0);
	
	HEncoder_voidReset(Encoder);
	
	Encoder->Address = Address;
	
}

void HEncoder_voidReadFactoryInfo(Encoder_t* Encoder, EncoderInfo_t* EnInfo)
{
	u8 Command[2] = {Encoder->Address+0xF0,8};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,2);
	
	u8 Buffer[15] = {0};
	HEncoder_voidReadData(Encoder,Buffer,15);
	
	HEncoder_u8CheckSum(Buffer[14],Command,2,Buffer,14);
	
	
	
	EnInfo->ModelNumber = (Buffer[0]<<8)|Buffer[1];
	EnInfo->VersionNumber = (Buffer[2]<<8)|Buffer[3];
	EnInfo->Configuration = (Buffer[4]<<8)|Buffer[5];
	memcpy(EnInfo->SerialNumber,Buffer+6,4);
	EnInfo->Months = Buffer[10];
	EnInfo->Day = Buffer[11];
	EnInfo->Year = (Buffer[12]<<8)|Buffer[13];
	
	
}

void HEncoder_voidReadResolution(Encoder_t* Encoder)
{
	u8 Command[2] = {Encoder->Address+0xF0,9};
		
	HEncoder_voidSendMultiByteCommand(Encoder,Command,2);
	
	u8 Buffer[3] = {0};
	HEncoder_voidReadData(Encoder,Buffer,3);
	
	HEncoder_u8CheckSum(Buffer[2],Command,2,Buffer,2);
	
	Encoder->Resolution = (Buffer[0]<<8) | Buffer[1];
		
	
}


void HEncoder_voidChangeResolution(Encoder_t* Encoder)
{
	u8 Command[4] = {Encoder->Address+0xF0,10,Encoder->Resolution>>8,Encoder->Resolution&0xFF};
		
	HEncoder_voidSendMultiByteCommand(Encoder,Command,4);
	
	u8 Checksum = 0;
	HEncoder_voidReadData(Encoder,&Checksum,1);
	
	HEncoder_u8CheckSum(Checksum,Command,4,0,0);
	
	//Encoder->Resolution = Resolution;
	
	//HEncoder_voidReset(Encoder);
	//first reading after changing resolution is wrong
}

void HEncoder_voidReadMode(Encoder_t* Encoder)
{
	u8 Command[2] = {Encoder->Address+0xF0,11};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,2);
	
	u8 Buffer[2] = {0};
	HEncoder_voidReadData(Encoder,Buffer,2);
	
	HEncoder_u8CheckSum(Buffer[1],Command,2,Buffer,1);
	
	Encoder->CountDirection = GET_BIT(Buffer[0],0); 
	Encoder->ConvMode = GET_BIT(Buffer[0],1);
	Encoder->Mode = GET_BIT(Buffer[0],2);
}

void HEncoder_voidChangeMode(Encoder_t* Encoder)
{
	u8 Mode = (1<<3) | (Encoder->Mode << 2) | (Encoder->ConvMode << 1) | (Encoder->CountDirection);
	u8 Command[3] = {Encoder->Address+0xF0,12,Mode};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,3);
	
	u8 Checksum = 0;
	HEncoder_voidReadData(Encoder,&Checksum,1);
	
	HEncoder_u8CheckSum(Checksum,Command,3,0,0);
	
}

void HEncoder_voidChangePowerUpMode(Encoder_t* Encoder)
{
	u8 Mode = (1<<3) | (Encoder->Mode << 2) | (Encoder->ConvMode << 1) | (Encoder->CountDirection);
	u8 Command[3] = {Encoder->Address+0xF0,13,Mode};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,3);
	
	u8 Checksum = 0;
	HEncoder_voidReadData(Encoder,&Checksum,1);
	
	HEncoder_u8CheckSum(Checksum,Command,3,0,0);
}


void HEncoder_voidReset(Encoder_t* Encoder)
{
	u8 Command[2] = {Encoder->Address+0xF0,14};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,2);
	
	u8 Checksum = 0;
	HEncoder_voidReadData(Encoder,&Checksum,1);
	
	HEncoder_u8CheckSum(Checksum,Command,2,0,0);
	
	_delay_ms(100);
}

u8 HEncoder_u8LoopBack(Encoder_t* Encoder)
{
	u8 Command[2] = {Encoder->Address+0xF0,16};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,2);
	
	u8 Buffer[2] = {0};
	
	HEncoder_voidReadData(Encoder,Buffer,2);
	if (memcmp(Command,Buffer,2*sizeof(u8)) != 0)
	{
		return 0;
	}
	return 1;
}

void HEncoder_voidChangeBaudRate(Encoder_t* Encoder,u8 NewBaud)
{
	u8 Command[3] = {Encoder->Address+0xF0,15,NewBaud};
	
	HEncoder_voidSendMultiByteCommand(Encoder,Command,3);
	
	u8 Checksum = 0;
	HEncoder_voidReadData(Encoder,&Checksum,1);
	
	u8 c = Checksum;HEncoder_u8CheckSum(Checksum,Command,2,0,0);
	
	
	Encoder->BaudRate = NewBaud;
	
	u32 Baud = (Encoder->BaudRate) <=1? Bauds[Encoder->BaudRate]:Bauds[(Encoder->BaudRate)-14];

	MUART_voidInit(Encoder->UARTID,Baud,UART_DATA_BITS_8,UART_STOP_BITS_1,UART_PARITY_DISABLED);
}

u8 HEncoder_u8CheckSum(u8 CheckByte, u8* CommandSend, u8 CommandLength, u8* DataReceived, u8 DataLength)
{
	u8 Sum = 0, i = 0 ;
	while (i < DataLength)
	{
		Sum ^= DataReceived[i++];
	}
	i = 0;
	while (i < CommandLength)
	{
		Sum ^= CommandSend[i++];
	}
	
	if (Sum == CheckByte)
		return 1;
	else
		return 0;
}
