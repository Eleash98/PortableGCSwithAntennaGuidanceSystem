/*
 * USART_PRIV.h
 *
 * Created: 4/19/2021 12:04:40 PM
 *  Author: ahmed
 */ 


#ifndef USART_PRIV_H_
#define USART_PRIV_H_


#define USART0RX_ISR __vector_25
#define USART0UDRE_ISR __vector_26
#define USART0TX_ISR __vector_27

#define USART1RX_ISR __vector_36
#define USART1UDRE_ISR __vector_37
#define USART1TX_ISR __vector_38

#define USART2RX_ISR __vector_51
#define USART2UDRE_ISR __vector_52
#define USART2TX_ISR __vector_53

#define USART3RX_ISR __vector_54
#define USART3UDRE_ISR __vector_55
#define USART3TX_ISR __vector_56

#define UDR0 (*((volatile u8 *)0xC6 ))
#define UDR1 (*((volatile u8 *)0xCE ))
#define UDR2 (*((volatile u8 *)0xD6 ))
#define UDR3 (*((volatile u8 *)0x136))

#define UDR0RXB (*((volatile u8 *)0xC6 ))
#define UDR1RXB (*((volatile u8 *)0xCE ))
#define UDR2RXB (*((volatile u8 *)0xD6 ))
#define UDR3RXB (*((volatile u8 *)0x136))

#define UDR0TXB (*((volatile u8 *)0xC6 ))
#define UDR1TXB (*((volatile u8 *)0xCE ))
#define UDR2TXB (*((volatile u8 *)0xD6 ))
#define UDR3TXB (*((volatile u8 *)0x136))

#define UCSR0A (*((volatile u8 *)0xC0))
#define UCSR1A (*((volatile u8 *)0xC8))
#define UCSR2A (*((volatile u8 *)0xD0))
#define UCSR3A (*((volatile u8 *)0x130))

#define UCSR0B (*((volatile u8 *)0xC1))
#define UCSR1B (*((volatile u8 *)0xC9))
#define UCSR2B (*((volatile u8 *)0xD1))
#define UCSR3B (*((volatile u8 *)0x131))

#define UCSR0C (*((volatile u8 *)0xC2))
#define UCSR1C (*((volatile u8 *)0xCA))
#define UCSR2C (*((volatile u8 *)0xD2))
#define UCSR3C (*((volatile u8 *)0x132))

#define UBRR0 (*((volatile u16 *)0xC4))
#define UBRR1 (*((volatile u16 *)0xCC))
#define UBRR2 (*((volatile u16 *)0xD4))
#define UBRR3 (*((volatile u16 *)0x134))

#define UBRRL0 (*((volatile u8 *)0xC4))
#define UBRRL1 (*((volatile u8 *)0xCC))
#define UBRRL2 (*((volatile u8 *)0xD4))
#define UBRRL3 (*((volatile u8 *)0x134))

#define UBRRH0 (*((volatile u8 *)0xC5))
#define UBRRH1 (*((volatile u8 *)0xCD))
#define UBRRH2 (*((volatile u8 *)0xD5))
#define UBRRH3 (*((volatile u8 *)0x135))


#endif /* USART_PRIV_H_ */