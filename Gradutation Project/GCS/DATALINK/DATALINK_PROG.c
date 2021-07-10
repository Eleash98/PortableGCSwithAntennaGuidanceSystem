#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "CCSDS_FOP_INTERFACE.h"

#include "AX25_interface.h"

#include "DL_interface.h"
#include "DL_flags.h"

#include "MDL_interface.h"
#include "MDL_flags.h"

#include "Timer_interface.h"

#include "RS_INTERFACE.h"
#include "RANDOMIZER.h"
#include "GOLAY_ENCODING.h"
#include "GOLAY_DECODING_AND_CORRECT.h"

#include "DATALINK_interface.h"
#include "DATALINK_private.h"

#include "UART.h"
#include "CC1101_interface.h"
#define DATALINK_DEBUG          0

u8 DATALINK_VCID = 0;


char Configport[] = "/dev/xxxxxx";
char Dataport[] = "/dev/xxxxxx";

void* ConfigReceiver(void*S){

    int* SerialPort = (int*)S;
    u8* ConfigArray = NULL;
    while(1){
        ConfigArray = UART_voidRead(*SerialPort);
        if(ConfigArray == NULL)
            continue;
        Parse(ConfigArray);
    }
    return NULL;
}

void* DataReceiver(void*S){

    int* SerialPort = (int*)S;
    while(1){
        UART_voidRead(*SerialPort);
    }
    return NULL;
}


void* CC1101_ReceiveThread(void*S){

    while(1){
    u32 len = 0;
    u8* Frame = CC1101_u8PtrReadFrame(&len);

    }
    return NULL;
}


/*
void SetChannelEncoderConfiguration(ChannelEncoderConfig_t* Config, SynchWord_t* SynchConfig){

    ChannelConfig.ReedSolomon.Flag = Config->ReedSolomon.Flag;
    ChannelConfig.ReedSolomon.Poly = Config->ReedSolomon.Poly;
    ChannelConfig.ReedSolomon.InitialRoot = Config->ReedSolomon.InitialRoot;

    ChannelConfig.PCHFlag = Config->PCHFlag;

    ChannelConfig.Golay.Flag = Config->Golay.Flag;
    ChannelConfig.Golay.PolyID = Config->Golay.PolyID;
    **Call the Configuration function*

    ChannelConfig.Randomizer.Flag = Config->Randomizer.Flag;
    ChannelConfig.Randomizer.Poly = Config->Randomizer.Poly;

    if(ChannelConfig.ReedSolomon.Flag == ENCODER_ENABLE)
        RS_polynomial_generator(ChannelConfig.ReedSolomon.Poly,ChannelConfig.ReedSolomon.InitialRoot);
    if(ChannelConfig.Randomizer.Flag == ENCODER_ENABLE)
        CCSDS_void_Set_polynomial(ChannelConfig.Randomizer.Poly);

    Synch.Length = SynchConfig->Length;
    Synch.SynchWord = (u8*)malloc(sizeof(u8)*SynchConfig->Length);
    memcpy(Synch.SynchWord,SynchConfig->SynchWord,SynchConfig->Length);
}
*/
void TransmitFrame(u8* Frame,u32 FrameLength, u8 FrameType, u8 VCID){

    //Call Channel Encoder on frame
    if(GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.Flag == DATALINK_ENABLE){
        RS_polynomial_generator(GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.Poly,GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.InitialRoot);
        Frame = RS_223_255_encoder_driver_pointer_unsigned_char(Frame,&FrameLength);
    }
    if(GCS_DataLink.Transmitter.ChannelEncoder.Golay.Flag == DATALINK_ENABLE){

        set_polynomial(GCS_DataLink.Transmitter.ChannelEncoder.Golay.Poly);
        Frame = Golay_u8pointerEncode(Frame,&FrameLength);
    }
    if(GCS_DataLink.Transmitter.ChannelEncoder.PCHFlag== DATALINK_ENABLE)
        //Frame = PCH_u8pointerEncode(Frame,&FrameLength);

    if(GCS_DataLink.Transmitter.ChannelEncoder.Randomizer.Flag == DATALINK_ENABLE){
        CCSDS_void_Set_polynomial(GCS_DataLink.Transmitter.ChannelEncoder.Randomizer.Poly);
        CCSDS_void_Randomizer(Frame,FrameLength);
    }

    //Synch Word
    u8 S[GCS_DataLink.Transmitter.ProtocolSynchWord.Length];
    for(u8 i = 0;i<GCS_DataLink.Transmitter.ProtocolSynchWord.Length;i++){
        S[i] = GCS_DataLink.Transmitter.ProtocolSynchWord.SynchWord;
    }
    Frame = (u8*)realloc(Frame,FrameLength+2*GCS_DataLink.Transmitter.ProtocolSynchWord.Length);
    memcpy(Frame+GCS_DataLink.Transmitter.ProtocolSynchWord.Length,Frame,FrameLength);
    memcpy(Frame,S,GCS_DataLink.Transmitter.ProtocolSynchWord.Length);
    memcpy(Frame+FrameLength+GCS_DataLink.Transmitter.ProtocolSynchWord.Length,S,GCS_DataLink.Transmitter.ProtocolSynchWord.Length);
    FrameLength += 2*GCS_DataLink.Transmitter.ProtocolSynchWord.Length;

    /***Send Frame to Module*/
    /***Frame Sent*/

    u8 LocalResponse = 0;
    switch(FrameType){
    case 1:
        LocalResponse = CCSDS_FOP_AD_ACCEPT;
        break;
    case 2:
        LocalResponse = CCSDS_FOP_BD_ACCEPT;
        break;
    case 3:
        LocalResponse = CCSDS_FOP_BC_ACCEPT;
        break;
    default:
        break;
    }
    if(FrameType != 0)
        CCSDS_void_FOP(CCSDS_FOP_RECEIVE_RESPONSEFROM_LOWER_PROCEDURE,VCID,&LocalResponse,0,0);


}


