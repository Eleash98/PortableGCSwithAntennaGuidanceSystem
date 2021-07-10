/************************************************/
/*	Author		:	Mohamed G.Eleish			*/
/*	Date		:	14 DEC 2020					*/
/*	Version		:	V01							*/
/************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "AX25_interface.h"
#include "AX25_private.h"

#include "CRC_interface.h"

#include "PRINT_interface.h"


void AX25_voidFreeFrame(frame_t* Frame)
{
  free(Frame->PtrtoFrame);
  free(Frame);
}


u8 AX25_u8ControlModulo(void)
{
	return AX25_CONTROL_MODULO_TYPE;
}

/*A function that reads a frame and returns it*/


void AX25_voidGetFrame(frame_t* Frame)
{

	/* (1) Reset the frame counter*/
//	LastFrameLength = 0;

	/*create an array using malloc to hold the received frame*/
	u8 *AX25_u8PtrRecievedFrame = (u8 *)(malloc(1));

	/*Check if there is data available*/


	/*if true {*/
		/* () Read Byte from the interface*/

		/* () Store the byte in the created frame*/
//				AX25_u8PtrRecievedFrame[LastFrameLength] = //readByte
		/* () Increment the frame byte counter*/
//		LastFrameLength++;

		/*create new space for the new data using realloc*/
//		AX25_u8PtrRecievedFrame = realloc(AX25_u8PtrRecievedFrame,LastFrameLength+1);
		/* () Go to () }*/

	/*if False and frame counter is zero {return 0}*/

	/*if False and frame counter is not zero*/
	Frame->PtrtoFrame = AX25_u8PtrRecievedFrame;
//	Frame->FrameLength = LastFrameLength - 1;
}


/*A function that returns the Length of the last received frame*/
u32 AX25_u32GetFrameLength(frame_t *Frame)
{
    return Frame -> FrameLength;
}

/*Sets the Modulo type for either 8 bits or 16 bit*/
void AX25_voidSetControlModulo(u8 ControlModulo)
{
	AX25_CONTROL_MODULO_TYPE = ControlModulo;
}


/*A function to do the BIT stuffing upon Reception of a frame*/
void AX25_voidStuff(frame_t* Frame)
{
  Frame->StuffBits=0;
  Frame->DeStuffBits=0;
    /*Bit Counter*/
	u32 Local_u32BitCounter = 0;

	u8 OnesCounter = 0;

	while(Local_u32BitCounter < (Frame -> FrameLength) *8 -1)
	{
		if(GET_BIT((Frame->PtrtoFrame[Local_u32BitCounter/8]),(7-Local_u32BitCounter%8)) == 1)
			OnesCounter++;
        else
            OnesCounter = 0;

		if (OnesCounter == 5)
		{
            /*if 5 consicutive ones are detected, insert a zero after them*/
			AX25_voidInsertZero(Frame, Local_u32BitCounter+1);
			/*Reset OnesCounter*/
			OnesCounter = 0;
		}

		Local_u32BitCounter++;
	}
}

/*A function to do the BIT De_Stuffing upon Reception of a frame*/
void AX25_voidDestuff(frame_t* Frame)
{
    /*Counts the bits of the frame*/
	u32 Local_u32BitCounter = 0;

    /*Counts consecutive ones in a frame*/
	u8 OnesCounter = 0;
    Frame->DeStuffBits = 0;
	while(Local_u32BitCounter < (Frame -> FrameLength) *8-1)
	{
		if(GET_BIT((Frame->PtrtoFrame[Local_u32BitCounter/8]),(7-Local_u32BitCounter%8)) == 1)
			OnesCounter++;
        else
            OnesCounter = 0;

		if (OnesCounter == 5)
		{
			/*Shift all Bits to the left by one*/
            AX25_voidShiftArray(AX25_SHIFTLEFT, Frame, Local_u32BitCounter+1);

            /*Increase DeStuffBits indicating that a destuffing operation has done*/
            Frame -> DeStuffBits++;

            /*reset OnesCounter*/
			OnesCounter = 0;
		}

		Local_u32BitCounter++;
	}
    if (Frame->DeStuffBits == 0)
    {
        return;
    }
    else if(Frame->DeStuffBits %8 != 0 )
    {
        Frame->PtrtoFrame = realloc(Frame->PtrtoFrame,Frame->FrameLength - (Frame->DeStuffBits/8 +1));
        Frame->FrameLength -= Frame->DeStuffBits/8 +1;
    }
    else if (Frame->DeStuffBits%8 == 0 )
    {
        Frame->PtrtoFrame = realloc(Frame->PtrtoFrame,Frame->FrameLength - (Frame->DeStuffBits/8));
        Frame->FrameLength -= Frame->DeStuffBits/8;
    }
}

