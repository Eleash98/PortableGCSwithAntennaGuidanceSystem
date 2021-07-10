/*
 * DIO_PRIV.h
 *
 * Created: 11/3/2020 6:03:19 AM
 *  Author: ahmed
 */ 


#ifndef _DIO_PRIV_H_
#define _DIO_PRIV_H_

#define PINA  (*((volatile u8 *)0x20))
#define DDRA  (*((volatile u8 *)(0x20+1)))
#define PORTA (*((volatile u8 *)(0x20+2)))
#define PINB  (*((volatile u8 *)0x23))
#define DDRB  (*((volatile u8 *)(0x23+1)))
#define PORTB (*((volatile u8 *)(0x23+2)))
#define PINC  (*((volatile u8 *)0x26))
#define DDRC  (*((volatile u8 *)(0x26+1)))
#define PORTC (*((volatile u8 *)(0x26+2)))
#define PIND  (*((volatile u8 *)0x29))
#define DDRD  (*((volatile u8 *)(0x29+1)))
#define PORTD (*((volatile u8 *)(0x29+2)))
#define PINE  (*((volatile u8 *)0x2C))
#define DDRE  (*((volatile u8 *)(0x2C+1)))
#define PORTE (*((volatile u8 *)(0x2C+2)))
#define PINF  (*((volatile u8 *)0x2F))
#define DDRF  (*((volatile u8 *)(0x2F+1)))
#define PORTF (*((volatile u8 *)(0x2F+2)))
#define PING  (*((volatile u8 *)0x32))
#define DDRG  (*((volatile u8 *)(0x32+1)))
#define PORTG (*((volatile u8 *)(0x32+2)))
#define PINH  (*((volatile u8 *)0x100))
#define DDRH  (*((volatile u8 *)(0x100+1)))
#define PORTH (*((volatile u8 *)(0x100+2)))
#define PINJ  (*((volatile u8 *)0x103))
#define DDRJ  (*((volatile u8 *)(0x103+1)))
#define PORTJ (*((volatile u8 *)(0x103+2)))
#define PINK  (*((volatile u8 *)0x106))
#define DDRK  (*((volatile u8 *)(0x106+1)))
#define PORTK (*((volatile u8 *)(0x106+2)))
#define PINL  (*((volatile u8 *)0x109))
#define DDRL  (*((volatile u8 *)(0x109+1)))
#define PORTL (*((volatile u8 *)(0x109+2)))

#endif /* DIO_PRIV_H_ */