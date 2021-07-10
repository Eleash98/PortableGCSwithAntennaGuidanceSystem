/*
 * INT_PRIV.h
 *
 * Created: 11/6/2020 9:53:50 PM
 *  Author: ahmed
 */ 


#ifndef INT_PRIV_H_
#define INT_PRIV_H_



#define PCICR  (*(volatile u8 *)0x68)
#define EICRA  (*(volatile u8 *)0x69)
#define EICRB  (*(volatile u8 *)0x6A)
#define PCMSK0 (*(volatile u8 *)0x6B)
#define PCMSK1 (*(volatile u8 *)0x6C)
#define PCMSK2 (*(volatile u8 *)0x6D)
#define PCIFR  (*(volatile u8 *)0x1B)
#define EIFR   (*(volatile u8 *)0x1C)
#define EIMSK  (*(volatile u8 *)0x1D)

#define INT0_ISR __vector_1
#define INT1_ISR __vector_2
#define INT2_ISR __vector_3
#define INT3_ISR __vector_4
#define INT4_ISR __vector_5
#define INT5_ISR __vector_6
#define INT6_ISR __vector_7
#define INT7_ISR __vector_8
#define PCI0_ISR __vector_9
#define PCI1_ISR __vector_10
#define PCI2_ISR __vector_11

#endif /* INT_PRIV_H_ */