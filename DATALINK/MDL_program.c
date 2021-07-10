/*
 * MDL_program.c
 *
 *  Created on: Jan 21, 2021
 *      Author: MohammedGamalEleish
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "AX25_interface.h"
//#include "PH_interface.h"
//#include "PH_flags.h"
#include "Timer_interface.h"
#include "Timer_flags.h"

#include "PRINT_interface.h"

#include "DL_interface.h"
#include "DL_flags.h"


#include "MDL_interface.h"
#include "MDL_private.h"
#include "MDL_flags.h"

void MDL_voidSetAcceptableValues(u8 MDL_Patameter, u32 Value){

    switch(MDL_Patameter){

        case MDL_TRANSMISSION_MODE:
            AcceptTransmission = Value;
            break;
        case MDL_REJECT_TYPE:
            AcceptReject = Value;
            break;
        case MDL_CONTROL_MODULO_TYPE:
            AcceptModulo = Value;
            break;
        case MDL_RX_MAX_LEN:
            AcceptRXLen = Value;
            break;
        case MDL_RX_K:
            AcceptRXWindow = Value;
            break;
        case MDL_TX_MAX_LEN:
            AcceptTXLen = Value;
            break;
        case MDL_TX_K:
            AcceptTXWindow = Value;
            break;
        case MDL_ACK_TIMER_T1:
            AcceptACKTimer = Value;
            break;
        case MDL_RETRY_COUNT:
            AcceptRetries = Value;
            break;

        default:
            break;

    }

}

/*************************Main function***************************/
void SM_voidManagmentDataLink(void){

    switch(MDL_CurrentState){

    case SM_MDL_STATE_READY:
        MDL_ReadyState();
        break;
    case SM_MDL_STATE_NEGOTIATING:
        MDL_NegotiatingState();
        break;
    default:
        break;
    }
}



