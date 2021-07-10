/*
 * TIM_PROG.c
 *
 * Created: 12/4/2020 10:33:46 PM
 *  Author: ahmed
 */ 
#include "../LIB/BIT_MATH.h"
#include "../LIB/std-types.h"
#include "TIM_INT.h"
#include "TIM_PRIV.h"
#include "TIM_CONFIG.h"
#include "CLK_CONFIG.h"

static void (*TIM2_CMPA_Callback)(void) = 0;
static void (*TIM2_CMPB_Callback)(void) = 0;
static void (*TIM2_OVF_Callback	)(void) = 0;
static void (*TIM1_CAPT_Callback)(void) = 0;
static void (*TIM1_CMPA_Callback)(void) = 0;
static void (*TIM1_CMPB_Callback)(void) = 0;
static void (*TIM1_CMPC_Callback)(void) = 0;
static void (*TIM1_OVF_Callback	)(void) = 0;
static void (*TIM0_CMPA_Callback)(void) = 0;
static void (*TIM0_CMPB_Callback)(void) = 0;
static void (*TIM0_OVF_Callback	)(void) = 0;
static void (*TIM3_CAPT_Callback)(void) = 0;
static void (*TIM3_CMPA_Callback)(void) = 0;
static void (*TIM3_CMPB_Callback)(void) = 0;
static void (*TIM3_CMPC_Callback)(void) = 0;
static void (*TIM3_OVF_Callback	)(void) = 0;
static void (*TIM4_CAPT_Callback)(void) = 0;
static void (*TIM4_CMPA_Callback)(void) = 0;
static void (*TIM4_CMPB_Callback)(void) = 0;
static void (*TIM4_CMPC_Callback)(void) = 0;
static void (*TIM4_OVF_Callback	)(void) = 0;
static void (*TIM5_CAPT_Callback)(void) = 0;
static void (*TIM5_CMPA_Callback)(void) = 0;
static void (*TIM5_CMPB_Callback)(void) = 0;
static void (*TIM5_CMPC_Callback)(void) = 0;
static void (*TIM5_OVF_Callback	)(void) = 0;

static double timesteps[6]={0.0};

