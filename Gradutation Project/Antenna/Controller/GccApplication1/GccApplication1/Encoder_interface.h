/*
 * Encoder_interface.h
 *
 * Created: 4/20/2021 9:59:29 PM
 *  Author: MohammedGamalEleish
 */ 


#ifndef ENCODER_INTERFACE_H_
#define ENCODER_INTERFACE_H_

/*Baud Options*/
#define ENCODER_BAUD_115200					0x00
#define ENCODER_BAUD_57600					0x01
#define ENCODER_BAUD_38400					0x10
#define ENCODER_BAUD_19200					0x11
#define ENCODER_BAUD_9600					0x12
#define ENCODER_BAUD_4800					0x13
#define ENCODER_BAUD_2400					0x14
#define ENCODER_BAUD_1200					0x15

/*Mode Options*/
#define ENCODER_MODE_SINGLE_TURN					0
#define ENCODER_MODE_MULTI_TURN						1

/*Conversion Mode*/
#define ENCODER_CONVERSION_CONT						0
#define ENCODER_CONVERSION_DIS						1

/*Count Direction Options*/
#define ENCODER_DIRECTION_CLOCKWISE					0
#define ENCODER_DIRECTION_COUNTER_CLOCKWISE			1


/*Readings Options*/
#define ENCODER_READING_POS							0
#define ENCODER_READING_POS_STATUS					1
#define ENCODER_READING_POS_TIME_STATUS				2

/*Multi Encoder Options*/
#define ENCODER_SET									1
#define ENCODER_CLEAR								0

/*Error IDs*/
#define ENCODER_ERROR_NO_ERROR						0
#define ENCODER_ERROR_NOT_ENOUGH_LIGHT				1
#define ENCODER_ERROR_TOO_MUCH_LIGHT				2
#define ENCODER_ERROR_MISALL_DUST					3
#define ENCODER_ERROR_HARDWARE_PROBLEM				6
#define ENCODER_ERROR_FAST_MODE_ERROR				7
#define ENCODER_ERROR_MULTI_TURN_NO_INIT			8



typedef struct  
{
	u8 Pos[4];
	u8 Time[2];
	u8 Status;
	float Angle;
}Reading_t;


typedef struct{
	
	//u8 PBusyChannel;
	//u8 NBusyChannel;
	u8 UARTID;
	u8 Address;
	u8 BaudRate;
	u8 Mode;
	u8 ConvMode;
	u16 Resolution;
	u8 CountDirection;
	u8 SerialNumber[4];
}Encoder_t;

typedef struct  
{
	u8 SerialNumber[4];
	u16 ModelNumber;
	u16 VersionNumber;
	u16 Configuration;
	u8 Months;
	u8 Day;
	u16 Year;
}EncoderInfo_t;

void HEncoder_voidInit(Encoder_t* Encoder);

Reading_t HEncoder_ReadingTGetReadings(Encoder_t* Encoder, u8 Copy_u8Reading);

//void HEncoder_voidGetPosition(Encoder_t* Encoder, Reading_t* R);


//void HEncoder_voidGetStatus(Encoder_t* Encoder, Reading_t* R);

//void HEncoder_voidGetTime(Encoder_t* Encoder, Reading_t* R);

u8 HEncoder_u8GetErrorState(u8 RequestSend, u8* DataReceived, u8 DataSize);

void HEncoder_voidSleep(Encoder_t* Encoder, u8 MultiEncoder);

void HEncoder_voidWakeup(Encoder_t* Encoder, u8 MultiEncoder);

void HEncoder_voidSetOrigin(Encoder_t* Copy_Encoder_tPtrEncoder);

void HEncoder_voidSetAbolutePosition(Encoder_t* Encoder, u8* Pos, u8 PosLength);

void HEncoder_voidReadSerialNumber(Encoder_t* Encoder, EncoderInfo_t* EnIn);

u8 HEncoder_u8CheckSerialNumber(Encoder_t* Encoder, EncoderInfo_t* EnIn);

u8 HEncoder_u8FailSerialNumber(Encoder_t* Encoder, EncoderInfo_t* EnIn);

u8 HEncoder_u8GetAddress(Encoder_t* Encoder, EncoderInfo_t* EnIn);

void HEncoder_voidSetAddress(Encoder_t* Encoder, u8 Address, EncoderInfo_t * EnInfo);

void HEncoder_voidReadFactoryInfo(Encoder_t* Encoder, EncoderInfo_t* EnInfo);

void HEncoder_voidReadResolution(Encoder_t* Encoder);

void HEncoder_voidChangeResolution(Encoder_t* Encoder);

void HEncoder_voidReadMode(Encoder_t* Encoder);

void HEncoder_voidChangeMode(Encoder_t* Encoder);

void HEncoder_voidChangePowerUpMode(Encoder_t* Encoder);

void HEncoder_voidReset(Encoder_t* Encoder);



void HEncoder_voidChangeBaudRate(Encoder_t* Encoder,u8 NewBaud);


//Diagnostics
u8 HEncoder_u8LoopBack(Encoder_t* Encoder);

void HEncoder_voidGetPosition(Encoder_t* Encoder, Reading_t* R);




#endif /* ENCODER_INTERFACE_H_ */