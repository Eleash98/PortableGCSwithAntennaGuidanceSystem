/*
 * DIO_PROG.c
 *
 * Created: 11/3/2020 6:15:43 AM
 *  Author: ahmed
 */ 
#include "BIT_MATH.h"
#include "std-types.h"
#include "DIO_INT.h"
#include "DIO_PRIV.h"

void IO_voidPinMode  (IOPortType Px, IOPinType pin, IOPinModeType mode)
{
	u8 *addr=(u8 *)(0x21+Px*3+(Px>=PH?0xCB:0));
	if (mode==INPUT)
		BIT_CLR(*addr,pin);
	else if (mode==OUTPUT)
		BIT_SET(*addr,pin);
}

void IO_voidSetPin   (IOPortType Px, IOPinType pin)
{
	u8 *addr=(u8 *)(0x22+Px*3+(Px>=PH?0xCB:0));
	BIT_SET(*addr,pin);
}

void IO_voidClearPin (IOPortType Px, IOPinType pin)
{
	u8 *addr=(u8 *)(0x22+Px*3+(Px>=PH?0xCB:0));
	BIT_CLR(*addr,pin);
}

void IO_voidWritePort(IOPortType Px, u8 value)
{
	u8 *addr=(u8 *)(0x22+Px*3+(Px>=PH?0xCB:0));
	*addr=value;
}

u8   IO_u8ReadPin	 (IOPortType Px, IOPinType pin)
{
	u8 *addr=(u8 *)(0x20+Px*3+(Px>=PH?0xCB:0));
	return BIT_GET(*addr,pin);
}

u8   IO_u8ReadPort	 (IOPortType Px)
{
	u8 *addr=(u8 *)(0x20+Px*3+(Px>=PH?0xCB:0));
	return *addr;
}

void IO_voidTglPin (IOPortType Px, IOPinType pin)
{
		u8 *addr=(u8 *)(0x22+Px*3+(Px>=PH?0xCB:0));
		BIT_TGL(*addr,pin);
}

void IO_voidWritePin (IOPortType Px, IOPinType pin, u8 value)
{
		u8 *addr=(u8 *)(0x22+Px*3+(Px>=PH?0xCB:0));
		*addr=(*addr&~(1<<pin))|((value&1)<<pin);	
}
