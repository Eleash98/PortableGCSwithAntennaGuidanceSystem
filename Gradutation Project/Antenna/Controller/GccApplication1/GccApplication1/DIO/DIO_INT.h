#ifndef _DIO_INT_H_
#define _DIO_INT_H_

typedef enum {INPUT,OUTPUT} IOPinModeType;
typedef enum {PA, PB, PC, PD, PE, PF, PG, PH, PJ, PK, PL} IOPortType;
typedef enum {PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7, PIN8} IOPinType;

void IO_voidPinMode  (IOPortType Px, IOPinType pin, IOPinModeType mode);
void IO_voidSetPin   (IOPortType Px, IOPinType pin);
void IO_voidClearPin (IOPortType Px, IOPinType pin);
void IO_voidTglPin (IOPortType Px, IOPinType pin);
void IO_voidWritePin (IOPortType Px, IOPinType pin, u8 value);
void IO_voidWritePort(IOPortType Px, u8 value);
u8   IO_u8ReadPin	 (IOPortType Px, IOPinType pin);
u8   IO_u8ReadPort	 (IOPortType Px);

#endif