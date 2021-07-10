/*
 * Segmenter_private.h
 *
 *  Created on: Apr 6, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef SEGMENTER_PRIVATE_H_
#define SEGMENTER_PRIVATE_H_

/****************States************************/
#define SEGMENTER_STATE_READY						0
#define REASSEMBLER_STATE_READY						1
#define REASSEMBLER_STATE_REASSEMBLING_DATA			2
#define REASSEMBLER_STATE_REASSEMBLING_UNIT_DATA	3

/*****************State function***************/
void segmenter_ReadyState(void);
void Reassember_ReadyState(void);
void Reassember_ReassemblingDataState(void);
void Reassember_ReassemblingUnitDataState(void);

void AssembleData(u8 *Info,u32 InfoLength);

/***************Variables**********************/
u8 ReassemblerCurrentState = REASSEMBLER_STATE_READY;




/***************Internal Flags******************/
typedef struct{
	u8 N;			//Number of Segments remaining to be reassembled
}Segmenter_InternalFlag_t;

Segmenter_InternalFlag_t Segmenter_InternalFlag;


#endif /* SEGMENTER_PRIVATE_H_ */
