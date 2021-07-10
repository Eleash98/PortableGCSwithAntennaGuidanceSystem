/*
 * INT_INT.h
 *
 * Created: 11/6/2020 9:53:27 PM
 *  Author: ahmed
 */ 


#ifndef INT_INT_H_
#define INT_INT_H_

typedef enum {INT0, INT1, INT2, INT3, INT4, INT5, INT6, INT7} INTxType;
typedef enum {LEVEL,BOTH,RISING,FALLING} INTTriggerType;
typedef enum {PCINT0, PCINT1, PCINT2, PCINT3, PCINT4, PCINT5, PCINT6, PCINT7, PCINT8, PCINT9, PCINT10, PCINT11, PCINT12, PCINT13, PCINT14, PCINT15, PCINT16, PCINT17, PCINT18, PCINT19, PCINT20, PCINT21, PCINT22, PCINT23} PCINTxType;

void INT_voidGlobEnable();
void INT_voidGlobDisable();
void INT_voidINTxConfig(INTxType intx, INTTriggerType trigger);
void INT_voidINTxMask(INTxType intx);
void INT_voidINTxUnmask(INTxType intx);
void INT_voidPCIxEnable(PCINTxType pcintx);
void INT_voidPCIxDisable(PCINTxType pcintx);
void INT_voidPCIxMask(PCINTxType pcintx);
void INT_voidPCIxUnmask(PCINTxType pcintx);
void INT_voidSetINTxCallback(INTxType intx, void (*callback)(void));
void INT_voidSetPCIxCallback(PCINTxType pcintx, void (*callback)(void));

#endif