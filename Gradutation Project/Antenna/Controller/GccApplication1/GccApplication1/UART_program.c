/*
 * UART_program.c
 *
 * Created: 4/19/2021 10:05:25 PM
 *  Author: MohammedGamalEleish
 */ 

#include "STD_TYPES.h"
#include "bit-math.h"

#include "UART_interface.h"
#include "UART_private.h"
#include "UART_config.h"

#include <stdio.h>
#include <avr/interrupt.h>

volatile UART_t* UART_UART_tPtrGetRegisters(u8 Copy_u8UARTID)
{
	volatile UART_t* U = 0;
	switch(Copy_u8UARTID){
		case UART0:
			U = U0;
			break;
		case UART1:
			U = U1;
			break;
		case UART2:
			U = U2;
			break;
		case UART3:
			U = U3;
			break;
		default:
			return 0;
	}
	return U;
}

void MUART_voidInit(u8 Copy_u8UARTID, u32 Copy_u32BaudRate,u8 Copy_u8NumberDataBits, u8 Copy_u8NumberStopBits, u8 Copy_u8ParityMode)
{
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if ( U == 0 )
		return;
	
	Local_u8DataBits[Copy_u8UARTID] = Copy_u8NumberDataBits;
	
	U->UCSRA = 0;
	U->UCSRB = Copy_u8NumberDataBits == UART_DATA_BITS_9?  0x1C:0x18;
	U->UCSRC = (Copy_u8ParityMode << 4) | (Copy_u8NumberStopBits << 3) | ((Copy_u8NumberDataBits & 3) << 1);
	
	u16 Local_u16Baud = (UART_SYSTEM_CLOCK/(16UL*Copy_u32BaudRate)) - 1;
	U->UBRRH = (Local_u16Baud >>8) & 0xFF;
	U->UBRRL = (Local_u16Baud & 0xFF);
}

void MUART_voidWriteData(u8 Copy_u8UARTID, u8 Copy_u16Data)
{	
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if ( U == 0 )
		return;
	while(!GET_BIT(U->UCSRA,5));
		
	if (Local_u8DataBits[Copy_u8UARTID] == UART_DATA_BITS_9)
	{
		CLR_BIT((U->UCSRB),0);
		U->UCSRB |= GET_BIT(Copy_u16Data,8);
	}
	U->UDR = Copy_u16Data & 0xFF;
}

u16 MUART_u16ReadData(u8 Copy_u8UARTID)
{
	u16 Local_Received = 0;
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if (U == 0){
		//MUART_voidWriteStr(UART0,"Wrong UART!!\n");
		return 0;
	}
	long signed int TimeOut = 1000000;
	while(!GET_BIT(U->UCSRA,7) && TimeOut-->1);
	//char *A = TimeOut == 0?	"Timeout...":"Received...";
	//MUART_voidWriteStr(UART0,A);
	//char S[50];
	//sprintf(S,"Time = %lu\n",TimeOut);
	//MUART_voidWriteStr(UART0,S);
	Local_Received = Local_u8DataBits[Copy_u8UARTID] == UART_DATA_BITS_9? (GET_BIT((U->UCSRB),1)<<8 | U->UDR) : U->UDR;
	
	if (TimeOut == 0)
	return 255;
	
	return Local_Received;
}


void MUART_voidWriteStr(u8 Copy_u8UARTID, char* Copy_charPtrStr)
{
	u32 i = 0;
	while(Copy_charPtrStr[i] != '\0')
		MUART_voidWriteData(Copy_u8UARTID,Copy_charPtrStr[i++]);
}

void MUART_voidReadStr(u8 Copy_u8UARTID, u16* Copy_u8PtrBuffer, u32 Copy_u32BuffSize)
{
	u32 i = 0;
	u32 Timeout = 10000;
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if (U == 0)
		return;
	while(i < Copy_u32BuffSize)
	{	
		while(!GET_BIT(U->UCSRA,7) && Timeout--);
		if (Timeout == 0)
		{
			Copy_u8PtrBuffer[i] = 0;
			return;
		}
		Copy_u8PtrBuffer[i] = Local_u8DataBits[Copy_u8UARTID] == UART_DATA_BITS_9? (GET_BIT((U->UCSRB),1)<<8 | U->UDR) : U->UDR;
		i++;
	}
}

void MUART_voidEnableInterrupt(u8 Copy_u8UARTID, u8 Copy_u8InterruptID)
{
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if (U == 0)
		return;
	SET_BIT((U->UCSRB),Copy_u8InterruptID);
	sei();
}

void MUART_voidDisableInterrupt(u8 Copy_u8UARTID, u8 Copy_u8InterruptID)
{
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if (U == 0)
		return;
	CLR_BIT((U->UCSRB),Copy_u8InterruptID);
}

