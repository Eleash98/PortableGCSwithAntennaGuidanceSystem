#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "CC1101_interface.h"

#include "CC1101_private.h"

uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t My_addr, Tx_addr, Rx_addr, Pktlen, pktlen, Lqi, Rssi;
uint8_t rx_addr,sender,lqi;
int8_t rssi_dbm;

int CC1101_freq_select, CC1101_mode_select, CC1101_channel_select;

uint32_t prev_millis_1s_timer = 0;

uint8_t cc1100_debug = 0;								//set CC1100 lib in no-debug output mode
uint8_t tx_retries = 1;
uint8_t rx_demo_addr = 3;
int interval = 1000;

u8 CC1101_CurrentState = CC1101_STATE_RECEIVE;
u8 CC1101_ReceivingFlag = 0;
u8 CC1101_TransmitFlag = 0;
u32 CC1101_DataRate = 0;

u8 ProtocolSynchWord = 0;
u8 ProtocolSynchWordLength = 0;


u8* CC1101_u8PtrReadFrame(u32* ReceivedLength){

    u8 Buff[10000];
    u32 BuffCount = 0;
    memset(Buff,0,10000);
    while(CC1101_TransmitFlag == 1)
        delayMicroseconds(1);
    while(!digitalRead(GDO1) && CC1101_CurrentState == CC1101_STATE_RECEIVE);
    if(CC1101_CurrentState == CC1101_STATE_TRANSMIT)
        return NULL;
    //Wait for some bytes to be received
    CC1101_ReceivingFlag = 1;
    delayMicroseconds((ProtocolSynchWordLength+1)*1000000*8/CC1101_DataRate + 10);
    u8 BytesAvailable = CC1101_spi_read_register(RXBYTES);

    for(u8 i = 0;i <ProtocolSynchWordLength;i++){
        if (CC1101_spi_read_register(RXFIFO_SINGLE_BYTE) != ProtocolSynchWord){
            CC1101_ReceivingFlag = 0;
            return NULL;
        }
    }
    u8 LocalSynch = 0;
    while(1){
        BytesAvailable = CC1101_spi_read_register(RXBYTES);
        if(BytesAvailable == 1)
            delayMicroseconds(1000000*8/CC1101_DataRate + 10);
        if(BytesAvailable == 0){
            CC1101_ReceivingFlag = 0;
            return NULL;
        }
        Buff[BuffCount] = CC1101_spi_read_register(RXFIFO_SINGLE_BYTE);
        if(Buff[BuffCount++] == ProtocolSynchWord)
            LocalSynch++;
        else
            LocalSynch = 0;
        if(LocalSynch == ProtocolSynchWordLength){
            //finished
            CC1101_ReceivingFlag = 0;
            BuffCount -= ProtocolSynchWordLength;
            u8* Frame = (u8*)malloc(sizeof(u8));
            memcpy(Frame,Buff,BuffCount);
            *ReceivedLength = BuffCount;
            return Frame;
        }
   }

}


void CC1101_voidSetSynchWord(u8 PSynchWord,u8 PLength,u8 CCSynch[2], u8 CCSynchLength, u8 PreambleLength){

    ProtocolSynchWord = PSynchWord;
    ProtocolSynchWordLength = PLength;

    CC1101_spi_write_register(SYNC1,CCSynch[0]);
    CC1101_spi_write_register(SYNC0,CCSynch[1]);

    u8 mdmcfg2 = CC1101_spi_read_register(MDMCFG2);
    u8 mdmcfg3 = CC1101_spi_read_register(MDMCFG3);

    mdmcfg2 &= 0xf8;
    mdmcfg3 &= 0x8f;

    if(CCSynchLength == 2)
        mdmcfg2 |= 6;
    else if(CCSynchLength == 4)
        mdmcfg2 |= 7;
    CC1101_spi_write_register(MDMCFG2,mdmcfg2);


    if(PreambleLength <= 4)
        mdmcfg3 |= (PreambleLength-2);
    else if(PreambleLength <= 8)
        mdmcfg3 |= (PreambleLength/2);
    else if(PreambleLength == 12)
        mdmcfg3 |= 5;
    else if(PreambleLength == 16)
        mdmcfg3 |= 6;
    else if(PreambleLength == 24)
        mdmcfg3 |= 7;

    CC1101_spi_write_register(MDMCFG3,mdmcfg3);

}

void CC1101_voidInit(u32 Frequency, u8 ModulationType,f32 ModulationIndex,u32 DataRate){


    CC1101_begin(0);
    CC1101_DataRate = DataRate;
    u32 BandWidth;
    CC1101_voidSetFrequency(Frequency);
    CC1101_set_modulation_type(ModulationType);

    f32 Dev = 0;

    if(ModulationType == CC1101_MOD_4FSK)
        Dev = ModulationIndex * DataRate*3.0/2.0;
    else
        Dev = ModulationIndex * DataRate /2.0;

    BandWidth = DataRate + 2 * Dev;

    u8 Deve = log2f(Dev*pow(2,14)/26000000);
    u8 Devm = (Dev/pow(2,Deve))*(pow(2,17)/26000000) - 8;
    u8 deviant = (Deve & 7)<<4 | (Devm & 7);

    u8 Dre = log2f(DataRate*pow(2,20)/26000000);
    u8 Drm = (DataRate*pow(2,28))/(26000000*pow(2,Dre)) - 256;

    u8 Be = log2f(26000000/(32*BandWidth));
    u8 Bm = 26000000/(8*pow(2,Be)) -4;

    u8 mdmcfg4 = (Be & 3)<<6 | (Bm & 3)<<4 | (Dre & 15);
    u8 mdmcfg3 = Drm;

    CC1101_set_datarate(mdmcfg4,mdmcfg3,deviant);

    //Disable Address Filtering

    u8 Reg = CC1101_spi_read_register(PKTCTRL1);
    Reg &= 0xFC;
    CC1101_spi_write_register(PKTCTRL1,Reg);

    //Set Tx,Rx Threshhold limit
    CC1101_spi_write_register(FIFOTHR,0x0e);

    //Set Signals on GD pins
    //high when Tx fifo is full, low when below threshold
    CC1101_spi_write_register(IOCFG0,3);    //GD0

    //high when Rx fifo is full, low when below threshold
    CC1101_spi_write_register(IOCFG1,6);    //GD1

    CC1101_receive();
    CC1101_spi_write_register(PKTCTRL0,0x2);

}