void AX25_voidShiftArray(u8 Copy_u8Direction, frame_t *Frame, u32 Copy_u32Offset)
{
	if (Copy_u8Direction == AX25_SHIFTRIGHT)
	{
		u16 Local_u16Counter = 0;

		u8 i;
		/*Starting from the last byte, shift the current byte right by 1, then copy the 0's bit
		from the previous byte to the 7's bit in the current byte, do this for all bytes with
		index less than the index of the offset*/
		for (Local_u16Counter=Frame->FrameLength-1; (Local_u16Counter)*8 > Copy_u32Offset; Local_u16Counter--)
		{
			Frame->PtrtoFrame[Local_u16Counter] >>= 1;
			Frame->PtrtoFrame[Local_u16Counter] |= (GET_BIT((Frame->PtrtoFrame[Local_u16Counter-1]), 0) << 7);
		}
		/*starting from the last bit of the offset bit, copy the second to last bit to the last bit,
		do this for all bits finishing with the offset bit*/
		for (i = 7; i > Copy_u32Offset%8 ;i--)
		{
			CLR_BIT((Frame->PtrtoFrame[Local_u16Counter]),(7-i));
			Frame->PtrtoFrame[Local_u16Counter] |= (GET_BIT((Frame->PtrtoFrame[Local_u16Counter]), (8-i)) << (7-i)) ;
		}

	}
	else if (Copy_u8Direction == AX25_SHIFTLEFT)
	{
		u16 Local_u16Counter = 0;

		u8 i;

		/*Starting with the offset bit, copy the next bet to the current bit, do this for all
        bits greater than the offset bit*/
		for (i= Copy_u32Offset%8; i < 7 ;i++)
		{
		    CLR_BIT((Frame->PtrtoFrame[Copy_u32Offset/8]),(7-i%8));
			Frame->PtrtoFrame[Copy_u32Offset/8] |= GET_BIT((Frame->PtrtoFrame[Copy_u32Offset/8]),(6-i)) << (7-i);

		}
		/*Stasrting from the offset byte, copy the 7's bit of the next byte to the 0's bit
		of the current byte and shift the next byte left by 1*/
		for (Local_u16Counter=Copy_u32Offset/8; (Local_u16Counter) < Frame->FrameLength-1; Local_u16Counter++)
		{
			CLR_BIT(Frame->PtrtoFrame[Local_u16Counter],0);
			Frame->PtrtoFrame[Local_u16Counter] |= GET_BIT(Frame->PtrtoFrame[Local_u16Counter+1],7);
			Frame->PtrtoFrame[Local_u16Counter+1] = (Frame->PtrtoFrame[Local_u16Counter+1]<<1) & 0xFF;
		}
	}
}

/*A function that inserts a 0 at the position Offset Bits in a frame created By malloc*/
void AX25_voidInsertZero(frame_t* Frame, u32 Offset)
{
	/*Check if StuffBits has a number divisible by 8*/
	if(Frame-> StuffBits % 8 == 0)
	{
		/*reallocate by one byte, increment the length and the StuffBits*/
		Frame -> PtrtoFrame =(u8*) realloc(Frame -> PtrtoFrame, (Frame -> FrameLength+1)*sizeof(u8));
		Frame -> PtrtoFrame[Frame->FrameLength] = 0;
		Frame -> FrameLength++;
	}

	/*Shift all Bits to the right by one and insert a zero at the offset location*/
	AX25_voidShiftArray(AX25_SHIFTRIGHT, Frame, Offset);

	/*Insert Zero at the Offset*/
	CLR_BIT((Frame->PtrtoFrame[Offset/8]),(7-(Offset%8)));

	(Frame -> StuffBits)++;
}


/*Constructs control field for a specific frame type*/
u16 AX25_u16ConstructControlField(u8 Copy_u8FrameType, u8 Copy_u8ControlFieldType, u8 Copy_u8PollFinal)
{
	switch(Copy_u8FrameType)
	{
		case AX25_S_FRAME:
		{
			if (AX25_CONTROL_MODULO_TYPE == AX25_8BITS)
			{
				u8 Local_u8ControlField = 0;
				Local_u8ControlField = (AX25_u8GetSequenceNumber(AX25_NR)<<5)
										| (Copy_u8PollFinal<<4)
										|(Copy_u8ControlFieldType<<2) | 1;
				return Local_u8ControlField;
			}
			else if (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)
			{
				u16 Local_u16ControlField = 0;
				Local_u16ControlField = (AX25_u8GetSequenceNumber(AX25_NR)<<9)
										| (Copy_u8PollFinal<<8)
										|(Copy_u8ControlFieldType<<2) | 1;
				return Local_u16ControlField;
			}
			else
			{
				return 0;
			}

		}
		case AX25_U_FRAME:
		{
			if (AX25_CONTROL_MODULO_TYPE == AX25_8BITS)
			{
                u8 Local_u8ControlField = 0;
                Local_u8ControlField =  Copy_u8ControlFieldType | (Copy_u8PollFinal<<4);
                return Local_u8ControlField;
			}
			else
			{
				return 0;
			}
		}
		case AX25_I_FRAME:
		{
			if (AX25_CONTROL_MODULO_TYPE == AX25_8BITS)
			{
				u8 Local_u8ControlField = 0;
				Local_u8ControlField = (AX25_u8GetSequenceNumber(AX25_NR) << 5)
										| (Copy_u8PollFinal<<4)
										| (AX25_u8GetSequenceNumber(AX25_NS) <<1);
				return Local_u8ControlField;
			}
			else if (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)
			{
				u16 Local_u16ControlField = 0;
				Local_u16ControlField = (AX25_u8GetSequenceNumber(AX25_NR) << 9)
										| (Copy_u8PollFinal<<8)
										| (AX25_u8GetSequenceNumber(AX25_NS) <<1);
				return Local_u16ControlField;
			}
			else
			{
				return 0;
			}
		}
	    default:
	    {
			return 0;
	    }
	}
}





/*A function that returns the requested sequence number*/
u8 AX25_u8GetSequenceNumber(u8 Copy_u8SequenceNumber)
{
	switch(Copy_u8SequenceNumber)
	{
		case AX25_NR:
		{
			return NR;
		}

		case AX25_NS:
		{
			return NS;
		}

		default:
		{
			return 255;
		}

	}

}



