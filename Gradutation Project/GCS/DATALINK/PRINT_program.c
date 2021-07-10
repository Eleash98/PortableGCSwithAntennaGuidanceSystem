#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "AX25_interface.h"


#include "PRINT_interface.h"

void PrintFrameType(frame_t* F){

    char buff[50];
    if(F->FrameType == AX25_S_FRAME){
        strcpy(buff,"S Frame : ");
        if(F->ControlFieldType == Reject)
            strcat(buff,"Reject Type\n");
        else if (F->ControlFieldType == SelectiveReject)
            strcat(buff,"Selective Reject Type\n");
        else if (F->ControlFieldType == ReceiveReady)
            strcat(buff,"Receive Ready Type\n");
        else if (F->ControlFieldType == ReceiveNotReady)
            strcat(buff,"Receive Not Ready Type\n");
        else
            strcat(buff, "Unkonwn\n");
    }
    else if(F->FrameType == AX25_I_FRAME){
        strcpy(buff,"I Frame\n");
        }
    else if (F->FrameType == AX25_U_FRAME){
        strcpy(buff,"U Frame : ");
        if(F->ControlFieldType == Set_Async_Balanced_mode_Extended)
            strcat(buff,"SABME\n");
        else if(F->ControlFieldType == Set_Async_Balanced_mode)
            strcat(buff,"SABM\n");
        else if(F->ControlFieldType == Disconnect)
            strcat(buff,"DISC\n");
        else if(F->ControlFieldType == Disconnect_Mode)
            strcat(buff,"DM\n");
        else if(F->ControlFieldType == Unnumbered_Acknowledge)
            strcat(buff,"UA\n");
        else if (F->ControlFieldType == Unnumbered_Information)
            strcat(buff,"UI\n");
        else if (F->ControlFieldType == Exchange_Identification)
            strcat(buff,"XID\n");
        else if(F->ControlFieldType == Test)
            strcat(buff,"TEST\n");
        else
            strcat(buff,"Unkown\n");
    }
    else
        strcpy(buff,"Unkown Frame");
    printf("%s",buff);

}

void PrintNumberBIN(u32 Copy_u32Number, u8 Bits)
{
	s8 BitCounter;
	printf("0b");
	for(BitCounter=8*Bits-1;BitCounter>=0;BitCounter--)
		printf("%lu",GET_BIT(Copy_u32Number,BitCounter));
	printf(" ");
}


void PrintFrameHEX(frame_t *F)
{
	u32 i;
	printf("Printing Frame in HEX....\n");
	for (i=0 ; i < F->FrameLength ; i++)
	{
		printf("0x%X",(F->PtrtoFrame[i]>>4) & 0xF);
		printf("%X ",(F->PtrtoFrame[i]) & 0xF);
	}
	printf("\n");
	printf("Destination Address Bytes: ");
	for(i=1;i<8;i++)
	{
		printf("0x%X",(F->PtrtoFrame[i]>>4) & 0xF);
		printf("%X ",(F->PtrtoFrame[i]) & 0xF);
	}
	printf("\n");
	printf("Source Address Bytes: 	   ");
	for(i=8;i<15;i++)
	{
		printf("0x%X",(F->PtrtoFrame[i]>>4) & 0xF);
		printf("%X ",(F->PtrtoFrame[i]) & 0xF);
	}
	printf("\n");
	if(AX25_u8ControlModulo() == AX25_8BITS)
	{
		printf("Control Byte: 0x%X",F->PtrtoFrame[15]);
	}
	else
	{
		printf("Control Bytes: 0x%X 0x%x",F->PtrtoFrame[15],F->PtrtoFrame[16]);
	}
	printf("\n");
	printf("Frame Length: %lu\n",F->FrameLength);
}


void PrintFrameBIN(frame_t *F)
{
	printf("Printing Frame in Binary:\n");
	u32 ByteCounter = 0;
	for(ByteCounter = 0; ByteCounter< F->FrameLength ; ByteCounter++ )
	{
		PrintNumberBIN(F->PtrtoFrame[ByteCounter],_8_);
	}
	printf("\n");
	printf("Destination Address Bytes: ");
	for(ByteCounter=1;ByteCounter<8;ByteCounter++)
	{
		PrintNumberBIN(F->PtrtoFrame[ByteCounter],_8_);
	}
	printf("\n");
	printf("Source Address Bytes: ");
	for(ByteCounter=8;ByteCounter<15;ByteCounter++)
	{
		PrintNumberBIN(F->PtrtoFrame[ByteCounter],_8_);
	}
	printf("\n");
	if(AX25_u8ControlModulo() == AX25_8BITS)
	{
		PrintNumberBIN(F->PtrtoFrame[15],_8_);
	}
	else
	{
		PrintNumberBIN(F->PtrtoFrame[15],_8_);
		PrintNumberBIN(F->PtrtoFrame[16],_8_);
	}
}