void CC1101_voidTransferInfinitePayload(u8 *SendBuff, u32 BuffLen){

    while(CC1101_ReceivingFlag == 1)
        (delayMicroseconds(1));

    CC1101_TransmitFlag = 1;
    CC1101_CurrentState = CC1101_STATE_TRANSMIT;
    if(GET_BIT(CC1101_spi_read_register(TXBYTES),7) == 1){
        //Underflow Happend
        CC1101_spi_write_strobe(SFTX);
        delayMicroseconds(100);
        CC1101_spi_write_strobe(STX);
    }

    u32 buffIndex = 0;
    CC1101_spi_write_register(PKTCTRL0,0x2);
    CC1101_spi_write_register(PKTLEN,BuffLen%256);

    CC1101_spi_write_burst(TXFIFO_BURST,SendBuff,64);
    CC1101_spi_write_strobe(STX);
    BuffLen -= 64;
    buffIndex += 64;

    u32 N = BuffLen/60;
    for(u32 i = 0; i < N;i++){
        while(digitalRead(GDO0));
        CC1101_spi_write_burst(TXFIFO_BURST,SendBuff+buffIndex,60);
        if(BuffLen < 252)
            CC1101_spi_write_register(PKTCTRL0,0);
        BuffLen -= 60;
        buffIndex += 60;
    }
    while(digitalRead(GDO0));
    CC1101_spi_write_burst(TXFIFO_BURST,SendBuff+buffIndex,BuffLen);

    CC1101_CurrentState = CC1101_STATE_RECEIVE;
    CC1101_TransmitFlag = 0;
}

void CC1101_voidSetFrequency(u32 Frequency){

    u8 freq2,freq1,freq0;
    u32 FreqWord = Frequency*pow(2,16)/26000000;
    freq2 = (FreqWord>>16) & 0xff;
    freq1 = (FreqWord>>8) & 0xff;
    freq0 = (FreqWord) & 0xff;

    CC1101_spi_write_register(FREQ2,freq2);                                         //stores the new freq setting for defined ISM band
    CC1101_spi_write_register(FREQ1,freq1);
    CC1101_spi_write_register(FREQ0,freq0);

}

//-------------------------[CC1101 reset function]------------------------------
void CC1101_reset(void)                  // reset defined in CC1101 datasheet
{
    digitalWrite(SS_PIN, LOW);
    delayMicroseconds(10);
    digitalWrite(SS_PIN, HIGH);
    delayMicroseconds(40);

    CC1101_spi_write_strobe(SRES);
    delay(1);
}
//-----------------------------[END]--------------------------------------------

//------------------------[set Power Down]--------------------------------------
void CC1101_powerdown(void)
{
    CC1101_sidle();
    CC1101_spi_write_strobe(SPWD);               // CC1101 Power Down
}
//-----------------------------[end]--------------------------------------------

//---------------------------[WakeUp]-------------------------------------------
void CC1101_wakeup(void)
{
    digitalWrite(SS_PIN, LOW);
    delayMicroseconds(10);
    digitalWrite(SS_PIN, HIGH);
    delayMicroseconds(10);
    CC1101_receive();                            // go to RX Mode
}
//-----------------------------[end]--------------------------------------------

//---------------------[CC1101 set debug level]---------------------------------
uint8_t CC1101_set_debug_level(uint8_t set_debug_level)  //default ON
{
    debug_level = set_debug_level;        //set debug level of CC1101 outputs

    return debug_level;
}
//-----------------------------[end]--------------------------------------------

//---------------------[CC1101 get debug level]---------------------------------
uint8_t CC1101_get_debug_level(void)
{
    return debug_level;
}
//-----------------------------[end]--------------------------------------------

