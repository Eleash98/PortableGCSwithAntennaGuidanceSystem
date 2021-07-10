/*
 * TIM_CONFIG.h
 *
 * Created: 12/5/2020 2:08:40 PM
 *  Author: ahmed
 */ 


#ifndef TIM_CONFIG_H_
#define TIM_CONFIG_H_

#define DISABLED 0
#define ENABLED  1

#define CLKIO       1
#define CLKIO_8     2
#define CLKIO_64    3
#define CLKIO_256   4
#define CLKIO_1024  5
#define EXT_FE      6
#define EXT_RE      7

#define CLKT2       1
#define CLKT2_8     2
#define CLKT2_32    3
#define CLKT2_64    4
#define CLKT2_128   5
#define CLKT2_256   6
#define CLKT2_1024  7

#define MODE_NORMAL 0
#define MODE_PWM    1
#define MODE_CTC    2

#define PWM_FAST            0
#define PWM_PHASE_CRCT      1
#define PWM_PHASE_FREQ_CRCT 2

#define TOP_8B   0
#define TOP_9B   1
#define TOP_10B  2
#define TOP_ICR  3
#define TOP_OCRA 4

#define COM_DISCONNECT 0
#define COM_OCRA_ONLY  1
#define COM_NONINV     2
#define COM_INV        4

#define T0_PWM  -1
#define T1_PWM  -1
#define T2_PWM  -1
#define T3_PWM  -1
#define T4_PWM  -1
#define T5_PWM  -1

#define T0_TOP  -1
#define T1_TOP  -1
#define T2_TOP  -1
#define T3_TOP  -1
#define T4_TOP  -1
#define T5_TOP  -1

#define T0_STATE DISABLED
#define T1_STATE DISABLED
#define T2_STATE DISABLED
#define T3_STATE ENABLED
#define T4_STATE ENABLED
#define T5_STATE DISABLED

//////////////////////////////////////////////////////////////////////////////////
#if T0_STATE==ENABLED

#define T0CS CLKIO
#define T0_MODE MODE_NORMAL

#if T0_MODE==MODE_PWM
#define T0_PWM PWM_FAST
#define T0_TOP TOP_8B
#endif

#define T0_COMA COM_DISCONNECT
#define T0_COMB COM_DISCONNECT

#endif
/////////////////////////////////////////////////////////////////////////////////
#if T1_STATE==ENABLED

#define T1CS CLKIO_64
#define T1_MODE MODE_NORMAL

#if (T1_MODE==MODE_CTC || T1_MODE==MODE_PWM)
#define T1_PWM PWM_FAST
#define T1_TOP TOP_OCRA
#endif

#define T1_COMA COM_DISCONNECT
#define T1_COMB COM_DISCONNECT
#define T1_COMC COM_DISCONNECT

#endif
/////////////////////////////////////////////////////////////////////////////////
#if T2_STATE==ENABLED

#define T2CS CLKT2
#define T2_MODE MODE_NORMAL

#if T2_MODE==MODE_PWM
#define T2_PWM PWM_FAST
#define T2_TOP TOP_8B
#endif

#define T2_COMA COM_DISCONNECT
#define T2_COMB COM_DISCONNECT

#endif
/////////////////////////////////////////////////////////////////////////////////
#if T3_STATE==ENABLED

#define T3CS CLKIO_64
#define T3_MODE MODE_PWM

#if (T3_MODE==MODE_CTC || T3_MODE==MODE_PWM)
#undef T3_PWM
#undef T3_TOP
#define T3_PWM PWM_FAST
#define T3_TOP TOP_8B
#endif

#define T3_COMA COM_NONINV
#define T3_COMB COM_NONINV
#define T3_COMC COM_NONINV

#endif
/////////////////////////////////////////////////////////////////////////////////
#if T4_STATE==ENABLED

#define T4CS CLKIO_64
#define T4_MODE MODE_PWM

#if (T4_MODE==MODE_CTC || T4_MODE==MODE_PWM)
#undef T4_PWM
#undef T4_TOP
#define T4_PWM PWM_FAST
#define T4_TOP TOP_8B
#endif

#define T4_COMA COM_NONINV
#define T4_COMB COM_NONINV
#define T4_COMC COM_NONINV

#endif
/////////////////////////////////////////////////////////////////////////////////
#if T5_STATE==ENABLED

#define T5CS CLKIO
#define T5_MODE MODE_NORMAL

#if (T5_MODE==MODE_CTC || T5_MODE==MODE_PWM)
#define T5_PWM PWM_FAST
#define T5_TOP TOP_8B
#endif

#define T5_COMA COM_DISCONNECT
#define T5_COMB COM_DISCONNECT
#define T5_COMC COM_DISCONNECT

#endif
/////////////////////////////////////////////////////////////////////////////////

#endif /* TIM_CONFIG_H_ */