void TIM_voidTIMConfig()
{
#if T0_STATE == ENABLED
	TCCR0B |= T0CS;
	TCCR0A |= (T0_MODE == MODE_PWM);
	TCCR0A |= (T0_MODE == MODE_CTC) << 1;
	
	TCCR0A |= (T0_PWM == PWM_FAST) << 1;
	
	#if T0_MODE==MODE_PWM
	TCCR0B |= (T0_TOP==TOP_OCRA);
	#endif
	
	TCCR0A |= T0_COMA << 6;
	TCCR0A |= T0_COMB << 4;

#if T0CS==CLKIO
	timesteps[0]=1.0d/FOSC;
#elif T0CS==CLKIO_8
	timesteps[0]=8.0d/FOSC;
#elif T0CS==CLKIO_64
	timesteps[0]=64.0d/FOSC;
#elif T0CS==CLKIO_256
	timesteps[0]=256.0d/FOSC;
#elif T0CS==CLKIO_1024
	timesteps[0]=1024.0d/FOSC;
#endif

#endif
/////////////////////////////////////////////////////
#if T1_STATE == ENABLED

	TCCR1B |= T1CS;

	#if T1_MODE==MODE_PWM
	TCCR1A |= (T1_TOP!=TOP_9B && T1_TOP!=TOP_ICR);
	TCCR1A |= (T1_TOP==TOP_9B || T1_TOP==TOP_10B) << 1;
	TCCR1A |= ((T1_PWM==PWM_PHASE_CRCT  || T1_PWM==PWM_FAST)&&(T1_TOP==TOP_ICR || T1_TOP==TOP_OCRA)) << 1;
	TCCR1B |= (T1_TOP==TOP_OCRA || T1_TOP == TOP_ICR) << 4;
	#endif

	TCCR1B |= (T1_MODE == MODE_CTC) << 3;
	TCCR1B |= (T1_PWM == PWM_FAST) << 3;
	TCCR1B |= (T1_MODE==MODE_CTC && T1_TOP==TOP_ICR) << 4;

	TCCR1A |= T1_COMA << 6;
	TCCR1A |= T1_COMB << 4;
	TCCR1A |= T1_COMC << 2;

#if T1CS==CLKIO
timesteps[1]=1.0d/FOSC;
#elif T1CS==CLKIO_8
timesteps[1]=8.0d/FOSC;
#elif T1CS==CLKIO_64
timesteps[1]=64.0d/FOSC;
#elif T1CS==CLKIO_256
timesteps[1]=256.0d/FOSC;
#elif T1CS==CLKIO_1024
timesteps[1]=1024.0d/FOSC;
#endif

#endif
////////////////////////////////////////////////////////////////
#if T2_STATE == ENABLED
	TCCR2B |= T2CS;
	TCCR2A |= (T2_MODE == MODE_PWM);
	TCCR2A |= (T2_MODE == MODE_CTC) << 1;
	
	TCCR2A |= (T2_PWM == PWM_FAST) << 1;
	
	#if T2_MODE==MODE_PWM
	TCCR2B |= (T2_TOP==TOP_OCRA);
	#endif
	
	TCCR2A |= T2_COMA << 6;
	TCCR2A |= T2_COMB << 4;
#endif

#if T3_STATE == ENABLED

	TCCR3B |= T3CS;

	#if T3_MODE==MODE_PWM
	TCCR3A |= (T3_TOP!=TOP_9B && T3_TOP!=TOP_ICR);
	TCCR3A |= (T3_TOP==TOP_9B || T3_TOP==TOP_10B) << 1;
	TCCR3A |= ((T3_PWM==PWM_PHASE_CRCT  || T3_PWM==PWM_FAST)&&(T3_TOP==TOP_ICR || T3_TOP==TOP_OCRA)) << 1;
	TCCR3B |= (T3_TOP==TOP_OCRA || T3_TOP == TOP_ICR) << 4;
	#endif

	TCCR3B |= (T3_MODE == MODE_CTC) << 3;
	TCCR3B |= (T3_PWM == PWM_FAST) << 3;
	TCCR3B |= (T3_MODE==MODE_CTC && T3_TOP==TOP_ICR) << 4;

	TCCR3A |= T3_COMA << 6;
	TCCR3A |= T3_COMB << 4;
	TCCR3A |= T3_COMC << 2;
#if T3CS==CLKIO
timesteps[3]=1.0d/FOSC;
#elif T3CS==CLKIO_8
timesteps[3]=8.0d/FOSC;
#elif T3CS==CLKIO_64
timesteps[3]=64.0d/FOSC;
#elif T3CS==CLKIO_256
timesteps[3]=256.0d/FOSC;
#elif T3CS==CLKIO_1024
timesteps[3]=1024.0d/FOSC;
#endif

#endif

#if T4_STATE == ENABLED

	TCCR4B |= T4CS;

	#if T4_MODE==MODE_PWM
	TCCR4A |= (T4_TOP!=TOP_9B && T4_TOP!=TOP_ICR);
	TCCR4A |= (T4_TOP==TOP_9B || T4_TOP==TOP_10B) << 1;
	TCCR4A |= ((T4_PWM==PWM_PHASE_CRCT  || T4_PWM==PWM_FAST)&&(T4_TOP==TOP_ICR || T4_TOP==TOP_OCRA)) << 1;
	TCCR4B |= (T4_TOP==TOP_OCRA || T4_TOP == TOP_ICR) << 4;
	#endif

	TCCR4B |= (T4_MODE == MODE_CTC) << 3;
	TCCR4B |= (T4_PWM == PWM_FAST) << 3;
	TCCR4B |= (T4_MODE==MODE_CTC && T4_TOP==TOP_ICR) << 4;


	TCCR4A |= T4_COMA << 6;
	TCCR4A |= T4_COMB << 4;
	TCCR4A |= T4_COMC << 2;
#if T4CS==CLKIO
timesteps[4]=1.0d/FOSC;
#elif T4CS==CLKIO_8
timesteps[4]=8.0d/FOSC;
#elif T4CS==CLKIO_64
timesteps[4]=64.0d/FOSC;
#elif T4CS==CLKIO_256
timesteps[4]=256.0d/FOSC;
#elif T4CS==CLKIO_1024
timesteps[4]=1024.0d/FOSC;
#endif

#endif

#if T5_STATE == ENABLED

	TCCR5B |= T5CS;

	#if T5_MODE==MODE_PWM
	TCCR5A |= (T5_TOP!=TOP_9B && T5_TOP!=TOP_ICR);
	TCCR5A |= (T5_TOP==TOP_9B || T5_TOP==TOP_10B) << 1;
	TCCR5A |= ((T5_PWM==PWM_PHASE_CRCT  || T5_PWM==PWM_FAST)&&(T5_TOP==TOP_ICR || T5_TOP==TOP_OCRA)) << 1;
	TCCR5B |= (T5_TOP==TOP_OCRA || T5_TOP == TOP_ICR) << 4;
	#endif

	TCCR5B |= (T5_MODE == MODE_CTC) << 3;
	TCCR5B |= (T5_PWM == PWM_FAST) << 3;
	TCCR5B |= (T5_MODE==MODE_CTC && T5_TOP==TOP_ICR) << 4;


	TCCR5A |= T5_COMA << 6;
	TCCR5A |= T5_COMB << 4;
	TCCR5A |= T5_COMC << 2;
#if T5CS==CLKIO
timesteps[5]=1.0d/FOSC;
#elif T5CS==CLKIO_8
timesteps[5]=8.0d/FOSC;
#elif T5CS==CLKIO_64
timesteps[5]=64.0d/FOSC;
#elif T5CS==CLKIO_256
timesteps[5]=256.0d/FOSC;
#elif T5CS==CLKIO_1024
timesteps[5]=1024.0d/FOSC;
#endif

#endif
}