void AX25_ControlTest(void)
{
	//u8 i = 0;
	enum SFrameControlFieldType i=ReceiveReady;
	printf("S Frame Control Field P/F=0, 8 BIT: \n");
	for(i=0;i<4;i++)
	{
		PrintNumberBIN(AX25_u16ConstructControlField(AX25_S_FRAME, i, 0),_8_);
		printf("\n");
	}
	printf("S Frame Control Field P/F=1, 8 BIT: \n");
	for(i=0;i<4;i++)
	{
		PrintNumberBIN(AX25_u16ConstructControlField(AX25_S_FRAME, i, 1),_8_);
		printf("\n");
	}
	printf("S Frame Control Field P/F=0, 16 BIT: \n");
	for(i=0;i<4;i++)
	{
		PrintNumberBIN(AX25_u16ConstructControlField(AX25_S_FRAME, i, 0),_16_);
		printf("\n");
	}
	printf("S Frame Control Field P/F=1, 16 BIT: \n");
	for(i=0;i<4;i++)
	{
		PrintNumberBIN(AX25_u16ConstructControlField(AX25_S_FRAME, i, 1),_16_);
		printf("\n");
	}
	enum UFrameControlFieldType U=0;

	printf("*****************************************************\n");
	printf("U Frame Control Field P/F=0, 8 BIT: \n");
	for(U=0;U<9;U++)
	{
		PrintNumberBIN(AX25_u16ConstructControlField(AX25_U_FRAME, U, 0),_8_);
		printf("\n");
	}
	printf("U Frame Control Field P/F=1, 8 BIT: \n");
	for(U=0;U<9;U++)
	{
		PrintNumberBIN(AX25_u16ConstructControlField(AX25_U_FRAME, U, 1),_8_);
		printf("\n");
	}
	printf("*******************************************************\n");
	printf("I Frame Control Field P/F = 0, 8 BIT: \n");
	PrintNumberBIN(AX25_u16ConstructControlField(AX25_I_FRAME, AX25_NONE, 0), _8_);
	printf("\n");

	printf("I Frame Control Field P/F = 1, 8 BIT: \n");
	PrintNumberBIN(AX25_u16ConstructControlField(AX25_I_FRAME, AX25_NONE, 1 ), _8_);
	printf("\n");

	printf("I Frame Control Field P/F = 0, 16 BIT: \n");
	PrintNumberBIN(AX25_u16ConstructControlField(AX25_I_FRAME, AX25_NONE, 0), _16_);
	printf("\n");

	printf("I Frame Control Field P/F = 1, 16 BIT: \n");
	PrintNumberBIN(AX25_u16ConstructControlField(AX25_I_FRAME, AX25_NONE, 1), _16_);
	printf("\n");
}

void PrintState(u8 MachineLayer,u8 State)
{
    if(MachineLayer == SIMPLEX_PHYSICAL)
    {
        switch(State)
        {
            case 0:
                printf("Next State: Ready\n");
                break;
            case 1:
                printf("Next State: Receiving\n");
                break;
            case 2:
                printf("Next State: Transmitter Suppression\n");
                break;
            case 3:
                printf("Next State: Transmitter Start\n");
                break;
            case 4:
                printf("Next State: Transmitting\n");
                break;
            case 5:
                printf("Next State: Digipeating\n");
                break;
            case 6:
                printf("Next State: Receiver Start\n");
                break;


        }
    }
    else if(MachineLayer == DUPLEX_PHYSICAL)
    {
        switch(State)
        {
            case 0:
                printf("Next State: Receiver Ready\n");
                break;
            case 1:
                printf("Next State: Receiving\n");
                break;
            case 2:
                printf("Next State: Transmitter Ready\n");
                break;
            case 3:
                printf("Next State: Transmitter Start\n");
                break;
            case 4:
                printf("Next State: Transmitting\n");
                break;
            default:
                break;
        }
    }
    else if (MachineLayer == DATA_LINK)
    {
        switch(State)
        {
            case 0:
                printf("State: Disconnected\n");
                break;
            case 1:
                printf("State: Awaiting Connection\n");
                break;
            case 2:
                printf("State: Awaiting Release\n");
                break;
            case 3:
                printf("State: Connected\n");
                break;
            case 4:
                printf("State: Timer Recovery\n");
                break;
            case 5:
                printf("State: Awaiting Connection 2.2\n");
                break;
            default:
                break;
        }
    }
	printf("*****************************************************\n");
}

void PrintTimer(u8 TimerID)
{
    switch(TimerID)
	{
	    case 0:
			printf("T1\n");
			break;
        case 1:
			printf("T2\n");
			break;
        case 2:
			printf("T3\n");
			break;
		case 3:
			printf("T100\n");
			break;
		case 4:
			printf("T101\n");
			break;
		case 5:
			printf("T102\n");
			break;
		case 6:
			printf("T103\n");
			break;
		case 7:
			printf("T104\n");
			break;
		case 8:
			printf("T105\n");
			break;
		case 9:
			printf("T106\n");
			break;
        case 10:
            printf("T107\n");
            break;
        case 11:
            printf("T108\n");
            break;
        case 12:
            printf("TR210\n");
            break;
        default:
            break;
	}
}
