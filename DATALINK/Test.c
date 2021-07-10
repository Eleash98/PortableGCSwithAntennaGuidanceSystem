#include <pthread.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "AX25_interface.h"

#include "Timer_flags.h"
#include "Timer_interface.h"

//#include "PH_interface.h"
//#include "PH_flags.h"

#include "DL_interface.h"
#include "DL_flags.h"

#include "Segmenter_interface.h"
#include "Segmenter_flags.h"

#include "PRINT_interface.h"
#include "UART.h"

#include "Test.h"

void TurnOff(void)
{
	//printf("Turning off the Transmitter...\n");
}
void TurnOn(void)
{
	//printf("Turning on the Transmitter...\n");
}
/*void Transmitframe(u8* PtrtoFrame, u32 Length)
{
  //AX25_voidFreeFrame(F);
    //printf("frame to transmit:\n");
   // for(int i = 0;i<Length;i++){
    //    printf("%X",PtrtoFrame[i]>>4);
   //     printf("%X ",PtrtoFrame[i]&15);
   // }
   // printf("%c",'\n');

	//UART_voidWriteArray(PtrtoFrame,Length);
    //printf("hh\n");
}*/
void *Receiving(void *S)
{
    while(1)
    {
        UART_voidRead();
    }
    return 0;
}

void* D_L(void* S){

    while(1)
    {
        SM_voidDataLinkLayer();
    }
    return 0;
}

void* Seg(void* S){

    while(1)
    {
        SM_voidSegmenter();
    }
    return NULL;
}

void* Assem(void *S){
    while(1){
        SM_voidAssembler();
    }
    return NULL;

}


void* P_H(void* S){

    while(1)
    {
        //SM_voidSimplexPhysicalLayer();
    }
    return 0;
}



u8 Data1[5]={1,2,3,4,5};
u8 Data2[5]={10,20,30,40,50};
/*
void TestSimplexStateMachine(void)
{
    UART_voidInit();
    pthread_t RecThread;
    pthread_create(&RecThread, NULL, Receiving ,0);

  SetCallBack(PH_CALLBACK_TURN_ON_TRANSMITTER, TurnOn);
  SetCallBack(PH_CALLBACK_TURN_OFF_TRANSMITTER, TurnOff);
  SetCallBack(PH_CALLBACK_TRANSMIT,(void(*)(void)) Transmitframe);
  SetCallBack(PH_CALLBACK_RECEIVE, (void(*)(void))Receiving);

  SM_voidSimplexPhysicalLayerInit();
  AX25Timer_voidInit();
    AX25Timer_voidStop(AX25_TIMER_T1);
    AX25Timer_voidStop(AX25_TIMER_T2);
    AX25Timer_voidStop(AX25_TIMER_T3);
    AX25Timer_voidStop(AX25_TIMER_TR210);

	AX25Timer_voidStop(AX25_TIMER_T100);
	AX25Timer_voidStop(AX25_TIMER_T101);
	AX25Timer_voidStop(AX25_TIMER_T102);
	AX25Timer_voidStop(AX25_TIMER_T103);
	AX25Timer_voidStop(AX25_TIMER_T104);
	AX25Timer_voidStop(AX25_TIMER_T105);
	AX25Timer_voidStop(AX25_TIMER_T106);
	AX25Timer_voidStop(AX25_TIMER_T107);
	AX25Timer_voidStop(AX25_TIMER_T108);

	s8 signal = '+';
	//int A = 0;
	while( signal != '/')
	{

	    scanf("%c",&signal);
		//scanf(" %c",&signal);
		if(A == 0)
            signal = '3';
        else if (A == 1)
            signal = '2';
        else if (A == 2)
            signal = '2';
        else if (A == 3)
            signal = 'f';
        else
            signal = '0';
        A++;
       // frame_t* F =
  switch(signal)
		{

			case '1':
			    printf("Expedited Data Request\n");
                DLtoPhysical = AX25_frame_tPtrCreatIFrame("Magdy ", 3, "mohyy ", 4, AX25_RESPONSE_FRAME, 0, Compressed_TCP_IP_packet, Data2, 5);
				PH_Request.ExpeditedDataRequest = PH_FLAG_SET;
				break;

			case '2':
			    printf("Data Request\n");
			    //DLtoPhysical = AX25_frame_tPtrCreatIFrame("Gimy  ", 1, "sheham", 2, AX25_RESPONSE_FRAME, 1, ISO_8208_CCITT_X_25_PLP, Data1, 5);
				DLtoPhysical = AX25_frame_tPtrCreatSFrame("Hello ",3,"shit  ",4,AX25_COMMAND_FRAME,1,SelectiveReject,0,0);
				PH_Request.DataRequest = PH_FLAG_SET;
				break;

			case '3':
			    printf("Seize Request\n");
			    PH_Request.SiezeRequest = PH_FLAG_SET;

				break;
			case '4':
			    printf("Release Request\n");
				PH_Request.ReleaseRequest = PH_FLAG_SET;
				break;
			case '5':
			    printf("Acquisition of Signal\n");
				Hardware.AcquisitionOfSignal = PH_FLAG_SET;
				break;
			case '6':
			    printf("Loss of signal\n");
				Hardware.LossOfSignal = PH_FLAG_SET;
				break;
			case '7':
			    printf("Frame\n");
				Hardware.Frame = PH_FLAG_SET;
				break;
			case '8':

				break;
			case '9':

				break;
			case '0':

				break;
            case 'a':
                printf("Timer T100 Expire\n");
                Timer.T100_EXPIRE = PH_FLAG_SET;
                break;
            case 'b':
                printf("Timer T101 Expire\n");
                Timer.T101_EXPIRE = PH_FLAG_SET;
                break;
            case 'c':
                printf("Timer T102 Expire\n");
                Timer.T102_EXPIRE = PH_FLAG_SET;
                break;
            case 'd':
                printf("Timer T103 Expire\n");
                Timer.T103_EXPIRE = PH_FLAG_SET;
                break;
            case 'e':
                printf("Timer T104 Expire\n");
                Timer.T104_EXPIRE = PH_FLAG_SET;
                break;
            case 'f':
                printf("Timer T105 Expire\n");
                Timer.T105_EXPIRE = PH_FLAG_SET;
                break;
            case 'g':
                printf("Timer T106 Expire\n");
                Timer.T106_EXPIRE = PH_FLAG_SET;
                break;
            case 'h':
                printf("Timer T107 Expire\n");
                Timer.T107_EXPIRE = PH_FLAG_SET;
                break;
            case 'i':
                printf("Timer T108 Expire\n");
                Timer.T108_EXPIRE = PH_FLAG_SET;
                break;
            case 'j':

                break;
            case 'k':

                break;

			default:

                break;
		}
		SM_voidSimplexPhysicalLayer();
	}

	SM_voidExit();
}
*/



