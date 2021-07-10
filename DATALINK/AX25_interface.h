/************************************************/
/*	Author		:	Mohamed G.Eleish			*/
/*	Date		:	14 DEC 2020					*/
/*	Version		:	V01							*/
/************************************************/


/************************************************************/
/*Version 01 Descreption:									*/
/*	(1) Created the File									*/
/*	(2) Added functions Declerations':						*/
/*		(a)	void AX25_voidGetFrame((void*) PtrtoFrame);		*/
/*		(b)	u32 AX25_u32GetFrameLength((void*) PtrtoFrame);	*/
/************************************************************/

/************************************************************/
/*Version 02 Descreption:									*/
/*	(1) Added functions Declerations':						*/
/*		(a)	void AX25_voidStuff(frame_t* Frame);		    */
/*		(b)	void AX25_voidDestuff(frame_t* Frame);	        */
/*  (2) Moved frame_t struct here                           */
/*  (3) Added DeStuffBits member to frame_t struct          */
/************************************************************/

/****************************************************************************************************************************/
/*Version 03Descreption:																									*/
/*	(1) Added functions Declerations':																						*/
/*		(a)	u16 AX25_u16ConstructControlField(u8 Copy_u8FrameType, u8 Copy_u8ControlFieldType, u8 Copy_u8PollFinal);	    */
/*		(b)	AX25_voidSetControlModulo(u8 Copy_u8Modulo);	        														*/
/*  (2) Added enum:		          																							*/
/*		enum SFrameControlFieldType{																						*/
/*						ReceiveReady = 0,																					*/
/*						ReceiveNotReady,																					*/
/*						Reject,																								*/
/*						SelectiveReject																						*/
/*						};																									*/
/*	(3) Added Macros:																										*/
/*		(a)	AX25_S_FRAME																									*/
/*		(b) AX25_U_FRAME																									*/
/*		(c) AX25_I_FRAME																									*/
/*		(d) AX25_8BITS																										*/
/*		(e) AX25_16BITS																										*/
/****************************************************************************************************************************/
/****************************************************************************************************************************/
/*Version 04Descreption:																									*/
/*	(1) Edit in functions Declerations':																					*/
/*		(a)	u16 AX25_u16ConstructControlField(u8 Copy_u8FrameType, u8 Copy_u8ControlFieldType, u8 Copy_u8PollFinal , u8 Copy_u8Poll , u8 Copy_u8Final)*/
/*  (2) Added enum:*/
/*		enum UFrameControlFieldType{*/
/*	Set_Async_Balanced_mode_Extended SABME => Command*/
/*	Set_Async_Balanced_mode SABM => Command            */
/*	Disconnect DISC => Command,                      */
/*	Disconnect_Mode DM => Response,                    */
/*	Unnumbered_Acknowledge UA => Response,*/
/*	Frame_Reject FRMR => Response,*/
/*	Unnumbered_Information UI => Either,*/
/*	Exchange_Identification XID => Either,*/
/*	Test TEST => Either*/
/*};
*/
/****************************************************************************************************************************/
/****************************************************************/
/*Version 05 Descreption:									    */
/*	(1) Edit in functions Declerations':						*/
/*		(a)	u16 AX25_u16ConstructControlField(u8 Copy_u8FrameType, u8 Copy_u8ControlFieldType, u8 Copy_u8PollFinal)*/
/*  (2) Added Marcos:                                           */
/*      (a) #define AX25_Flag          0x7E                     */
/*      (b) #define AX25_REPEATER       0                       */
/*      (c) #define AX25_NO_REPEATER    1                       */
/*  (3) Added Function:                                         */
/*      (a) void AX25_voidConstructAddress (address_t* Address , u8* ConstructedAddress);*/
/*      (b) void AX25_voidConstructFrame (frame_t* Frame , address_t* Address , u8 Copy_u8ControlFieldType, u8 Copy_u8PollFinal , u8 ProtocolPID , u8 Copy_u8Repeater , u8 FrameType , u8* Info , u32 InfoLength);*/
/*  (4) Added Struct:                                           */
/*typedef struct
{
    char Destination[6];
    char Source[6];
    char Repeater[6];
    u8 DesitinationSSID;
    u8 SourceSSID;
    u8 RepeaterSSID;
    u8 DestCommandResponse;
    u8 SrcCommandResponse;
    u8 Reserved;
    u8 HRepeated;
} address_t;                                                    */
/****************************************************************/
#ifndef AX25_INTERFACE
#define AX25_INTERFACE

