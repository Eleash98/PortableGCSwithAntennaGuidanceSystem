/*
 * ADC_PRIV.h
 *
 * Created: 12/7/2020 10:58:22 PM
 *  Author: ahmed
 */ 


#ifndef ADC_PRIV_H_
#define ADC_PRIV_H_


#define ADC_ISR __vector_29

#define ADMUX (*(volatile u8 *)0x7C)
#define ADCSRB (*(volatile u8 *)0x7B)
#define ADCSRA (*(volatile u8 *)0x7A)
#define ADCL (*(volatile u8 *)0x78)
#define ADCH (*(volatile u8 *)0x79)
#define DIDR0 (*(volatile u8 *)0x7E)
#define DIDR2 (*(volatile u8 *)0x7D)


#endif /* ADC_PRIV_H_ */