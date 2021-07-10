#ifndef SSP_PRIVATE_H_INCLUDED
#define SSP_PRIVATE_H_INCLUDED

///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>........
/*
byte stuffing parameters
*/
#define FEND     0xc0
#define FESC     0xdb
#define TFEND    0xdc
#define TFESC    0xdd

///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>...........
/*
frames types
*/

#define PING              0x00            ///dataless ping
#define INIT              0x01            ///dataless init for simplicity
//#define re_start          0x            ///init with data to restart depend on the ss field
#define ACK_RESPONSE      0x02            ///dataless ack "no " response
#define ACK_DATA           0x42            ///dataless ack "yes" response

#define NAK_UNKOWN        0x03            ///dataless nak "unkown" response
#define NAK_INCORRECT     0x43            ///dataless nak "incorrect"
#define NAK_FAILED        0x83            ///dataless nak "failed" response

#define GET_0             0x04             ///address_space_0
#define GET_1             0x44             ///address_space_1

#define PUT_0             0x05
#define PUT_1             0x45

#define READ_0            0x06
#define READ_1            0x46
#define READ_2            0x86
#define READ_3            0xC6

#define WRITE_0           0x07
#define WRITE_1           0x47
#define WRITE_2           0x87
#define WRITE_3           0xC7

#define ID_0              0x08            ///ID/0 for phase0 identification
#define ID_1              0x48            ///ID/1 for phase1 identification
//////////////////////////////////////////////////////////

//*>**************************************/
#define ADDRT              54
#define DELRT              55
#define GETRT              56
/*>***********************monitoring variables for address_space_1***************************************/

#define  character_framing_parity_error  0x00
#define  receiver_overrun_event          0x01
#define  runt_packet                     0x02
#define  oversize_packet                 0x03
#define  bad_CRC                         0x04
#define  ownership_error                 0x05
#define  unknown_format_error            0x06
#define  wrong_direction_error           0x07
#define  response_timeout                0x08

///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>........
#define min_data_len   7                          /*no data here*/
#define max_data_len   100
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.........
/*
source and destination addresses for transmitting and recieving

*/
#define  ADD1     0xc1
#define  ADD2     0xc2
#define  ADD3     0xc3
#define  ADD4     0xc4
#define  ADD5     0xc5
#define  ADD6     0xc6
#define  ADD7     0xc7

#define MY_ADD        0xda


#define  flag        0x05 ///rrrr.iiii
///>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>.........../*


#endif // SSP_PRIVATE_H_INCLUDED