//----------------------[CC1101 init functions]---------------------------------
uint8_t CC1101_begin(volatile uint8_t My_addr)
{
    uint8_t partnum, version;
    //extern int CC1101_freq_select, CC1101_mode_select, CC1101_channel_select;

    pinMode(GDO0, INPUT);                 //setup AVR GPIO ports
    pinMode(GDO1, INPUT);

    CC1101_set_debug_level(CC1101_set_debug_level(1));   //set debug level of CC1101 outputs

    if(debug_level > 0){
          printf("Init CC1101...\r\n");
    }

    CC1101_spi_begin();                          //inits SPI Interface
    CC1101_reset();                              //CC1101 init reset

    CC1101_spi_write_strobe(SFTX);delayMicroseconds(100);//flush the TX_fifo content
    CC1101_spi_write_strobe(SFRX);delayMicroseconds(100);//flush the RX_fifo content

    partnum = CC1101_spi_read_register(PARTNUM); //reads CC1101 partnumber
    version = CC1101_spi_read_register(VERSION); //reads CC1101 version number

    //checks if valid Chip ID is found. Usualy 0x03 or 0x14. if not -> abort
    if(version == 0x00 || version == 0xFF){
        if(debug_level > 0){
            printf("no CC11xx found!\r\n");
        }
        return FALSE;
    }

    if(debug_level > 0){
          printf("Partnumber: 0x%02X\r\n", partnum);
          printf("Version   : 0x%02X\r\n", version);
    }




    //set modulation mode
    //CC1101_set_mode(CC1101_mode_select);

    //set ISM band
    //CC1101_set_ISM(CC1101_freq_select);

    //set channel
    CC1101_set_channel(0);

    //set output power amplifier
    CC1101_set_output_power_level(8);            //set PA to 0dBm as default

    //set my receiver address
    //CC1101_set_myaddr(My_addr);                  //My_Addr from EEPROM to global variable

    if(debug_level > 0){
          printf("...done!\r\n");
    }

                                  //set CC1101 in receive mode

    return TRUE;
}
//-------------------------------[end]------------------------------------------

//-----------------[finish's the CC1101 operation]------------------------------
void CC1101_end(void)
{
    CC1101_powerdown();                          //power down CC1101
}
//-------------------------------[end]------------------------------------------

//-----------------------[show all CC1101 registers]----------------------------
void CC1101_show_register_settings(void)
{
    if(debug_level > 0){
        uint8_t config_reg_verify[CFG_REGISTER],Patable_verify[CFG_REGISTER];

        CC1101_spi_read_burst(READ_BURST,config_reg_verify,CFG_REGISTER);  //reads all 47 config register from CC1101
        CC1101_spi_read_burst(PATABLE_BURST,Patable_verify,8);             //reads output power settings from CC1101

        //show_main_settings();
        printf("Config Register:\r\n");

        for(uint8_t i = 0 ; i < CFG_REGISTER; i++)  //showes rx_buffer for debug
        {
            printf("0x%02X ", config_reg_verify[i]);
            if(i==9 || i==19 || i==29 || i==39) //just for beautiful output style
            {
                printf("\r\n");
            }
        }
        printf("\r\n");
        printf("PaTable:\r\n");

        for(uint8_t i = 0 ; i < 8; i++)         //showes rx_buffer for debug
        {
            printf("0x%02X ", Patable_verify[i]);
        }
        printf("\r\n");
    }
}
//-------------------------------[end]------------------------------------------

//--------------------------[show settings]-------------------------------------
/*
void CC1101_show_main_settings(void)
{
     extern volatile uint8_t My_addr;
     extern int CC1101_mode_select, CC1101_freq_select, CC1101_channel_select;

     printf("Mode: %d\r\n", CC1101_mode_select);
     printf("Frequency: %d\r\n", CC1101_freq_select);
     printf("Channel: %d\r\n", CC1101_channel_select);
     printf("My_Addr: %d\r\n", My_addr);
}
*/
//-------------------------------[end]------------------------------------------

//----------------------------[idle mode]---------------------------------------
uint8_t CC1101_sidle(void)
{
    uint8_t marcstate;

    CC1101_spi_write_strobe(SIDLE);              //sets to idle first. must be in

    marcstate = 0xFF;                     //set unknown/dummy state value

    while(marcstate != 0x01)              //0x01 = sidle
    {
        marcstate = (CC1101_spi_read_register(MARCSTATE) & 0x1F); //read out state of CC1101 to be sure in RX
        //printf("marcstate_rx: 0x%02X\r", marcstate);
    }
    //Serial.println();
    delayMicroseconds(100);
    return TRUE;
}
//-------------------------------[end]------------------------------------------

//---------------------------[transmit mode]------------------------------------
uint8_t CC1101_transmit(void)
{
    uint8_t marcstate;

    CC1101_sidle();                              //sets to idle first.
    CC1101_spi_write_strobe(STX);                //sends the data over air

    marcstate = 0xFF;                     //set unknown/dummy state value

    while(marcstate != 0x01)              //0x01 = ILDE after sending data
    {
        marcstate = (CC1101_spi_read_register(MARCSTATE) & 0x1F); //read out state of CC1101 to be sure in IDLE and TX is finished
        //printf("marcstate_tx: 0x%02X ",marcstate);
    }
    //printf("\r\n");
    delayMicroseconds(100);
    return TRUE;
}
///-------------------------------[end]------------------------------------------

//---------------------------[receive mode]-------------------------------------
uint8_t CC1101_receive(void)
{
    uint8_t marcstate;

    CC1101_sidle();                              //sets to idle first.
    CC1101_spi_write_strobe(SRX);                //writes receive strobe (receive mode)

    marcstate = 0xFF;                     //set unknown/dummy state value

    while(marcstate != 0x0D)              //0x0D = RX
    {
        marcstate = (CC1101_spi_read_register(MARCSTATE) & 0x1F); //read out state of CC1101 to be sure in RX
        //printf("marcstate_rx: 0x%02X\r", marcstate);
    }
    //printf("\r\n");
    delayMicroseconds(100);
    return TRUE;
}
//-------------------------------[end]------------------------------------------

