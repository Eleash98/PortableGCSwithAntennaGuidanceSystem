#ifndef DATALINK_INTERFACE_H_INCLUDED
#define DATALINK_INTERFACE_H_INCLUDED



//void SetChannelEncoderConfiguration(ChannelEncoderConfig_t* Config, SynchWord_t* SynchConfig);

void TransmitFrame(u8* Frame,u32 FrameLength, u8 FrameType, u8 VCID);

//void ParseReceived(char* Buff,u32 Len);


void Parse(u8 *ConfigArray);



#endif // DADTLINK_INTERFACE_H_INCLUDED