void AX25_voidConstructAddress (address_t* Address , u8* ConstructedAddress)
{
    if (Address->RepeaterUsed == AX25_REPEATER)
    {
		/*if A repeater is used, Copy its address to its place in the constructed address array*/
    	u8 i;
    	for(i=1;i <= Address->NOfRepeaters;i++)
    	{
    	memcpy(ConstructedAddress+i*14 , Address->Repeater[i-1] ,6);
		/*Shift in the "has been repeated" bit, the reserved bits and ssid to their prespective loactions in the 7th byte of the address*/
        ConstructedAddress[13 + 7*i] = (RESERVED<<5) | (Address->RepeaterSSID[i-1]<<1);

    	}
    	SET_BIT(ConstructedAddress[13+7*(i-1)],0);
    }

    else if (Address->RepeaterUsed == AX25_NO_REPEATER)
    {
       /*if no repeater, Set the 0 bit in the Source address to indicate that this is the last byte in the address field*/
        ConstructedAddress[13] = 1;
    }
	/*Copy the Destination and Source addresses to their perspective locations in the constructed address array*/
    memcpy (ConstructedAddress , Address->Destination, 6);
    memcpy (ConstructedAddress+7 , Address->Source , 6);
    u8 i;
	/*Shift all address bytes left by 1*/
	for (i=0;i<6;i++)
	{
		ConstructedAddress[i]<<= 1;
		ConstructedAddress[7+i]<<=1;
		if (Address->RepeaterUsed == AX25_REPEATER)
		  ConstructedAddress[14+i]<<=1;
	}
	/*Shift in the "Command/Response bit, Reserved bits and the SSID for the destination and source addresses*/
    ConstructedAddress[6] = (Address->DestCommandResponse<<7) | (RESERVED<<5) | (Address->DestinationSSID<<1);
    ConstructedAddress[13] |= (Address->SrcCommandResponse<<7) | (RESERVED<<5) | (Address->SourceSSID<<1);
}

void AX25_voidConstructFrame (frame_t* Frame)
{
	/*To point at the current byte number in the frame*/
    u32 FrameIndex =0;
    u8 AddressLength= 14;
    if (Frame->FrameAddress.RepeaterUsed == AX25_REPEATER)
    	AddressLength += 7*Frame->FrameAddress.NOfRepeaters;
    /*Allocate memory to the address field*/
    u8 *ConstructedAddress = (u8*)malloc(AddressLength);
    u8 IFrame=0 , Control=0;
    /*If frame type is I-frame, Set IFrame to 1 for the PID field*/
    if (Frame->FrameType == AX25_I_FRAME){
        IFrame++;
        Frame->NR = NR;
        Frame->NS = NS;
        }
    if (Frame->FrameType == AX25_S_FRAME)
        Frame->NR = NR;
    /*If the Control Modulo is 16 bits, set Control to 1*/
    if (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)
        Control++;
    /*IFrame and Control are used to get the frame length*/
    Frame->FrameLength = AddressLength + Frame->InfoLength + IFrame + Control +3 ;
    /*Allocate memory to the frame*/
    Frame->PtrtoFrame = (u8*)malloc(Frame->FrameLength);

	if(AX25_VERSION	== AX25_VERSION_2_0)
	{
		if(Frame->CMDRES == AX25_COMMAND_FRAME)
		{
			Frame->FrameAddress.DestCommandResponse = 0;
			Frame->FrameAddress.SrcCommandResponse = 0;
		}
		else
		{
			Frame->FrameAddress.DestCommandResponse = 1;
			Frame->FrameAddress.SrcCommandResponse = 1;
		}

	}
	else
	{
		if(Frame->CMDRES == AX25_COMMAND_FRAME)
		{
			Frame->FrameAddress.DestCommandResponse = 1;
			Frame->FrameAddress.SrcCommandResponse = 0;
		}
		else if(Frame->CMDRES == AX25_RESPONSE_FRAME)
		{
			Frame->FrameAddress.DestCommandResponse = 0;
			Frame->FrameAddress.SrcCommandResponse = 1;
		}
		else
		{
			////Errorrrrr!!!!!!!!!!!!!
		}
	}

    /*Construct the Address Field*/
    AX25_voidConstructAddress (&(Frame->FrameAddress) , ConstructedAddress);
    /*Skip the start flag*/
    //    FrameIndex += 1;
    /*Copy the constructed address field to the frame*/
    memcpy(Frame->PtrtoFrame+FrameIndex , ConstructedAddress, AddressLength);
    /*free the Allocated memory for the address field*/
    free(ConstructedAddress);
    FrameIndex += AddressLength;
    /*Construct the Control Field*/
    Frame->ControlField = AX25_u16ConstructControlField(Frame->FrameType,Frame->ControlFieldType,Frame->PollFinal);
    /*Add in the Control Field to the frame*/
    if(AX25_CONTROL_MODULO_TYPE == AX25_16BITS)
    {
        Frame->PtrtoFrame[FrameIndex++] = Frame->ControlField>>8;
        Frame->PtrtoFrame[FrameIndex++] = Frame->ControlField& 0XFF;
    }
    else
        Frame->PtrtoFrame[FrameIndex++] = Frame->ControlField& 0xFF;

    /*if the frame is I-frame, Add the PID field*/
    if(Frame->FrameType == AX25_I_FRAME)
        Frame->PtrtoFrame[FrameIndex++]= Frame->PID;
    /*Copy the Info to the frame*/
    memcpy(Frame->PtrtoFrame+FrameIndex , Frame->Info , Frame->InfoLength);
    FrameIndex +=Frame->InfoLength;
    /*Generate the CRC starting from the first byte in the Address field to the last byte of the Info field*/
	u16 CRC = CRC16_u16CCITTFalse(Frame->PtrtoFrame, Frame->FrameLength-2);
	/*Add in the CRC code to the frame*/
	Frame->PtrtoFrame[FrameIndex++] = (CRC>>8) & 0xFF;
	Frame->PtrtoFrame[FrameIndex++] = (CRC) & 0xFF;
/*
	if(Frame->FrameType == AX25_I_FRAME){
        printf("Frame before stuffing:\n");
        for(int i = 0;i < Frame->FrameLength;i++){
            printf("%X",Frame->PtrtoFrame[i]>>4);
            printf("%X ",Frame->PtrtoFrame[i]&15);
        }
        printf("\n");

	}
*/
	/*Apply Bit Stuffing on the Frame*/
	AX25_voidStuff(Frame);
	/*Add in the Flags*/
	Frame->PtrtoFrame = (u8*)realloc(Frame->PtrtoFrame,(Frame->FrameLength+2));
	u8* TempArr = (u8*)malloc(sizeof(u8)*(Frame->FrameLength));
	memcpy(TempArr,Frame->PtrtoFrame,Frame->FrameLength);
	//memcpy(Frame->PtrtoFrame+1,Frame->PtrtoFrame,Frame->FrameLength);
	memcpy(Frame->PtrtoFrame+1,TempArr,Frame->FrameLength);
	free(TempArr);
	Frame->FrameLength +=2;
	Frame->PtrtoFrame[0] = AX25_FLAG;
	Frame->PtrtoFrame[Frame->FrameLength-1] = AX25_FLAG;
}

