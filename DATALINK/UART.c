#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "AX25_interface.h"

#include "DL_interface.h"
#include "DL_flags.h"

#include <wiringPi.h>
#include <wiringSerial.h>
#include "PRINT_interface.h"
#include "UART.h"


#define MAX_BUFFER_LENGTH       4096

//

int UART_voidInit(char *PortAddress, u32 BaudRate){
    int Serial_Port = -1;
    if ((Serial_Port = serialOpen(PortAddress,BaudRate)) < 0){
        perror("Error opening port\n");
    }

    if (wiringPiSetup() < 0){
        perror("Unable to start Wiring Pi\n");
    }

    return Serial_Port;

}


void UART_voidTransmit (int SerialID,char* tx_string)
{
#if (DEBUGGING == 2)
printf("Transmitting Frame\n");
#endif

	serialPrintf(SerialID,tx_string);
}

void UART_voidWriteArray(int SerialID, unsigned char* Arr, unsigned long int len){
#if (DEBUGGING == 5)
printf("Transmitting Frame\n");
#endif
    //u32 i = 0;
    write(SerialID,Arr,len);
    //while(i<len)
      //  serialPutchar(Serial_Port,Arr[i++]);
}


u8* UART_voidRead(int SerialID){

    u8 buff[MAX_BUFFER_LENGTH];
    u32 buffcount = 0;
    u32 Timeout = 60000000;
    while(!serialDataAvail(SerialID));
    char R = serialGetchar(SerialID);
    if(R != 0xc0)
        return NULL;
    buff[buffcount++] = R;

    while(!serialDataAvail(SerialID) && Timeout--);
    Timeout = 6000000;
    R = serialGetchar(SerialID);
    buff[buffcount++] = R;
    if (R == 0xc0){
        u8* F = (u8*)malloc(sizeof(u8)*buffcount);
        memcpy(F,buff,buffcount);
        buffcount = 0;
        return F;
    }
    if (Timeout == 0){
        buffcount = 0;
        return NULL;
    }
    return NULL;
}


/*
void UART_voidRead(void){
    u8 buff[1024];
    u32 buffcount = 0;
    u8 Start_Flag = 0;
    while(1){
        int rx_len = serialDataAvail(Serial_Port);
        if(rx_len > 0)
        {
            char R = serialGetchar(Serial_Port);
            if(R == 0x7e && Start_Flag == 0){
                buff[buffcount++] = R;
                Start_Flag = 1;
            }
            else if(Start_Flag == 1)
            {

                if(buffcount == 1024)
                    {
                        Start_Flag = 0;
                        buffcount = 0;
                        continue;
                    }
                R = serialGetchar(Serial_Port);
                buff[buffcount++] = R;
                if (R == 0x7e)
                {
                    HardwaretoDL = (frame_t*)malloc(sizeof(frame_t));
                    HardwaretoDL->PtrtoFrame = (u8*)malloc(buffcount);
                    HardwaretoDL->FrameLength = buffcount;
                    memcpy(HardwaretoDL->PtrtoFrame,buff,buffcount);
                    printf("Frame_Received:\n");
                    for (int i=0;i<buffcount;i++){
                        printf("%X",buff[i]>>4);
                        printf("%X ",buff[i]&15);
                    }
                    printf("\n");
                    Hardware.Frame = DL_FLAG_SET;
                    buffcount = 0;
                    Start_Flag = 0;
                }

            }

        }
    }
}
*/

/*
void UART_voidRead(void){
int rx_length = serialDataAvail(Serial_Port);
    if(rx_length){
		// Read up to 255 characters from the port if they are there
		static unsigned char rx_buffer[1024] = {0};
		unsigned char R = serialGetchar(Serial_Port);
		static int bufferCounter = 0;
		static int StartFlagAquired = 0, EndFlagAquired = 0;
		if (rx_length < 0)
		{
			//An error occured (will occur if there are no bytes)
		}
		else
		{
			//Bytes received
            if (R == 0x7E && StartFlagAquired == 0){
            ///////////////////////////
            ******Start Flag Read*********
            ******Aquisition of signal*****
            rx_buffer[bufferCounter++] = R;
            //Hardware.AcquisitionOfSignal = PH_FLAG_SET;
            //SM_voidSimplexPhysicalLayer();
            StartFlagAquired = 1;
            }
            else if(R == 0x7E && EndFlagAquired == 0 && StartFlagAquired == 1)
            {
            *******End Flag Read************
            *************Frame************
                rx_buffer[bufferCounter++] = R;

                frame_t* Frame = (frame_t*)malloc(sizeof(frame_t));
                Frame->PtrtoFrame = (u8*)malloc(bufferCounter);
                Frame->FrameLength = bufferCounter;
                memcpy(Frame->PtrtoFrame,rx_buffer,bufferCounter);
                HardwaretoDL = Frame;
                Hardware.Frame = PH_FLAG_SET;
                *printf("Frame Received\n");
                for(int i=0;i<bufferCounter;i++){
                    printf("%X",Frame->PtrtoFrame[i]>>4);
                    printf("%X ",Frame->PtrtoFrame[i]&15);

                }
                printf("\n");*
                //PrintFrameHEX(Frame);
                //printf("////////////////////////////\n////////////////////////////\n")
                //SM_voidSimplexPhysicalLayer();
                StartFlagAquired = 0;
                bufferCounter = 0;
                //Hardware.LossOfSignal = PH_FLAG_SET;
                //SM_voidSimplexPhysicalLayer();


                ///////////////////////////////////
                ///////////////////////////////////
            }
            else if (StartFlagAquired == 1 && EndFlagAquired == 0)
            {
                if (bufferCounter == 1023){
                    *********Loss of Signal*************
                    bufferCounter = 0;
                    StartFlagAquired = 0;
                    EndFlagAquired = 0;
                    //Hardware.LossOfSignal = PH_FLAG_SET;
                    //SM_voidSimplexPhysicalLayer();

                    return;
                }
                rx_buffer[bufferCounter++] = R;
            }

		}
	}
}
*/
