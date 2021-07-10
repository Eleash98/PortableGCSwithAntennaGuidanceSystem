#ifndef MDL_FLAGS_H_INCLUDED
#define MDL_FLAGS_H_INCLUDED

#define MDL_FLAG_CLEAR			    0
#define MDL_FLAG_SET				1

typedef struct
{
    u8  NegotiateRequest;
}MDL_Request_t;

typedef struct
{
    u8  NegotiateConfirm;
    u8  ErrorIndicate;
}MDL_Indication_t;


typedef struct
{
    u64  N1;
    u8   K;
    u32  T1;
    u32  Retry;
    u8  TransmissionMode;
    u8  RejectType;
    u8  ControlModulo;
}MDL_NegotiateRequest_t;

enum MDL_ErrorCode_t
{
    MDL_A,  /*XID command without P=1*/
    MDL_B,  /*Unexpected XID response*/
    MDL_C,  /*Management retry limit exceeded*/
    MDL_D  /*XID response without F=1*/
};


MDL_Request_t           MDL_Request;
MDL_Indication_t        MDL_Indication;
MDL_NegotiateRequest_t  MDL_NegotiateRequest;


#endif // MDL_FLAGS_H_INCLUDED
