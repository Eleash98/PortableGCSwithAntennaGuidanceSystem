#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
//#include "CCSDS_FOP_PACKET_PRIVATE.h"
//#include "CCSDS_CRC.h"
/*DirectiveVar is V*(R) if the directive is set V(R) (E27) && V*(S) if the directive is set V(S) (E35)
&& set FOP window width (E36)&& set T1 initial (E37) && set transmission limit (E38) && set time out type (TT) (E39)*/
void CCSDS_void_ReceiveDirectiveFromManagementFunction(u8 VCID,u8 DirectiveType,CCSDS_TC TC,u32 DirectiveVar)
{
    //u8 VCID_number = CCSDS_u8_GetVCIDnumber(VCID);
    if (VCID<64)
    {
        switch (DirectiveType)
        {
            case CCSDS_INITIATE_AD_WITHOUT_CLCW:
                /*E23*/
                if (VirtualChannels[VCID].State==CCSDS_INITIAL)
                {
                    CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                    CCSDS_void_INITIALIZE(VCID);
                    CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                    /*next state state 1*/
                    VirtualChannels[VCID].State=CCSDS_ACTIVE_STATE;
                }
                else
                {
                    CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                }
                break;
            case CCSDS_INITIATE_AD_WITH_CLCW:
                /*E24*/
                if (VirtualChannels[VCID].State==CCSDS_INITIAL)
                {
                    CCSDS_void_ACCEPT(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                    CCSDS_void_INITIALIZE(VCID);
                    CCSDS_void_Start_Timer(VCID);
                    /*next state state 4*/
                    VirtualChannels[VCID].State=CCSDS_INIT_WITHOUT_BC_FRAME;
                }
                else
                {
                    CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                }
                break;
            case CCSDS_INITIATE_AD_WITH_UNLOCK:
                /*E25 & E26*/
                if (VirtualChannels[VCID].BCOutFlag==CCSDS_READY)
                {
                    /*E25 REVB*/
                    if (VirtualChannels[VCID].State==CCSDS_INITIAL)
                    {
                        /*unlock data type BC frame*/
                        TC.first_byte_pointer_TC_Data=(u8*)malloc(sizeof(u8));
                        TC.first_byte_pointer_TC_Data[0]=0;
                        TC.length_TC_Data=1;
                        CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                        CCSDS_void_INITIALIZE(VCID);
                        /*transmitting unlock type BC frame*/
                        CCSDS_void_TRANSMITTBC(&TC,VCID);
                        /*next state state 5*/
                        VirtualChannels[VCID].State=CCSDS_INIT_WITH_BC_FRAME;
                    }
                    else
                    {
                        CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                    }
                }
                else
                {
                    /*E26*/
                    CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                }
                break;
            case CCSDS_INITIATE_AD_WITH_SET_VR:
                /*E27 & E28*/
                if (VirtualChannels[VCID].BCOutFlag==CCSDS_READY)
                {
                    /*E27 REVB*/
                    if (VirtualChannels[VCID].State==CCSDS_INITIAL)
                    {
                        CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                        CCSDS_void_INITIALIZE(VCID);
                        /*set V(R) data type BC frame*/
                        VirtualChannels[VCID].NNR=(u8)(DirectiveVar&0xff);
                        VirtualChannels[VCID].V_of_S=(u8)(DirectiveVar&0xff);
                        TC.first_byte_pointer_TC_Data=(u8*)malloc(3);
                        TC.first_byte_pointer_TC_Data[0]=0b10000010;
                        TC.first_byte_pointer_TC_Data[1]=0;
                        TC.first_byte_pointer_TC_Data[2]=VirtualChannels[VCID].V_of_S;
                        TC.length_TC_Data=3;
                        /*transmitting set V(R) type BC frame*/
                        CCSDS_void_TRANSMITTBC(&TC,VCID);
                        /*next state state 5*/
                        VirtualChannels[VCID].State=CCSDS_INIT_WITH_BC_FRAME;
                        //free(TC.first_byte_pointer_TC_Data);
                    }
                    else
                    {
                        CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                    }
                }
                else
                {
                    /*E28*/
                    CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                }
                break;
            case CCSDS_TERMINATE_AD_SERVICE:
                /*E29*/
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                if (VirtualChannels[VCID].State!=CCSDS_INITIAL)
                {
                    CCSDS_void_alert(CCSDS_ALERT_TERM,VCID);
                }
                /*next state state 6*/
                VirtualChannels[VCID].State=CCSDS_INITIAL;
                break;
            case CCSDS_RESUME_AD_SERVICE:
                /*from E30 to E34*/
                switch (VirtualChannels[VCID].SS)
                {
                    case 0:
                        /*E30*/
                        CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                        break;
                    case 1:
                        /*E31 REVB*/
                        CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                        /*RESUME*/
                        CCSDS_void_Start_Timer(VCID);
                        VirtualChannels[VCID].SS=0;
                        CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                        VirtualChannels[VCID].State=CCSDS_ACTIVE_STATE;
                        break;
                    case 2:
                        /*E32 REVB*/
                        CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                        /*RESUME*/
                        CCSDS_void_Start_Timer(VCID);
                        VirtualChannels[VCID].SS=0;
                        CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                        VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITHOUT_WAIT;
                        break;
                    case 3:
                        /*E33 REVB*/
                        CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                        /*RESUME*/
                        CCSDS_void_Start_Timer(VCID);
                        VirtualChannels[VCID].SS=0;
                        CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                        VirtualChannels[VCID].State=CCSDS_RETRANSMIT_WITH_WAIT;
                        break;
                    case 4:
                        /*E34 REVB*/
                        CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                        /*RESUME*/
                        CCSDS_void_Start_Timer(VCID);
                        VirtualChannels[VCID].SS=0;
                        CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                        VirtualChannels[VCID].State=CCSDS_INIT_WITHOUT_BC_FRAME;
                        break;
                    default:
                        break;
                }
                break;
            case CCSDS_SET_VS:
                /*E35 REVB*/
                if ((VirtualChannels[VCID].State==CCSDS_INITIAL)&&(VirtualChannels[VCID].SS==0))
                {
                    CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                    VirtualChannels[VCID].V_of_S=(u8)(DirectiveVar&0xff);
                    VirtualChannels[VCID].NNR=(u8)(DirectiveVar&0xff);
                    CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                }
                else
                {
                    CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                }
                break;
            case CCSDS_SET_FOP_SILDING_WINDOW:
                /*E36*/
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                VirtualChannels[VCID].K=(u8)(DirectiveVar&0xff);
                CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                break;
            case CCSDS_SET_T1_INITIAL:
                /*E37*/
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                VirtualChannels[VCID].T1Initial=DirectiveVar;
                CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                break;
            case CCSDS_SET_TRANSMISSION_LIMIT:
                /*E38*/
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                VirtualChannels[VCID].TrasmissionLimit=DirectiveVar;
                CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                break;
            case CCSDS_SET_TIMEOUT_TYPE:
                /*E39*/
                CCSDS_void_ACCEPT(CCSDS_ACCEPT_RESPONSETO_DIRECTIVE);
                VirtualChannels[VCID].TT=(u8)(DirectiveVar&0xff);
                CCSDS_void_CONFIRM(CCSDS_POSITIVE_CONFIRM_RESPONSETO_DIRECTIVE);
                break;
            default:
                CCSDS_void_REJECT(CCSDS_REJECT_RESPONSETO_DIRECTIVE);
                break;
        }
    }
    else
    {
        if (CCSDS_FOP_DEBUG)
            printf("CCSDS unknown VCID\n");
    }
}
