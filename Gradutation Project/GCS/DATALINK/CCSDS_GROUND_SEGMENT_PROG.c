#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "CCSDS_FOP_PRIVATE.h"
#include "CRC_interface.h"
void CCSDS_void_TC_Framing(CCSDS_TC* TC_frame)
{
    u16 CRC=0;
    TC_frame->frame_length=TC_frame->length_TC_Data+7;
	TC_frame->frame=(u8*)malloc(TC_frame->frame_length);
	/*select the frame Version Number*/
	TC_frame->frame[0]=0;
	TC_frame->frame[0]|=(u8)(VER&0x03<<6);
	/*set the bypass flag*/
	TC_frame->frame[0]|=(u8)(TC_frame->BypassFlag<<5);
	/*set the control command flag*/
	TC_frame->frame[0]|=(u8)(TC_frame->ControlCommandFlag<<4);
	//reserved bits//
	CLR_BIT(TC_frame->frame[0],3);
	CLR_BIT(TC_frame->frame[0],2);
    /*SET SPACCECRAFT ID TO THE FRAME*/
	TC_frame->frame[0]&=~0x03;
	TC_frame->frame[0]|=(u8)(FOP_SCID>>8&0x03);
	TC_frame->frame[1]=(u8)(FOP_SCID&0xff);
	/*SET VIRTUAL CHANNEL ID TO THE FRAME*/
	TC_frame->frame[2]=(u8)(TC_frame->TC_VCID<<2);
	TC_frame->frame[2]&=~(0x03);
	TC_frame->frame[2]|=(u8)((TC_frame->length_TC_Data+6)>>8&0x3);
    TC_frame->frame[3]=(u8)((TC_frame->length_TC_Data+6)&0xff);
    if ((TC_frame->BypassFlag==1)&&(TC_frame->ControlCommandFlag==0))
    {
        TC_frame->frame[4]=0;
    }
    else if ((TC_frame->BypassFlag==1)&&(TC_frame->ControlCommandFlag==1))
    {
        //FSN[TC_frame->TC_VCID] = 0;
        TC_frame->frame[4]=0;
    }
    else
    {
        //FSN[TC_frame->TC_VCID]=(FSN[TC_frame->TC_VCID]+1)%256;
        TC_frame->frame[4]=(u8)TC_frame->N_of_S;
    }
    memcpy(TC_frame->frame+5,TC_frame->first_byte_pointer_TC_Data,TC_frame->length_TC_Data);
    /*CRC*/
    CRC = CRC16_u16CCITTFalse(TC_frame->frame,TC_frame->frame_length-2);
    TC_frame->frame[TC_frame->frame_length-1]=(u8)CRC&0xff;
    TC_frame->frame[TC_frame->frame_length-2]=(u8)((CRC>>8)&0xff);
}


u8 CCSDS_u8_TM_DEFRAME(u8* Frame,u32 Frame_Length,CCSDS_TC* TC_data)
{
    /*CRC*/
    u16 TMCRC = CRC16_u16CCITTFalse(Frame,Frame_Length);
    if (TMCRC != 0)
        return 1;
    /*Spacecraft ID*/
    TC_data->SCID = (Frame[0]&(0x3F)<<4)|((Frame[1]&0xF0)>>4);

    /*Virtual Channel ID*/
    TC_data->TM_VCID = (Frame[1]&0x0E)>>1;

    /*Operation control field flag*/
    TC_data->OCF_flag = (Frame[1]&0x01);

    /*Master channel Frame Counter*/
    TC_data->MCFC = Frame[2];

    /*Virtual Channel frame Counter*/
    TC_data->VCFC = Frame[3];

    /*Transfer frame secondary header flag*/
    TC_data->TFSH_flag = GET_BIT(Frame[4],7);

    /*Synch Flag*/
    TC_data->Synch_flag = GET_BIT(Frame[4],6);

    /*Packet order flag*/
    TC_data->PacketOrder_flag = GET_BIT(Frame[4],5);

    /*segment length ID*/
    TC_data->SLI = (GET_BIT(Frame[4],4)<<1)|GET_BIT(Frame[4],3);

    /*First header Pointer*/

    TC_data->TFSH_length= 0;

    if (TC_data->TFSH_flag == 1)
    {
        /*Transfer Frame Secondary Header Length*/
        TC_data->TFSH_length = (Frame[6]&0x3F)+ 1;

        TC_data->SecondaryHeaderData = (u8*)malloc(TC_data->TFSH_length-1);
        memcpy(TC_data->SecondaryHeaderData,Frame+7,TC_data->TFSH_length-1);
    }

    TC_data->length_TM_Data = Frame_Length-TC_data->TFSH_length-8;
    TC_data->length_TM_Data = (TC_data->OCF_flag == 1)? TC_data->length_TM_Data-4 : TC_data->length_TM_Data;

    TC_data->first_byte_pointer_TM_Data = (u8*)malloc(TC_data->length_TM_Data);
    memcpy(TC_data->first_byte_pointer_TM_Data,Frame+6+TC_data->TFSH_length,TC_data->length_TM_Data);
    if (TC_data->OCF_flag == 1)
    {
        /*Control Word Type*/
        u8 ControlWordType = GET_BIT(Frame[Frame_Length-6],7);

        if(ControlWordType == 0)
        {
            TC_data->COPInEffect = (Frame[Frame_Length-6])&0x03;
            TC_data->StatusField = (Frame[Frame_Length-6]>>2) & 0x07;
            TC_data->Rec_TC_VCID = (Frame[Frame_Length-5]>>2) &0x3F;
            TC_data->Flags = (Frame[Frame_Length-4]>>3) & 0x1F;
            TC_data->FarmBCounter = (Frame[Frame_Length-4]>>1) & 0x3;
            TC_data->ReportValue= Frame[Frame_Length-3];
            VirtualChannels[TC_data->Rec_TC_VCID].N_of_R= TC_data->ReportValue;
        }
        else
        {
            /*Error*/
            /*Should never be entered*/
            if(CCSDS_FOP_DEBUG)
                printf("Error !!!!\nNo CLCW found!");
        }
    }
    /*writing into receive queue*/
    CCSDS_FOP_void_packet_receiption(TC_data->first_byte_pointer_TM_Data,
        TC_data->length_TM_Data,TC_data->SecondaryHeaderData,TC_data->TFSH_length-1,TC_data->Rec_TC_VCID);
    free(TC_data->first_byte_pointer_TM_Data);
    free(TC_data->SecondaryHeaderData);
    /*______________written____________*/
    return 0;
}