u8 AX25_u8DeConstructFrame (frame_t* Frame)
{
//    /*Counter begins from second byte in frame "Without Start Flag" */
//    u32 FrameCounter=1;
//
//    /*Check if Start Flag found at frame in first byte */
//    if (OldArray[0]!= AX25_FLAG)
//        return AX25_START_FLAG_NOT_FOUND;
//
//    /*Count the frame Length*/
//    while (OldArray[FrameCounter++]!= AX25_FLAG);
//
//    /*Put the frame counter value in frame length then create array by malloc using frame length as size*/
//    FrameCounter-=2;
//    Frame->FrameLength = FrameCounter;
//    Frame->PtrtoFrame = (u8*)malloc(Frame->FrameLength);
//
//    /*Copy frame from old array to frame structure the destroy this array*/
//    memcpy(Frame->PtrtoFrame,OldArray+1,Frame->FrameLength);
//    free(OldArray);
/*
    printf("Frame RAW:\n");
  for(int i = 0;i < Frame->FrameLength;i++)
  {
    printf("%X",Frame->PtrtoFrame[i]>>4);
    printf("%X ",Frame->PtrtoFrame[i]&15);
  }
  printf("\n");*/
  u8 *TempArr = (u8*)malloc(sizeof(u8)*(Frame->FrameLength-2));
  memcpy(TempArr,Frame->PtrtoFrame+1,Frame->FrameLength-2);
  memcpy(Frame->PtrtoFrame,TempArr,Frame->FrameLength-2);
  free(TempArr);
  Frame->PtrtoFrame = (u8*)realloc(Frame->PtrtoFrame,Frame->FrameLength-2);
  Frame->FrameLength -= 2;

  /*
  printf("Frame after remocing flags:\n");
  for(int i = 0;i < Frame->FrameLength;i++)
  {
    printf("%X",Frame->PtrtoFrame[i]>>4);
    printf("%X ",Frame->PtrtoFrame[i]&15);
  }
  printf("\n");
    */

/*Apply Bit DeStuffing on the Frame*/
    AX25_voidDestuff(Frame);
/*
    printf("Frame after destuffing:\n");
  for(int i = 0;i < Frame->FrameLength;i++)
  {
    printf("%X",Frame->PtrtoFrame[i]>>4);
    printf("%X ",Frame->PtrtoFrame[i]&15);
  }
  printf("\n");
  */
    /*Generate the CRC on the frame without Start and End Flags*/
    u16 NewCRC = CRC16_u16CCITTFalse(Frame->PtrtoFrame, Frame->FrameLength);

    /*Check if CRC is equal zero or no */
    /*if equal zero that mean correct Frame*/
    /*if not that mean Wrong Frame*/
    if(NewCRC == 0x00)
    {
        /*Correct Frame*/
        /*This variable to count from the frame beginning*/
        u32 FromStartFrame=0;
        /*Copy Destination to frame address destination*/
        memcpy(Frame->FrameAddress.Destination , Frame->PtrtoFrame+FromStartFrame , 6);
        Frame->FrameAddress.Destination[6] = 0;
        FromStartFrame+=6;
        /*Get Destination SSID value*/
        Frame->FrameAddress.DestinationSSID = ((Frame->PtrtoFrame[FromStartFrame]>>1) &0xF);
        /*Get Destination Command and Response bit*/
        Frame->FrameAddress.DestCommandResponse = GET_BIT(Frame->PtrtoFrame[FromStartFrame++],7);
        /*Copy Source to frame address source*/
        memcpy(Frame->FrameAddress.Source , Frame->PtrtoFrame+FromStartFrame ,6);
        Frame->FrameAddress.Source[6] = 0;
        FromStartFrame+=6;
        /*Get Source SSID value*/
        Frame->FrameAddress.SourceSSID = ((Frame->PtrtoFrame[FromStartFrame]>>1) &0xF);
        /*Get Source Command and Response bit*/
        Frame->FrameAddress.SrcCommandResponse = GET_BIT(Frame->PtrtoFrame[FromStartFrame],7);
        /*Check if there is Repeater or no ?!*/
        u8 i=0;

        if((Frame->FrameAddress.DestCommandResponse == 0 && Frame->FrameAddress.SrcCommandResponse == 0)
        		||(Frame->FrameAddress.DestCommandResponse == 1 && Frame->FrameAddress.SrcCommandResponse == 0))
        {
        	Frame->CMDRES = AX25_COMMAND_FRAME;
        }
        else if ((Frame->FrameAddress.DestCommandResponse == 1 && Frame->FrameAddress.SrcCommandResponse == 1)
        		||(Frame->FrameAddress.DestCommandResponse == 0 && Frame->FrameAddress.SrcCommandResponse == 1))
        {
        	Frame->CMDRES = AX25_RESPONSE_FRAME;
        }

        if(GET_BIT(Frame->PtrtoFrame[FromStartFrame++],0) ==0)
        {
        	/*if frame has repeater address*/
			Frame->FrameAddress.RepeaterUsed = AX25_REPEATER;
			Frame->FrameAddress.NOfRepeaters = 1;
			while (GET_BIT(Frame->PtrtoFrame[FromStartFrame],0) ==0)
			{
				Frame->FrameAddress.Repeater[i] = (char *)malloc(7);
				/*Copy Repeater to frame address Repeater*/
				memcpy (Frame->FrameAddress.Repeater[i] , Frame->PtrtoFrame+FromStartFrame +7*i , 6);
				FromStartFrame+=6;
				/*Get Repeater SSID value*/
				Frame->FrameAddress.RepeaterSSID[i] = ((Frame->PtrtoFrame[FromStartFrame]>>1) &0xF);
				/*Get Has-Been-Repeated bit*/
				Frame->FrameAddress.HRepeated[i] = GET_BIT(Frame->PtrtoFrame[FromStartFrame++],7);
				i++;
				Frame->FrameAddress.NOfRepeaters++;
			}
		}
        else
        {
            /*if frame does't have repeater address*/
            Frame->FrameAddress.RepeaterUsed = AX25_NO_REPEATER;
        }

        /*Shift left then and with 0x7F for each of Destination , Source and repeater if it found, to back them as original addresses*/
        for(i=0;i<6;i++)
        {
        	Frame->FrameAddress.Destination[i] = (Frame->FrameAddress.Destination[i]>>1)&0x7F;
        	Frame->FrameAddress.Source[i] = (Frame->FrameAddress.Source[i]>>1)&0x7F;
        }
        i=0;
        /*if(Frame->FrameAddress.RepeaterUsed == AX25_REPEATER)
        	while(i<Frame->FrameAddress.NOfRepeaters)
        	{
        		u8 j;
        		for(j=0; j<6;j++)
        			Frame->FrameAddress.Repeater[i][j] = (Frame->FrameAddress.Repeater[i][j]>>1)&0x7F;
        		i++;
			}*/
        /*Check if Control Module is 16Bits or 8Bits ?!*/
        /*if 16Bits then copy two bytes*/
        /*if 8Bits then Copy one byte*/
        if (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)
        {
            /*Copy two byte to ControlField*/
            memcpy (&(Frame->ControlField) , Frame->PtrtoFrame+FromStartFrame ,2);
            FromStartFrame+=2;
        }
        else
        {
            /*Copy one byte to ControlField*/
            Frame->ControlField = Frame->PtrtoFrame[FromStartFrame++];
        }
        /*Get the Frame Type*/
        if (GET_BIT(Frame->ControlField,0) == 0)
        {
        	Frame->FrameType = AX25_I_FRAME;
        	Frame->PollFinal = (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)? GET_BIT(Frame->ControlField,8):GET_BIT(Frame->ControlField,4);
            Frame->NR = (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)? (Frame->ControlField>>9)&0x7f:(Frame->ControlField>>5)&7;
            Frame->NS = (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)? (Frame->ControlField>>1)&0x7f:(Frame->ControlField>>1)&7;

        }
        else
        {
        	if(GET_BIT(Frame->ControlField,1) == 0)
        	{
        		Frame->FrameType = AX25_S_FRAME;
        		Frame->ControlFieldType = (Frame->ControlField>>2) &3;
        		Frame->PollFinal = (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)? GET_BIT(Frame->ControlField,8):GET_BIT(Frame->ControlField,4);
        		Frame->NR = (AX25_CONTROL_MODULO_TYPE == AX25_16BITS)? (Frame->ControlField>>9)&0x7f:(Frame->ControlField>>5)&7;
        		Frame->NS = 255;
        	}
        	else
        	{
        		Frame->FrameType = AX25_U_FRAME;
        		Frame->ControlFieldType = (Frame->ControlField);
        		CLR_BIT(Frame->ControlFieldType,4);
        		Frame->PollFinal = GET_BIT(Frame->ControlField,4);
        		Frame->NR = 255;
        		Frame->NS = 255;
        	}
        }
        /*Check if Frame Type is I_Frame or no ?!*/
        /*if yes, That meaning there is PID in the frame*/
        /*if no, That meaning there is no PID in the frame*/
        if (Frame->FrameType == AX25_I_FRAME)
        {
            /*Copy PID to frame PID*/
            Frame->PID = Frame->PtrtoFrame[FromStartFrame++];
        }
        /*Calculate Info length*/
        Frame->InfoLength = Frame->FrameLength-2 - FromStartFrame;
        /*check if Info length equal 1 or bigger ?! */
        /*if yes, copy Info to frame Info*/
        /*if no, so Info length equal zero that meaning there is no Info in the frame */
        if(Frame->InfoLength>=1)
        {
	  Frame->Info = Frame->PtrtoFrame+FromStartFrame;//(u8*)malloc(Frame->FrameLength-2 - FromStartFrame);
	  //memcpy (Frame->Info , Frame->PtrtoFrame+FromStartFrame ,Frame->InfoLength);
        }
        else
        {
        	Frame->Info = 0;
        	Frame->InfoLength = 0;
        }
        return AX25_FRAME_CORRECT;
    }
    else
    {
        /*Wrong Frame*/
        return AX25_WRONG_CRC;
    }
}

