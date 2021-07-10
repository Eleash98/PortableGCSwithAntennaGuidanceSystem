/*
 * PRINT_interface.h
 *
 *  Created on: Jan 7, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef PRINT_INTERFACE_H_
#define PRINT_INTERFACE_H_

#define _8_					1
#define _16_				2
#define _32_				3
#define _64_				4

#define SIMPLEX_PHYSICAL             0
#define DUPLEX_PHYSICAL                 1
#define DATA_LINK                   2


void PrintFrameType(frame_t* F);

void PrintFrameHEX(frame_t *F);

void PrintFrameBIN(frame_t *F);

void PrintState(u8 MachineLayer, u8 State);

void PrintTimer(u8 TimerID);

void PrintNumberBIN(u32 Copy_u32Number, u8 Bits);
#endif /* PRINT_INTERFACE_H_ */