//------------[enables WOR Mode  EVENT0 ~1890ms; rx_timeout ~235ms]--------------------
void CC1101_wor_enable()
{
/*
    EVENT1 = WORCTRL[6:4] -> Datasheet page 88
    EVENT0 = (750/Xtal)*(WOREVT1<<8+WOREVT0)*2^(5*WOR_RES) = (750/26Meg)*65407*2^(5*0) = 1.89s

                        (WOR_RES=0;RX_TIME=0)               -> Datasheet page 80
i.E RX_TIMEOUT = EVENT0*       (3.6038)      *26/26Meg = 235.8ms
                        (WOR_RES=0;RX_TIME=1)               -> Datasheet page 80
i.E.RX_TIMEOUT = EVENT0*       (1.8029)      *26/26Meg = 117.9ms
*/
    CC1101_sidle();

    CC1101_spi_write_register(MCSM0, 0x18);    //FS Autocalibration
    CC1101_spi_write_register(MCSM2, 0x01);    //MCSM2.RX_TIME = 1b

    // configure EVENT0 time
    CC1101_spi_write_register(WOREVT1, 0xFF);  //High byte Event0 timeout
    CC1101_spi_write_register(WOREVT0, 0x7F);  //Low byte Event0 timeout

    // configure EVENT1 time
    CC1101_spi_write_register(WORCTRL, 0x78);  //WOR_RES=0b; tEVENT1=0111b=48d -> 48*(750/26MHz)= 1.385ms

    CC1101_spi_write_strobe(SFRX);             //flush RX buffer
    CC1101_spi_write_strobe(SWORRST);          //resets the WOR timer to the programmed Event 1
    CC1101_spi_write_strobe(SWOR);             //put the radio in WOR mode when CSn is released

    delayMicroseconds(100);
}
//-------------------------------[end]------------------------------------------

//------------------------[disable WOR Mode]-------------------------------------
void CC1101_wor_disable()
{
    CC1101_sidle();                            //exit WOR Mode
    CC1101_spi_write_register(MCSM2, 0x07);    //stay in RX. No RX timeout
}
//-------------------------------[end]------------------------------------------

//------------------------[resets WOR Timer]------------------------------------
void CC1101_wor_reset()
{
    CC1101_sidle();                            //go to IDLE
    CC1101_spi_write_register(MCSM2, 0x01);    //MCSM2.RX_TIME = 1b
    CC1101_spi_write_strobe(SFRX);             //flush RX buffer
    CC1101_spi_write_strobe(SWORRST);          //resets the WOR timer to the programmed Event 1
    CC1101_spi_write_strobe(SWOR);             //put the radio in WOR mode when CSn is released

    delayMicroseconds(100);
}
//-------------------------------[end]------------------------------------------

//-------------------------[tx_payload_burst]-----------------------------------
uint8_t CC1101_tx_payload_burst(uint8_t my_addr, uint8_t rx_addr,
                              uint8_t *txbuffer, uint8_t length)
{
    txbuffer[0] = length-1;
    txbuffer[1] = rx_addr;
    txbuffer[2] = my_addr;

    CC1101_spi_write_burst(TXFIFO_BURST,txbuffer,length); //writes TX_Buffer +1 because of pktlen must be also transfered

    if(debug_level > 0){
        printf("TX_FIFO: ");
        for(uint8_t i = 0 ; i < length; i++)       //TX_fifo debug out
        {
             printf("0x%02X ", txbuffer[i]);
        }
        printf("\r\n");
  }
  return TRUE;
}
//-------------------------------[end]------------------------------------------

//------------------[rx_payload_burst - package received]-----------------------
uint8_t CC1101_rx_payload_burst(uint8_t rxbuffer[], uint8_t pktlen)
{
    uint8_t bytes_in_RXFIFO = 0;
    uint8_t res = 0;

    bytes_in_RXFIFO = CC1101_spi_read_register(RXBYTES);              //reads the number of bytes in RXFIFO

    if((bytes_in_RXFIFO & 0x7F) && !(bytes_in_RXFIFO & 0x80))  //if bytes in buffer and no RX Overflow
    {
        CC1101_spi_read_burst(RXFIFO_BURST, rxbuffer, bytes_in_RXFIFO);
        pktlen = rxbuffer[0];
        res = TRUE;
    }
    else
    {
        if(debug_level > 0){
            printf("no bytes in RX buffer or RX Overflow!: ");printf("0x%02X \r\n", bytes_in_RXFIFO);
        }
        CC1101_sidle();                                                  //set to IDLE
        CC1101_spi_write_strobe(SFRX);delayMicroseconds(100);            //flush RX Buffer
        CC1101_receive();                                                //set to receive mode
        res = FALSE;
    }

    return res;
}
//-------------------------------[end]------------------------------------------