void Test_DL_PH(void)
{

    //Enable UART
    UART_voidInit();

    //creat threads for DL, PH, Receiving
    pthread_t Receiver_Thread, DL_Thread, Segmenter_Thread, Assembler_Thread;




    //Set CallBacks for PH
    //SetCallBack(PH_CALLBACK_TURN_ON_TRANSMITTER, TurnOn);
    //SetCallBack(PH_CALLBACK_TURN_OFF_TRANSMITTER, TurnOff);
    //SetCallBack(PH_CALLBACK_TRANSMIT,(void(*)(void)) Transmitframe);
    //SetCallBack(PH_CALLBACK_RECEIVE, (void(*)(void))Receiving);

    //Initiate All timers
    AX25Timer_voidInit();

    //Stop All Timers
    AX25Timer_voidStop(AX25_TIMER_T1);
    AX25Timer_voidStop(AX25_TIMER_T2);
    AX25Timer_voidStop(AX25_TIMER_T3);
    AX25Timer_voidStop(AX25_TIMER_TR210);

    //Initialize Physical & Data Link state machines
    //SM_voidSimplexPhysicalLayerInit();
    DL_voidInit();

    //Set Address and SSID
    strcpy(MYAddress,"ELEISH");
    strcpy(DestAddress,"GIMY  ");

    MySSID = 0;
    DestSSID = 0;

    N1 = 256;
    N2 = 5;

    NofRepeaters = 0;

    //run threads of Phusical and Data link state machines
    sleep(1);
    printf("Sending Connect Request\n");
    DLSAP_Request.ConnectRequest = SEGMENTER_FLAG_SET;


    //pthread_create(&PH_Thread,NULL,(void*(*)(void*))P_H,0);
    pthread_create(&DL_Thread,NULL,(void*(*)(void*))D_L,0);
    pthread_create(&Receiver_Thread,NULL,Receiving,0);
    pthread_create(&Segmenter_Thread,NULL,Seg,0);
    pthread_create(&Assembler_Thread,NULL,Assem,0);

    //while(1);
    while(DLSAP_Indication.ConnectConfirm == SEGMENTER_FLAG_CLEAR);

    //if(DL_CurrentState == SM_DL_STATE_CONNECTED)
        //printf("In Connected State\n");

    //u8 flag = 0;
        /*while(!((DL_Indication.ConnectIndication == DL_FLAG_SET
        || DL_Indication.ConnectConfirm == DL_FLAG_SET)
        && (flag == 0)));
        printf("Connect Indication: %s\n",(DL_Indication.ConnectIndication == DL_FLAG_SET)? "SET":"CLEARED");
        printf("Connect Confirm: %s\n",(DL_Indication.ConnectConfirm == DL_FLAG_SET)? "SET":"CLEARED");
        flag = 1;
	*/



    printf("Connected With other TNC!\n");
    //NofSegments = 50;
    //SegmentertoDL = (u8**)malloc(NofSegments*sizeof(u8*));

    //SegmentsLengths = (s32*)malloc(NofSegments*sizeof(s32));

    printf("Sending Unit Data Request\n");
    //u8 UnitData[8];// = {1,2,3,4,5,6,7};

    /*for(int i=0;i<NofSegments;i++){
        SegmentsLengths[i] = 8;
        SegmentertoDL[i] = (u8*)malloc(sizeof(u8)*SegmentsLengths[i]);
        for(int j=i;j<8*(i+1);j++){
            SegmentertoDL[i][j%8] = j;
        }

        //frame_t* I = AX25_frame_tPtrCreatIFrame(DestAddress,0,MYAddress,0,AX25_COMMAND_FRAME,0,No_layer_3_protocol_implemented,UnitData,8);
        //SegmentertoDL[i] = I;
        //while(DL_Request.DataRequest == DL_FLAG_SET)    usleep(10);
        //DL_Request.DataRequest = DL_FLAG_SET;
        //while(DL_Request.DataRequest == DL_FLAG_SET)
        //{
        // usleep(1000);
        //}
    }
    for (int i=0;i<NofSegments;i++){
        printf("Segment No %d: ",i);
        for(int j=0;j<SegmentsLengths[i];j++){
            printf("%d ");
        }
        printf("\n");
    }*/

    ApptoSegmenterLength = 1000;
    ApptoSegmenterInfo = (u8*)malloc(ApptoSegmenterLength*sizeof(u8));

    for (int i=0;i<ApptoSegmenterLength;i++){
	ApptoSegmenterInfo[i] = i;
    }

    DLSAP_Request.DataRequest = SEGMENTER_FLAG_SET;

    //frame_t* I = AX25_frame_tPtrCreatIFrame(DestAddress,0,MYAddress,0,AX25_COMMAND_FRAME,1,No_layer_3_protocol_implemented,UnitData,7);

    //while(1){usleep(1);}
    //sleep(100);
    //pause();

    //sleep(10);

    while(1){
        if(DLSAP_Indication.DataIndication == SEGMENTER_FLAG_SET)
        {
            printf("Received I Frame:\n");
            for(int i=0;i<AssemblertoAppLength;i++)
            {
                printf("%d ",AssemblertoAppInfo[i]);
            }
            printf("\n");
            DLSAP_Indication.DataIndication = SEGMENTER_FLAG_CLEAR;
            sleep(500);
            break;
        }


    }




    printf("Sending Disconnect Request\n");
    DLSAP_Request.DisconnectRequest = SEGMENTER_FLAG_SET;



        //flag = 0;
        /*while(!((DL_Indication.DisConnectConfirm == DL_FLAG_SET
        || DL_Indication.DisConnectIndication == DL_FLAG_SET)
        && (flag == 0))){
            printf("Dis Confirm %u\n",DL_Indication.DisConnectConfirm);
            printf("Dis indication %u\n",DL_Indication.DisConnectIndication);
            sleep(1);
        }*/

	while(DLSAP_Indication.DisconnectConfirm == SEGMENTER_FLAG_CLEAR);
        printf("Disconnected from Other TNC!\n");

        AX25Timer_voidExit();
        pthread_cancel(Receiver_Thread);
        pthread_cancel(DL_Thread);

        //usleep(100000);
}



