/*
 * MDL_statemachine.h
 *
 *  Created on: Jan 21, 2021
 *      Author: MohammedGamalEleish
 */

#ifndef MDL_INTERFACE_H_
#define MDL_INTERFACE_H_


#define MDL_NEGOTIATE_REQUEST				0
#define MDL_NEGOTIATE_CONFIRM				1
#define MDL_ERROR_INDICATE					2

//Transmission Mode Options
#define MDL_FULL_DUPLEX                     0
#define MDL_HALF_DUPLEX                     1

//Transmission mode
#define MDL_TRANSMISSION_MODE               0

//Reject Type Options
#define MDL_SEL_REJ_REJ                     0
#define MDL_SEL_REJ                         1
#define MDL_IMP_REJ                         2

//Optional Functions
#define MDL_REJECT_TYPE                     1

//Control Modulo Options
#define MDL_MOD_8                           0
#define MDL_MOD_16                          1

//Control modulo
#define MDL_CONTROL_MODULO_TYPE             2

//max receive info length
#define MDL_RX_MAX_LEN                      3

//tx transmit info max length
#define MDL_TX_MAX_LEN                      4

//max receive window vale
#define MDL_RX_K                            5

#define MDL_TX_K                            6

//Ack Timer value (ms)
#define MDL_ACK_TIMER_T1                    7

//MDL retry count value
#define MDL_RETRY_COUNT                     8

void MDL_voidSetAcceptableValues(u8 MDL_Patameter, u32 Value);

void SM_voidManagmentDataLink(void);

#endif /* MDL_INTERFACE_H_ */