//---------------------------[sent packet]--------------------------------------
uint8_t CC1101_sent_packet(uint8_t my_addr, uint8_t rx_addr, uint8_t *txbuffer,
                            uint8_t pktlen,  uint8_t tx_retries)
{
    uint8_t pktlen_ack=0;//, rssi, lqi;                              //default package len for ACK
    uint8_t rxbuffer[FIFOBUFFER];
    uint8_t tx_retries_count = 0;
    uint8_t from_sender;
    uint16_t ackWaitCounter = 0;

    if(pktlen > (FIFOBUFFER - 1))
    {
        printf("ERROR: package size overflow\r\n");
        return FALSE;
    }

    do                                                          //sent package out with retries
    {
        CC1101_tx_payload_burst(my_addr, rx_addr, txbuffer, pktlen);   //loads the data in CC1101 buffer
        CC1101_transmit();                                             //sents data over air
        CC1101_receive();                                              //receive mode

        if(rx_addr == BROADCAST_ADDRESS){                       //no wait acknowledge if sent to broadcast address or tx_retries = 0
            return TRUE;                                        //successful sent to BROADCAST_ADDRESS
        }

        while (ackWaitCounter < ACK_TIMEOUT )                   //wait for an acknowledge
        {
            if (CC1101_packet_available() == TRUE)                     //if RF package received check package acknowge
            {
                from_sender = rx_addr;                          //the original message sender address
                CC1101_rx_fifo_erase(rxbuffer);                        //erase RX software buffer
                CC1101_rx_payload_burst(rxbuffer, pktlen_ack);         //reads package in buffer
                CC1101_check_acknowledge(rxbuffer, pktlen_ack, from_sender, my_addr); //check if received message is an acknowledge from client
                return TRUE;                                    //package successfully sent
            }
            else{
                ackWaitCounter++;                               //increment ACK wait counter
                delay(1);                                       //delay to give receiver time
            }
        }

        ackWaitCounter = 0;                                     //resets the ACK_Timeout
        tx_retries_count++;                                     //increase tx retry counter

        if(debug_level > 0){                                    //debug output messages
            printf(" #:");
            printf("0x%02X \r\n", tx_retries_count);
        }
    }while(tx_retries_count <= tx_retries);                     //while count of retries is reaches

    return FALSE;                                               //sent failed. too many retries
}
//-------------------------------[end]------------------------------------------

//--------------------------[sent ACKNOWLEDGE]------------------------------------
void CC1101_sent_acknowledge(uint8_t my_addr, uint8_t tx_addr)
{
    uint8_t pktlen = 0x06;                                      //complete Pktlen for ACK packet
    uint8_t tx_buffer[0x06];                                    //tx buffer array init

    tx_buffer[3] = 'A'; tx_buffer[4] = 'c'; tx_buffer[5] = 'k'; //fill buffer with ACK Payload

    CC1101_tx_payload_burst(my_addr, tx_addr, tx_buffer, pktlen);      //load payload to CC1101
    CC1101_transmit();                                                 //sent package over the air
    CC1101_receive();                                                  //set CC1101 in receive mode

    if(debug_level > 0){                                        //debut output
        printf("Ack_sent!\r\n");
    }
}
//-------------------------------[end]------------------------------------------
//----------------------[check if Packet is received]---------------------------
uint8_t CC1101_packet_available()
{
    if(digitalRead(GDO1) == TRUE)                           //if RF package received
    {
        if(CC1101_spi_read_register(IOCFG2) == 0x06)               //if sync word detect mode is used
        {
            while(digitalRead(GDO1) == TRUE){               //wait till sync word is fully received
                printf("!\r\n");
            }                                                  //for sync word receive
        }

        if(debug_level > 0){
             //printf("Pkt->:\r\n");
        }

        return TRUE;
    }
    return FALSE;
}
//-------------------------------[end]------------------------------------------

//------------------[check Payload for ACK or Data]-----------------------------
uint8_t CC1101_get_payload(uint8_t rxbuffer[], uint8_t pktlen, uint8_t my_addr,
                            uint8_t sender, int8_t rssi_dbm, uint8_t lqi)
{
    uint8_t crc;

    CC1101_rx_fifo_erase(rxbuffer);                               //delete rx_fifo bufffer

    if(CC1101_rx_payload_burst(rxbuffer, pktlen) == FALSE)        //read package in buffer
    {
        CC1101_rx_fifo_erase(rxbuffer);                           //delete rx_fifo bufffer
        return FALSE;                                    //exit
    }
    else
    {
        my_addr = rxbuffer[1];                             //set receiver address to my_addr
        sender = rxbuffer[2];

        if(CC1101_check_acknowledge(rxbuffer, pktlen, sender, my_addr) == TRUE) //acknowlage received?
        {
            CC1101_rx_fifo_erase(rxbuffer);                       //delete rx_fifo bufffer
            return FALSE;                                //Ack received -> finished
        }
        else                                               //real data, and sent acknowladge
        {
            rssi_dbm = CC1101_rssi_convert(rxbuffer[pktlen + 1]); //converts receiver strength to dBm
            lqi = CC1101_lqi_convert(rxbuffer[pktlen + 2]);       //get rf quialtiy indicator
            crc = CC1101_check_crc(lqi);                          //get packet CRC

            if(debug_level > 0){                           //debug output messages
                if(rxbuffer[1] == BROADCAST_ADDRESS)       //if my receiver address is BROADCAST_ADDRESS
                {
                    printf("BROADCAST message\r\n");
                }

                printf("RX_FIFO:");
                for(uint8_t i = 0 ; i < pktlen + 1; i++)   //showes rx_buffer for debug
                {
                    printf("0x%02X ", rxbuffer[i]);
                }
                printf("| 0x%02X 0x%02X |", rxbuffer[pktlen+1], rxbuffer[pktlen+2]);
                printf("\r\n");

                printf("RSSI:%d ", rssi_dbm);
                printf("LQI:");printf("0x%02X ", lqi);
                printf("CRC:");printf("0x%02X ", crc);
                printf("\r\n");
            }

            my_addr = rxbuffer[1];                         //set receiver address to my_addr
            sender = rxbuffer[2];                          //set from_sender address

            if(my_addr != BROADCAST_ADDRESS)               //send only ack if no BROADCAST_ADDRESS
            {
                CC1101_sent_acknowledge(my_addr, sender);           //sending acknowlage to sender!
            }

            return TRUE;
        }
        return FALSE;
    }
}
//-------------------------------[end]------------------------------------------