/*Extracts A selected Sequence number from a control field*/
u8 AX25_u8ExtractSequenceNumber(u8 FrameType, u16 FrameControlField,u8 SequenceType)
{
	switch(FrameType)
	{
	case AX25_U_FRAME:
		return 255;
	case AX25_I_FRAME:
		if(AX25_CONTROL_MODULO_TYPE == AX25_8BITS)
		{
			if(SequenceType == AX25_NR)
				return (FrameControlField>>5) & 0x07;
			else
				return (FrameControlField>>1) & 0x07;
		}
		else
		{
			if(SequenceType == AX25_NR)
				return (FrameControlField>>9) & 0x7F;
			else
				return (FrameControlField>>1) & 0x7F;
		}
	case AX25_S_FRAME:
		if(SequenceType == AX25_NS)
			return 255;
		if(AX25_CONTROL_MODULO_TYPE == AX25_8BITS)
			return (FrameControlField>>5) & 0x7;
		else
			return (FrameControlField>>9) & 0x7F;
	default:
		return 255;
	}

}

u8 AX25_u8ExtractPollFinal(u8 FrameType, u16 FrameControlField)
{
	u8 ControlModulo = AX25_CONTROL_MODULO_TYPE;
	if(FrameType ==  AX25_U_FRAME)
		return ControlModulo == AX25_8BITS? GET_BIT(FrameControlField,4):255;

	return ControlModulo == AX25_8BITS? GET_BIT(FrameControlField,4):GET_BIT(FrameControlField,8);
}


