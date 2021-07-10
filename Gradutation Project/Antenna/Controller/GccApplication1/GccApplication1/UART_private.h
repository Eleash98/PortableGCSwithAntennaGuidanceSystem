/*
 * UART_private.h
 *
 * Created: 4/19/2021 10:06:15 PM
 *  Author: MohammedGamalEleish
 */ 


#ifndef UART_PRIVATE_H_
#define UART_PRIVATE_H_

typedef struct {
	
	volatile u8 UCSRA;
	volatile u8 UCSRB;
	volatile u8 UCSRC;
	volatile u8 RESERVED;
	volatile u8 UBRRL;
	volatile u8 UBRRH;
	volatile u8 UDR;
} UART_t;

#define U0		((volatile UART_t*)(0xC0))
#define U1		((volatile UART_t*)(0xC8))
#define U2		((volatile UART_t*)(0xD0))
#define U3		((volatile UART_t*)(0x130))


u8 Local_u8DataBits[4] = {255};

void (*TransmitComplete[4])(void) = {0};
void (*ReceiveComplete[4])(u16 ReceivedData) = {0};
void (*BufferEmpty[4])(void) = {0};
	
	
	
volatile UART_t* UART_UART_tPtrGetRegisters(u8 Copy_u8UARTID);

#endif /* UART_PRIVATE_H_ */