#define AX25_S_FRAME		0
#define AX25_U_FRAME		1
#define AX25_I_FRAME		2

#define AX25_8BITS			0
#define AX25_16BITS			1

#define AX25_FLAG          0x7E

#define AX25_REPEATER       1
#define AX25_NO_REPEATER    0

#define AX25_NONE			0

#define AX25_START_FLAG_NOT_FOUND   1
#define AX25_FRAME_CORRECT          0
#define AX25_WRONG_CRC              2


/////////////////////////////////////////////////////////////////////
#define AX25_XID_ABSENT										(s8)(-1)

#define AX25_XID_FORMAT_INDICATOR							0x82

#define AX25_XID_GROUP_IDENTIFIER							0x80


#define AX25_XID_CLASSES_OF_PROCEDURES_IDENTIFIER			2

#define AX25_XID_TRANSMISSION_MODE							1	//Defaults to half duplex

/*Transmission mode options*/
#define AX25_XID_HALFDUPLEX									1
#define AX25_XID_FULLDUPLEX									2


#define AX25_XID_HDLC_OPTIONAL_FUNCTIONS_IDENTIFIER			3

#define AX25_XID_HDLC_DEFAULT								0x2A486

/*Reject Type Options*/
#define AX25_XID_IMPLICIT_REJECT							1
#define AX25_XID_SELECTIVE_REJECT							2
#define AX25_XID_SELECTIVE_REJECT_REJECT					3

/*Modulo Options*/
#define AX25_XID_8BIT_MODULO								1
#define AX25_XID_128BIT_MODULO								2

#define AX25_XID_TRANSMIT								0
#define AX25_XID_RECEIVE								1

#define AX25_XID_I_TX_LENGTH								5
#define AX25_XID_I_TX_DEFAULT								256

#define AX25_XID_I_RX_LENGTH								6
#define AX25_XID_I_RX_DEFAULT								256

#define AX25_XID_TX_WINDOW_SIZE								7

#define AX25_XID_RX_WINDOW_SIZE								8
#define AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO8				4
#define AX25_XID_RX_DEFAUL_WINDOW_SIZE_MODULO128			32

#define AX25_XID_ACK_TIMER									9
#define AX25_XID_DEFAULT_ACK_TIMER						3000

#define AX25_XID_RETRIES								10
#define AX25_XID_DEFAULT_RETRIES							10


///////////////////////////////////////////////////////

#define AX25_COMMAND_FRAME				0
#define AX25_RESPONSE_FRAME				1


#define AX25_VERSION_2_0				0
#define AX25_VERSION_2_X				1


#define AX25_NR					0
#define AX25_NS					1
///////////////////////////////////////////////////////
enum
{
	Sieze,Release,Data,Expedited
}Requsets;


typedef struct
{
    char Destination[7];
    char Source[7];
    char *Repeater[7];
    u8 NOfRepeaters;
    u8 RepeaterUsed;
    u8 DestinationSSID;
    u8 SourceSSID;
    u8 *RepeaterSSID;
    u8 DestCommandResponse;
    u8 SrcCommandResponse;
    u8 *HRepeated;
} address_t;

typedef struct
{
	u8 FrameType;
	address_t FrameAddress;
	u8 ControlFieldType;
	u8 PollFinal;
	u16 ControlField;
	u8 CMDRES;
	u8 NR;
	u8 NS;
	u8 PID;
	u8 *Info;
	u32 InfoLength;
	u8 *PtrtoFrame;
	u32 FrameLength;
	u32 StuffBits;
	u32 DeStuffBits;
} frame_t;


typedef struct
{
  enum{isRequest,isFrame}type;

  union{
    u8 Request;
    frame_t *F;
  }content;
}Queue_t;


