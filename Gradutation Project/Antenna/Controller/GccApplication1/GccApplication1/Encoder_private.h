/*
 * Encoder_private.h
 *
 * Created: 4/20/2021 9:59:43 PM
 *  Author: MohammedGamalEleish
 */ 


#ifndef ENCODER_PRIVATE_H_
#define ENCODER_PRIVATE_H_


#define ENCODER_COMMAND_CONTROL								0
#define ENCODER_COMMAND_POS									1
#define ENCODER_COMMAND_POS_1_STATUS						2
#define ENCODER_COMMAND_POS_2_TIME_STATUS					3
#define ENCODER_COMMAND_STROBE								4
#define ENCODER_COMMAND_SLEEP								5
#define ENCODER_COMMAND_WAKEUP								6
#define ENCODER_COMMAND_MULTIBYTE							15


#define ENCODER_MCOMMAND_SET_ORIGIN							1
#define ENCODER_MCOMMAND_SET_BAUD

u8 HEncoder_u8ReadBusyLine(Encoder_t* Encoder);


u32 Bauds[8] = {115200,57600,38400,19200,9600,4800,2400,1200};

void HEncoder_voidSendCommand(Encoder_t* Encoder, u8 Copy_u8Command);

void HEncoder_voidSendMultiByteCommand(Encoder_t* Encoder, u8* Copy_u8Command, u8 Copy_CommandLength);

void HEncoder_voidReadData(Encoder_t* Encoder,u8* Buffer, u8 Bytes);


void HEncoder_voidGetStatus(Encoder_t* Encoder, Reading_t* R);

void HEncoder_voidGetTime(Encoder_t* Encoder, Reading_t* R);

u8 HEncoder_u8GetErrorState(u8 RequestSend, u8* DataReceived, u8 DataSize);

u8 HEncoder_u8CheckSum(u8 CheckByte, u8* CommandSend, u8 CommandLength, u8* DataReceived, u8 DataLength);




#endif /* ENCODER_PRIVATE_H_ */