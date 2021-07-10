
#ifndef CCSDS_PACKET_PRIVATE_H
#define CCSDS_PACKET_PRIVATE_H
#include "STD_TYPES.h"
#include "CCSDS_PACKET_INTERFACE.h"
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
//#define CCSDS_send_packet                   0
//#define CCSDS_receive_packet                1
#define CCSDS_PACKET_DEPUG                  1
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/

u16 packet_seq_count_TC;
u16 packet_seq_count_TM;

u8* CCSDS_u32_PACKETASSEMBLY(u16 APID,u8* SDU,u8 packet_type,u8 sec_head_flag,
    u32* packet_data_length,u8* Time_code_field,u16 time_code_length,u8* ancillary_data,
    u16 ancillary_length);

u8* CCSDS_u32_PACKETDEASSEMBLY(u8* packet,u32* packet_length,u16*APID,u8* Secondary_Header_Indicator);
#endif