void TIM_voidPWMSetDuty (TIMxType timx, PWMChannelType channel, u8 percent_duty)
{
	static const u8 pwm_modes[]={T0_PWM,T1_PWM,T2_PWM,T3_PWM,T4_PWM,T5_PWM};
	static volatile u8* const icrs[]={0, &ICR1L, 0, &ICR3L, &ICR4L, &ICR5L};
	static volatile u8* const ocras[]={&OCR0A, &OCR1AL, &OCR2A, &OCR3AL, &OCR4AL, &OCR5AL};
	static volatile u8* const ocrbs[]={&OCR0B, &OCR1BL, &OCR2B, &OCR3BL, &OCR4BL, &OCR5BL};
	static volatile u8* const ocrcs[]={0, &OCR1CL, 0, &OCR3CL, &OCR4CL, &OCR5CL};
	static const volatile u8 tops[]={T0_TOP, T1_TOP, T2_TOP, T3_TOP, T4_TOP, T5_TOP};
	if (pwm_modes[timx]==-1) return;
	if ((timx==TIM0 || timx == TIM2)&&channel==PWM_CH_C) return;
	if (channel == PWM_CH_A && tops[timx]==TOP_OCRA) return;
	u8 is_16b = (timx!=TIM0&&timx!=TIM2);
	u16 top=0;
	switch (tops[timx])
	{
	case TOP_8B	 :
		top=0xFF;
		break;
	case TOP_9B	 :
		top=0x1FF;
		break;
	case TOP_10B :
		top=0x3FF;
		break;
	case TOP_ICR :
		if (is_16b)
			top=*(u16 *)(icrs[timx]);
		else
			top=*icrs[timx];
		break;
	case TOP_OCRA:
		if (is_16b)
			top=*(u16 *)(ocras[timx]);
		else
			top=*ocras[timx];
		break;
	}
	
	switch (channel)
	{
	case PWM_CH_A:
		if (is_16b)
			*(u16 *) ocras[timx]=(((top>>8)*percent_duty/100)<<8)+(top%0x100)*percent_duty/100;
		else
			*ocras[timx]=(u8)(top*percent_duty/100);
		break;
	case PWM_CH_B:
		if (is_16b)
			*(u16 *) ocrbs[timx]=(((top>>8)*percent_duty/100)<<8)+(top%0x100)*percent_duty/100;
		else
			*ocrbs[timx]=(u8)(top*percent_duty/100);
		break;
	case PWM_CH_C:
		*(u16 *) ocrcs[timx]=(((top>>8)*percent_duty/100)<<8)+(top%0xFF)*percent_duty/100;
		break;
	}
}

void TIM_voidWriteCounter(TIMxType timx,u16 count)
{
	static volatile u8 * const timer_counters[]={&TCNT0,&TCNT1L,&TCNT2,&TCNT3L,&TCNT4L,&TCNT5L};
	if (timx==TIM0||timx==TIM2)
		*(timer_counters[timx])=(u8)count;
	else if (timx==TIM1||timx==TIM3||timx==TIM4||timx==TIM5)
	{
		*(timer_counters[timx]+1)=(u8)(count>>8);
		*(timer_counters[timx])=(u8)count;
	}
}

