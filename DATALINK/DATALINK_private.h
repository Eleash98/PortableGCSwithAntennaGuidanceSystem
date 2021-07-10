#ifndef DATALINK_PRIVATE_H_INCLUDED
#define DATALINK_PRIVATE_H_INCLUDED

/**Channel Encoders Stuff*/

#define ENCODER_REED_SOLOMON    1
#define ENCODER_GOLAY           2
#define ENCODER_PCH             3
#define RANDOMIZER              4

#define DECODER_REED_SOLOMON    1
#define DECODER_GOLAY           2
#define DECODER_PCH             3


#define DATALINK_DISABLE         0
#define DATALINK_ENABLE          1

#define DATALINK_AX25               0
#define DATALINK_CCSDS              1

#define DATALINK_MOD_GFSK           0
#define DATALINK_MOD_PFSK           1
#define DATALINK_MOD_MFSK           2
#define DATALINK_MOD_QAM            3

#define DATALINK_AX25_HALFDUPLEX        0
#define DATALINK_AX25_FULLDUPLEX        1

#define DATALINK_AX25_CONTROL_8BIT      0
#define DATALINK_AX25_CONTROL_16BIT      1


u16 RS_Polys[16] = {285,299,301,333,351,355,357,361,369,391,397,425,451,463,487,501};
u16 Golay_Polys[2] = {0xAE3,0xC75};

typedef struct{

    u8 Flag;
    u16 Poly;
    u8 InitialRoot;

}ReedSolomonConfig_t;

typedef struct{
    u8 Flag;
    u8 Poly;

}RandomizerConfig_t;

typedef struct{
    u8 Flag;
    u16 Poly;
}GolayConfig_t;

typedef struct{

    ReedSolomonConfig_t ReedSolomon;
    u8 PCHFlag;
    GolayConfig_t Golay;
    RandomizerConfig_t Randomizer;

}ChannelEncoderConfig_t;

typedef struct{
    u8 Flag;
    u8 SynchWord;
    u8 Length;
}SynchWord_t;


typedef struct{

    u8 TransmissionMode;
    u8 ControlFieldLength;
    u32 N1;
    u32 N2;
    u32 AckTimer;
    char DestAddress[7];
    u8 DestSSID;
    char SrcAddress[7];
    u8 SrcSSID;
} AX_Config_t;

typedef struct{

    u32 TransmissionLimit;
    u32 T1Initial;
    u32 FopSlidingWindow;
    u16 SCID;           //spacecraft ID
} CC_Config_t;

typedef union {
    AX_Config_t AX;
    CC_Config_t CC;
}Protocol_Config_t;

typedef struct{
    u8 UsedProtocol;
    Protocol_Config_t Config;
    ChannelEncoderConfig_t ChannelEncoder;
    SynchWord_t ProtocolSynchWord;
    SynchWord_t ModulatorSynchWord;
    u8 KissFlag;
    u32 Frequency;
    u32 BaudRate;
    u8 ModulationType;
    f32 ModulationIndex;/***/
}Protocol_t;



typedef struct{

    Protocol_t Transmitter;
    Protocol_t Receiver;

}DataLink_Config_t;

/*ReedSolomonConfig_t ReedSolomon = {0,0,0};
GolayConfig_t       Golay = {0,0};
RandomizerConfig_t  Randomizer = {0,0};

ChannelEncoderConfig_t  ChannelConfig = {{0,0,0},0,{0,0},{0,0}};

SynchWord_t             Synch = {0,0};
*/
u8* ChannelEncode(u8 Encoder_ID,u8*EncoderFrame,u32*EncoderFrameLength);
u8* ChannelDecode(u8 Decoder_ID,u8*DecoderFrame,u32*DecoderFrameLength);



u32 GetKissLength(u8* KissFrame);



DataLink_Config_t GCS_DataLink;



















#endif // DATALINK_PRIVATE_H_INCLUDED
