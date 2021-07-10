#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
//#include "CCSDS_CRC.h"
void CCSDS_void_FOPTIMEREXP(u8 VCID,CCSDS_TC* TC)
{
    //printf ("time out type =%d \n",VirtualChannels[VCID].TT);
    if (VCID<64)
    {
        if ((VirtualChannels[VCID].TrasmissionCount)<(VirtualChannels[VCID].TrasmissionLimit))
        {
            if (VirtualChannels[VCID].TT==0)
            {
                /*E16 REV.B*/
                switch (VirtualChannels[VCID].State)
                {
                    case CCSDS_ACTIVE_STATE:
                        /*state 1*/
                        /*initiate AD retransmission*/
                        CCSDS_void_initiateADretransmission(VCID);
                        /*look for FDU*/
                        CCSDS_void_lookforFDU(TC,VCID);
                        /*next state state number 1*/
                        break;
                    case CCSDS_RETRANSMIT_WITHOUT_WAIT:
                        /*state 2*/
                        /*initiate AD retransmission*/
                        CCSDS_void_initiateADretransmission(VCID);
                        /*look for FDU*/
                        CCSDS_void_lookforFDU(TC,VCID);
                        /*next state state number 2*/
                        break;
                    case CCSDS_RETRANSMIT_WITH_WAIT:
                        /*state 3*/
                        /*Ignore*/
                        /*next state state 3*/
                        break;
                    case CCSDS_INIT_WITHOUT_BC_FRAME:
                        /*state 4*/
                        /*alert*/
                        CCSDS_void_alert(CCSDS_ALERT_T1,VCID);
                        /*next state state 6*/
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    case CCSDS_INIT_WITH_BC_FRAME:
                        /*state 3*/
                        /*initiate BC retransmission*/
                        CCSDS_void_initiateBCretransmission(VCID);
                        /*look for directive*/
                        CCSDS_void_lookfordirective(VCID);
                        /*next state state 5*/
                        break;
                    default:
                        break;
                }
            }
            else if (VirtualChannels[VCID].TT==1)
            {
                /*E104*/
                switch (VirtualChannels[VCID].State)
                {
                    case CCSDS_ACTIVE_STATE:
                        /*state 1*/
                        /*initiate AD retransmission*/
                        CCSDS_void_initiateADretransmission(VCID);
                        /*look for FDU*/
                        CCSDS_void_lookforFDU(TC,VCID);
                        /*next state state number 1*/
                        break;
                    case CCSDS_RETRANSMIT_WITHOUT_WAIT:
                        /*state 2*/
                        /*initiate AD retransmission*/
                        CCSDS_void_initiateADretransmission(VCID);
                        /*look for FDU*/
                        CCSDS_void_lookforFDU(TC,VCID);
                        /*next state state number 2*/
                        break;
                    case CCSDS_RETRANSMIT_WITH_WAIT:
                        /*state 3*/
                        /*Ignore*/
                        /*next state state 3*/
                        break;
                    case CCSDS_INIT_WITHOUT_BC_FRAME:
                        /*state 4*/
                        /*alert*/
                        VirtualChannels[VCID].SS=4;
                        CCSDS_void_Suspend();
                        /*next state state 6*/
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    case CCSDS_INIT_WITH_BC_FRAME:
                        /*state 3*/
                        /*initiate BC retransmission*/
                        CCSDS_void_initiateBCretransmission(VCID);
                        /*look for directive*/
                        CCSDS_void_lookfordirective(VCID);
                        /*next state state 5*/
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
            if (VirtualChannels[VCID].TT==0)
            {
                /*E17 REV B*/
                /*alert*/
                CCSDS_void_alert(CCSDS_ALERT_T1,VCID);
                VirtualChannels[VCID].State=CCSDS_INITIAL;
            }
            else if(VirtualChannels[VCID].TT==1)
            {
                /*E18 REVB*/
                switch (VirtualChannels[VCID].State)
                {
                    case CCSDS_ACTIVE_STATE:
                        /*state 1*/
                        VirtualChannels[VCID].SS=1;
                        CCSDS_void_Suspend();
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    case CCSDS_RETRANSMIT_WITHOUT_WAIT:
                        /*state 2*/
                        VirtualChannels[VCID].SS=2;
                        CCSDS_void_Suspend();
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    case CCSDS_RETRANSMIT_WITH_WAIT:
                        /*state 3*/
                        VirtualChannels[VCID].SS=3;
                        CCSDS_void_Suspend();
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    case CCSDS_INIT_WITHOUT_BC_FRAME:
                        /*state 4*/
                        VirtualChannels[VCID].SS=4;
                        CCSDS_void_Suspend();
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    case CCSDS_INIT_WITH_BC_FRAME:
                        /*state 5*/
                        CCSDS_void_alert(CCSDS_ALERT_T1,VCID);
                        //printf("i'm here\n");
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else
    {
        if (CCSDS_FOP_DEBUG)
            printf("CCSDS unknown VCID\n");
    }
}
