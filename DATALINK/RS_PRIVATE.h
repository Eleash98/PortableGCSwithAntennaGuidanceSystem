#ifndef RS_PRIVATE_H
#define RS_PRIVATE_H
#include "RS_INTERFACE.h"
#define RS_DEBUG            1

unsigned char* RS_223_to_255_encoder_P_pointer_unsigned_char(unsigned char* data);
unsigned char* RS_223_to_255_decoder_P_pointer_unsigned_char(unsigned char* data);
unsigned char* RS_223_to_255_void_divide_poly(unsigned char* poly1,unsigned char* poly2,unsigned char* q1);
unsigned char* RS_223_to_255_void_RS_mult_poly(unsigned char*poly1,unsigned char*poly2);

#endif
