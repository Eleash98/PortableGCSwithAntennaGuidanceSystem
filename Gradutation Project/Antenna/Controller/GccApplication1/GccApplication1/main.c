/*
 * GccApplication1.c
 *
 * Created: 11/3/2020 5:00:19 AM
 * Author : ahmed
 */ 

#include "std-types.h"
#include "PWMD.h"
#include "Control.h"
#include "USART_INT.h"
#include "EasyComm.h"
#include "Encoder_interface.h"
#include "UART_interface.h"
#include "TIM_INT.h"
#include <stdio.h>

#define F_CPU	16000000UL

#include <util/delay.h>


int main(void)
{
    /* Replace with your application code */
	USART_voidConfig();
	Encoder_t EA={1,0x5,ENCODER_BAUD_9600,ENCODER_MODE_SINGLE_TURN,ENCODER_CONVERSION_CONT,65535,ENCODER_DIRECTION_CLOCKWISE,{0,0,0,0}},
			  EE={2,0x5,ENCODER_BAUD_9600,ENCODER_MODE_SINGLE_TURN,ENCODER_CONVERSION_CONT,65535,ENCODER_DIRECTION_CLOCKWISE,{0,0,0,0}};
	//MUART_voidInit(UART0,9600,UART_DATA_BITS_8,UART_STOP_BITS_1,UART_PARITY_DISABLED);
	//HEncoder_voidInit(&EA);
	HEncoder_voidInit(&EE);
	TIM_voidTIMConfig();
	/*
	double p_g[]={1,1};
	double i_g[]={0,0};
	double d_g[]={0,0};
	double init_y[]={0,0};
	double init_r[]={0,0};
	Control_voidPIDConfig(2,p_g,i_g,d_g,init_y,init_r,.2);
	*/
	PWMD_voidInit();
	//Control_voidStart();
	PWMD_voidRun(MOTOR_AZ);
	PWMD_voidRun(MOTOR_EL);
	PWMD_voidSetAmp(MOTOR_AZ,100);
	PWMD_voidSetAmp(MOTOR_EL,100);
	u8 cmd[100];
	u8 resp[20];
	Reading_t R;
	while(1)
	{
		
		
		u8 k=0;
		while((cmd[k++]=USART_u8Receive(USART0)));
		EasyComm_voidParseCmd(cmd,resp);
		u8 i=0;
		while(resp[i])
			USART_voidTransfer(USART0,resp[i++]);
		USART_voidTransfer(USART0,'\0');
		
		
	}
}
