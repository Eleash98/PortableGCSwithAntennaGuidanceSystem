/*
 * ADC_CONFIG.h
 *
 * Created: 12/7/2020 9:41:21 PM
 *  Author: ahmed
 */ 


#ifndef ADC_CONFIG_H_
#define ADC_CONFIG_H_


#define AREF 0
#define AVCC 1
#define INT_1_1 2
#define INT_2_56 3

#define GND 17
#define VBG 16

#define RIGHT_ADJUST 0
#define LEFT_ADJUST 1

#define MODE_SINGLE 0
#define MODE_DIFF 1

#define GAIN_1       0
#define GAIN_10      1
#define GAIN_200     2

#define TRIG_TYPE_NORMAL 0
#define TRIG_TYPE_AUTO   1

#define TRIG_SRC_FREERUN 0
#define TRIG_SRC_AN_COMP 1
#define TRIG_SRC_EXT0 2
#define TRIG_SRC_T0COMA 3
#define TRIG_SRC_T0OVF 4
#define TRIG_SRC_T1COMB 5
#define TRIG_SRC_T1OVF 6
#define TRIG_SRC_T1CAP 7


#define PS_2 1
#define PS_4 2
#define PS_8 3
#define PS_16 4
#define PS_32 5
#define PS_64 6
#define PS_128 7

#define REF_SELECT AREF
#define ADJUST LEFT_ADJUST
#define INPUT_MODE MODE_SINGLE


#if INPUT_MODE==MODE_SINGLE
#define INPUT_CH 0
#elif INPUT_MODE==MODE_DIFF
#define INPUT_POS 0
#define INPUT_NEG 0
#define GAIN GAIN_1
#endif

#define CLK_PRESCALER PS_2
#define TRIG_TYPE TRIG_TYPE_AUTO

#if TRIG_TYPE==TRIG_TYPE_AUTO
#define TRIG_SRC TRIG_SRC_FREERUN
#endif


#endif /* ADC_CONFIG_H_ */