u8 AX25_u8ExtractFrameControlType(u8 FrameType, u16 ControlField)
{
	switch(FrameType)
	{
	case AX25_I_FRAME:
		return 0;
	case AX25_S_FRAME:
		return (ControlField>>2) & 0x03;
	case AX25_U_FRAME:
		CLR_BIT(ControlField,4);
		return ControlField;
	default:
		return 255;
	}
}




XIDInfo_t* AX25_XIDInfo_tPtrConstructXIDInfo(u8 TrasmissionMode,u8 RejectType, u8 ControlModulo,u64 MaxTxLength,u64 MaxRxLength,u8 TxWindowSize,u8 RxWindowSize, u32 AckTimer, u32 Retries)
{
	u32 CurrentByte = 4;
	XIDInfo_t* Info = (XIDInfo_t *) malloc(sizeof(XIDInfo_t));
	Info->FormatIndicator = AX25_XID_FORMAT_INDICATOR;
	Info->GroupIdentifier = AX25_XID_GROUP_IDENTIFIER;
	Info->XIDInfoLength = 4;
	Info->GroupLength = 0;
	Info->XIDInfo = (u8*) malloc(Info->XIDInfoLength);
	if (TrasmissionMode != AX25_XID_ABSENT)
	{
		/*Half or full-duplex*/
		Info->XIDInfoLength += 4;
		Info->ClassesOfProcedures = AX25_XID_TRANSMISSION_MODE | (TrasmissionMode<<5);
		Info->XIDInfo = (u8*) realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] = AX25_XID_CLASSES_OF_PROCEDURES_IDENTIFIER;
		Info->XIDInfo[CurrentByte++] = 2;
		Info->XIDInfo[CurrentByte++] = (Info->ClassesOfProcedures>>8) & 0xFF;
		Info->XIDInfo[CurrentByte++] = (Info->ClassesOfProcedures) & 0xFF;
	}
	if(RejectType != (u8)AX25_XID_ABSENT || ControlModulo != (u8)AX25_XID_ABSENT)
	{
		Info->XIDInfoLength += 5;
		Info->HDLCOptionalFunctions = 0x02A080;
		if(RejectType != (u8)AX25_XID_ABSENT)
			Info->HDLCOptionalFunctions |= (RejectType<<1);
		else
			Info->HDLCOptionalFunctions |= (AX25_XID_SELECTIVE_REJECT_REJECT<<1);
		if(ControlModulo != (u8)AX25_XID_ABSENT)
			Info->HDLCOptionalFunctions |= (ControlModulo<<10);
		else
			Info->HDLCOptionalFunctions |= (AX25_XID_8BIT_MODULO);
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] = AX25_XID_HDLC_OPTIONAL_FUNCTIONS_IDENTIFIER;
		Info->XIDInfo[CurrentByte++] = 3;
		Info->XIDInfo[CurrentByte++] = (Info->HDLCOptionalFunctions>>16)&0xFF;
		Info->XIDInfo[CurrentByte++] = (Info->HDLCOptionalFunctions>>8)&0xFF;
		Info->XIDInfo[CurrentByte++] = (Info->HDLCOptionalFunctions)&0xFF;
	}
	if(MaxTxLength != (u64)AX25_XID_ABSENT)
	{
		u8 ParameterLength = AX25_u8GetBytes(MaxTxLength);
		Info->XIDInfoLength += 2+ParameterLength;
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] =  5;
		Info->XIDInfo[CurrentByte++] = ParameterLength;
		while(ParameterLength>0)
		{
			Info->XIDInfo[CurrentByte++] = (MaxTxLength>>(8*(ParameterLength-1))) & 0xFF;
			ParameterLength--;
		}
	}
	if(MaxRxLength != (u64)AX25_XID_ABSENT)
	{
		u8 ParameterLength = AX25_u8GetBytes(MaxRxLength);
		Info->XIDInfoLength += 2+ParameterLength;
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] =  6;
		Info->XIDInfo[CurrentByte++] = ParameterLength;
		while(ParameterLength>0)
		{
			Info->XIDInfo[CurrentByte++] = (MaxRxLength>>(8*(ParameterLength-1))) & 0xFF;
			ParameterLength--;
		}
	}
	if(TxWindowSize != (u8)AX25_XID_ABSENT)
	{
		Info->XIDInfoLength += 3;
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] = 7;
		Info->XIDInfo[CurrentByte++] = 1;
		Info->XIDInfo[CurrentByte++] = TxWindowSize & 0xFF;
	}
	if(RxWindowSize != (u8)AX25_XID_ABSENT)
	{
		Info->XIDInfoLength += 3;
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] = 8;
		Info->XIDInfo[CurrentByte++] = 1;
		Info->XIDInfo[CurrentByte++] = RxWindowSize & 0xFF;
	}
	if(AckTimer != (u64)AX25_XID_ABSENT)
	{
		u8 ParameterLength = AX25_u8GetBytes(AckTimer);
		Info->XIDInfoLength += 2+ParameterLength;
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] = 9;
		Info->XIDInfo[CurrentByte++] = ParameterLength;
		while(ParameterLength>0)
		{
			Info->XIDInfo[CurrentByte++] = (AckTimer>>(8*(ParameterLength-1))) & 0xFF;
			ParameterLength--;
		}
	}
	if(Retries != (u64)AX25_XID_ABSENT)
	{
		u8 ParameterLength = AX25_u8GetBytes(Retries);
		Info->XIDInfoLength += 2+ParameterLength;
		Info->XIDInfo = (u8 *)realloc(Info->XIDInfo,Info->XIDInfoLength);
		Info->XIDInfo[CurrentByte++] = 10;
		Info->XIDInfo[CurrentByte++] = ParameterLength;
		while(ParameterLength>0)
		{
			Info->XIDInfo[CurrentByte++] = (Retries>>(8*(ParameterLength-1))) & 0xFF;
			ParameterLength--;
		}
	}
	Info->GroupLength = Info->XIDInfoLength-4;
	Info->XIDInfo[0] = Info->FormatIndicator;
	Info->XIDInfo[1] = Info->GroupIdentifier;
	Info->XIDInfo[2] = (Info->GroupLength>>8) & 0xFF;
	Info->XIDInfo[3] = (Info->GroupLength) & 0xFF;

	return Info;
}

