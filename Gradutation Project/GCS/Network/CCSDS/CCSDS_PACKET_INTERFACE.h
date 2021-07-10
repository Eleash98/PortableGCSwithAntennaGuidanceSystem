
#ifndef CCSDS_PACKET_INTERFACE_H
#define CCSDS_PACKET_INTERFACE_H
#include "STD_TYPES.h"
/**the APID may uniquely identify the individual sending or receiving
application process within a particular apace vehicle**/

/*_______________________________________________________________________________________*/
/*packeting services*/
//#define CCSDS_PACKET_SERVICE                0
//#define CCSDS_OCTET_STRING_SERVICE          1
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
/*Packet queues size*/
//#define CCSDS_TRANSMIT_PACKET_QUEUE_SIZE    5120
//#define CCSDS_RECEIVE_PACKET_QUEUE_SIZE     5120
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
typedef struct
{
    u8* Queue;
    u16 Queue_counter;
    u32* packet_lengths;
    //u32 Queue_length;
    u32 Queue_used_size;
    u16* APIDs;
}CCSDS_PACKET_TRANSMIT_Queue;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
typedef struct
{
    u8* Queue;
    u16 Queue_counter;
    u32* packet_lengths;
    u32 Queue_used_size;
    u16* APIDs;
}CCSDS_PACKET_RECEIVE_Queue;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
CCSDS_PACKET_TRANSMIT_Queue CCSDS_TRANSMIT_PACKET_QUEUE;
CCSDS_PACKET_RECEIVE_Queue CCSDS_RECEIVE_PACKET_QUEUE;
/*_______________________________________________________________________________________*/
/*_______________________________________________________________________________________*/
void CCSDS_PACKET_void_init();
/*The PACKET.request primitive shall be passed to the service provider to request it to send the Space Packet.*/
void CCSDS_PACKET_void_PACKET_REQUEST(u16 APID,u32 packet_length,u8* packet);

/*The PACKET.indication primitive shall be passed from the service provider to the Packet
Service user at the receiving end to deliver a Space Packet.*/
u8* CCSDS_PACKET_u8pointer_PACKET_INDICATION(u16* APID,u32* packet_length);

/*get a packet from the TRANSMIT_PACKET_QUEUE to send it to the Subnetwork*/
u8* CCSDS_PACKET_u8pointer_packetTOSubnetwork(u32* packet_length);

/*get a packet from the Subnetwork to add it to RECEIVE_PACKET_QUEUE*/
void CCSDS_PACKET_u8pointer_packetFROMSubnetwork(u8* packet,u32 packet_length);

/*The OCTET_STRING.request primitive shall be passed to the service provider to request it
to send the Octet String.*/
void CCSDS_PACKET_void_OCTET_STRING_REQUEST(u16 APID,u8* OCTET_STRING,
    u32 OCTET_STRING_length,u8 packet_type,u8 sec_head_flag,u8* Time_code_field,
    u16 time_code_length,u8* ancillary_data,u16 ancillary_length);

/*The OCTET_STRING.indication primitive is used to deliver Octet Strings to the Octet
String Service user identified with the APID.*/
u8* CCSDS_PACKET_u8pointer_OCTET_STRING_INDICATION(u16* APID,u32* OCTET_STRING_length,u8* Secondary_Header_Indicator);

void CCSDS_PACKET_void_free();

#endif
