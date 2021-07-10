/*
 * TIM_PRIV.h
 *
 * Created: 12/4/2020 10:33:18 PM
 *  Author: ahmed
 */ 


#ifndef TIM_PRIV_H_
#define TIM_PRIV_H_


#define TIM2_CMPA_ISR __vector_13
#define TIM2_CMPB_ISR __vector_14
#define TIM2_OVF_ISR  __vector_15
#define TIM1_CAPT_ISR __vector_16
#define TIM1_CMPA_ISR __vector_17
#define TIM1_CMPB_ISR __vector_18
#define TIM1_CMPC_ISR __vector_19
#define TIM1_OVF_ISR  __vector_20
#define TIM0_CMPA_ISR __vector_21
#define TIM0_CMPB_ISR __vector_22
#define TIM0_OVF_ISR  __vector_23
#define TIM3_CAPT_ISR __vector_31
#define TIM3_CMPA_ISR __vector_32
#define TIM3_CMPB_ISR __vector_33
#define TIM3_CMPC_ISR __vector_34
#define TIM3_OVF_ISR  __vector_35
#define TIM4_CAPT_ISR __vector_41
#define TIM4_CMPA_ISR __vector_42
#define TIM4_CMPB_ISR __vector_43
#define TIM4_CMPC_ISR __vector_44
#define TIM4_OVF_ISR  __vector_45
#define TIM5_CAPT_ISR __vector_46
#define TIM5_CMPA_ISR __vector_47
#define TIM5_CMPB_ISR __vector_48
#define TIM5_CMPC_ISR __vector_49
#define TIM5_OVF_ISR  __vector_50


#define TIFR0  (*(volatile u8 *)0x35)
#define TIFR1  (*(volatile u8 *)0x36)
#define TIFR2  (*(volatile u8 *)0x37)
#define TIFR3  (*(volatile u8 *)0x38)
#define TIFR4  (*(volatile u8 *)0x39)
#define TIFR5  (*(volatile u8 *)0x3A)
#define TCCR0A (*(volatile u8 *)0x44)
#define TCCR0B (*(volatile u8 *)0x45)
#define TCNT0  (*(volatile u8 *)0x46)
#define OCR0A  (*(volatile u8 *)0x47)
#define OCR0B  (*(volatile u8 *)0x48)
#define TIMSK0 (*(volatile u8 *)0x6E)
#define TIMSK1 (*(volatile u8 *)0x6F)
#define TIMSK2 (*(volatile u8 *)0x70)
#define TIMSK3 (*(volatile u8 *)0x71)
#define TIMSK4 (*(volatile u8 *)0x72)
#define TIMSK5 (*(volatile u8 *)0x73)
#define TCCR1A (*(volatile u8 *)0x80)
#define TCCR1B (*(volatile u8 *)0x81)
#define TCCR1C (*(volatile u8 *)0x82)
#define TCNT1L (*(volatile u8 *)0x84)
#define TCNT1H (*(volatile u8 *)0x85)
#define ICR1L  (*(volatile u8 *)0x86)
#define ICR1H  (*(volatile u8 *)0x87)
#define OCR1AL (*(volatile u8 *)0x88)
#define OCR1AH (*(volatile u8 *)0x89)
#define OCR1BL (*(volatile u8 *)0x8A)
#define OCR1BH (*(volatile u8 *)0x8B)
#define OCR1CL (*(volatile u8 *)0x8C)
#define OCR1CH (*(volatile u8 *)0x8D)
#define TCCR3A (*(volatile u8 *)0x90)
#define TCCR3B (*(volatile u8 *)0x91)
#define TCCR3C (*(volatile u8 *)0x92)
#define TCNT3L (*(volatile u8 *)0x94)
#define TCNT3H (*(volatile u8 *)0x95)
#define ICR3L  (*(volatile u8 *)0x96)
#define ICR3H  (*(volatile u8 *)0x97)
#define OCR3AL (*(volatile u8 *)0x98)
#define OCR3AH (*(volatile u8 *)0x99)
#define OCR3BL (*(volatile u8 *)0x9A)
#define OCR3BH (*(volatile u8 *)0x9B)
#define OCR3CL (*(volatile u8 *)0x9C)
#define OCR3CH (*(volatile u8 *)0x9D)
#define TCCR4A (*(volatile u8 *)0xA0)
#define TCCR4B (*(volatile u8 *)0xA1)
#define TCCR4C (*(volatile u8 *)0xA2)
#define TCNT4L (*(volatile u8 *)0xA4)
#define TCNT4H (*(volatile u8 *)0xA5)
#define ICR4L  (*(volatile u8 *)0xA6)
#define ICR4H  (*(volatile u8 *)0xA7)
#define OCR4AL (*(volatile u8 *)0xA8)
#define OCR4AH (*(volatile u8 *)0xA9)
#define OCR4BL (*(volatile u8 *)0xAA)
#define OCR4BH (*(volatile u8 *)0xAB)
#define OCR4CL (*(volatile u8 *)0xAC)
#define OCR4CH (*(volatile u8 *)0xAD)
#define TCCR2A (*(volatile u8 *)0xB0)
#define TCCR2B (*(volatile u8 *)0xB1)
#define TCNT2  (*(volatile u8 *)0xB2)
#define OCR2A  (*(volatile u8 *)0xB3)
#define OCR2B  (*(volatile u8 *)0xB4)
#define TCCR5A (*(volatile u8 *)0x120)
#define TCCR5B (*(volatile u8 *)0x121)
#define TCCR5C (*(volatile u8 *)0x122)
#define TCNT5L (*(volatile u8 *)0x124)
#define TCNT5H (*(volatile u8 *)0x125)
#define ICR5L  (*(volatile u8 *)0x126)
#define ICR5H  (*(volatile u8 *)0x127)
#define OCR5AL (*(volatile u8 *)0x128)
#define OCR5AH (*(volatile u8 *)0x129)
#define OCR5BL (*(volatile u8 *)0x12A)
#define OCR5BH (*(volatile u8 *)0x12B)
#define OCR5CL (*(volatile u8 *)0x12C)
#define OCR5CH (*(volatile u8 *)0x12D)


#endif /* TIM_PRIV_H_ */