XIDInfo_t* AX25_XIDInfo_tPtrDeconstructXIDInfo(u8 *Data,u32 InfoLength)
{
	XIDInfo_t *Info = (XIDInfo_t *)malloc(sizeof(XIDInfo_t));
	Info->XIDInfo = (u8*)malloc(InfoLength);
	memcpy(Info->XIDInfo,Data,InfoLength);
	Info->FormatIndicator = Data[0];
	Info->GroupIdentifier = Data[1];
	Info->GroupLength = (Data[2]<<8) | Data[3];
    Info->State = 0;
	u32 CurrentByte = 4;
	//u8 Flag = 0;
	while (CurrentByte <= InfoLength-1)
	{
		switch(Data[CurrentByte])
		{
			case AX25_XID_CLASSES_OF_PROCEDURES_IDENTIFIER:
			{
				Info->ClassesOfProcedures = (Data[CurrentByte+2]<<8) |Data[CurrentByte+3];
				CurrentByte += 4;
				SET_BIT(Info->State,0);
				break;
			}
			case AX25_XID_HDLC_OPTIONAL_FUNCTIONS_IDENTIFIER:
			{
				Info->HDLCOptionalFunctions = (Data[CurrentByte+2]<<16) | (Data[CurrentByte+3]<<8) | Data[CurrentByte+4];
				CurrentByte += 5;
				SET_BIT(Info->State,1);
				break;
			}
			case AX25_XID_I_TX_LENGTH:
			{
				u8 ParameterLength=Data[CurrentByte+1],counter = 2;
				Info->MaxTXIFieldLength = 0;
				while(ParameterLength-- > 0)
				{
					Info->MaxTXIFieldLength = Info->MaxTXIFieldLength<<8 | Data[CurrentByte+counter++];
				}
				CurrentByte += counter;
				SET_BIT(Info->State,2);
				break;
			}
			case AX25_XID_I_RX_LENGTH:
			{
				u8 ParameterLength=Data[CurrentByte+1],counter = 2;
				Info->MaxRXIFieldLength = 0;
				while(ParameterLength-- > 0)
				{
					Info->MaxRXIFieldLength = Info->MaxRXIFieldLength<<8 | Data[CurrentByte+counter++];
				}
				CurrentByte += counter;
				SET_BIT(Info->State,3);
				break;
			}
			case AX25_XID_TX_WINDOW_SIZE:
			{
				Info->TXWindowsize = Data[CurrentByte+2];
				CurrentByte += 3;
				SET_BIT(Info->State,4);
				break;
			}
			case AX25_XID_RX_WINDOW_SIZE:
			{
				Info->RXWindowsize = Data[CurrentByte+2];
				CurrentByte +=3;
				SET_BIT(Info->State,5);
				break;
			}
			case AX25_XID_ACK_TIMER:
			{
				u8 ParameterLength=Data[CurrentByte+1],counter = 2;
				Info->ACKTimer = 0;
				while(ParameterLength-- > 0)
				{
					Info->ACKTimer = Info->ACKTimer<<8 | Data[CurrentByte+counter++];
				}
				CurrentByte += counter;
				SET_BIT(Info->State,6);
				break;
			}
			case AX25_XID_RETRIES:
			{
				u8 ParameterLength=Data[CurrentByte+1],counter = 2;
				Info->RetryCount = 0;
				while(ParameterLength-- > 0)
				{
					Info->RetryCount = Info->RetryCount<<8 | Data[CurrentByte+counter++];
				}
				CurrentByte += counter;
				SET_BIT(Info->State,7);
				break;
			}
			default :
			{
				CurrentByte++;
				break;
			}
		}
	}
   /* if(GET_BIT(Info->State,0))
    {
    	if(((Info->ClassesOfProcedures>>5) & 0x3) == 1)
    	{
    		Half-duplex
    	}
    	else if (((Info->ClassesOfProcedures>>5) & 0x3) == 2)
    	{
    		FullDuplex
    	}
    	else
    	{
    		Error must not happen
    	}
    }
    else
    {
    	*Set to half-duplex
    }
    if(GET_BIT(Flag,1))
    {
    	if(((Info->HDLCOptionalFunctions>>1 ) & 0x3) == 1)
    	{
    		*Implicit Reject
    	}
    	else if(((Info->HDLCOptionalFunctions>>1 ) & 0x3) == 1)
		{
    		*SREJ
		}
    	else if(((Info->HDLCOptionalFunctions>>1 ) & 0x3) == 1)
		{
    		*SREJ/REJ*
		}
    	else
    	{
    		*Error !!!!*
    	}

    	if(((Info->HDLCOptionalFunctions>>10) & 0x3) == 1)
    	{
    		*8Bit*
    	}
    	else if (((Info->HDLCOptionalFunctions>>10) & 0x3) == 2)
    	{
    		*16Bit*
    	}
    	else
    	{
    		*Error*
    	}

    }
    else
    {
    	*Set to 8bit modulo and Selective reject reject*
    }
    if(GET_BIT(Flag,2))
    {
    	Set I field Length to Info->MaxTXfieldLengh*
    }
    else
    {
    	*Set I field Length to the default*
    }
    if(GET_BIT(Flag,3))
    {
    	*Set I field Length to Info->MaxRXfieldLengh*
    }
    else
    {
    	*Set I field Length to 256 byte*
    }
    if(GET_BIT(Flag,4))
    {
    	*Set Window size tx to Info->TXWindowsize*
    }
    else
    {
    	*Set Window size tx to its default*
    }

    if(GET_BIT(Flag,5))
	{
		*Set Window size rx to Info->RXWindowsize*
	}
	else
	{
		*Set Window size RX to 7 if 8bit control modulo or 127 if 16bit control modulo*
	}
    if(GET_BIT(Flag,6))
    {
    	*Set ACK timer to Info->AckTimer*
    }
    else
}
    	*Set ACK timer to  3000 Msec*
    }
    if(GET_BIT(Flag,7))
    {
    	*Set number of retries to Info->RetryCount*
    }
    else
    {
    	*Set number of retries to 10*
    }
*/
	return Info;
}