u32 GetKissLength(u8* KissFrame){

    if(KissFrame[0] != 0xc0)
        return 0;
    u32 i=1;
    while(KissFrame[i] != 0xc0)
        i++;
    i++;
    return i;

}


u32 IntFromArray(u8* S){
    int i = 0,len = strlen((char*)S)-1;
    u32 N = 0;
    while(S[i]!=0){
        N += (S[len-i]-'0') * pow(10,i);
        i++;
    }
    return N;
}


u32 GetNumberFromString(u8* S)
{
    u8 N[15];
    int i = 0;
    while(S[i] != '\n'){
        N[i] = S[i];
        i++;
    }
    N[i] = 0;
    u32 V = (u32)IntFromArray(N);
    return V;
}


void DataLink_voidInit(void){

    int ID1 = UART_voidInit(Configport,9600);
    int ID2 = UART_voidInit(Dataport,115200);

    pthread_t ConfigPortThread, DataPortThread;

    pthread_create(&ConfigPortThread,NULL,ConfigReceiver,&ID1);
    pthread_create(&DataPortThread,NULL,DataReceiver,&ID2);

    GCS_DataLink.Transmitter.BaudRate = 0;
    GCS_DataLink.Transmitter.ChannelEncoder.Golay.Flag = DATALINK_DISABLE;
    GCS_DataLink.Transmitter.ChannelEncoder.PCHFlag = DATALINK_DISABLE;
    GCS_DataLink.Transmitter.ChannelEncoder.Randomizer.Flag = DATALINK_DISABLE;
    GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.Flag = DATALINK_DISABLE;
    GCS_DataLink.Transmitter.Frequency = 0;
    GCS_DataLink.Transmitter.KissFlag = DATALINK_DISABLE;
    GCS_DataLink.Transmitter.ModulationIndex = 0;
    GCS_DataLink.Transmitter.ModulatorSynchWord.Flag = DATALINK_DISABLE;
    GCS_DataLink.Transmitter.ProtocolSynchWord.Flag = DATALINK_DISABLE;

    GCS_DataLink.Receiver.BaudRate = 0;
    GCS_DataLink.Receiver.ChannelEncoder.Golay.Flag = DATALINK_DISABLE;
    GCS_DataLink.Receiver.ChannelEncoder.PCHFlag = DATALINK_DISABLE;
    GCS_DataLink.Receiver.ChannelEncoder.Randomizer.Flag = DATALINK_DISABLE;
    GCS_DataLink.Receiver.ChannelEncoder.ReedSolomon.Flag = DATALINK_DISABLE;
    GCS_DataLink.Receiver.Frequency = 0;
    GCS_DataLink.Receiver.KissFlag = DATALINK_DISABLE;
    GCS_DataLink.Receiver.ModulationIndex = 0;
    GCS_DataLink.Receiver.ModulatorSynchWord.Flag = DATALINK_DISABLE;
    GCS_DataLink.Receiver.ProtocolSynchWord.Flag = DATALINK_DISABLE;

}


