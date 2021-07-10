/*
 * INT_PROG.c
 *
 * Created: 11/6/2020 9:54:14 PM
 *  Author: ahmed
 */ 
#include "BIT_MATH.h"
#include "std-types.h"
#include "INT_INT.h"
#include "INT_PRIV.h"



static  void (*INTx_Callback[8])(void) = { 0 };
static  void (*PCIx_Callback[3])(void) = { 0 };


void INT_voidGlobEnable()
{
	asm("sei");
}

void INT_voidGlobDisable()
{
	asm("cli");
}

void INT_voidINTxConfig(INTxType intx, INTTriggerType trigger)
{
	if (intx < INT4)
	{
		EICRA &= ~(3 << (2*intx));
		EICRA |= trigger << (2*intx);
	}
	else
	{
		EICRB &= ~(3 << (2*intx-8));
		EICRB |= trigger << (2*intx-8);
	}
}

void INT_voidINTxMask(INTxType intx)
{
	BIT_CLR(EIMSK,intx);
}

void INT_voidINTxUnmask(INTxType intx)
{
	BIT_SET(EIMSK,intx);
}

void INT_voidPCIxEnable(PCINTxType pcintx)
{
	BIT_SET(PCICR,pcintx>>3);
}

void INT_voidPCIxDisable(PCINTxType pcintx)
{
	BIT_CLR(PCICR,pcintx>>3);
}

void INT_voidPCIxMask(PCINTxType pcintx)
{
	BIT_SET(*(&PCMSK0 + (pcintx >> 3)),pcintx&7);
}

void INT_voidPCIxUnmask(PCINTxType pcintx)
{
	BIT_CLR(*(&PCMSK0 + (pcintx >> 3)),pcintx&7);
}

void INT_voidSetINTxCallback(INTxType intx, void (*callback)(void))
{
	INTx_Callback[intx]=callback;
}

void INT_voidSetPCIxCallback(PCINTxType pcintx, void (*callback)(void))
{
	PCIx_Callback[pcintx>>3]=callback;
}



void INT0_ISR () __attribute__((signal,used));
void INT0_ISR ()
{
	(*INTx_Callback[0])();
}

void INT1_ISR () __attribute__((signal,used));
void INT1_ISR ()
{
	(*INTx_Callback[1])();
}

void INT2_ISR () __attribute__((signal,used));
void INT2_ISR ()
{
	(*INTx_Callback[2])();
}

void INT3_ISR () __attribute__((signal,used));
void INT3_ISR ()
{
	(*INTx_Callback[3])();
}

void INT4_ISR () __attribute__((signal,used));
void INT4_ISR ()
{
	(*INTx_Callback[4])();
}

void INT5_ISR () __attribute__((signal,used));
void INT5_ISR ()
{
	(*INTx_Callback[5])();
}

void INT6_ISR () __attribute__((signal,used));
void INT6_ISR ()
{
	(*INTx_Callback[6])();
}

void INT7_ISR () __attribute__((signal,used));
void INT7_ISR ()
{
	(*INTx_Callback[7])();
}

void PCI0_ISR () __attribute__((signal,used));
void PCI0_ISR ()
{
	(*PCIx_Callback[0])();
}

void PCI1_ISR () __attribute__((signal,used));
void PCI1_ISR ()
{
	(*PCIx_Callback[1])();
}

void PCI2_ISR () __attribute__((signal,used));
void PCI2_ISR ()
{
	(*PCIx_Callback[2])();
}