void TestDuplexStateMachine(void)
{
  /*DL_voidInit();
    s8 Signal = 'z';
    while(Signal != '/')
    {
        scanf("%c",&Signal);
        switch(Signal)
        {
            case '1':
                DL_Request.ConnectRequest = DL_FLAG_SET;
                break;
            case '2':
                DL_Request.DisconnectRequest = DL_FLAG_SET;
                break;
            case '3':
                {
                frame_t* Frame =AX25_frame_tPtrCreatIFrame("QWER  ",0,"TYUI  ",0,AX25_COMMAND_FRAME,1,No_layer_3_protocol_implemented,Data1,5);
                //Frame = AX25_frame_tPtrCreatIFrame("QWER  ",0,"TYUI  ",0,AX25_COMMAND_FRAME,1,No_layer_3_protocol_implemented,Data1,5);
                SegmentertoDL = Frame;
                DL_Request.DataRequest = DL_FLAG_SET;
                break;
                }
            case '4':
                DL_Request.UnitDataRequest = DL_FLAG_SET;
                break;
            case '5':
                DL_Request.FlowoffRequest = DL_FLAG_SET;
                break;
            case '6':
                DL_Request.FlowonRequest = DL_FLAG_SET;
                break;
            case '7':
                PH_Indication.BusyIndication = PH_FLAG_SET;
                break;
            case '8':
                PH_Indication.QuietIndication = PH_FLAG_SET;
                break;
            case '9':
                PH_Indication.SiezeConfirm = PH_FLAG_SET;
                break;
            case '0':

                break;
            case 'a':
                Timer.T1_EXPIRE = PH_FLAG_SET;
                break;
            case 'b':
                Timer.T3_EXPIRE = PH_FLAG_SET;
                break;
            case 'c':

                break;
            case 'd':

                break;
            case 'e':

                break;
            case 'f':

                break;
            case 'g':

                break;
            case 'h':

                break;
            case 'i':

                break;
            case 'j':

                break;
            case 'k':

                break;
            case 'l':

                break;
            default:
                break;
        }
        SM_voidDataLinkLayer();
	}*/
}
void TestDataLinkStateMachine(void){

    char signal = '=',dum;
    while(signal != '/')
    {
        scanf("%c%c",&signal,&dum);
      switch(signal)
		{
			case '1':
			    printf("Connect Request\n");
                DL_Request.ConnectRequest = DL_FLAG_SET;
				break;

			case '2':
			    printf("Disconnect Request\n");
				DL_Request.DisconnectRequest = DL_FLAG_SET;
				break;

			case '3':
			    printf("Data Request\n");
			    DL_Request.DataRequest = DL_FLAG_SET;
                /////////////////////////////////////////////
                /////////////////////////////////////////////
				break;
			case '4':
			    printf("Unit Data Request\n");
				DL_Request.UnitDataRequest = DL_FLAG_SET;
				/////////////////////////////////////////////
                /////////////////////////////////////////////
				break;
			case '5':
			    printf("Flow Off Request\n");
				DL_Request.FlowoffRequest = DL_FLAG_SET;
				break;
			case '6':
			    printf("Flow On Reqest\n");
				DL_Request.FlowonRequest = DL_FLAG_SET;
				break;
			case '7':
			    printf("Frame\n");
				Hardware.Frame = PH_FLAG_SET;
				break;
			case '8':

				break;
			case '9':

				break;
			case '0':

				break;
            case 'a':
                printf("Timer T100 Expire\n");
                Timer.T100_EXPIRE = PH_FLAG_SET;
                break;
            case 'b':
                printf("Timer T101 Expire\n");
                Timer.T101_EXPIRE = PH_FLAG_SET;
                break;
            case 'c':
                printf("Timer T102 Expire\n");
                Timer.T102_EXPIRE = PH_FLAG_SET;
                break;
            case 'd':
                printf("Timer T103 Expire\n");
                Timer.T103_EXPIRE = PH_FLAG_SET;
                break;
            case 'e':
                printf("Timer T104 Expire\n");
                Timer.T104_EXPIRE = PH_FLAG_SET;
                break;
            case 'f':
                printf("Timer T105 Expire\n");
                Timer.T105_EXPIRE = PH_FLAG_SET;
                break;
            case 'g':
                printf("Timer T106 Expire\n");
                Timer.T106_EXPIRE = PH_FLAG_SET;
                break;
            case 'h':
                printf("Timer T107 Expire\n");
                Timer.T107_EXPIRE = PH_FLAG_SET;
                break;
            case 'i':
                printf("Timer T108 Expire\n");
                Timer.T108_EXPIRE = PH_FLAG_SET;
                break;
            case 'j':

                break;
            case 'k':

                break;

			default:

                break;
		}
		SM_voidDataLinkLayer();
	}

	DL_voidExit();

}