//-------------------------[check ACKNOWLEDGE]------------------------------------
uint8_t CC1101_check_acknowledge(uint8_t *rxbuffer, uint8_t pktlen, uint8_t sender, uint8_t my_addr)
{
    int8_t rssi_dbm;
    uint8_t crc, lqi;

    if((pktlen == 0x05 && (rxbuffer[1] == my_addr || rxbuffer[1] == BROADCAST_ADDRESS)) &&
        rxbuffer[2] == sender &&
        rxbuffer[3] == 'A' && rxbuffer[4] == 'c' && rxbuffer[5] == 'k')   //acknowledge received!
        {
            if(rxbuffer[1] == BROADCAST_ADDRESS){                           //if receiver address BROADCAST_ADDRESS skip acknowledge
                if(debug_level > 0){
                    printf("BROADCAST ACK\r\n");
                }
                return FALSE;
            }
            rssi_dbm = CC1101_rssi_convert(rxbuffer[pktlen + 1]);
            lqi = CC1101_lqi_convert(rxbuffer[pktlen + 2]);
            crc = CC1101_check_crc(lqi);

            if(debug_level > 0){
                printf("ACK! ");
                printf("RSSI:%i ",rssi_dbm);
                printf("LQI:0x%02X ",lqi);
                printf("CRC:0x%02X\r\n",crc);
            }
            return TRUE;
        }
    return FALSE;
}
//-------------------------------[end]------------------------------------------

//------------[check if Packet is received within defined time in ms]-----------
uint8_t CC1101_wait_for_packet(uint16_t milliseconds)
{
    for(uint16_t i = 0; i < milliseconds; i++)
        {
            delay(1);                 //delay till system has data available
            if (CC1101_packet_available())
            {
                return TRUE;
            }
        }
    return FALSE;
}
//-------------------------------[end]------------------------------------------

//--------------------------[tx_fifo_erase]-------------------------------------
void CC1101_tx_fifo_erase(uint8_t *txbuffer)
{
    memset(txbuffer, 0, sizeof(FIFOBUFFER));  //erased the TX_fifo array content to "0"
}
//-------------------------------[end]------------------------------------------

//--------------------------[rx_fifo_erase]-------------------------------------
void CC1101_rx_fifo_erase(uint8_t *rxbuffer)
{
    memset(rxbuffer, 0, sizeof(FIFOBUFFER)); //erased the RX_fifo array content to "0"
}
//-------------------------------[end]------------------------------------------

//------------------------[set CC1101 address]----------------------------------
void CC1101_set_myaddr(uint8_t addr)
{
    CC1101_spi_write_register(ADDR,addr);          //stores MyAddr in the CC1101
}
//-------------------------------[end]------------------------------------------

//---------------------------[set channel]--------------------------------------
void CC1101_set_channel(uint8_t channel)
{
    CC1101_spi_write_register(CHANNR,channel);   //stores the new channel # in the CC1101

    return;
}
//-------------------------------[end]------------------------------------------

//-[set modulation mode 1 = GFSK_1_2_kb; 2 = GFSK_38_4_kb; 3 = GFSK_100_kb; 4 = MSK_250_kb; 5 = MSK_500_kb; 6 = OOK_4_8_kb]-
void CC1101_set_mode(uint8_t mode)
{

    switch (mode)
    {
        case 0x01:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_GFSK_1_2_kb,CFG_REGISTER);
                    break;
        case 0x02:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_GFSK_38_4_kb,CFG_REGISTER);
                    break;
        case 0x03:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_GFSK_100_kb,CFG_REGISTER);
                    break;
        case 0x04:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_MSK_250_kb,CFG_REGISTER);
                    break;
        case 0x05:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_MSK_500_kb,CFG_REGISTER);
                    break;
        case 0x06:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_OOK_4_8_kb,CFG_REGISTER);
                    break;
        default:
                    CC1101_spi_write_burst(WRITE_BURST,CC1101_GFSK_100_kb,CFG_REGISTER);
                    break;
    }
    return;
}
//------------------------------------------end]-----------------------------------