void TIM_voidTIMxIntMask (TIMxType timx,TIMxIntType inttype)
{
	BIT_CLR(*(&TIMSK0+timx),inttype);
}

void TIM_voidTIMxIntUnmask(TIMxType timx,TIMxIntType inttype)
{
	BIT_SET(*(&TIMSK0+timx),inttype);
}

void TIM_voidTIMxClrFlag(TIMxType timx, TIMxIntType inttype)
{
	BIT_SET(*(&TIFR0+timx),inttype);
}

void TIM_voidTIMxSetCallback(TIMxType timx,TIMxIntType inttype,void(*callback)(void))
{
	if (timx==TIM0)
	{
		if  (inttype==TIM_INT_OVF) TIM0_OVF_Callback=callback;
		else if (inttype  == TIM_INT_CMPA) TIM0_CMPA_Callback=callback;
		else if (inttype == TIM_INT_CMPB) TIM0_CMPB_Callback=callback;
	}
	else if (timx==TIM2)
	{
		if  (inttype==TIM_INT_OVF) TIM2_OVF_Callback=callback;
		else if (inttype  == TIM_INT_CMPA) TIM2_CMPA_Callback=callback;
		else if (inttype == TIM_INT_CMPB) TIM2_CMPB_Callback=callback;
	}
	else if (timx==TIM1)
	{
		if  (inttype==TIM_INT_OVF) TIM1_OVF_Callback=callback;
		else if (inttype  == TIM_INT_CMPA) TIM1_CMPA_Callback=callback;
		else if (inttype == TIM_INT_CMPB) TIM1_CMPB_Callback=callback;
		else if (inttype  == TIM_INT_CMPC) TIM1_CMPC_Callback=callback;
		else if (inttype == TIM_INT_CAPT) TIM1_CAPT_Callback=callback;
	}
	else if (timx==TIM5)
	{
		if  (inttype==TIM_INT_OVF) TIM5_OVF_Callback=callback;
		else if (inttype  == TIM_INT_CMPA) TIM5_CMPA_Callback=callback;
		else if (inttype == TIM_INT_CMPB) TIM5_CMPB_Callback=callback;
		else if (inttype  == TIM_INT_CMPC) TIM5_CMPC_Callback=callback;
		else if (inttype == TIM_INT_CAPT) TIM5_CAPT_Callback=callback;
	}
	else if (timx==TIM3)
	{
		if  (inttype==TIM_INT_OVF) TIM3_OVF_Callback=callback;
		else if (inttype  == TIM_INT_CMPA) TIM3_CMPA_Callback=callback;
		else if (inttype == TIM_INT_CMPB) TIM3_CMPB_Callback=callback;
		else if (inttype  == TIM_INT_CMPC) TIM3_CMPC_Callback=callback;
		else if (inttype == TIM_INT_CAPT) TIM3_CAPT_Callback=callback;
	}
	else if (timx==TIM4)
	{
		if  (inttype==TIM_INT_OVF) TIM4_OVF_Callback=callback;
		else if (inttype  == TIM_INT_CMPA) TIM4_CMPA_Callback=callback;
		else if (inttype == TIM_INT_CMPB) TIM4_CMPB_Callback=callback;
		else if (inttype  == TIM_INT_CMPC) TIM4_CMPC_Callback=callback;
		else if (inttype == TIM_INT_CAPT) TIM4_CAPT_Callback=callback;
	}
}

void TIM_voidSetCompare(TIMxType timx, OCChannelType ch, u16 value)
{
	static volatile u8* const ocras[]={&OCR0A, &OCR1AL, &OCR2A, &OCR3AL, &OCR4AL, &OCR5AL};
	u8 is_16b = (timx!=TIM0&&timx!=TIM2);
	if (is_16b)
		*((u16 *)ocras[timx]+ch)=value;
	else
		*(ocras[timx]+ch)=(u8)value;
}
void TIM_voidChCompare(TIMxType timx, OCChannelType ch, u16 delta)
{
	static volatile u8* const ocras[]={&OCR0A, &OCR1AL, &OCR2A, &OCR3AL, &OCR4AL, &OCR5AL};
	u8 is_16b = (timx!=TIM0&&timx!=TIM2);
	if (is_16b)
	*((u16 *)ocras[timx]+ch)+=delta;
	else
	*(ocras[timx]+ch)+=(u8)delta;
}
double TIM_doubleTIMxGetTimestep(TIMxType timx)
{
	return timesteps[timx];
}

