#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
//#include "CCSDS_FOP_PACKET_PRIVATE.h"
//#include "CCSDS_CRC.h"
void CCSDS_void_Receive_Request_to_Transfer_FDU(u8 VCID,u8 service_type,u8* FDU_PTR,u16 FDU_length,CCSDS_TC* TC_frame)
{
    /*Receive Request to Transfer FDU*/
    //u8 VCID_Number =CCSDS_u8_GetVCIDnumber(VCID);
    if (VCID<64)
    {
        if (service_type==CCSDS_FOP_AD_Service)
        {
            /*AD Service*/
            if (VirtualChannels[VCID].wait_queue_FDU_length==0)
            {
                /*wait queue empty*/
                /*E19*/
                switch (VirtualChannels[VCID].State)
                {
                    case CCSDS_ACTIVE_STATE:
                        /*state 1*/
                        /*add to wait queue*/
                        VirtualChannels[VCID].WaitQueue=(u8*)malloc(FDU_length);
                        memcpy(VirtualChannels[VCID].WaitQueue,FDU_PTR,FDU_length);
                        VirtualChannels[VCID].wait_queue_FDU_length=FDU_length;
                        /*look for FDU*/
                        CCSDS_void_lookforFDU(TC_frame,VCID);
                        /*stay in the same state*/
                        break;
                    case CCSDS_RETRANSMIT_WITHOUT_WAIT:
                        /*state 2*/
                        /*add to wait queue*/
                        VirtualChannels[VCID].WaitQueue=(u8*)malloc(FDU_length);
                        memcpy(VirtualChannels[VCID].WaitQueue,FDU_PTR,FDU_length);
                        VirtualChannels[VCID].wait_queue_FDU_length=FDU_length;
                        /*look for FDU*/
                        CCSDS_void_lookforFDU(TC_frame,VCID);
                        /*stay in the same state*/
                        break;
                    case CCSDS_RETRANSMIT_WITH_WAIT:
                        /*state 3*/
                        /*add to wait queue*/
                        VirtualChannels[VCID].WaitQueue=(u8*)malloc(FDU_length);
                        memcpy(VirtualChannels[VCID].WaitQueue,FDU_PTR,FDU_length);
                        VirtualChannels[VCID].wait_queue_FDU_length=FDU_length;
                        /*stay in the same state*/
                        break;
                    case CCSDS_INIT_WITHOUT_BC_FRAME:
                        /*state 4*/
                        CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_TRANSFARFDU);
                        /*stay in the same state*/
                        break;
                    case CCSDS_INIT_WITH_BC_FRAME:
                        /*state 5*/
                        CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_TRANSFARFDU);
                        /*stay in the same state*/
                        break;
                    case CCSDS_INITIAL:
                        /*state 6*/
                        CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_TRANSFARFDU);
                        /*stay in the same state*/
                        break;
                    default:
                        break;
                }
            }
            else
            {
                /*E20*/
                CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_TRANSFARFDU);
                /*stay in the same state*/
            }
        }
        else if(service_type==CCSDS_FOP_BD_Service)
        {
            if (VirtualChannels[VCID].BDOutFlag==CCSDS_READY)
            {
                /*E21 REV.B*/
                /*ACCEPT*/
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_TRANSFARFDU);
                /*transmit user data type BD frame*/
                TC_frame->TC_VCID=VCID;
                TC_frame->first_byte_pointer_TC_Data=(u8*)malloc(FDU_length);
                memcpy(TC_frame->first_byte_pointer_TC_Data,FDU_PTR,FDU_length);
                TC_frame->length_TC_Data=FDU_length;
                CCSDS_void_TRANSMITTBD(TC_frame,VCID);
            }
            else if (VirtualChannels[VCID].BDOutFlag==0)
            {
                /*E22*/
                CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_TRANSFARFDU);
                /*stay in the same state*/
            }
        }
    }
    else
    {
        if (CCSDS_FOP_DEBUG)
            printf("CCSDS unknown VCID\n");
    }
}
