/*
 * CRC_program.c
 *
 *  Created on: Jan 4, 2021
 *      Author: MohammedGamalEleish
 */


#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "CRC_interface.h"

u16 CRC16_u16CCITTFalse(u8* Copy_u8PtrData, u32 Copy_u32DataLength)
{

	if(Copy_u32DataLength <= 0)
		return 0xFFFF;

	u8 B1 = 0, B0 = 0;
	u32 Bindex;

	#define CRC_POLY		0x1021

	/*16 bit CRC register with 0xFFFF initial value*/
	u16 CRC_REG = 0xFFFF;

	/*Check the first two Bytes if they exist*/
	if(Copy_u32DataLength >= 1)
		B1 = Copy_u8PtrData[0];
	if(Copy_u32DataLength >= 2)
		B0 = Copy_u8PtrData[1];

	/*Get The Actual Initial Value*/
	CRC_REG ^= ((B1<<8)|B0);

	/*looping on the data bytes starting from the third element to the last element
	 * of the data + 2 additional bytes all zeros									*/
	for(Bindex=2;Bindex<Copy_u32DataLength+2;Bindex++)
	{
		/*Get the current byte of the data, if the data is finished, add two bytes of 0 */
		u8 CurrentByte = Bindex >= Copy_u32DataLength?	0:Copy_u8PtrData[Bindex];
		u8 i;

		/*Looping on the bits of the current byte*/
		for(i=0;i<8;i++)
		{
			/*Check if the MSB of the CRC register is set*/
			if(GET_BIT(CRC_REG,15))
			{
				/*Shift in the next bit of data starting from the MSB to LSB
				 * and XOR the CRC register with the polynomial*/
				CRC_REG <<=1;
				CRC_REG |= GET_BIT(CurrentByte,(7-i));
				CRC_REG ^= CRC_POLY;
			}
			else
			{
				/*if Not, shift in the next bit of data starting from MSB to LSB*/
				CRC_REG <<= 1;
				CRC_REG |= GET_BIT(CurrentByte,(7-i));
			}

		}
	}

	/*After finishing the loop, return the CRC bytes*/
	return CRC_REG;
}
