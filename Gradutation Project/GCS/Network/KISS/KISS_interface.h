#ifndef KISS_INTERFACE_H_INCLUDED
#define KISS_INTERFACE_H_INCLUDED

#include "STD_TYPES.h"

typedef struct
{
	u8 *PtrtoFrame;
	u32 FrameLength;
	u16 stuffingbytes;
	u16 destuffingbytes;
	u8 commandtypebyte;
} Kiss_frame_t;
/*it is a function receiving the data and the command
 and port byte to build and construct all frame*/
u8* construct_frame(u8* data,u32*data_len);

/* function that extract the command and the data from received frame */
u8* deframing_function (u8 *receivedframe,u32*data_len);





#endif // KISS_INTERFACE_H_INCLUDED
