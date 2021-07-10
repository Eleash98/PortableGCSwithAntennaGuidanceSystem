/************************************************/
/*	Author		:	Mohamed G.Eleish			*/
/*	Date		:	14 DEC 2020					*/
/*	Version		:	V01							*/
/************************************************/


/********************************************************************************************/
/*Version 01 Descreption:																	*/
/*	(1) Created the File																	*/
/*	(2) Added functions Declerations':														*/
/*		(a)	void AX25_voidStuff(u8 *PtrtoFrame, u32 NumerOfBytes);							*/
/*		(b)	void AX25_voidInsertZero(u8 *PtrtoFrame, u32 NumberOfFrameBytes, u32 Offset)	*/
/********************************************************************************************/

/****************************************************/
/*Version 02 Descreption:                           */
/*  (1) Deleted functions Declerations':            */
/*      (a) void AX25_voidStuff(frame_t* Frame);    */
/*      (b) void AX25_voidDestuff(frame_t* Frame);  */
/*	(2) Added Macros:								*/
/*		(a) AX25_SHIFTLEFT							*/
/*		(b) AX25_SHIFTRIGHT							*/
/****************************************************/

/********************************************************************/
/*Version 03 Descreption:                           				*/
/*  (1) Added Variable:					            				*/
/*      (a) AX25_CONTROL_MODULO_TYPE			    				*/
/*	(2) Added Macros:												*/
/*		(a) AX25_NR													*/
/*		(b) AX25_NS													*/
/*	(3) Added Function Decleration:									*/
/*		(a) AX25_u8GetSequenceNumber(u8 Copy_u8SequenceNumber);		*/
/********************************************************************/

/************************************************************************************************************************************/
/*Version 04 Descreption:                           				                                                                */
/*	(1) Added Function Decleration:									                                                                */
/*		(a) u8 AX25_u8UFrameControlField(u8 Copy_u8UFrameControlFieldType , u8 Copy_u8PollFinal , u8 Copy_u8Poll , u8 Copy_u8Final);*/
/************************************************************************************************************************************/
/************************************************************************************************************************************/
/*Version 05 Descreption:                           				                                                                */
/*	(1) Edit Function Decleration:									                                                                */
/*		(a) u8 AX25_u8UFrameControlField(u8 Copy_u8UFrameControlFieldType , u8 Copy_u8PollFinal );*/
/************************************************************************************************************************************/

#ifndef AX25_PRIVATE_INCLUDED
#define AX25_PRIVATE_INCLUDED


#define AX25_SHIFTRIGHT			1
#define AX25_SHIFTLEFT			0



#define RESERVED				3

u8 AX25_CONTROL_MODULO_TYPE 	= AX25_8BITS;
u8 AX25_VERSION				= AX25_VERSION_2_X;




/*A function that inserts a 0 at the position Offset Bits in a frame created By malloc*/
void AX25_voidInsertZero(frame_t* Frame, u32 Offset);

/*A function that shifts an array right or lift by 1 bit starting from an offset*/
void AX25_voidShiftArray(u8 Copy_u8Direction, frame_t *Frame, u32 Copy_u32Offset);

/*A function that returns the requested sequence number*/
u8 AX25_u8GetSequenceNumber(u8 Copy_u8SequenceNumber);

/*A function to Construct address field*/
void AX25_voidConstructAddress (address_t* Address , u8* ConstructedAddress);


u8 AX25_u8GetBytes(u64 Number);

#endif // AX_25_H_INCLUDED
