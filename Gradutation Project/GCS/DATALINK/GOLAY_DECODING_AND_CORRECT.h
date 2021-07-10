#ifndef GOLAY_DECODING_AND_CORRECT_H_INCLUDED
#define GOLAY_DECODING_AND_CORRECT_H_INCLUDED

void GOLAY_DECODING_FUNCTION(u32* decode_codeword_array,u32* syndrome_array,u32 length,u16 errs);
u32 correct(u32 cw, u16* errs);
void GOLAY_TEST(u8* frame_ptr,u8* encoded_frame_ptr,u16 frame_length);
u32 syndrome(u32 cw);

#endif // GOLAY_DECODING_AND_CORRECT_H_INCLUDED