u8 AX25_u8GetBytes(u64 Number)
{
	u8 Bytes = 1;

	while((Number)/255 >= 1)
	{
		Number = ((Number)/255);
		Bytes++;
	}

	return Bytes;

}



frame_t *AX25_frame_tPtrCreatSFrame(char*DestAddress,u8 DestSSID,char*SrcAddress,u8 SrcSSID,u8 CMDRES,u8 PollFinal,enum SFrameControlFieldType ControllFieldType,u8 *Info,u32 InfoLength)
{
	frame_t* SFrame = (frame_t*)malloc(sizeof(frame_t));
	memcpy(SFrame->FrameAddress.Destination,DestAddress,6);
	SFrame->FrameAddress.DestinationSSID = DestSSID;
	memcpy(SFrame->FrameAddress.Source,SrcAddress,6);
	SFrame->FrameAddress.SourceSSID = SrcSSID;
	SFrame->FrameAddress.RepeaterUsed = AX25_NO_REPEATER;
	SFrame->FrameAddress.NOfRepeaters = 0;
	SFrame->CMDRES = CMDRES;

	SFrame->FrameType = AX25_S_FRAME;
	SFrame->ControlFieldType = ControllFieldType;
	SFrame->PollFinal = PollFinal;
	SFrame->Info = Info;
	SFrame->InfoLength = InfoLength;
    SFrame->NS = 255;
	AX25_voidConstructFrame(SFrame);

	return SFrame;
}
frame_t *AX25_frame_tPtrCreatUFrame(char*DestAddress,u8 DestSSID,char*SrcAddress,u8 SrcSSID,u8 CMDRES,u8 PollFinal,enum UFrameControlFieldType ControllFieldType,u8 *Info,u32 InfoLength)
{
	frame_t* UFrame = (frame_t*)malloc(sizeof(frame_t));

	memcpy(UFrame->FrameAddress.Destination,DestAddress,6);
	UFrame->FrameAddress.DestinationSSID = DestSSID;
	memcpy(UFrame->FrameAddress.Source,SrcAddress,6);
	UFrame->FrameAddress.SourceSSID = SrcSSID;
	UFrame->FrameAddress.RepeaterUsed = AX25_NO_REPEATER;
	UFrame->FrameAddress.NOfRepeaters = 0;
	UFrame->FrameType = AX25_U_FRAME;
	UFrame->ControlFieldType = ControllFieldType;
	UFrame->PollFinal = PollFinal;
	if(ControllFieldType != Unnumbered_Information){
        UFrame->Info = Info;
	}
	else {
        UFrame->Info = (u8*)malloc(sizeof(u8)*InfoLength);
	}
	UFrame->InfoLength = InfoLength;
	UFrame->CMDRES = CMDRES;
    UFrame->NR = 255;
    UFrame->NS = 255;
	AX25_voidConstructFrame(UFrame);


	return UFrame;
}
frame_t *AX25_frame_tPtrCreatIFrame(char*DestAddress,u8 DestSSID,char*SrcAddress,u8 SrcSSID,u8 CMDRES,u8 PollFinal,enum Protocol_Identifier_PID PID,u8 *Info,u32 InfoLength)
{
	frame_t* IFrame = (frame_t*)malloc(sizeof(frame_t));


	memcpy(IFrame->FrameAddress.Destination,DestAddress,6);
	IFrame->FrameAddress.DestinationSSID = DestSSID;
	memcpy(IFrame->FrameAddress.Source,SrcAddress,6);
	IFrame->FrameAddress.SourceSSID = SrcSSID;
	IFrame->FrameAddress.RepeaterUsed = AX25_NO_REPEATER;
	IFrame->FrameAddress.NOfRepeaters = 0;
	IFrame->FrameType = AX25_I_FRAME;
	IFrame->PID = PID;
	IFrame->PollFinal = PollFinal;
	IFrame->Info = (u8*)malloc(sizeof(u8)*InfoLength);
	memcpy(IFrame->Info,Info,InfoLength);
	IFrame->InfoLength = InfoLength;

	IFrame->CMDRES = CMDRES;

	AX25_voidConstructFrame(IFrame);

	return IFrame;
}

void AX25_voidFreeXIDInfo(XIDInfo_t* Info){
  free(Info->XIDInfo);
  free(Info);
}
