/*
 * USART_INT.h
 *
 * Created: 4/19/2021 12:11:46 PM
 *  Author: ahmed
 */ 


#ifndef USART_INT_H_
#define USART_INT_H_


typedef enum {USART0, USART1, USART2, USART3} USARTType;

void USART_voidConfig();
void USART_voidTransfer(USARTType usart, u8 data);
u8 USART_u8Receive(USARTType usart);


#endif /* USART_INT_H_ */