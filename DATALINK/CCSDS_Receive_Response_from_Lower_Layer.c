#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
//#include "CCSDS_FOP_PACKET_PRIVATE.h"
//#include "CCSDS_CRC.h"
void CCSDS_void_ReceiveResponseFromLowerLayer(u8 VCID,u8 Response,CCSDS_TC* TC)
{
    //u8 VCID_number=CCSDS_u8_GetVCIDnumber(VCID);
    if (VCID<64)
    {
        switch (Response)
        {
            case CCSDS_FOP_AD_ACCEPT:
                /*E41*/
                VirtualChannels[VCID].ADOutFlag=CCSDS_READY;
                if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT))
                    {
                        CCSDS_void_lookforFDU(TC,VCID);
                    }
                break;
            case CCSDS_FOP_AD_REJECT:
                /*E42*/
                CCSDS_void_alert(CCSDS_ALERT_LLIF,VCID);
                VirtualChannels[VCID].State=CCSDS_INITIAL;
                break;
            case CCSDS_FOP_BC_ACCEPT:
                /*E43*/
                VirtualChannels[VCID].BCOutFlag=CCSDS_READY;
                if(VirtualChannels[VCID].State==CCSDS_INIT_WITH_BC_FRAME)
                {
                    CCSDS_void_lookfordirective(VCID);
                }
                break;
            case CCSDS_FOP_BC_REJECT:
                /*E44*/
                CCSDS_void_alert(CCSDS_ALERT_LLIF,VCID);
                VirtualChannels[VCID].State=CCSDS_INITIAL;
                break;
            case CCSDS_FOP_BD_ACCEPT:
                /*45*/
                VirtualChannels[VCID].BDOutFlag=CCSDS_READY;
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                break;
            case CCSDS_FOP_BD_REJECT:
                /*E46*/
                CCSDS_void_alert(CCSDS_ALERT_LLIF,VCID);
                VirtualChannels[VCID].State=CCSDS_INITIAL;
                break;
            default:
                break;
        }
    }
    else
    {
        if (CCSDS_FOP_DEBUG)
            printf("CCSDS unknown VCID\n");
    }
}
