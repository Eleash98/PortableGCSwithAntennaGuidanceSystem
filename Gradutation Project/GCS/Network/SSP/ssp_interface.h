#ifndef SSP_INTERFACE_H_INCLUDED
#define SSP_INTERFACE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "STD.h"
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.....
extern u8  character_framing_parity_error_f;
extern u8  receiver_overrun_event_f;
extern u8 runt_packet_f ;
extern u8 oversize_packet_f;
extern u8 bad_CRC_f ;
extern u8 ownership_error_f ;
extern u8 unknown_format_error_f ;
extern u8 wrong_direction_error_f ;
extern u8 response_timeout_f;



///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/*
///structures used decleration
*/
typedef struct
{
    u8* frame;         //frame packet
    u8 frame_length;   //frame length
    u8 data_length;    //length of frame data
    u8 frame_type;     //frame type
    u8 dataless;       //flag to indicate if there is data in the frame or it's dataless frame packet
    u8 flushed_frame;  //flag to indicate if the buffer is flusshed or not?(if flushed>> 1,if not >>0)
    u8 crc0;
    u8 crc1;


} frame_struct;



///>>>>>>>>>>>>>functions >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>....

/*
///decleration functions for tramnsmitting
*/
void build_frame( frame_struct * ,u8 *,u8 dataless,u8 datalength);
void build_std_frame(frame_struct *FRAME);
void frame_stuffing(frame_struct *FRAME);
void shift_right(frame_struct *FRAME, u8 pos,u8 moves);
void build_other_frame(frame_struct *FRAME ,u8* data  , u8 data_len);

///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.......
/*
///decleration functions for recieving
*/
void destuffing_frame(frame_struct * RX_FRAME);
void shift_left(frame_struct *FRAME, u8 pos,u8 moves);
u8 recieve_frame(u8 *RX_frame);
u8 crc_check(frame_struct *RX_FRAME, u16 crc_new);
void get_response(frame_struct *FRAME,u8);
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>......
/*
///interfacing functions
*/
void print_frame(u8*FRAME,u8 rx_len);
u16 crc_generate(u8 *data,u8 length);
void transmit(u8 type ,u8 *data,u8 dataless ,u8 datalength);
void echo_frame(u8 type ,u8 *data,u8 dataless,u8 datalen);
u8 check_valid(frame_struct* );
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.......

#endif // SSP_INTERFACE_H_INCLUDED
