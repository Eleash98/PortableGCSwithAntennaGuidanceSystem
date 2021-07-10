/*
 * Segmenter_program.c
 *
 *  Created on: Apr 6, 2021
 *      Author: MohammedGamalEleish
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"


#include "AX25_interface.h"
//#include "PH_interface.h"
//#include "PH_flags.h"
#include "Timer_interface.h"
#include "Timer_flags.h"

//#include "PRINT_interface.h"

#include "DL_interface.h"
#include "DL_flags.h"


//#include "MDL_flags.h"

#include "Segmenter_interface.h"
#include "Segmenter_flags.h"
#include "Segmenter_private.h"


void SM_voidAssembler(void){

    switch(ReassemblerCurrentState){

    case REASSEMBLER_STATE_READY:
        Reassember_ReadyState();
        break;

    case REASSEMBLER_STATE_REASSEMBLING_DATA:
        Reassember_ReassemblingDataState();
        break;
    case REASSEMBLER_STATE_REASSEMBLING_UNIT_DATA:

        break;
        Reassember_ReassemblingUnitDataState();
    default:
        break;


    }


}


void SM_voidSegmenter(void){

    segmenter_ReadyState();

}

void AssembleData(u8* Info, u32 InfoLength)
{
	memcpy(AssemblertoAppInfo+AssemblertoAppLength,Info+2,InfoLength-2);
	AssemblertoAppLength += InfoLength-2;
	free(Info);
	Segmenter_InternalFlag.N--;
	//AX25_voidFreeFrame(Frame);
}

void segmenter_ReadyState(void)
{
	#if DEBUGGING == 1
    printf("In Segmenter Ready State (Segmenter Ready state)\n");
    #endif // DEBUGGING
	if (DLSAP_Request.DataRequest == SEGMENTER_FLAG_SET || DLSAP_Request.UnitDataRequest == SEGMENTER_FLAG_SET)
	{
        #if DEBUGGING == 1
        printf("DLSAP Request is Data request or unit Data request => Set (Segmenter Ready State)\n");
        #endif // DEBUGGING
		if (ApptoSegmenterLength < N1)
		{
			#if DEBUGGING == 1
            printf("App To Segmenter Length is smaller than N1 (Segmenter Ready State)\n");
            #endif // DEBUGGING
			/////////////////////////////////////////////
			NofSegments = 1;
			SegmentsLengths = (s32*)malloc(sizeof(s32));
			SegmentsLengths[0] = ApptoSegmenterLength;
			SegmentertoDL = (u8**)malloc(sizeof(u8*));
			SegmentertoDL[0] = (u8*)malloc(sizeof(u8)*ApptoSegmenterLength);
			memcpy(SegmentertoDL[0],ApptoSegmenterInfo,SegmentsLengths[0]);
			//frame_t* Frame = AX25_frame_tPtrCreatIFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 0, No_layer_3_protocol_implemented, ApptoSegmenterInfo, ApptoSegmenterLength);
			/////////////////////////////////////////////
			//SegmentertoDL = Frame;
			if (DLSAP_Request.DataRequest == SEGMENTER_FLAG_SET)
            {
                DL_Request.DataRequest = DL_FLAG_SET;

                #if DEBUGGING == 1
                printf("DLSP request is Data request so => Data Request is SET (Segmenter Ready State)\n");
                #endif // DEBUGGING
            }
			else
			{
			    DL_Request.UnitDataRequest = DL_FLAG_SET;

			    #if DEBUGGING == 1
                printf("DLSP request is unit Data request so => unit Data Request is SET (Segmenter Ready State)\n");
                #endif // DEBUGGING
			}
		}
		else if (ApptoSegmenterLength > 128*N1)
		{
			DLSAP_Indication.ErrorIndication = Y;
			free(ApptoSegmenterInfo);
			ApptoSegmenterLength = 0;
			#if DEBUGGING == 1
            printf("App To Segmenter Length is greater than N1*128 (Segmenter Ready State)\n");
            #endif // DEBUGGING
		}
		else
		{
			#if DEBUGGING == 1
            printf("App To Segmenter Length isn't smaller than N1 and App To Segmenter Length isn't greater than N1*128 (Segmenter Ready State)\n");
            #endif // DEBUGGING
			//////////////////////////////////////////////////
			/*NofSegments = 1;
			SegmentsLengths = (s32*)malloc(sizeof(s32));
			SegmentsLengths[0] = N1;
			SegmentertoDL = (u8**)malloc(sizeof(u8*));

			u32 InfoIndex = 0;
			for(InfoIndex = 0; InfoIndex<ApptoSegmenterLength;InfoIndex+= N1-2){
			}*/
			if(N1 != 2)
            {
            //////////////////////////////////////////////////
                #if DEBUGGING == 1
                printf("N1 isn't equal '2' (Segmenter Ready State)\n");
                #endif // DEBUGGING

				u8 NumofSegments = ApptoSegmenterLength/(N1-2);
				if(ApptoSegmenterLength%(N1-2))
                {
                    NumofSegments++;
                    #if DEBUGGING == 1
                    printf("Number of Segments %% (N1-2) !=0 (Segmenter Ready State)\n");
                    #endif // DEBUGGING
                }
				u8 SentSegments = 0,F = 1;
				u32 Local_dataLength = ApptoSegmenterLength;
				NofSegments = NumofSegments;
				SegmentsLengths = (s32*)malloc(sizeof(s32)*NofSegments);
				//SegmentsLengths[0] = N1;
				SegmentertoDL = (u8**)malloc(sizeof(u8*)*NofSegments);
				//SegmentertoDL[0] = (u8*)malloc(sizeof(u8)*SegmentsLengths[0]);
				u32 i = 0,InfoIndex = 0;
				while (SentSegments < NumofSegments)
				{
					u32 InfoLength =  Local_dataLength>= N1 ?	N1-2:Local_dataLength;
					Local_dataLength -= InfoLength;
					//u8* Info = malloc((InfoLength+2)*sizeof(u8));
					SegmentsLengths[i] = InfoLength+2;
					SegmentertoDL[i] = (u8*)malloc(sizeof(u8)*SegmentsLengths[i]);
					memcpy(*(SegmentertoDL+i)+2,ApptoSegmenterInfo+InfoIndex,InfoLength);

					//memcpy(Info+2,ApptoSegmenterInfo+InfoIndex,InfoLength+2);
					InfoIndex = InfoIndex>= ApptoSegmenterLength? ApptoSegmenterLength-1:InfoIndex+N1-2;
					//if (InfoIndex > ApptoSegmenterLength)

					SegmentertoDL[i][0] = 0x08;
					SegmentertoDL[i][1] = F*128+NumofSegments-SentSegments-1;
					//Info[0] = 0x08;
					//Info[1] = F*128+NumofSegments-SentSegments-1;
					F = 0;
					/////////////////////////////////////////////////
					//frame_t* Iframe = AX25_frame_tPtrCreatIFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 0, No_layer_3_protocol_implemented, Info, InfoLength);
					/////////////////////////////////////////////////
					//SegmentertoDL = Iframe;

					SentSegments++;
					i++;
				}
				if (DLSAP_Request.DataRequest == SEGMENTER_FLAG_SET)
					{
					    DL_Request.DataRequest = DL_FLAG_SET;
					    #if DEBUGGING == 1
                        printf("DLSP request is Data request so => Data Request is SET (Segmenter Ready State)\n");
                        #endif // DEBUGGING
					}
					else
					{
					    DL_Request.UnitDataRequest = DL_FLAG_SET;
					    #if DEBUGGING == 1
                        printf("DLSP request is unit Data request so => unit Data Request is SET (Segmenter Ready State)\n");
                        #endif // DEBUGGING
					}
			}
		}
		DLSAP_Request.UnitDataRequest = SEGMENTER_FLAG_CLEAR;
        DLSAP_Request.DataRequest = SEGMENTER_FLAG_CLEAR;

	}

	else if(DLSAP_Request.FlowoffRequest == SEGMENTER_FLAG_SET)
	{
		/***Send unmodified Primitive****/
		#if DEBUGGING == 1
        printf("DLSAP Request isn't Data request or unit Data request (Segmenter Ready State)\n");
        #endif // DEBUGGING
        DL_Request.FlowoffRequest = DL_FLAG_SET;
        DLSAP_Request.FlowoffRequest = SEGMENTER_FLAG_CLEAR;
	}

	else if(DLSAP_Request.FlowonRequest == SEGMENTER_FLAG_SET)
	{
		/***Send unmodified Primitive****/
		#if DEBUGGING == 1
        printf("DLSAP Request isn't Data request or unit Data request (Segmenter Ready State)\n");
        #endif // DEBUGGING
        DL_Request.FlowonRequest = DL_FLAG_SET;
        DLSAP_Request.FlowonRequest = SEGMENTER_FLAG_CLEAR;
	}

	else if(DLSAP_Request.ConnectRequest == SEGMENTER_FLAG_SET)
	{

		#if DEBUGGING == 1
        printf("DLSAP Request isn't Data request or unit Data request (Segmenter Ready State)\n");
        #endif // DEBUGGING
        DL_Request.ConnectRequest = DL_FLAG_SET;
        DLSAP_Request.ConnectRequest = SEGMENTER_FLAG_CLEAR;
	}

	else if(DLSAP_Request.DisconnectRequest == SEGMENTER_FLAG_SET)
	{

		#if DEBUGGING == 1
        printf("DLSAP Request isn't Data request or unit Data request (Segmenter Ready State)\n");
        #endif // DEBUGGING
        DL_Request.DisconnectRequest = DL_FLAG_SET;
        DLSAP_Request.DisconnectRequest = SEGMENTER_FLAG_CLEAR;
	}

}

