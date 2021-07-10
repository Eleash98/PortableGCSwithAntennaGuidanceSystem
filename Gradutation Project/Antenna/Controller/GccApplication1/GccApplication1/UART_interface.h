/*
 * UART_interface.h
 *
 * Created: 4/19/2021 10:05:47 PM
 *  Author: MohammedGamalEleish
 */ 


#ifndef UART_INTERFACE_H_
#define UART_INTERFACE_H_

/*UART devices IDs*/
#define UART0			0
#define UART1			1
#define UART2			2
#define UART3			3

/*Data Bits IDs*/
#define UART_DATA_BITS_5			0
#define UART_DATA_BITS_6			1
#define UART_DATA_BITS_7			2
#define UART_DATA_BITS_8			3
#define UART_DATA_BITS_9			7

/*Stop Bits IDs*/
#define UART_STOP_BITS_1			0
#define UART_STOP_BITS_2			1


/*Parity Mode IDs*/
#define UART_PARITY_DISABLED		0
#define UART_PARITY_EVEN			2
#define UART_PARITY_ODD				3


/*Interrupt IDs*/
#define UART_INTERRUPT_TRANSMIT_COMPLETE		6
#define UART_INTERRUPT_RECEIVE_COMPLETE			7
#define UART_INTERRUPT_BUFFER_EMPTY				5


/*Interrupt Flags Status*/
#define UART_FLAG_SET					1
#define UART_FLAG_CLEAR					0

void MUART_voidInit(u8 Copy_u8UARTID, u32 Copy_u32BaudRate,u8 Copy_u8NumberDataBits, u8 Copy_u8NumberStopBits, u8 Copy_u8ParityMode);

void MUART_voidWriteData(u8 Copy_u8UARTID, u8 Copy_u16Data);
u16 MUART_u16ReadData(u8 Copy_u8UARTID);


void MUART_voidWriteStr(u8 Copy_u8UARTID,char* Copy_charPtrStr);
void MUART_voidReadStr(u8 Copy_u8UARTID, u16* Copy_u8PtrBuffer, u32 Copy_u32BuffSize);

void MUART_voidEnableInterrupt(u8 Copy_u8UARTID, u8 Copy_u8InterruptID);
void MUART_voidDisableInterrupt(u8 Copy_u8UARTID, u8 Copy_u8InterruptID);

u8		MUART_voidReadFlag(u8 Copy_u8UARTID, u8 Copy_u8InterruptID);
void	MUART_voidClearFlag(u8 Copy_u8UARTID, u8 Copy_u8InterruptID);

void MUART_voidSetCallBack(u8 Copy_u8UARTID, u8 Copy_u8InterruptID, void* CallBack);

#endif /* UART_INTERFACE_H_ */