/*************************State Functions*************************/
void MDL_ReadyState(void)
{
    #if DEBUGGING == 1
    printf("In MDL Ready State (Management Data-link ready state)\n");
    #endif // DEBUGGING
    u8 TrasmissionMode, RejectType, ControlModulo, TxWindowSize, RxWindowSize;
    u64 MaxTxLength, MaxRxLength;
    u32 AckTimer, Retries;
    if(MDL_Request.NegotiateRequest == MDL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Request is Negotiate Request (Management Data-link ready state)\n");
        #endif // DEBUGGING
        if(MDL_NegotiateRequest.TransmissionMode != AX25_XID_ABSENT)
        {
            TrasmissionMode = MDL_NegotiateRequest.TransmissionMode;
            #if DEBUGGING == 1
            printf("Transmission Mode was found (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        else
        {
            TrasmissionMode = AX25_XID_ABSENT;
            #if DEBUGGING == 1
            printf("Transmission Mode was Absent (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        if((MDL_NegotiateRequest.RejectType != AX25_XID_ABSENT) || (MDL_NegotiateRequest.ControlModulo != AX25_XID_ABSENT))
        {
            #if DEBUGGING == 1
            printf("Reject Type or Control Modulo were found (Management Data-link ready state)\n");
            #endif // DEBUGGING
            if(MDL_NegotiateRequest.RejectType != AX25_XID_ABSENT)
            {
                RejectType = MDL_NegotiateRequest.RejectType;
                #if DEBUGGING == 1
                printf("Reject Type was found (Management Data-link ready state)\n");
                #endif // DEBUGGING
            }
            else
            {
                RejectType = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf("Reject Type was Absent (Management Data-link ready state)\n");
                #endif // DEBUGGING
            }
            if(MDL_NegotiateRequest.ControlModulo != AX25_XID_ABSENT)
            {
                ControlModulo = MDL_NegotiateRequest.ControlModulo;
                #if DEBUGGING == 1
                printf("Control Modulo was found (Management Data-link ready state)\n");
                #endif // DEBUGGING
            }
            else
            {
                ControlModulo = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf("Control Modulo was Absent (Management Data-link ready state)\n");
                #endif // DEBUGGING
            }
        }
        else
        {
            RejectType = AX25_XID_ABSENT;
            ControlModulo = AX25_XID_ABSENT;
            #if DEBUGGING == 1
            printf("Reject Type or Control Modulo were Absent (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        if(MDL_NegotiateRequest.N1 != AX25_XID_ABSENT)
        {
            MaxRxLength = MDL_NegotiateRequest.N1;
            #if DEBUGGING == 1
            printf("MaxRxLength was found (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        else
        {
            MaxRxLength = AX25_XID_ABSENT;
            #if DEBUGGING == 1
            printf("MaxRxLength was Absent (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        if(MDL_NegotiateRequest.K != AX25_XID_ABSENT)
        {
            RxWindowSize = MDL_NegotiateRequest.K;
            #if DEBUGGING == 1
            printf("RxWindowSize was found (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        else
        {
            RxWindowSize = AX25_XID_ABSENT;
            #if DEBUGGING == 1
            printf("RxWindowSize was Absent (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        if(MDL_NegotiateRequest.Retry != AX25_XID_ABSENT)
        {
            Retries = MDL_NegotiateRequest.Retry;
            #if DEBUGGING == 1
            printf("Retries was found (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        else
        {
            Retries = AX25_XID_ABSENT;
            #if DEBUGGING == 1
            printf("Retries was Absent (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        if(MDL_NegotiateRequest.T1 != AX25_XID_ABSENT)
        {
            AckTimer = MDL_NegotiateRequest.T1;
            #if DEBUGGING == 1
            printf("AckTimer was found (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        else
        {
            AckTimer = AX25_XID_ABSENT;
            #if DEBUGGING == 1
            printf("AckTimer was Absent (Management Data-link ready state)\n");
            #endif // DEBUGGING
        }
        MDL_Internal.RC =0;
        AX25Timer_voidStart(AX25_TIMER_TR210);
        MaxTxLength = AX25_XID_ABSENT;
        TxWindowSize = AX25_XID_ABSENT;
        XIDInfo_t* XIDInfo = AX25_XIDInfo_tPtrConstructXIDInfo(TrasmissionMode, RejectType, ControlModulo, MaxTxLength, MaxRxLength, TxWindowSize, RxWindowSize, AckTimer, Retries);
        frame_t* XIDFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Exchange_Identification, XIDInfo->XIDInfo,XIDInfo->XIDInfoLength);
        Transmit(XIDFrame->PtrtoFrame,XIDFrame->FrameLength);
        TransmittedFrame = XIDFrame;
        //AX25_voidFreeFrame(XIDFrame);
        //DLtoPhysical = XIDFrame;
        //PH_Request.DataRequest = PH_FLAG_SET;
        MDL_CurrentState = SM_MDL_STATE_NEGOTIATING;
        MDL_Request.NegotiateRequest = MDL_FLAG_CLEAR;
    }
    else if(Hardware.Frame == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("PH_Indication => Data Indication was Set (Management Data-link ready state)\n");
        #endif // DEBUGGING
        frame_t* Frame = HardwaretoDL;
        if(0);
        else
        {
            #if DEBUGGING == 1
            printf("DL Check Error isn't '0' => 'Free Frame' (Management Data-link ready state)\n");
            #endif // DEBUGGING
            if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
            {
                #if DEBUGGING == 1
                printf("Frame is U Frame and Exchange Identification (Management Data-link ready state)\n");
                #endif // DEBUGGING
                if(Frame->CMDRES == AX25_COMMAND_FRAME)
                {
                    #if DEBUGGING == 1
                    printf("Frame is Command frame (Management Data-link ready state)\n");
                    #endif // DEBUGGING
                    if(Frame->PollFinal == 1)
                    {
                        #if DEBUGGING == 1
                        printf("Frame poll final is '1' (Management Data-link ready state)\n");
                        #endif // DEBUGGING
                        XIDInfo_t* XIDInfo = AX25_XIDInfo_tPtrDeconstructXIDInfo(Frame->Info, Frame->InfoLength);
                        if(GET_BIT(XIDInfo->State, 0) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("Classes of Procedure was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            /*Classes of Procedure*/
                            if(((XIDInfo->ClassesOfProcedures>>5) & 0x3) == 2 && AcceptTransmission == MDL_FULL_DUPLEX)/*Full Duplex Acceptable*/
                            {
                                /*****************************************/
                                /**Set The Hardware to run on full duplex*/
                                /*****************************************/
                                TrasmissionMode = AX25_XID_FULLDUPLEX;
                                #if DEBUGGING == 1
                                printf("Full Duplex is Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                /*****************************************/
                                /**Set The Hardware to run on half duplex*/
                                /*****************************************/
                                TrasmissionMode = AX25_XID_HALFDUPLEX;
                                #if DEBUGGING == 1
                                printf("Half Duplex is Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                        }
                        else
                        {
                            /*****************************************/
                            /**Set The Hardware to run on full duplex*/
                            /*****************************************/
                           TrasmissionMode = AX25_XID_ABSENT;
                           #if DEBUGGING == 1
                            printf("Classes of Procedure was Absent => Transmission mode was Absent (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        if(GET_BIT(XIDInfo->State, 1) == 1)
                        {
                            /*HDLC Optional Functions*/
                            #if DEBUGGING == 1
                            printf("HDLC Optional Functions was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptModulo == MDL_MOD_16)/*Modulo 128 Acceptable*/
                            {
                                /*************************************/
                                /**Set Software to use 16 bit control*/
                                /*************************************/
                                AX25_voidSetControlModulo(AX25_16BITS);
                                ControlModulo = AX25_16BITS;
                                #if DEBUGGING == 1
                                printf(" Modulo 128 '16' was Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                /*************************************/
                                /**Set Software to use 16 bit control*/
                                /*************************************/
                                AX25_voidSetControlModulo(AX25_8BITS);
                                ControlModulo = AX25_8BITS;
                                #if DEBUGGING == 1
                                printf(" Modulo 8 was Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            if(AcceptReject == MDL_SEL_REJ_REJ)/*SREJ / REJ Acceptable*/
                            {
                                /*************************************/
                                /**Set Software to use SEL REJ REJ   */
                                /*************************************/
                                RejectType = AX25_XID_SELECTIVE_REJECT_REJECT;
                                #if DEBUGGING == 1
                                printf("SREJ / REJ was Acceptable so => Reject Type was Selective Reject Reject (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                #if DEBUGGING == 1
                                printf("SREJ / REJ wasn't Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                                if(AcceptReject == MDL_SEL_REJ)/*SREJ Acceptable*/
                                {
                                    /*************************************/
                                    /**Set Software to Selective Reject  */
                                    /*************************************/
                                    SREJ_Enable = DL_FLAG_SET;
                                    RejectType = AX25_XID_SELECTIVE_REJECT;
                                    #if DEBUGGING == 1
                                    printf("SREJ was Acceptable so => Reject Type was Selective Reject (Management Data-link ready state)\n");
                                    #endif // DEBUGGING
                                }
                                else
                                {
                                    /*************************************/
                                    /**Set Software to use 16 bit control*/
                                    /*************************************/
                                    SREJ_Enable = DL_FLAG_CLEAR;
                                    RejectType = AX25_XID_IMPLICIT_REJECT;
                                    #if DEBUGGING == 1
                                    printf("SREJ wasn't Acceptable so => Reject Type was Implicit Reject (Management Data-link ready state)\n");
                                    #endif // DEBUGGING
                                }
                            }
                        }
                        else
                        {
                            /*************************************/
                            /**Set Software to use 8 bit control
                              With Selective Reject              */
                            /*************************************/
                            SREJ_Enable = DL_FLAG_SET;
                            ControlModulo = AX25_8BITS;
                            RejectType = AX25_XID_ABSENT;
                            ControlModulo = AX25_XID_ABSENT;
                            #if DEBUGGING == 1
                            printf("HDLC Optional Functions was Absent so => Reject Type was Absent and Control Modulo was Absent too (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        /////////////////////////////////////
                        if(GET_BIT(XIDInfo->State, 2) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("TX Length was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(XIDInfo->MaxRXIFieldLength <= AcceptTXLen)/*TX N1 Acceptable*/
                            {
                                N1 = XIDInfo->MaxTXIFieldLength;
                                MaxRxLength = AcceptRXLen;//XIDInfo->MaxTXIFieldLength;
                                #if DEBUGGING == 1
                                printf("TX Length was Acceptable so => put this value in MaxRXLength (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {

                                MaxRxLength = AcceptRXLen;//AX25_XID_I_TX_DEFAULT;
                                N1 = AX25_XID_I_TX_DEFAULT;
                                #if DEBUGGING == 1
                                printf("TX Length wasn't Acceptable so => put default value in MaxRXLength (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                        }
                        else
                        {
                            /*maybe default not absent*/
                            MaxRxLength = AX25_XID_ABSENT;
                            N1 = AX25_XID_I_TX_DEFAULT;
                            #if DEBUGGING == 1
                            printf("TX Length wasn't found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        if(GET_BIT(XIDInfo->State, 4) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("TX window size was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptTXWindow >= XIDInfo->RXWindowsize)/*New K Acceptable*/
                            {
                                K = XIDInfo->RXWindowsize;
                                RxWindowSize = AcceptRXWindow;//XIDInfo->TXWindowsize;
                                #if DEBUGGING == 1
                                printf("TX window size was Acceptable so => put this value in RXWindowSize (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                #if DEBUGGING == 1
                                printf("TX window size wasn't Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                                if(ControlModulo == AX25_8BITS)
                                {
                                    RxWindowSize = AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO8;
                                    #if DEBUGGING == 1
                                    printf("Control Modulo was 8 bits so => put default value for modulo 8 in RXWindowSize (Management Data-link ready state)\n");
                                    #endif // DEBUGGING
                                }
                                else
                                {
                                    RxWindowSize = AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO128;
                                    #if DEBUGGING == 1
                                    printf("Control Modulo was 128 '16' bits so => put default value for modulo 128 '16' in RXWindowSize (Management Data-link ready state)\n");
                                    #endif // DEBUGGING
                                }
                            }
                        }
                        else
                        {
                            /*maybe default not absent*/
                            RxWindowSize = AX25_XID_ABSENT;
                            #if DEBUGGING == 1
                            printf("TX window size wasn't found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        if(GET_BIT(XIDInfo->State, 7) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("Retries was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptRetries >= XIDInfo->RetryCount)/*New N2 Acceptable*/
                            {
                                MDL_Internal.NM201 =  XIDInfo->RetryCount;
                                Retries = XIDInfo->RetryCount;
                                #if DEBUGGING == 1
                                printf("Retries was Acceptable so => Retries = Retry Count (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                MDL_Internal.NM201 = AX25_XID_DEFAULT_RETRIES;
                                Retries = AX25_XID_DEFAULT_RETRIES;
                                #if DEBUGGING == 1
                                printf("Retries wasn't Acceptable so => Retries = Default value (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                        }
                        else
                        {
                            Retries = AX25_XID_ABSENT;
                            #if DEBUGGING == 1
                            printf("Retries wasn't found so => Retries = Retry Count (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        if(GET_BIT(XIDInfo->State, 6) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("Ack Timer was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptACKTimer >= XIDInfo->ACKTimer)/*New T1 Acceptable*/
                            {
                                AX25Timer_voidSetTime(AX25_TIMER_T1,XIDInfo->ACKTimer);
                                AckTimer = XIDInfo->ACKTimer;
                                #if DEBUGGING == 1
                                printf("Ack Timer was Acceptable so => Ack Timer = Ack Timer (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                AX25Timer_voidSetTime(AX25_TIMER_T1,AX25_XID_DEFAULT_ACK_TIMER);
                                AckTimer = AX25_XID_DEFAULT_ACK_TIMER;
                                #if DEBUGGING == 1
                                printf("Ack Timer wasn't Acceptable so => Ack Timer = Default value (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                        }
                        else
                        {
                            /*maybe default value not absent*/
                            AckTimer = AX25_XID_ABSENT;
                            #if DEBUGGING == 1
                            printf("Ack Timer wasn't found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        /*check if TX Length and TX windowsize are absent or no*/
                        ////////////////////////////////////////////
                        MaxTxLength = AX25_XID_ABSENT;
                        TxWindowSize = AX25_XID_ABSENT;
                        ////////////////////////////////////////////
                        XIDInfo_t* SendXIDInfo = AX25_XIDInfo_tPtrConstructXIDInfo(TrasmissionMode, RejectType, ControlModulo, MaxTxLength, MaxRxLength, TxWindowSize,RxWindowSize, AckTimer, Retries);
                        frame_t* SendFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_RESPONSE_FRAME, 1, Exchange_Identification, SendXIDInfo->XIDInfo, SendXIDInfo->XIDInfoLength);
                        Transmit(SendFrame->PtrtoFrame,SendFrame->FrameLength);
                        AX25_voidFreeFrame(SendFrame);
                        //DLtoPhysical = SendFrame;
                        //PH_Request.DataRequest = PH_FLAG_SET;
                        MDL_CurrentState = SM_MDL_STATE_READY;
                    }
                    else
                    {
                        MDL_Indication.ErrorIndicate = MDL_A;
                        MDL_CurrentState = SM_MDL_STATE_READY;
                        #if DEBUGGING == 1
                        printf("Frame poll final isn't '1' (Management Data-link ready state)\n");
                        #endif // DEBUGGING
                    }
                }
                else
                {
                    MDL_Indication.ErrorIndicate = MDL_B;
                    MDL_CurrentState = SM_MDL_STATE_READY;
                    #if DEBUGGING == 1
                    printf("Frame isn't Command frame (Management Data-link ready state)\n");
                    #endif // DEBUGGING
                }
            }
            else
            {
                //AX25_voidFreeFrame(Frame);
                #if DEBUGGING == 1
                printf("Frame isn't U Frame and Exchange Identification so => 'Free Frame'(Management Data-link ready state)\n");
                #endif // DEBUGGING
            }
        }
        Hardware.Frame = DL_FLAG_CLEAR;
        //AX25_voidFreeFrame(Frame);
    }
    /*Clear Flags Dude*/
}

void MDL_NegotiatingState(void)
{
    #if DEBUGGING == 1
    printf("In MDL Negotiating State (Management Data-link ready state)\n");
    #endif // DEBUGGING
    //u8 TrasmissionMode, RejectType, ControlModulo, TxWindowSize, RxWindowSize;
    //u64 MaxTxLength, MaxRxLength;
    //u32 AckTimer, Retries;
    u8 ControlModulo = AX25_8BITS;

    if(MDL_Request.NegotiateRequest == MDL_FLAG_SET)
    {
        MDL_Request.NegotiateRequest = MDL_FLAG_CLEAR;
        /*ReQueue*/
        #if DEBUGGING == 1
        printf("MDL Request is Negotiate Request and this Request is Set (Management Data-link negotiating state)\n");
        #endif // DEBUGGING
    }
    else if(Timer.TR210_EXPIRE == MDL_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf("Timer 210 was Expire (Management Data-link negotiating state)\n");
        #endif // DEBUGGING
        MDL_Internal.RC++;
        if(MDL_Internal.RC >MDL_Internal.NM201)
        {
            MDL_Indication.ErrorIndicate = MDL_C;
            MDL_CurrentState = SM_MDL_STATE_READY;
            if(TransmittedFrame != NULL)
                AX25_voidFreeFrame(TransmittedFrame);
            #if DEBUGGING == 1
            printf("Retries count is greater than NM201 (Management Data-link negotiating state)\n");
            #endif // DEBUGGING
        }
        else
        {
            Transmit(TransmittedFrame->PtrtoFrame,TransmittedFrame->FrameLength);
            #if DEBUGGING == 1
            printf("Retries count is smaller than NM201 (Management Data-link negotiating state)\n");
            #endif //
            /*if(MDL_NegotiateRequest.TransmissionMode != AX25_XID_ABSENT)
            {
                TrasmissionMode = MDL_NegotiateRequest.TransmissionMode;
                #if DEBUGGING == 1
                printf("Transmission mode was found so => TrasmissionMode = MDL_NegotiateRequest.TransmissionMode (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            else
            {
                TrasmissionMode = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf("Transmission mode wasn't found so => TrasmissionMode = Absent (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            if((MDL_NegotiateRequest.RejectType != AX25_XID_ABSENT) || (MDL_NegotiateRequest.ControlModulo != AX25_XID_ABSENT))
            {
                #if DEBUGGING == 1
                printf(" Reject Type or Control Modulo were found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
                if(MDL_NegotiateRequest.RejectType != AX25_XID_ABSENT)
                {
                    RejectType = MDL_NegotiateRequest.RejectType;
                    #if DEBUGGING == 1
                    printf("Reject Type was found so => RejectType = MDL_NegotiateRequest.RejectType (Management Data-link negotiating state)\n");
                    #endif // DEBUGGING
                }
                else
                {
                    RejectType = AX25_XID_ABSENT;
                    #if DEBUGGING == 1
                    printf("Reject Type wasn't found so => RejectType = Absent (Management Data-link negotiating state)\n");
                    #endif // DEBUGGING
                }
                if(MDL_NegotiateRequest.ControlModulo != AX25_XID_ABSENT)
                {
                    ControlModulo = MDL_NegotiateRequest.ControlModulo;
                    #if DEBUGGING == 1
                    printf("Control Modulo was found so => ControlModulo = MDL_NegotiateRequest.ControlModulo (Management Data-link negotiating state)\n");
                    #endif // DEBUGGING
                }
                else
                {
                    ControlModulo = AX25_XID_ABSENT;
                    #if DEBUGGING == 1
                    printf("Control Modulo wasn't found so => ControlModulo = Absent (Management Data-link negotiating state)\n");
                    #endif // DEBUGGING
                }
            }
            else
            {
                RejectType = AX25_XID_ABSENT;
                ControlModulo = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf(" Reject Type or Control Modulo weren't found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            if(MDL_NegotiateRequest.N1 != AX25_XID_ABSENT)
            {
                MaxRxLength = MDL_NegotiateRequest.N1;
                #if DEBUGGING == 1
                printf(" TX Length was found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            else
            {
                MaxRxLength = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf(" TX Length wasn't found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            if(MDL_NegotiateRequest.K != AX25_XID_ABSENT)
            {
                RxWindowSize = MDL_NegotiateRequest.K;
                #if DEBUGGING == 1
                printf(" TX window size was found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            else
            {
                RxWindowSize = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf(" TX window size wasn't found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            if(MDL_NegotiateRequest.Retry != AX25_XID_ABSENT)
            {
                Retries = MDL_NegotiateRequest.Retry;
                #if DEBUGGING == 1
                printf(" Retries was found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            else
            {
                Retries = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf(" Retries wasn't found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            if(MDL_NegotiateRequest.T1 != AX25_XID_ABSENT)
            {
                AckTimer = MDL_NegotiateRequest.T1;
                #if DEBUGGING == 1
                printf(" Ack Timer was found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            else
            {
                AckTimer = AX25_XID_ABSENT;
                #if DEBUGGING == 1
                printf(" Ack Timer wasn't found (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
            *check if TX Length and TX windowsize are absent or no*
            MaxTxLength = AX25_XID_ABSENT;
            TxWindowSize = AX25_XID_ABSENT;
            XIDInfo_t* XIDInfo = AX25_XIDInfo_tPtrConstructXIDInfo(TrasmissionMode, RejectType, ControlModulo, MaxTxLength, MaxRxLength, TxWindowSize, RxWindowSize, AckTimer, Retries);
            frame_t* XIDFrame = AX25_frame_tPtrCreatUFrame(DestAddress, DestSSID, MYAddress, MySSID, AX25_COMMAND_FRAME, 1, Exchange_Identification, XIDInfo->XIDInfo,XIDInfo->XIDInfoLength);
            Transmit(XIDFrame->PtrtoFrame,XIDFrame->FrameLength);
            AX25_voidFreeFrame(XIDFrame);
            //DLtoPhysical = XIDFrame;
            //PH_Request.DataRequest = PH_FLAG_SET;
            AX25Timer_voidStart(AX25_TIMER_TR210);
            MDL_CurrentState = SM_MDL_STATE_NEGOTIATING;*/

        }
    }
    else if(Hardware.Frame == PH_FLAG_SET)
    {
        #if DEBUGGING == 1
        printf(" PH_Indication is Data Indication and this indication is Set (Management Data-link negotiating state)\n");
        #endif // DEBUGGING
        frame_t* Frame = HardwaretoDL;
       if(0);
        else
        {
            #if DEBUGGING == 1
            printf(" DL Check Error is '0' (Management Data-link negotiating state)\n");
            #endif // DEBUGGING
            if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Exchange_Identification))
            {
                #if DEBUGGING == 1
                printf(" Frame is U Frame and Exchange Identification (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
                if(Frame->CMDRES == AX25_COMMAND_FRAME)
                {
                    /*ReQueue*/
                    #if DEBUGGING == 1
                    printf(" Frame is Command frame (Management Data-link negotiating state)\n");
                    #endif // DEBUGGING
                }
                else
                {
                    #if DEBUGGING == 1
                    printf(" Frame isn't Command frame (Management Data-link negotiating state)\n");
                    #endif // DEBUGGING
                    if(Frame->PollFinal == 1)
                    {
                        #if DEBUGGING == 1
                        printf(" Frame poll final is '1' (Management Data-link negotiating state)\n");
                        #endif // DEBUGGING
                        for(int i = 0; i<Frame->InfoLength;i++){
                            printf("%X",Frame->Info[i]>>4);
                            printf("%X ",Frame->Info[i]&15);
                        }
                        printf("\n");
                        XIDInfo_t* XIDInfo = AX25_XIDInfo_tPtrDeconstructXIDInfo(Frame->Info, Frame->InfoLength);
                        if(GET_BIT(XIDInfo->State,0) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("Classes of Procedure was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(((XIDInfo->ClassesOfProcedures>>5) & 0x3) == 2 && AcceptTransmission == MDL_FULL_DUPLEX)
                            {
                                /*************************************/
                                /**Set Hardware to run on full duplex*/
                                /*************************************/
                                //TrasmissionMode = AX25_XID_FULLDUPLEX;
                                #if DEBUGGING == 1
                                printf("Full Duplex is Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                /*************************************/
                                /**Set Hardware to run on half duplex*/
                                /*************************************/
                                //TrasmissionMode = AX25_XID_HALFDUPLEX;
                                #if DEBUGGING == 1
                                printf("Half Duplex is Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                        }
                        if(GET_BIT(XIDInfo->State, 1) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("HDLC Optional Functions was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptModulo == MDL_MOD_16)/*Modulo 128 Acceptable*/
                            {
                                AX25_voidSetControlModulo(AX25_16BITS);
                                ControlModulo = AX25_XID_128BIT_MODULO;
                                #if DEBUGGING == 1
                                printf(" Modulo 128 '16' was Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                AX25_voidSetControlModulo(AX25_8BITS);
                                ControlModulo = AX25_XID_8BIT_MODULO;
                                #if DEBUGGING == 1
                                printf(" Modulo 8 was Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            if(((XIDInfo->HDLCOptionalFunctions>>1 ) & 0x3) == 1)
                            {
                                /*Implicit Reject*/
                                SREJ_Enable = DL_FLAG_CLEAR;
                                //RejectType = AX25_XID_IMPLICIT_REJECT;
                                #if DEBUGGING == 1
                                printf("Reject Type was Implicit Reject (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else if(((XIDInfo->HDLCOptionalFunctions>>1 ) & 0x3) == 2)
                            {
                                /*SREJ*/
                                SREJ_Enable = DL_FLAG_SET;
                                //RejectType = AX25_XID_SELECTIVE_REJECT;
                                #if DEBUGGING == 1
                                printf("Reject Type was Selective Reject (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else if(((XIDInfo->HDLCOptionalFunctions>>1 ) & 0x3) == 3)
                            {
                                /*SREJ/REJ*/
                                //RejectType = AX25_XID_SELECTIVE_REJECT_REJECT;
                                #if DEBUGGING == 1
                                printf("Reject Type was Selective Reject Reject (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                /*Error !!!!*/
                            }
                        }
                        if(GET_BIT(XIDInfo->State, 3) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("TX Length was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptTXLen >= XIDInfo->MaxRXIFieldLength)/*TX N1 Acceptable*/
                            {
                                N1 = XIDInfo->MaxRXIFieldLength;
                                ///////////////////////////
                                //MaxTxLength = XIDInfo->MaxRXIFieldLength;
                                ///////////////////////////
                                #if DEBUGGING == 1
                                printf("TX Length was Acceptable so MaxTxLength = XIDInfo->MaxRXIFieldLength (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                //MaxTxLength = AX25_XID_I_RX_DEFAULT;
                                N1 = AX25_XID_I_RX_DEFAULT;
                                #if DEBUGGING == 1
                                printf("TX Length wasn't Acceptable so MaxTxLength = Default value  (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                        }
                        if(GET_BIT(XIDInfo->State, 5) == 1)
                        {
                            #if DEBUGGING == 1
                            printf("TX Window size was found (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                            if(AcceptTXWindow >= XIDInfo->RXWindowsize)/*New K Acceptable*/
                            {
                                /////////////////////
                                //TxWindowSize = XIDInfo->RXWindowsize;
                                K = XIDInfo->RXWindowsize;
                                /////////////////////
                                #if DEBUGGING == 1
                                printf("TX Window size was Acceptable so TxWindowSize = XIDInfo->TXWindowsize (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                            }
                            else
                            {
                                #if DEBUGGING == 1
                                printf("TX Window size wasn't Acceptable (Management Data-link ready state)\n");
                                #endif // DEBUGGING
                                if(ControlModulo == AX25_8BITS)
                                {
                                    ///////////////////////////////////////////////////////
                                    //TxWindowSize = AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO8;
                                    K = AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO8;
                                    ///////////////////////////////////////////////////////
                                    #if DEBUGGING == 1
                                    printf("Control Modulo was 8 bits so => put default value for modulo 8 in RXWindowSize (Management Data-link ready state)\n");
                                    #endif // DEBUGGING
                                }
                                else
                                {
                                   // TxWindowSize = AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO128;
                                    K = AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO128;
                                    #if DEBUGGING == 1
                                    printf("Control Modulo was 128 '16' bits so => put default value for modulo 128 '16' in RXWindowSize (Management Data-link ready state)\n");
                                    #endif // DEBUGGING
                                }
                            }
                        }
                        if(GET_BIT(XIDInfo->State, 7) == 1 && AcceptRetries >= XIDInfo->RetryCount)
                        {
                            MDL_Internal.NM201 = XIDInfo->RetryCount;
                            //Retries = XIDInfo->RetryCount;
                            #if DEBUGGING == 1
                            printf("Retries was found so Retries = XIDInfo->RetryCount (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        if(GET_BIT(XIDInfo->State, 6) == 1 && AcceptACKTimer >= XIDInfo->ACKTimer)
                        {
                            AX25Timer_voidSetTime(AX25_TIMER_T1,XIDInfo->ACKTimer);
                            //AckTimer = XIDInfo->ACKTimer;
                            #if DEBUGGING == 1
                            printf("Ack Timer was found so  AckTimer = XIDInfo->ACKTimer (Management Data-link ready state)\n");
                            #endif // DEBUGGING
                        }
                        AX25Timer_voidStop(AX25_TIMER_TR210);
                        MDL_Indication.NegotiateConfirm = MDL_FLAG_SET;
                        MDL_CurrentState = SM_MDL_STATE_READY;

                    }
                    else
                    {
                        MDL_Indication.ErrorIndicate = MDL_D;
                        MDL_CurrentState = SM_MDL_STATE_NEGOTIATING;
                        //AX25_voidFreeFrame(Frame);
                        #if DEBUGGING == 1
                        printf(" Frame poll final isn't '1' (Management Data-link negotiating state)\n");
                        #endif // DEBUGGING
                    }
                }
            }
            //else if((Frame->FrameType == AX25_U_FRAME) && (Frame->ControlFieldType == Frame_Reject))
            //{
            //    if(0)/*XID Rejected*/
            //    {
            //        /*Set  Segmentor off*/
            //        /*Set Version 2.0 Default*/
            //        /*E not Found*/
            //        MDL_Indication.ErrorIndicate = /*E*/;
            //        MDL_CurrentState = SM_MDL_STATE_READY;
            //    }
            //    else
            //    {
            //       MDL_CurrentState = SM_MDL_STATE_READY;
            //    }
            //}
            else
            {
                //AX25_voidFreeFrame(Frame);
                #if DEBUGGING == 1
                printf(" Frame isn't U Frame and Exchange Identification (Management Data-link negotiating state)\n");
                #endif // DEBUGGING
            }
        }
        Hardware.Frame = DL_FLAG_CLEAR;
    }
}







