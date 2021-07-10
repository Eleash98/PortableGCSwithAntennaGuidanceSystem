#ifndef KISS_PRIVATE_H_INCLUDED
#define KISS_PRIVATE_H_INCLUDED
#include "KISS_interface.h"


/* initialize flags costants values*/
#define FEND    0xC0   /* Frame End flag*/
#define FESC    0xDB   /* Frame Escaped flag*/
#define TFEND   0xDC   /* Transposed Frame End flag*/
#define TFESC   0xDD   /* Transposed Frame Escaped flag*/



/* initialize commands constants values */

/* 1) This frame contains data that should be sent out of the TNC. The maximum
number of bytes is determined by the amount of memory in the TNC.*/
#define Dataframe   0x00
/* 2) The amount of time to wait between keying the transmitter and beginning to send
data (in 10 ms units).*/
#define TXDELAY     0x01
/* 3) The persistence parameter. Persistence=Data*256-1. Used for CSMA.*/
#define P            0x02
/* 4) Slot time in 10 ms units. Used for CSMA.*/
#define SlotTime     0x03
/* 5) The length of time to keep the transmitter keyed after sending the data (in 10 ms
units).*/
#define TXtail       0x04
/* 6) 0 means half duplex, anything else means full duplex.*/
#define FullDuplex   0x05
/* 7) Device dependent.*/
#define SetHardware  0x06
/* 8) Exit KISS mode. This applies to all ports and requires a port code of 0xF.*/
#define Return       0xFF


/*a function to add C0 flag at the last byte of the frame */
void add_FEND_flag(Kiss_frame_t *TXframe);

/*a function replace frame end flag (FEND=C0) with two flags (FESC=DB && TFEND=DC) */
void replace_FEND_flag(Kiss_frame_t *TXframe,u32 i);

/*a function replace frame escaped flag (FESC=C0) with two flags (FESC=DB && TFESC=DD)  */
void replace_FESC_flag(Kiss_frame_t *TXframe,u32 i);

/* it is a function to delete FEND flag (C0) from the frame */
void FEND_destuffing(Kiss_frame_t *RXframe);

/* it is a function to extract FEND and FESC from the data  replace (FESC,TFEND)
 flags with FEND flag and (FESC,TFEND) flags with FESC flag */
void extract_FEND_FESC(Kiss_frame_t *RXframe);

/* a function to recover FESC flag*/
void FESC_flag_recovery(Kiss_frame_t *RXframe,u32 i);

/* a function to recover FEND flag*/
void FEND_flag_recovery(Kiss_frame_t *RXframe,u32 i);

/*a function to calculate the unknown frame length */
void get_frame_length(Kiss_frame_t *RXframe);\

/*function to generate crc16*/
u16 CRC16_u16CCITTFalse(u8* Data, u16 Length);

#endif // KISS_PRIVATE_H_INCLUDED
