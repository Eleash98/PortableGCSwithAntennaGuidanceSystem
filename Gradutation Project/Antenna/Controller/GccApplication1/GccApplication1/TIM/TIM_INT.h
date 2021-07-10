/*
 * TIM_INT.h
 *
 * Created: 12/4/2020 10:33:00 PM
 *  Author: ahmed
 */ 


#ifndef TIM_INT_H_
#define TIM_INT_H_

typedef enum {TIM0, TIM1, TIM2, TIM3, TIM4, TIM5} TIMxType;
typedef enum {TIM_MODE_NORMAL, TIM_MODE_CTC} TIMModeType;
typedef enum {PWM_CH_A,PWM_CH_B,PWM_CH_C} PWMChannelType;
typedef enum {OCHA,OCHB,OCHC} OCChannelType;
typedef enum {TIM_INT_OVF,TIM_INT_CMPA,TIM_INT_CMPB,TIM_INT_CMPC,TIM_INT_CAPT=5} TIMxIntType;
void TIM_voidTIMConfig();
void TIM_voidPWMSetDuty (TIMxType timx, PWMChannelType channel, u8 percent_duty);
void TIM_voidWriteCounter(TIMxType timx,u16 count);
void TIM_voidTIMxIntMask (TIMxType timx,TIMxIntType inttype);
void TIM_voidTIMxIntUnmask(TIMxType timx,TIMxIntType inttype);
void TIM_voidTIMxClrFlag(TIMxType timx, TIMxIntType inttype);
void TIM_voidTIMxSetCallback(TIMxType timx,TIMxIntType inttype,void(*callback)(void));
void TIM_voidSetCompare(TIMxType timx, OCChannelType ch, u16 value);
void TIM_voidChCompare(TIMxType timx, OCChannelType ch, u16 delta);
double TIM_doubleTIMxGetTimestep(TIMxType timx);
#endif /* TIM_INT_H_ */