enum SFrameControlFieldType{
	ReceiveReady = 0,
	ReceiveNotReady,
	Reject,
	SelectiveReject
};
enum UFrameControlFieldType{
	Set_Async_Balanced_mode_Extended = 0x6F /*SABME => Command*/,
	Set_Async_Balanced_mode = 0x2F/*SABM => Command*/,
	Disconnect = 0x43/*DISC => Command*/,
	Disconnect_Mode = 0x0F/*DM => Response*/,
	Unnumbered_Acknowledge = 0x63 /*UA => Response*/,
	Frame_Reject = 0x87/*FRMR => Response*/,
	Unnumbered_Information = 0x03/*UI => Either*/,
	Exchange_Identification = 0xAF/*XID => Either*/,
	Test = 0xE3/*TEST => Either*/
};
enum Protocol_Identifier_PID {
	ISO_8208_CCITT_X_25_PLP = 0x01 ,
	Compressed_TCP_IP_packet = 0x06 ,
	Uncompressed_TCP_IP_packet = 0x07 ,
	Segmentation_fragment = 0x08 ,
	TEXNET_datagram_protocol = 0xC3 ,
	Link_Quality_Protocol = 0xC4 ,
	Appletalk = 0xCA ,
	Appletalk_ARP = 0xCB ,
	ARPA_Internet_Protocol = 0xCC ,
	ARPA_Address_resolution = 0xCD ,
    FlexNet = 0xCE ,
    Net_ROM = 0xCF ,
    No_layer_3_protocol_implemented = 0xF0 ,
    Escape_character = 0xFF
};


typedef struct
{
	u8 FormatIndicator;
	u8 GroupIdentifier;
	u16 GroupLength;
	u16 ClassesOfProcedures;
	u32 HDLCOptionalFunctions;
	u64 MaxTXIFieldLength;
	u64 MaxRXIFieldLength;
	u8 TXWindowsize;
	u8 RXWindowsize;
	u32 ACKTimer;
	u32 RetryCount;
	u8* XIDInfo;
	u32 XIDInfoLength;
	u8 State;
}XIDInfo_t;

u8 NR;
u8 NS;
u8 VR;
u8 VS;
u8 VA;


u8 AX25_u8ControlModulo(void);


/*A function that reads a frame and returns it*/
void AX25_voidGetFrame(frame_t* Frame);

/*A function that returns the Length of the last received frame*/
u32 AX25_u32GetFrameLength(frame_t *Frame);

/*A function to do the BIT stuffing before transmitting a frame*/
void AX25_voidStuff(frame_t* Frame);

/*A function to do the BIT de-stuffing upon reception of a frame*/
void AX25_voidDestuff(frame_t* Frame);

/*Constructs control field for a specific frame type*/
u16 AX25_u16ConstructControlField(u8 Copy_u8FrameType, u8 Copy_u8ControlFieldType, u8 Copy_u8PollFinal);

/*Sets the Modulo type for either 8 bits or 16 bits*/
void AX25_voidSetControlModulo(u8 Copy_u8Modulo);

/*A function to construct frame*/
void AX25_voidConstructFrame (frame_t* Frame);

/*returns the Control Modulo used*/
u8 AX25_u8ControlModulo(void);

void AX25_ControlTest(void);

/*A Function to DeConstructed*/
u8 AX25_u8DeConstructFrame (frame_t* Frame);

u8 AX25_u8ExtractSequenceNumber(u8 FrameType, u16 FrameControlField,u8 SequenceType);

u8 AX25_u8ExtractPollFinal(u8 FrameType, u16 FrameControlField);

u8 AX25_u8ExtractFrameControlType(u8 FrameType, u16 ControlField);

void AX25_voidSetControlModulo(u8 ControlModulo);

XIDInfo_t* AX25_XIDInfo_tPtrConstructXIDInfo(u8 TrasmissionMode,u8 RejectType, u8 ControlModulo,u64 MaxTxLength,u64 MaxRxLength,u8 TxWindowSize,u8 RxWindowSize, u32 AckTimer, u32 Retries);

XIDInfo_t* AX25_XIDInfo_tPtrDeconstructXIDInfo(u8 *Info,u32 InfoLength);

frame_t *AX25_frame_tPtrCreatIFrame(char*DestAddress,u8 DestSSID,char*SrcAddress,u8 SrcSSID,u8 CMDRES,u8 PollFinal,enum Protocol_Identifier_PID PID,u8 *Info,u32 InfoLength);
frame_t *AX25_frame_tPtrCreatUFrame(char*DestAddress,u8 DestSSID,char*SrcAddress,u8 SrcSSID,u8 CMDRES,u8 PollFinal,enum UFrameControlFieldType ControllFieldType,u8 *Info,u32 InfoLength);
frame_t *AX25_frame_tPtrCreatSFrame(char*DestAddress,u8 DestSSID,char*SrcAddress,u8 SrcSSID,u8 CMDRES,u8 PollFinal,enum SFrameControlFieldType ControllFieldType,u8 *Info,u32 InfoLength);

void AX25_voidFreeFrame(frame_t* Frame);

void AX25_voidFreeXIDInfo(XIDInfo_t* Info);

#endif // AX_25_H_INCLUDED