//---------[set ISM Band 1=315MHz; 2=433MHz; 3=868MHz; 4=915MHz; 5=437MHz]----------------
void CC1101_set_ISM(uint8_t ism_freq)
{
    uint8_t freq2, freq1, freq0;

    switch (ism_freq)                                                       //loads the RF freq which is defined in CC1101_freq_select
    {
        case 0x01:                                                          //315MHz
                    freq2=0x0C;
                    freq1=0x1D;
                    freq0=0x89;
                    CC1101_spi_write_burst(PATABLE_BURST,patable_power_315,8);
                    break;
        case 0x02:                                                          //433.92MHz
                    freq2=0x10;
                    freq1=0xB0;
                    freq0=0x71;
                    CC1101_spi_write_burst(PATABLE_BURST,patable_power_433,8);
                    break;
        case 0x03:                                                          //868.3MHz
                    freq2=0x21;
                    freq1=0x65;
                    freq0=0x6A;
                    CC1101_spi_write_burst(PATABLE_BURST,patable_power_868,8);
                    break;
        case 0x04:                                                          //915MHz
                    freq2=0x23;
                    freq1=0x31;
                    freq0=0x3B;
                    CC1101_spi_write_burst(PATABLE_BURST,patable_power_915,8);
                    break;

        case 0x05:                                                          //437MHz
                    freq2=0x10;
                    freq1=0xCE;
                    freq0=0xC5;
                    /***Power Amplifier settings should be written here***/
                    //spi_write_burst(PATABLE_BURST,patable_power_2430,8);
                    break;

        default:                                                             //default is 868.3MHz
                    freq2=0x21;
                    freq1=0x65;
                    freq0=0x6A;
                    CC1101_spi_write_burst(PATABLE_BURST,patable_power_868,8);    //sets up output power ramp register
                    break;
    }

    CC1101_spi_write_register(FREQ2,freq2);                                         //stores the new freq setting for defined ISM band
    CC1101_spi_write_register(FREQ1,freq1);
    CC1101_spi_write_register(FREQ0,freq0);

     return;
}
//-------------------------------[end]------------------------------------------

//--------------------------[set frequency]-------------------------------------
void CC1101_set_patable(uint8_t *patable_arr)
{
    CC1101_spi_write_burst(PATABLE_BURST,patable_arr,8);   //writes output power settings to CC1101    "104us"
}
//-------------------------------[end]------------------------------------------

//-------------------------[set output power]-----------------------------------
void CC1101_set_output_power_level(int8_t dBm)
{
    uint8_t pa = 0xC0;

    if      (dBm <= -30) pa = 0x00;
    else if (dBm <= -20) pa = 0x01;
    else if (dBm <= -15) pa = 0x02;
    else if (dBm <= -10) pa = 0x03;
    else if (dBm <= 0)   pa = 0x04;
    else if (dBm <= 5)   pa = 0x05;
    else if (dBm <= 7)   pa = 0x06;
    else if (dBm <= 10)  pa = 0x07;

    CC1101_spi_write_register(FREND0,pa);
}
//-------------------------------[end]------------------------------------------

//-------[set Modulation type 2-FSK=0; GFSK=1; ASK/OOK=3; 4-FSK=4; MSK=7]------
void CC1101_set_modulation_type(uint8_t cfg)
{
    uint8_t data;
    data = CC1101_spi_read_register(MDMCFG2);
    data = (data & 0x8F) | (((cfg) << 4) & 0x70);
    CC1101_spi_write_register(MDMCFG2, data);
    //printf("MDMCFG2: 0x%02X\n", data);
}
//-------------------------------[end]-----------------------------------------

//------------------------[set preamble Len]-----------------------------------
void CC1101_set_preamble_len(uint8_t cfg)
{
    uint8_t data;
    data = CC1101_spi_read_register(MDMCFG1);
    data = (data & 0x8F) | (((cfg) << 4) & 0x70);
    CC1101_spi_write_register(MDMCFG1, data);
    //printf("MDMCFG2: 0x%02X\n", data);
}
//-------------------------------[end]-----------------------------------------

//-------------------[set modem datarate and deviant]--------------------------
void CC1101_set_datarate(uint8_t mdmcfg4, uint8_t mdmcfg3, uint8_t deviant)
{
    CC1101_spi_write_register(MDMCFG4, mdmcfg4);
    CC1101_spi_write_register(MDMCFG3, mdmcfg3);
    CC1101_spi_write_register(DEVIATN, deviant);
}
//-------------------------------[end]-----------------------------------------

//----------------------[set sync mode no sync=0;]-----------------------------
void CC1101_set_sync_mode(uint8_t cfg) // 0=no sync word; 1,2 = 16bit sync word, 3= 32bit sync word
{
    uint8_t data;
    data = CC1101_spi_read_register(MDMCFG2);
    data = (data & 0xF8) | (cfg & 0x07);
    CC1101_spi_write_register(MDMCFG2, data);
    //printf("MDMCFG2: 0x%02X\n", data);
}
//-------------------------------[end]-----------------------------------------

//---------------[set FEC ON=TRUE; OFF=FALSE]----------------------------------
void CC1101_set_fec(uint8_t cfg)
{
    uint8_t data;
    data = CC1101_spi_read_register(MDMCFG1);
    data = (data & 0x7F) | (((cfg) << 7) & 0x80);
    CC1101_spi_write_register(MDMCFG1, data);
    printf("MDMCFG1: 0x%02X\n", data);
}
//-------------------------------[end]------------------------------------------

//---------------[set data_whitening ON=TRUE; OFF=FALSE]------------------------
void CC1101_set_data_whitening(uint8_t cfg)
{
    uint8_t data;
    data = CC1101_spi_read_register(PKTCTRL0);
    data = (data & 0xBF) | (((cfg) << 6) & 0x40);
    CC1101_spi_write_register(PKTCTRL0, data);
    //printf("PKTCTRL0: 0x%02X\n", data);
}
//-------------------------------[end]-----------------------------------------

//------------[set manchester encoding ON=TRUE; OFF=FALSE]---------------------
void CC1101_set_manchester_encoding(uint8_t cfg)
{
    uint8_t data;
    data = CC1101_spi_read_register(MDMCFG2);
    data = (data & 0xF7) | (((cfg) << 3) & 0x08);
    CC1101_spi_write_register(MDMCFG2, data);
    //printf("MDMCFG2: 0x%02X\n", data);
}
//-------------------------------[end]------------------------------------------