void TIM2_CMPA_ISR () __attribute__((signal,used));
void TIM2_CMPA_ISR ()
{
	(*TIM2_CMPA_Callback)();
}

void TIM2_CMPB_ISR () __attribute__((signal,used));
void TIM2_CMPB_ISR ()
{
	(*TIM2_CMPB_Callback)();
}

void TIM2_OVF_ISR  () __attribute__((signal,used));
void TIM2_OVF_ISR  ()
{
	(*TIM2_OVF_Callback)();
}

void TIM1_CAPT_ISR () __attribute__((signal,used));
void TIM1_CAPT_ISR ()
{
	(*TIM1_CAPT_Callback)();
}

void TIM1_CMPA_ISR () __attribute__((signal,used));
void TIM1_CMPA_ISR ()
{
	(*TIM1_CMPA_Callback)();
}

void TIM1_CMPB_ISR () __attribute__((signal,used));
void TIM1_CMPB_ISR ()
{
	(*TIM1_CMPB_Callback)();
}

void TIM1_CMPC_ISR () __attribute__((signal,used));
void TIM1_CMPC_ISR ()
{
	(*TIM1_CMPC_Callback)();
}

void TIM1_OVF_ISR  () __attribute__((signal,used));
void TIM1_OVF_ISR  ()
{
	(*TIM1_OVF_Callback)();
}

void TIM0_CMPA_ISR () __attribute__((signal,used));
void TIM0_CMPA_ISR ()
{
	(*TIM0_CMPA_Callback)();
}

void TIM0_CMPB_ISR () __attribute__((signal,used));
void TIM0_CMPB_ISR ()
{
	(*TIM0_CMPB_Callback)();
}

void TIM0_OVF_ISR  () __attribute__((signal,used));
void TIM0_OVF_ISR  ()
{
	(*TIM0_OVF_Callback)();
}

void TIM3_CAPT_ISR () __attribute__((signal,used));
void TIM3_CAPT_ISR ()
{
	(*TIM3_CAPT_Callback)();
}

void TIM3_CMPA_ISR () __attribute__((signal,used));
void TIM3_CMPA_ISR ()
{
	(*TIM3_CMPA_Callback)();
}

void TIM3_CMPB_ISR () __attribute__((signal,used));
void TIM3_CMPB_ISR ()
{
	(*TIM3_CMPB_Callback)();
}

void TIM3_CMPC_ISR () __attribute__((signal,used));
void TIM3_CMPC_ISR ()
{
	(*TIM3_CMPC_Callback)();
}

void TIM3_OVF_ISR  () __attribute__((signal,used));
void TIM3_OVF_ISR  ()
{
	(*TIM3_OVF_Callback)();
}

void TIM4_CAPT_ISR () __attribute__((signal,used));
void TIM4_CAPT_ISR ()
{
	(*TIM4_CAPT_Callback)();
}

void TIM4_CMPA_ISR () __attribute__((signal,used));
void TIM4_CMPA_ISR ()
{
	(*TIM4_CMPA_Callback)();
}

void TIM4_CMPB_ISR () __attribute__((signal,used));
void TIM4_CMPB_ISR ()
{
	(*TIM4_CMPB_Callback)();
}

void TIM4_CMPC_ISR () __attribute__((signal,used));
void TIM4_CMPC_ISR ()
{
	(*TIM4_CMPC_Callback)();
}

void TIM4_OVF_ISR  () __attribute__((signal,used));
void TIM4_OVF_ISR  ()
{
	(*TIM4_OVF_Callback)();
}

void TIM5_CAPT_ISR () __attribute__((signal,used));
void TIM5_CAPT_ISR ()
{
	(*TIM5_CAPT_Callback)();
}

void TIM5_CMPA_ISR () __attribute__((signal,used));
void TIM5_CMPA_ISR ()
{
	(*TIM5_CMPA_Callback)();
}

void TIM5_CMPB_ISR () __attribute__((signal,used));
void TIM5_CMPB_ISR ()
{
	(*TIM5_CMPB_Callback)();
}

void TIM5_CMPC_ISR () __attribute__((signal,used));
void TIM5_CMPC_ISR ()
{
	(*TIM5_CMPC_Callback)();
}

void TIM5_OVF_ISR  () __attribute__((signal,used));
void TIM5_OVF_ISR  ()
{
	(*TIM5_OVF_Callback)();
}