void DataLink_voidRun(void){

    CC1101_voidInit(GCS_DataLink.Transmitter.Frequency, GCS_DataLink.Transmitter.ModulationType,GCS_DataLink.Transmitter.ModulationIndex,GCS_DataLink.Transmitter.BaudRate);


    if(GCS_DataLink.Transmitter.UsedProtocol == DATALINK_AX25){

        AX25Timer_voidSetTime(AX25_TIMER_T1,GCS_DataLink.Transmitter.Config.AX.AckTimer);
        N1 = GCS_DataLink.Transmitter.Config.AX.N1;
        N2 = GCS_DataLink.Transmitter.Config.AX.N2;
        memcpy(MYAddress,GCS_DataLink.Transmitter.Config.AX.SrcAddress,7);
        MySSID = GCS_DataLink.Transmitter.Config.AX.SrcSSID;
        memcpy(DestAddress,GCS_DataLink.Transmitter.Config.AX.DestAddress,7);
        DestSSID = GCS_DataLink.Transmitter.Config.AX.DestSSID;
        DL_voidSetControlBits(GCS_DataLink.Transmitter.Config.AX.ControlFieldLength);

        DL_voidInit();
        pthread_t DL_Thread, ReceiverThread, Commands_Thread;


    }
    else if(GCS_DataLink.Transmitter.UsedProtocol == DATALINK_CCSDS)
    {
        u8 DirectiveType = CCSDS_SET_FOP_SILDING_WINDOW;
        CCSDS_void_FOPINIT();
        CCSDS_void_FOPconfig(GCS_DataLink.Transmitter.Config.CC.SCID);
        CCSDS_void_FOP(CCSDS_FOP_RECEIVE_DIRCTIVEFROM_MANGEMENT_FUNCTION,DATALINK_VCID,&DirectiveType,&GCS_DataLink.Transmitter.Config.CC.FopSlidingWindow,NULL);
        DirectiveType = CCSDS_SET_T1_INITIAL;
        CCSDS_void_FOP(CCSDS_FOP_RECEIVE_DIRCTIVEFROM_MANGEMENT_FUNCTION,DATALINK_VCID,&DirectiveType,&GCS_DataLink.Transmitter.Config.CC.T1Initial,NULL);
        DirectiveType = CCSDS_SET_TRANSMISSION_LIMIT;
        CCSDS_void_FOP(CCSDS_FOP_RECEIVE_DIRCTIVEFROM_MANGEMENT_FUNCTION,DATALINK_VCID,&DirectiveType,&GCS_DataLink.Transmitter.Config.CC.TransmissionLimit,NULL);

    }

}






