/*
 * ADC_INT.h
 *
 * Created: 12/7/2020 10:56:29 PM
 *  Author: ahmed
 */ 


#ifndef ADC_INT_H_
#define ADC_INT_H_


void ADC_voidConfig(void);
void ADC_voidEnable(void);
void ADC_voidDisable(void);
u16 ADC_u16Convert(void);
void ADC_voidIntMask(void);
void ADC_voidIntUnmask(void);
void ADC_voidClrFlag(void);
void ADC_voidSetCallback(void (* callback)(void));


#endif /* ADC_INT_H_ */