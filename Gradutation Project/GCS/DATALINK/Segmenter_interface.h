/*
 * Segmenter_interface.h
 *
 *  Created on: Apr 6, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef SEGMENTER_INTERFACE_H_
#define SEGMENTER_INTERFACE_H_


//void segmenter_ReadyState(void);

void SM_voidSegmenter(void);

void SM_voidAssembler(void);

u8* ApptoSegmenterInfo;
u32 ApptoSegmenterLength;

u8* AssemblertoAppInfo;
u32 AssemblertoAppLength;

#endif /* SEGMENTER_INTERFACE_H_ */
