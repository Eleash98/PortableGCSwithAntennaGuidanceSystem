#ifndef RS_INTERFACE_H
#define RS_INTERFACE_H

unsigned char* RS_223_255_encoder_driver_pointer_unsigned_char(unsigned char* data, unsigned long int* data_len);
unsigned char* RS_223_255_decoder_driver_pointer_unsigned_char(unsigned char* data, unsigned long int* data_len);
void RS_polynomial_generator(unsigned int poly,unsigned char Initial_root);
#endif
