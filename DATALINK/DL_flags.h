/*
 * DL_flags.h
 *
 *  Created on: Jan 26, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef DL_FLAGS_H_
#define DL_FLAGS_H_

#define DL_FLAG_CLEAR			0
#define DL_FLAG_SET				1

enum 
{
	A, 				//— F=1 received but P=1 not outstanding.
	B, 				//— Unexpected DM with F=1 in states 3, 4 or 5.
	C, 				//— Unexpected UA in states 3, 4 or 5.
	D, 				//— UA received without F=1 when SABM or DISC was sent P=1.
	E, 				//— DM received in states 3, 4 or 5.
	F, 				//— Data link reset; i.e., SABM received in state 3, 4 or 5.
	I, 				//— N2 timeouts: unacknowledged data.
	J, 				//— N(r) sequence error.
	L, 				//— Control field invalid or not implemented.
	M, 				//— Information field was received in a U- or S-type frame.
	N, 				//— Length of frame incorrect for frame type.
	O, 				//— I frame exceeded maximum allowed length.
	P, 				//— N(s) out of the window.
	Q, 				//— UI response received, or UI command with P=1 received.
	R, 				//— UI frame exceeded maximum allowed length.
	S, 				//— I response received.
	T, 				//— N2 timeouts: no response to enquiry.
	U, 				//— N2 timeouts: extended peer busy condition.
	V, 				//— No DL machines available to establish connection.
	NoError = 255
}ErrorCode;

typedef struct
{
	u8 ConnectRequest;
	u8 DisconnectRequest;
	u8 DataRequest;
	u8 UnitDataRequest;
	u8 FlowoffRequest;
	u8 FlowonRequest;
}DL_Request_t;

typedef struct
{
	u8 ConnectConfirm;
	u8 ConnectIndication;
	u8 DisConnectConfirm;
	u8 DisConnectIndication;
	u8 DataIndication;
	u8 UnitDataIndication;
	u16 ErrorIndication;
}DL_Indication_t;

DL_Request_t		DL_Request;
DL_Indication_t 	DL_Indication;


#endif /* DL_FLAGS_H_ */