void MUART_voidSetCallBack(u8 Copy_u8UARTID, u8 Copy_u8InterruptID, void* CallBack)
{
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if (U == 0)
		return;
	switch(Copy_u8InterruptID){
		case UART_INTERRUPT_TRANSMIT_COMPLETE:
			TransmitComplete[Copy_u8UARTID] = (void (*)(void)) CallBack;
			break;
		case UART_INTERRUPT_RECEIVE_COMPLETE:
			ReceiveComplete[Copy_u8UARTID] = (void (*)(u16))CallBack;
			break;
		case UART_INTERRUPT_BUFFER_EMPTY:
			BufferEmpty[Copy_u8UARTID] = (void (*)(void))CallBack;
			break;
		default:
			return;
	}
}

u8		MUART_voidReadFlag(u8 Copy_u8UARTID, u8 Copy_u8InterruptID)
{
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
	if (U == 0)
		return 255;
	return GET_BIT((U->UCSRA),Copy_u8InterruptID);
	
}

void	MUART_voidClearFlag(u8 Copy_u8UARTID, u8 Copy_u8InterruptID)
{
	volatile UART_t* U = UART_UART_tPtrGetRegisters(Copy_u8UARTID);
		if (U == 0)
	return;
	SET_BIT((U->UCSRA),Copy_u8InterruptID);
}

ISR (USART0_RX_vect)
{
	if (ReceiveComplete[0] == 0)
	{
		SET_BIT((U0->UCSRA),7);
		return;
	}
	
	u16 RecData = Local_u8DataBits[0] == UART_DATA_BITS_9? (GET_BIT((U0->UCSRB),1)<<8 | U0->UDR) : U0->UDR;
	ReceiveComplete[0](RecData);
	
	SET_BIT((U0->UCSRA),7);
	return;
};

ISR (USART0_TX_vect)
{
	if (TransmitComplete[0] == 0)
	{
		SET_BIT((U0->UCSRA),6);
		return;
	}
	
	TransmitComplete[0]();
	
	SET_BIT((U0->UCSRA),6);
	return;
};

ISR (USART0_UDRE_vect)
{
	if (BufferEmpty[0] == 0)
	{
		SET_BIT((U0->UCSRA),5);
		return;
	}
	
	BufferEmpty[0]();
	
	SET_BIT((U0->UCSRA),5);
	return;
};


ISR (USART1_RX_vect)
{
	if (ReceiveComplete[1] == 0)
	{
		SET_BIT((U1->UCSRA),7);
		return;
	}
	
	u16 RecData = Local_u8DataBits[1] == UART_DATA_BITS_9? (GET_BIT((U1->UCSRB),1)<<8 | U1->UDR) : U1->UDR;
	ReceiveComplete[1](RecData);
	
	SET_BIT((U1->UCSRA),7);
	return;
};

ISR (USART1_TX_vect)
{
	if (TransmitComplete[1] == 0)
	{
		SET_BIT((U1->UCSRA),6);
		return;
	}
	
	TransmitComplete[1]();
	
	SET_BIT((U1->UCSRA),6);
	return;
};

ISR (USART1_UDRE_vect)
{
	if (BufferEmpty[1] == 0)
	{
		SET_BIT((U1->UCSRA),5);
		return;
	}
	
	BufferEmpty[1]();
	
	SET_BIT((U1->UCSRA),5);
	return;
};


ISR (USART2_RX_vect)
{
	if (ReceiveComplete[2] == 0)
	{
		SET_BIT((U2->UCSRA),7);
		return;
	}
	
	u16 RecData = Local_u8DataBits[2] == UART_DATA_BITS_9? (GET_BIT((U2->UCSRB),1)<<8 | U2->UDR) : U2->UDR;
	ReceiveComplete[2](RecData);
	
	SET_BIT((U2->UCSRA),7);
	return;
};

ISR (USART2_TX_vect)
{
	if (TransmitComplete[2] == 0)
	{
		SET_BIT((U2->UCSRA),6);
		return;
	}
	
	TransmitComplete[2]();
	
	SET_BIT((U2->UCSRA),6);
	return;
};

ISR (USART2_UDRE_vect)
{
	if (BufferEmpty[2] == 0)
	{
		SET_BIT((U2->UCSRA),5);
		return;
	}
	
	BufferEmpty[2]();
	
	SET_BIT((U2->UCSRA),5);
	return;
};


ISR (USART3_RX_vect)
{
	if (ReceiveComplete[3] == 0)
	{
		SET_BIT((U3->UCSRA),7);
		return;
	}
	
	u16 RecData = Local_u8DataBits[3] == UART_DATA_BITS_9? (GET_BIT((U3->UCSRB),1)<<8 | U3->UDR) : U3->UDR;
	ReceiveComplete[3](RecData);
	
	SET_BIT((U3->UCSRA),7);
	return;
};

ISR (USART3_TX_vect)
{
	if (TransmitComplete[3] == 0)
	{
		SET_BIT((U3->UCSRA),6);
		return;
	}
	
	TransmitComplete[3]();
	
	SET_BIT((U3->UCSRA),6);
	return;
};

ISR (USART3_UDRE_vect)
{
	if (BufferEmpty[3] == 0)
	{
		SET_BIT((U3->UCSRA),5);
		return;
	}
	
	BufferEmpty[3]();
	
	SET_BIT((U3->UCSRA),5);
	return;
};