void Parse(u8* ConfigArray){

    //u32 Len = GetKissLength(ConfigArray);

    //ConfigArray = deframing_function (ConfigArray,&Len);
    u32 Len = strlen((char*)ConfigArray);
    if(ConfigArray[0] == '0'){        //Command


    }
    else if(ConfigArray[0] == '1'){
        u32 i = 1;
        u8 PolyIndex = 0;
        while(i < Len){
            switch (ConfigArray[i]){
            case 'A':   //Data Link Transmitter
                i +=2;
                if(ConfigArray[i] == 'A')
                    GCS_DataLink.Transmitter.UsedProtocol = DATALINK_AX25;
                else if(ConfigArray[i] == 'C')
                    GCS_DataLink.Transmitter.UsedProtocol = DATALINK_CCSDS;
                else{
                    #if DATALINK_DEBUG == 1
                    printf("Error At Transmitting protocol\n");
                    #endif // DATALINK_DEBUG
                }
                break;
            case 'B':
                i +=2;
                if(ConfigArray[i] == 'R')//RS Encoder
                    GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.Flag =  DATALINK_ENABLE;
                else if(ConfigArray[i] == 'B')
                    GCS_DataLink.Transmitter.ChannelEncoder.PCHFlag = DATALINK_ENABLE;
                else
                {
                    #if DATALINK_DEBUG == 1
                    printf("Error at transmitter Channel Encoder\n");
                    #endif // DATALINK_DEBUG
                }
                break;
            case 'C':
                i +=2;
                GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.InitialRoot=(u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'D':
                i +=2;
                PolyIndex = (u8)GetNumberFromString(ConfigArray+i)-1;
                GCS_DataLink.Transmitter.ChannelEncoder.ReedSolomon.Poly=RS_Polys[PolyIndex];
                break;
            case 'E':
                i+=2;
                PolyIndex = (u8)GetNumberFromString(ConfigArray+i)-1;
                GCS_DataLink.Transmitter.ChannelEncoder.Golay.Flag = DATALINK_ENABLE;
                GCS_DataLink.Transmitter.ChannelEncoder.Golay.Poly = Golay_Polys[PolyIndex];
                break;
            case 'F':
                i +=2;
                GCS_DataLink.Transmitter.ChannelEncoder.Randomizer.Poly = (u8)GetNumberFromString(ConfigArray+i);
                GCS_DataLink.Transmitter.ChannelEncoder.Randomizer.Flag = DATALINK_ENABLE;
                break;
            case 'G':
                i +=2;
                GCS_DataLink.Transmitter.ProtocolSynchWord.Flag = DATALINK_ENABLE;
                GCS_DataLink.Transmitter.ProtocolSynchWord.SynchWord = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'H':
                i+=2;
                GCS_DataLink.Transmitter.ProtocolSynchWord.Length = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'I':
                i+=2;
                GCS_DataLink.Transmitter.KissFlag = DATALINK_ENABLE;
                break;
            case 'J':   //Data Link Transmitter
                i +=2;
                if(ConfigArray[i] == 'A')
                    GCS_DataLink.Receiver.UsedProtocol = DATALINK_AX25;
                else if(ConfigArray[i] == 'C')
                    GCS_DataLink.Receiver.UsedProtocol = DATALINK_CCSDS;
                else{
                    #if DATALINK_DEBUG == 1
                    printf("Error At Transmitting protocol\n")
                    #endif // DATALINK_DEBUG
                }
                break;
            case 'K':
                i +=2;
                if(ConfigArray[i] == 'R')//RS Encoder
                    GCS_DataLink.Receiver.ChannelEncoder.ReedSolomon.Flag =  DATALINK_ENABLE;
                else if(ConfigArray[i] == 'B')
                    GCS_DataLink.Receiver.ChannelEncoder.PCHFlag = DATALINK_ENABLE;
                else
                {
                    #if DATALINK_DEBUG == 1
                    printf("Error at transmitter Channel Encoder\n");
                    #endif // DATALINK_DEBUG
                }
                break;
            case 'L':
                i +=2;
                GCS_DataLink.Receiver.ChannelEncoder.ReedSolomon.InitialRoot=(u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'M':
                i +=2;
                PolyIndex = (u8)GetNumberFromString(ConfigArray+i)-1;
                GCS_DataLink.Receiver.ChannelEncoder.ReedSolomon.Poly=RS_Polys[PolyIndex];
                break;
            case 'N':
                i+=2;
                PolyIndex = (u8)GetNumberFromString(ConfigArray+i)-1;
                GCS_DataLink.Receiver.ChannelEncoder.Golay.Flag = DATALINK_ENABLE;
                GCS_DataLink.Receiver.ChannelEncoder.Golay.Poly = Golay_Polys[PolyIndex];
                break;
            case 'O':
                i +=2;
                GCS_DataLink.Receiver.ChannelEncoder.Randomizer.Poly = (u8)GetNumberFromString(ConfigArray+i);
                GCS_DataLink.Receiver.ChannelEncoder.Randomizer.Flag = DATALINK_ENABLE;
                break;
            case 'P':
                i +=2;
                GCS_DataLink.Receiver.ProtocolSynchWord.Flag = DATALINK_ENABLE;
                GCS_DataLink.Receiver.ProtocolSynchWord.SynchWord = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'Q':
                i+=2;
                GCS_DataLink.Receiver.ProtocolSynchWord.Length = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'R':
                i+=2;
                GCS_DataLink.Receiver.KissFlag = DATALINK_ENABLE;
                break;
            case 'S':
                i+=2;
                GCS_DataLink.Transmitter.Frequency = GetNumberFromString(ConfigArray+i);
                break;
            case 'T':
                i+=2;
                GCS_DataLink.Transmitter.BaudRate = GetNumberFromString(ConfigArray+i);
                break;
            case 'U':
                i+=2;
                GCS_DataLink.Transmitter.ModulatorSynchWord.Flag = DATALINK_ENABLE;
                GCS_DataLink.Transmitter.ModulatorSynchWord.SynchWord = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'V':
                i+=2;
                GCS_DataLink.Transmitter.ModulatorSynchWord.Length = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'W':
                i+=2;
                switch(ConfigArray[i]){
                    case 'G':
                        GCS_DataLink.Transmitter.ModulationType = DATALINK_MOD_GFSK;
                        break;
                    case 'P':
                        GCS_DataLink.Transmitter.ModulationType = DATALINK_MOD_PFSK;
                        break;
                    case 'M':
                        GCS_DataLink.Transmitter.ModulationType = DATALINK_MOD_MFSK;
                        break;
                    case 'Q':
                        GCS_DataLink.Transmitter.ModulationType = DATALINK_MOD_QAM;
                        break;
                    default:
                        #if DATALINK_DEBUG == 1
                        printf("Error at Modulation Index in Transmitter\n");
                        #endif // DATALINK_DEBUG
                        break;
                }
                break;
            case 'X':
                i+=2;
                GCS_DataLink.Transmitter.ModulationIndex = (f32)GetNumberFromString(ConfigArray+i)/10;
                break;
             case 'Y':
                i+=2;
                GCS_DataLink.Receiver.Frequency = GetNumberFromString(ConfigArray+i);
                break;
            case 'Z':
                i+=2;
                GCS_DataLink.Receiver.BaudRate = GetNumberFromString(ConfigArray+i);
                break;
            case 'a':
                i+=2;
                GCS_DataLink.Receiver.ModulatorSynchWord.Flag = DATALINK_ENABLE;
                GCS_DataLink.Receiver.ModulatorSynchWord.SynchWord = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'b':
                i+=2;
                GCS_DataLink.Receiver.ModulatorSynchWord.Length = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'c':
                i+=2;
                switch(ConfigArray[i]){
                    case 'G':
                        GCS_DataLink.Receiver.ModulationType = DATALINK_MOD_GFSK;
                        break;
                    case 'P':
                        GCS_DataLink.Receiver.ModulationType = DATALINK_MOD_PFSK;
                        break;
                    case 'M':
                        GCS_DataLink.Receiver.ModulationType = DATALINK_MOD_MFSK;
                        break;
                    case 'Q':
                        GCS_DataLink.Receiver.ModulationType = DATALINK_MOD_QAM;
                        break;
                    default:
                        #if DATALINK_DEBUG == 1
                        printf("Error at Modulation Index in Transmitter\n");
                        #endif // DATALINK_DEBUG
                        break;
                }
                break;
            case 'd':
                i+=2;
                GCS_DataLink.Receiver.ModulationIndex = (f32)GetNumberFromString(ConfigArray+i)/10;
                break;
            case 'e':
                i+=2;
                GCS_DataLink.Transmitter.Config.CC.SCID = (u16)GetNumberFromString(ConfigArray+i);
                break;
            case 'f':
                i+=2;
                GCS_DataLink.Transmitter.Config.CC.FopSlidingWindow = GetNumberFromString(ConfigArray+i);
                break;
            case 'g':
                i+=2;
                GCS_DataLink.Transmitter.Config.CC.TransmissionLimit = GetNumberFromString(ConfigArray+i);
                break;
            case 'h':
                i+=2;
                GCS_DataLink.Transmitter.Config.CC.T1Initial = GetNumberFromString(ConfigArray+i);
                break;
            case 'i':
                i+=2;
                memcpy(GCS_DataLink.Transmitter.Config.AX.SrcAddress,ConfigArray+i,6);
                GCS_DataLink.Transmitter.Config.AX.SrcAddress[6] = 0;
                break;
            case 'j':
                i+=2;
                memcpy(GCS_DataLink.Transmitter.Config.AX.DestAddress,ConfigArray+i,6);
                GCS_DataLink.Transmitter.Config.AX.DestAddress[6] = 0;
                break;
            case 'k':
                i+=2;
                GCS_DataLink.Transmitter.Config.AX.SrcSSID = (u8)GetNumberFromString(ConfigArray+i);
                break;

            case 'l':
                i+=2;
                GCS_DataLink.Transmitter.Config.AX.DestSSID = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'm':
                i+=2;
                GCS_DataLink.Transmitter.Config.AX.N1 = GetNumberFromString(ConfigArray+i);
                break;
            case 'n':
                i+=2;
                GCS_DataLink.Transmitter.Config.AX.N2 = GetNumberFromString(ConfigArray+i);
                break;
            case 'o':
                i+=2;
                if(ConfigArray[i] == 'f')
                    GCS_DataLink.Transmitter.Config.AX.TransmissionMode = DATALINK_AX25_FULLDUPLEX;
                else if(ConfigArray[i] == 'h')
                    GCS_DataLink.Transmitter.Config.AX.TransmissionMode = DATALINK_AX25_HALFDUPLEX;
                else{
                    #if DATALINK_DEBUG == 1
                    printf("Error at transmitter Transmission mode AX25\n");
                    #endif // DATALINK_DEBUG
                }
                break;
            case 'p':
                i+=2;
                GCS_DataLink.Transmitter.Config.AX.ControlFieldLength = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'q':
                i+=2;
                GCS_DataLink.Transmitter.Config.AX.AckTimer = GetNumberFromString(ConfigArray+i);
                break;
            case 'r':
                i+=2;
                GCS_DataLink.Receiver.Config.CC.SCID = (u16)GetNumberFromString(ConfigArray+i);
                break;
            case 's':
                i+=2;
                GCS_DataLink.Receiver.Config.CC.FopSlidingWindow = GetNumberFromString(ConfigArray+i);
                break;

            case 't':
                i+=2;
                GCS_DataLink.Receiver.Config.CC.TransmissionLimit = GetNumberFromString(ConfigArray+i);
                break;

            case 'u':
                i+=2;
                GCS_DataLink.Receiver.Config.CC.T1Initial = GetNumberFromString(ConfigArray+i);
                break;
            case 'v':
                i+=2;
                memcpy(GCS_DataLink.Receiver.Config.AX.SrcAddress,ConfigArray+i,6);
                GCS_DataLink.Receiver.Config.AX.SrcAddress[6] = 0;
                break;
            case 'w':
                i+=2;
                memcpy(GCS_DataLink.Receiver.Config.AX.DestAddress,ConfigArray+i,6);
                GCS_DataLink.Receiver.Config.AX.DestAddress[6] = 0;
                break;
            case 'x':
                i+=2;
                GCS_DataLink.Receiver.Config.AX.SrcSSID = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'y':
                i+=2;
                GCS_DataLink.Receiver.Config.AX.DestSSID = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case 'z':
                i+=2;
                GCS_DataLink.Receiver.Config.AX.N1 = GetNumberFromString(ConfigArray+i);
                break;
            case '!':
                i+=2;
                GCS_DataLink.Receiver.Config.AX.N2 = GetNumberFromString(ConfigArray+i);
                break;
            case '@':
                i+=2;
                if(ConfigArray[i] == 'f')
                    GCS_DataLink.Receiver.Config.AX.TransmissionMode = DATALINK_AX25_FULLDUPLEX;
                else if(ConfigArray[i] == 'h')
                    GCS_DataLink.Receiver.Config.AX.TransmissionMode = DATALINK_AX25_HALFDUPLEX;
                else{
                    #if DATALINK_DEBUG == 1
                    printf("Error at transmitter Transmission mode AX25\n");
                    #endif // DATALINK_DEBUG
                }
                break;
            case '#':
                i+=2;
                GCS_DataLink.Receiver.Config.AX.ControlFieldLength = (u8)GetNumberFromString(ConfigArray+i);
                break;
            case '$':
                i+=2;
                GCS_DataLink.Receiver.Config.AX.AckTimer = GetNumberFromString(ConfigArray+i);
                break;
            }
            while(ConfigArray[i++] != '\n');
        }




    }
}