void Reassember_ReadyState(void)
{
	#if DEBUGGING == 1
    printf("In Reassembler Ready State (Reassembler Ready state)\n");
    #endif // DEBUGGING
	if (DL_Indication.ConnectConfirm == DL_FLAG_SET)
	{
		/***Send unmodified Primitive****/
		#if DEBUGGING == 1
        printf("All other DL Primitives (Reassembler Ready state)\n");
        #endif // DEBUGGING
        DLSAP_Indication.ConnectConfirm = SEGMENTER_FLAG_SET;
        DL_Indication.ConnectConfirm = DL_FLAG_CLEAR;
	}
	else if (DL_Indication.ConnectIndication == DL_FLAG_SET)
	{
		/***Send unmodified Primitive****/
		#if DEBUGGING == 1
        printf("All other DL Primitives (Reassembler Ready state)\n");
        #endif // DEBUGGING
        DLSAP_Indication.ConnectIndication = SEGMENTER_FLAG_SET;
        DL_Indication.ConnectIndication = DL_FLAG_CLEAR;
	}
	else if (DL_Indication.DisConnectConfirm == DL_FLAG_SET)
	{
		/***Send unmodified Primitive****/
		#if DEBUGGING == 1
        printf("All other DL Primitives (Reassembler Ready state)\n");
        #endif // DEBUGGING
        DLSAP_Indication.DisconnectConfirm = SEGMENTER_FLAG_SET;
        DL_Indication.DisConnectConfirm = DL_FLAG_CLEAR;
	}
	else if (DL_Indication.DisConnectIndication == DL_FLAG_SET)
	{
		/***Send unmodified Primitive****/
		#if DEBUGGING == 1
        printf("All other DL Primitives (Reassembler Ready state)\n");
        #endif // DEBUGGING
        DLSAP_Indication.DisconnectIndication = SEGMENTER_FLAG_SET;
        DL_Indication.DisConnectIndication = DL_FLAG_CLEAR;
	}
	else if ((DL_Indication.DataIndication == DL_FLAG_SET) ||(DL_Indication.UnitDataIndication == DL_FLAG_SET))
	{
		#if DEBUGGING == 1
        printf("DL Indication is Data Indication or unit Data Indication => SET (Reassembler Ready state)\n");
        #endif // DEBUGGING
		if(DLtoAssembler[0] != 0x08)
		{
			#if DEBUGGING == 1
            printf("Segment Info element was Absent (Reassembler Ready state)\n");
            #endif // DEBUGGING
			AssemblertoAppInfo = DLtoAssembler;
			AssemblertoAppLength = DLtoAssemblerLength;
			if(DL_Indication.DataIndication == DL_FLAG_SET)
            {
                DLSAP_Indication.DataIndication = SEGMENTER_FLAG_SET;
                #if DEBUGGING == 1
                printf("Data Indication was SET (Reassembler Ready state)\n");
                #endif // DEBUGGING
            }
			else
			{
			    DLSAP_Indication.UnitDataIndication = SEGMENTER_FLAG_SET;
			    #if DEBUGGING == 1
                printf("Unit Data Indication was SET (Reassembler Ready state)\n");
                #endif // DEBUGGING
			}
		}
		else
		{
			#if DEBUGGING == 1
            printf("Segment Info element was found (Reassembler Ready state)\n");
            #endif // DEBUGGING
			if(GET_BIT(DLtoAssembler[1],7))
			{
				#if DEBUGGING == 1
                printf("First Segment was found (Reassembler Ready state)\n");
                #endif // DEBUGGING
				//first segment
				Segmenter_InternalFlag.N = DLtoAssembler[1] & 0x3F;
				AssemblertoAppInfo =(u8*) malloc(sizeof(u8)*(Segmenter_InternalFlag.N+1)*(N1-2));
				AssemblertoAppLength = 0;
				//memcpy(AssemblertoAppInfo,DLtoAssembler->Info+2,AssemblertoAppLength);
				AssembleData(DLtoAssembler,DLtoAssemblerLength);
				AX25Timer_voidStart(AX25_TIMER_TR210);
				if(DL_Indication.DataIndication == DL_FLAG_SET)
				{

				    ReassemblerCurrentState = REASSEMBLER_STATE_REASSEMBLING_DATA;
				}
				else
				{

				    ReassemblerCurrentState = REASSEMBLER_STATE_REASSEMBLING_UNIT_DATA;
				}
			}
			else
			{
				//Not first segment
				//discard Segment
				free(DLtoAssembler);
				DLtoAssemblerLength=0;
				DLSAP_Indication.ErrorIndication = Z;
				ReassemblerCurrentState = REASSEMBLER_STATE_READY;
				#if DEBUGGING == 1
                printf("First Segment was Absent (Reassembler Ready state)\n");
                #endif // DEBUGGING
			}
		}

		DL_Indication.DataIndication = DL_FLAG_CLEAR;
		DL_Indication.UnitDataIndication = DL_FLAG_CLEAR;
	}

}
void Reassember_ReassemblingDataState(void)
{
	#if DEBUGGING == 1
    printf("In Reassembling Data State (Reassembling Data state)\n");
    #endif // DEBUGGING

	if(Timer.TR210_EXPIRE == TIMER_FLAG_SET)
	{
		free(AssemblertoAppInfo);
		free(DLtoAssembler);
		DLtoAssemblerLength = 0;
		DLSAP_Indication.ErrorIndication = Z;
		ReassemblerCurrentState = REASSEMBLER_STATE_READY;
		#if DEBUGGING == 1
        printf("Timer 210 was Expire (Reassembling Data state)\n");
        #endif // DEBUGGING
	}
	else if(DL_Indication.DataIndication == DL_FLAG_SET)
	{

		#if DEBUGGING == 1
        printf("Data Indication was SET (Reassembling Data state)\n");
        #endif // DEBUGGING
		if((DLtoAssembler[0] == 0x08 ) && (1/*subsequent segment*/)
				&& (DLtoAssembler[1] ==  Segmenter_InternalFlag.N))
		{
		    #if DEBUGGING == 1
            printf("Segment PID was found , Subsequent segment was found and number remaining segments equal 'N' (Reassembling Data state)\n");
            #endif // DEBUGGING
			if (Segmenter_InternalFlag.N == 0)
			{
				AX25Timer_voidStop(AX25_TIMER_TR210);
				AssembleData(DLtoAssembler,DLtoAssemblerLength);
				DLSAP_Indication.DataIndication = SEGMENTER_FLAG_SET;
				ReassemblerCurrentState = REASSEMBLER_STATE_READY;
				#if DEBUGGING == 1
                printf("'N' equal '0' (Reassembling Data state)\n");
                #endif // DEBUGGING
			}
			else
			{
				AX25Timer_voidStop(AX25_TIMER_TR210);
				AX25Timer_voidClear(AX25_TIMER_TR210);
				AX25Timer_voidStart(AX25_TIMER_TR210);
				AssembleData(DLtoAssembler,DLtoAssemblerLength);
				ReassemblerCurrentState = REASSEMBLER_STATE_REASSEMBLING_DATA;
				#if DEBUGGING == 1
                printf("'N' not equal '0' (Reassembling Data state)\n");
                #endif // DEBUGGING
			}
		}
		else
		{
			free(AssemblertoAppInfo);
			free(DLtoAssembler);
			DLtoAssemblerLength=0;
			AX25Timer_voidStop(AX25_TIMER_TR210);
			DLSAP_Indication.ErrorIndication = Z;
			ReassemblerCurrentState = REASSEMBLER_STATE_READY;
			#if DEBUGGING == 1
            printf("Segment PID wasn't found or Subsequent segment wasn't found or number remaining segments equal 'N' (Reassembling Data state)\n");
            #endif // DEBUGGING
		}
		DL_Indication.DataIndication = DL_FLAG_CLEAR;
	}
	else if (DL_Indication.ConnectConfirm == DL_FLAG_SET || DL_Indication.ConnectIndication ==DL_FLAG_SET
	 || DL_Indication.DisConnectConfirm == DL_FLAG_SET || DL_Indication.DisConnectIndication == DL_FLAG_SET)
	{

		free(AssemblertoAppInfo);
		AssemblertoAppLength =0;
		free(DLtoAssembler);
		DLtoAssemblerLength = 0;
		AX25Timer_voidStop(AX25_TIMER_TR210);
		DLSAP_Indication.ErrorIndication = Z;
		ReassemblerCurrentState = REASSEMBLER_STATE_READY;
		#if DEBUGGING == 1
        printf("All other DL primitives (Reassembling Data state)\n");
        #endif // DEBUGGING
        DL_Indication.ConnectConfirm = DL_FLAG_CLEAR;
        DL_Indication.ConnectIndication = DL_FLAG_CLEAR;
        DL_Indication.DisConnectConfirm = DL_FLAG_CLEAR;
        DL_Indication.DisConnectIndication = DL_FLAG_CLEAR;
	}

}
void Reassember_ReassemblingUnitDataState(void)
{
    #if DEBUGGING == 1
    printf("In Reassembling Unit Data State (Reassembling Unit Data state)\n");
    #endif // DEBUGGING
	//if(0);
	if(Timer.TR210_EXPIRE == TIMER_FLAG_SET)
    {
        free(AssemblertoAppInfo);
        AssemblertoAppLength = 0;
        free(DLtoAssembler);
        DLtoAssemblerLength = 0;
        DLSAP_Indication.ErrorIndication = Z;
        ReassemblerCurrentState = REASSEMBLER_STATE_READY;
        #if DEBUGGING == 1
        printf("Timer 210 was Expire (Reassembling Unit Data state)\n");
        #endif // DEBUGGING
    }
    else if(DL_Indication.UnitDataIndication == DL_FLAG_SET)
    {

        #if DEBUGGING == 1
        printf("Unit Data Indication was SET (Reassembling Unit Data state)\n");
        #endif // DEBUGGING
        if((DLtoAssembler[0] == 0x08 ) && (1/*subsequent segment*/)
                && (DLtoAssembler[1] ==  Segmenter_InternalFlag.N))
        {
            #if DEBUGGING == 1
            printf("Segment PID was found , Subsequent segment was found and number remaining segments equal 'N' (Reassembling Unit Data state)\n");
            #endif // DEBUGGING
            if (Segmenter_InternalFlag.N == 0)
            {
                AX25Timer_voidStop(AX25_TIMER_TR210);
                AssembleData(DLtoAssembler,DLtoAssemblerLength);
                DLSAP_Indication.UnitDataIndication = SEGMENTER_FLAG_SET;
                ReassemblerCurrentState = REASSEMBLER_STATE_READY;
                #if DEBUGGING == 1
                printf("'N' equal '0' (Reassembling Unit Data state)\n");
                #endif // DEBUGGING
            }
            else
            {
                AX25Timer_voidStop(AX25_TIMER_TR210);
                AX25Timer_voidClear(AX25_TIMER_TR210);
                AX25Timer_voidStart(AX25_TIMER_TR210);

                AssembleData(DLtoAssembler,DLtoAssemblerLength);
                ReassemblerCurrentState = REASSEMBLER_STATE_REASSEMBLING_UNIT_DATA;
                #if DEBUGGING == 1
                printf("'N' not equal '0' (Reassembling Unit Data state)\n");
                #endif // DEBUGGING
            }
        }
        else
        {
            free(AssemblertoAppInfo);
            free(DLtoAssembler);
            AssemblertoAppLength = 0;
            DLtoAssemblerLength = 0;
            AX25Timer_voidStop(AX25_TIMER_TR210);
            DLSAP_Indication.ErrorIndication = Z;
            ReassemblerCurrentState = REASSEMBLER_STATE_READY;
            #if DEBUGGING == 1
            printf("Segment PID wasn't found or Subsequent segment wasn't found or number remaining segments equal 'N' (Reassembling Unit Data state)\n");
            #endif // DEBUGGING
        }
        DL_Indication.UnitDataIndication = DL_FLAG_CLEAR;
    }
    else if (DL_Indication.ConnectConfirm == DL_FLAG_SET || DL_Indication.ConnectIndication ==DL_FLAG_SET
	|| DL_Indication.DisConnectConfirm == DL_FLAG_SET || DL_Indication.DisConnectIndication == DL_FLAG_SET)
    {
        free(AssemblertoAppInfo);
        free(DLtoAssembler);
        AssemblertoAppLength = 0;
        DLtoAssemblerLength = 0;
        AX25Timer_voidStop(AX25_TIMER_TR210);
        DLSAP_Indication.ErrorIndication = Z;
        ReassemblerCurrentState = REASSEMBLER_STATE_READY;
        #if DEBUGGING == 1
        printf("All other DL primitives (Reassembling Unit Data state)\n");
        #endif // DEBUGGING
        DL_Indication.ConnectConfirm = DL_FLAG_CLEAR;
        DL_Indication.ConnectIndication = DL_FLAG_CLEAR;
        DL_Indication.DisConnectConfirm = DL_FLAG_CLEAR;
        DL_Indication.DisConnectIndication = DL_FLAG_CLEAR;
    }

}

