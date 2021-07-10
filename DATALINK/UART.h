#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

int UART_voidInit(char *PortAddress, u32 BaudRate);
void UART_voidTransmit (int SerialID,char* tx_string);
void UART_voidWriteArray(int SerialID,unsigned char* Arr, unsigned long int len);
u8* UART_voidRead(int SerialID);
#endif // UART_H_INCLUDED