//--------------------------[rssi_convert]--------------------------------------
int8_t CC1101_rssi_convert(uint8_t Rssi_hex)
{
    int8_t rssi_dbm;
    int16_t Rssi_dec;

    Rssi_dec = Rssi_hex;        //convert unsigned to signed

    if(Rssi_dec >= 128){
        rssi_dbm=((Rssi_dec-256)/2)-RSSI_OFFSET_868MHZ;
    }
    else{
        if(Rssi_dec<128){
            rssi_dbm=((Rssi_dec)/2)-RSSI_OFFSET_868MHZ;
        }
    }
    return rssi_dbm;
}
//-------------------------------[end]------------------------------------------

//----------------------------[lqi convert]-------------------------------------
uint8_t CC1101_lqi_convert(uint8_t lqi)
{
    return (lqi & 0x7F);
}
//-------------------------------[end]------------------------------------------

//----------------------------[check crc]---------------------------------------
uint8_t CC1101_check_crc(uint8_t lqi)
{
    return (lqi & 0x80);
}
//-------------------------------[end]------------------------------------------

/*
//----------------------------[get temp]----------------------------------------
uint8_t CC1101_get_temp(uint8_t *ptemp_Arr)
{
    const uint8_t num_samples = 8;
    uint16_t adc_result = 0;
    uint32_t temperature = 0;

    sidle();                              //sets CC1101 into IDLE
    spi_write_register(PTEST,0xBF);       //enable temp sensor
    delay(50);                            //wait a bit

    for(uint8_t i=0;i<num_samples;i++)    //sampling analog temperature value
    {
        adc_result += analogRead(GDO0);
        delay(1);
    }
    adc_result = adc_result / num_samples;
    //Serial.println(adc_result);

    temperature = (adc_result * CC1101_TEMP_ADC_MV) / CC1101_TEMP_CELS_CO;

    ptemp_Arr[0] = temperature / 10;      //cut last digit
    ptemp_Arr[1] = temperature % 10;      //isolate last digit

    if(debug_level > 0){
        Serial.print(F("Temp:"));Serial.print(ptemp_Arr[0]);Serial.print(F("."));Serial.println(ptemp_Arr[1]);
    }

    spi_write_register(PTEST,0x7F);       //writes 0x7F back to PTest (app. note)

    receive();
    return (*ptemp_Arr);
}
*/
//-------------------------------[end]------------------------------------------

//|==================== SPI Initialisation for CC1101 =========================|
void CC1101_spi_begin(void)
{
     int x = 0;
     //printf ("init SPI bus... ");
     if ((x = wiringPiSPISetup (0, 8000000)) < 0)  //4MHz SPI speed
     {
          if(debug_level > 0){
          printf ("ERROR: wiringPiSPISetup failed!\r\n");
          }
     }
     else{
          //printf ("wiringSPI is up\r\n");
          }
}
//------------------[write register]--------------------------------
void CC1101_spi_write_register(uint8_t spi_instr, uint8_t value)
{
     uint8_t tbuf[2] = {0};
     tbuf[0] = spi_instr | WRITE_SINGLE_BYTE;
     tbuf[1] = value;
     uint8_t len = 2;
     wiringPiSPIDataRW (0, tbuf, len) ;

     return;
}
//|============================ Ein Register lesen ============================|
uint8_t CC1101_spi_read_register(uint8_t spi_instr)
{
     uint8_t value;
     uint8_t rbuf[2] = {0};
     rbuf[0] = spi_instr | READ_SINGLE_BYTE;
     uint8_t len = 2;
     wiringPiSPIDataRW (0, rbuf, len) ;
     value = rbuf[1];
     //printf("SPI_arr_0: 0x%02X\n", rbuf[0]);
     //printf("SPI_arr_1: 0x%02X\n", rbuf[1]);
     return value;
}
//|========================= ein Kommando schreiben ========================|
void CC1101_spi_write_strobe(uint8_t spi_instr)
{
     uint8_t tbuf[1] = {0};
     tbuf[0] = spi_instr;
     //printf("SPI_data: 0x%02X\n", tbuf[0]);
     wiringPiSPIDataRW (0, tbuf, 1) ;
 }
//|======= Mehrere hintereinanderliegende Register auf einmal lesen =======|
void CC1101_spi_read_burst(uint8_t spi_instr, uint8_t *pArr, uint8_t len)
{
     uint8_t rbuf[len + 1];
     rbuf[0] = spi_instr | READ_BURST;
     wiringPiSPIDataRW (0, rbuf, len + 1) ;
     for (uint8_t i=0; i<len ;i++ )
     {
          pArr[i] = rbuf[i+1];
          //printf("SPI_arr_read: 0x%02X\n", pArr[i]);
     }
}
//|======= Mehrere hintereinanderliegende Register auf einmal schreiben =======|
void CC1101_spi_write_burst(uint8_t spi_instr, uint8_t *pArr, uint8_t len)
{
     uint8_t tbuf[len + 1];
     tbuf[0] = spi_instr | WRITE_BURST;
     for (uint8_t i=0; i<len ;i++ )
     {
          tbuf[i+1] = pArr[i];
          //printf("SPI_arr_write: 0x%02X\n", tbuf[i+1]);
     }
     wiringPiSPIDataRW (0, tbuf, len + 1) ;
}
