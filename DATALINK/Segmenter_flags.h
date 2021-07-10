/*
 * Segmenter_flags.h
 *
 *  Created on: Apr 6, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef SEGMENTER_FLAGS_H_
#define SEGMENTER_FLAGS_H_

#define SEGMENTER_FLAG_SET			1
#define SEGMENTER_FLAG_CLEAR		0

typedef struct{
	u8 DataRequest;
	u8 UnitDataRequest;
	u8 ConnectRequest;
	u8 DisconnectRequest;
	u8 FlowoffRequest;
	u8 FlowonRequest;
}DLSAP_Request_t;

typedef struct{
	u8 DataIndication;
	u8 UnitDataIndication;
	u8 ConnectIndication;
	u8 ConnectConfirm;
	u8 DisconnectIndication;
	u8 DisconnectConfirm;
	u8 ErrorIndication;
}DLSAP_Indication_t;

enum Segmenter_ErrorCode{
	Y,					//Data too long to Segment
	Z					//Reassembly Error
};

DLSAP_Request_t			DLSAP_Request;
DLSAP_Indication_t		DLSAP_Indication;




#endif /* SEGMENTER_FLAGS_H_ */
