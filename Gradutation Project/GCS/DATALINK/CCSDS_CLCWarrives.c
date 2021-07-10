#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
#include "CRC_interface.h"
void CCSDS_void_CLCWarrives(u8 VCID,u8 COPeffect,u8 RecVCID,u8 flags ,u8 RecN_of_R ,CCSDS_TC* TC)
{
    if ((COPeffect==01)&&(VCID<64))
    {
        /**from E1 to E14*/
        if (!(GET_BIT(flags,2)))                    /**lock out flag**/
        {
            if (RecN_of_R==VirtualChannels[VCID].V_of_S)
            {
                if (!(GET_BIT(flags,0)))            /**Retransmitt flag**/
                {
                    if (!(GET_BIT(flags,1)))        /**Wait flag**/
                    {
                        if (RecN_of_R==VirtualChannels[VCID].NNR)
                        {
                            /**E1*/
                            switch (VirtualChannels[VCID].State)
                            {
                                case CCSDS_ACTIVE_STATE:
                                    /**state 1*/
                                    /**ignore*/
                                    break;
                                case CCSDS_RETRANSMIT_WITHOUT_WAIT:
                                    /**state 2*/
                                    CCSDS_void_alert(CCSDS_ALERT_SYNCH,VCID);
                                    VirtualChannels[VCID].State=CCSDS_INITIAL;
                                    break;
                                case CCSDS_RETRANSMIT_WITH_WAIT:
                                    /**state 3*/
                                    CCSDS_void_alert(CCSDS_ALERT_SYNCH,VCID);
                                    /**next state state 6*/
                                    VirtualChannels[VCID].State=CCSDS_INITIAL;
                                    break;
                                case CCSDS_INIT_WITHOUT_BC_FRAME:
                                    /**state 4*/
                                    CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                                    CCSDS_void_cancel_Timer(VCID);
                                    /**next state state 1*/
                                    VirtualChannels[VCID].State=CCSDS_ACTIVE_STATE;
                                    break;
                                case CCSDS_INIT_WITH_BC_FRAME:
                                    /**state 5*/
                                    CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                                    /**Release copy of type BC frame*/
                                    /**remove first BC frame from the sent queue*/
                                    CCSDS_void_ReleaseCopyOfTypeBCFrame(VCID);
                                    CCSDS_void_cancel_Timer(VCID);
                                    /**next state state 1*/
                                    VirtualChannels[VCID].State=CCSDS_ACTIVE_STATE;
                                    break;
                                case CCSDS_INITIAL:
                                    /**state 6*/
                                    /**ignore*/
                                    break;
                                default:
                                    break;
                            }
                        }
                        else
                        {
                            /**E2*/
                            if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT))
                            {
                                /**state 1 & 2 & 3*/
                                CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(RecN_of_R,VCID);
                                CCSDS_void_cancel_Timer(VCID);
                                CCSDS_void_lookforFDU(TC,VCID);
                                VirtualChannels[VCID].State=CCSDS_ACTIVE_STATE;
                            }
                        }
                    }
                    else
                    {
                        /**E3*/
                        if (VirtualChannels[VCID].State!=CCSDS_INITIAL)
                        {
                            CCSDS_void_alert(CCSDS_ALERT_CLCW,VCID);
                            VirtualChannels[VCID].State=CCSDS_INITIAL;
                        }
                    }
                }
                else
                {
                    /**E4*/
                    if ((VirtualChannels[VCID].State!=CCSDS_INITIAL)&&(VirtualChannels[VCID].State!=CCSDS_INIT_WITH_BC_FRAME))
                    {
                        CCSDS_void_alert(CCSDS_ALERT_SYNCH,VCID);
                        VirtualChannels[VCID].State=CCSDS_INITIAL;
                    }
                }
            }
            else if ((RecN_of_R<VirtualChannels[VCID].V_of_S)&&(RecN_of_R>=VirtualChannels[VCID].NNR))

            {
                if (!(GET_BIT(flags,0)))      /**Retransmitt flag**/
                {
                    if (!(GET_BIT(flags,1)))  /**Wait flag**/
                    {
                        if (RecN_of_R==VirtualChannels[VCID].NNR)
                        {
                            /**E5*/
                            if ((VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                            {
                                CCSDS_void_alert(CCSDS_ALERT_SYNCH,VCID);
                                VirtualChannels[VCID].State=CCSDS_INITIAL;
                            }
                        }
                        else
                        {
                            /**E6 REV*/
                            if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                            {
                                /**state 1 & 2 & 3*/
                                CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(RecN_of_R,VCID);
                                CCSDS_void_lookforFDU(TC,VCID);
                                VirtualChannels[VCID].State=CCSDS_ACTIVE_STATE;
                            }
                        }
                    }
                    else
                    {
                       if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                       {
                            /**E7 REVB*/
                            CCSDS_void_alert(CCSDS_ALERT_CLCW,VCID);
                            VirtualChannels[VCID].State=CCSDS_INITIAL;
                       }
                    }
                }
                else
                {
                    if (VirtualChannels[VCID].TrasmissionLimit==1)
                    {
                        if (RecN_of_R!=VirtualChannels[VCID].NNR)
                        {
                            /**E101*/
                            if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                            {
                                CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(RecN_of_R,VCID);
                                CCSDS_void_alert(CCSDS_ALERT_LIMIT,VCID);
                                VirtualChannels[VCID].State=CCSDS_INITIAL;
                            }
                        }
                        else
                        {
                            /**E102*/
                            if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                            {
                                CCSDS_void_alert(CCSDS_ALERT_LIMIT,VCID);
                                VirtualChannels[VCID].State=CCSDS_INITIAL;
                            }
                        }
                    }
                    else
                    {
                        if (RecN_of_R!=VirtualChannels[VCID].NNR)
                        {
                            if (!(GET_BIT(flags,1)))
                            {
                                /**E8 REVB*/
                                if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                                {
                                    CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(RecN_of_R,VCID);
                                    CCSDS_void_initiateADretransmission(VCID);
                                    CCSDS_void_lookforFDU(TC,VCID);
                                    VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITHOUT_WAIT;
                                }
                            }
                            else
                            {
                                /**E9 REVB*/
                                if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                                {
                                    CCSDS_void_RemoveAcknowledgedADFramesFromSentQueue(RecN_of_R,VCID);
                                    VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITH_WAIT;
                                }
                            }
                        }
                        else
                        {
                            if (VirtualChannels[VCID].TrasmissionCount<VirtualChannels[VCID].TrasmissionLimit)
                            {
                                if (!(GET_BIT(flags,1)))
                                {
                                    /**E10 REVB**/
                                    if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                                    {
                                        CCSDS_void_initiateADretransmission(VCID);
                                        CCSDS_void_lookforFDU(TC,VCID);
                                        VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITHOUT_WAIT;
                                    }
                                }
                                else
                                {
                                    /**E11 REVB**/
                                    if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                                    {
                                        VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITH_WAIT;
                                    }
                                }
                            }
                            else
                            {
                                if (!(GET_BIT(flags,1)))
                                {
                                    /**E12 REVB**/
                                    if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                                    {
                                        VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITHOUT_WAIT;
                                    }
                                }
                                else
                                {
                                    if ((VirtualChannels[VCID].State==CCSDS_ACTIVE_STATE)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITHOUT_WAIT)||(VirtualChannels[VCID].State==CCSDS_RETRANSMIT_WITH_WAIT))
                                    {
                                        VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITH_WAIT;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                /**E13**/
                if ((VirtualChannels[VCID].State!=CCSDS_INITIAL)&&(VirtualChannels[VCID].State!=CCSDS_INIT_WITH_BC_FRAME))
                {
                    CCSDS_void_alert(CCSDS_ALERT_NNR,VCID);
                    VirtualChannels[VCID].State=CCSDS_INITIAL;
                }
            }
        }
        else
        {
            /**E14**/
            if ((VirtualChannels[VCID].State!=CCSDS_INITIAL)&&(VirtualChannels[VCID].State!=CCSDS_INIT_WITH_BC_FRAME))
            {
                CCSDS_void_alert(CCSDS_ALERT_LOCKOUT,VCID);
                VirtualChannels[VCID].State=CCSDS_INITIAL;
            }
        }
    }
    else
    {
        if (VirtualChannels[VCID].State!=CCSDS_INITIAL)
        {
            /**E15**/
            CCSDS_void_alert(CCSDS_ALERT_CLCW,VCID);
            /**Next state state 6**/
            VirtualChannels[VCID].State=CCSDS_INITIAL;
        }
    }
}
