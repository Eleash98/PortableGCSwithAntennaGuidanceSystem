#ifndef CC1101_INTERFACE_H_INCLUDED
#define CC1101_INTERFACE_H_INCLUDED



#include <stdint.h>


/*----------------------------------[standard]--------------------------------*/
#define TRUE  (1==1)
#define FALSE (!TRUE)


//**************************** pins ******************************************//
#define SS_PIN   10
#define GDO1      6
#define GDO0     99

/*----------------------[CC1100 - misc]---------------------------------------*/
#define CRYSTAL_FREQUENCY         26000000
#define CFG_REGISTER              0x2F  //47 registers
#define FIFOBUFFER                0x42  //size of Fifo Buffer +2 for rssi and lqi
#define RSSI_OFFSET_868MHZ        0x4E  //dec = 74
#define TX_RETRIES_MAX            0x05  //tx_retries_max
#define ACK_TIMEOUT                250  //ACK timeout in ms
#define CC1101_COMPARE_REGISTER   0x00  //register compare 0=no compare 1=compare
#define BROADCAST_ADDRESS         0x00  //broadcast address
#define CC1101_FREQ_315MHZ        0x01
#define CC1101_FREQ_434MHZ        0x02
#define CC1101_FREQ_868MHZ        0x03
#define CC1101_FREQ_915MHZ        0x04
#define CC1101_FREQ_437MHZ        0x05
//#define CC1101_FREQ_2430MHZ       0x05
#define CC1101_TEMP_ADC_MV        3.225 //3.3V/1023 . mV pro digit
#define CC1101_TEMP_CELS_CO       2.47  //Temperature coefficient 2.47mV per Grad Celsius


/**Modulation Options***/
#define CC1101_MOD_2FSK             0
#define CC1101_MOD_GFSK             1
#define CC1101_MOD_ASK              3
#define CC1101_MOD_4FSK             4
#define CC1101_MOD_MSK              7

uint8_t debug_level;

uint8_t CC1101_set_debug_level(uint8_t set_debug_level);
uint8_t CC1101_get_debug_level(void);

uint8_t CC1101_begin(volatile uint8_t My_addr);
void CC1101_end(void);

void CC1101_spi_write_strobe(uint8_t spi_instr);
void CC1101_spi_write_register(uint8_t spi_instr, uint8_t value);
void CC1101_spi_write_burst(uint8_t spi_instr, uint8_t *pArr, uint8_t length);
void CC1101_spi_read_burst(uint8_t spi_instr, uint8_t *pArr, uint8_t length);
uint8_t CC1101_spi_read_register(uint8_t spi_instr);
uint8_t CC1101_spi_read_status(uint8_t spi_instr);

void CC1101_reset(void);
void CC1101_wakeup(void);
void CC1101_powerdown(void);

void CC1101_wor_enable(void);
void CC1101_wor_disable(void);
void CC1101_wor_reset(void);

uint8_t CC1101_sidle(void);
uint8_t CC1101_transmit(void);
uint8_t CC1101_receive(void);

void CC1101_show_register_settings(void);
void CC1101_show_main_settings(void);

uint8_t CC1101_packet_available();
uint8_t CC1101_wait_for_packet(uint16_t milliseconds);

uint8_t CC1101_get_payload(uint8_t rxbuffer[], uint8_t pktlen_rx,uint8_t my_addr,
                              uint8_t sender, int8_t rssi_dbm, uint8_t lqi);

uint8_t CC1101_tx_payload_burst(uint8_t my_addr, uint8_t rx_addr, uint8_t *txbuffer, uint8_t length);
uint8_t CC1101_rx_payload_burst(uint8_t rxbuffer[], uint8_t pktlen);

void CC1101_rx_fifo_erase(uint8_t *rxbuffer);
void CC1101_tx_fifo_erase(uint8_t *txbuffer);

uint8_t CC1101_sent_packet(uint8_t my_addr, uint8_t rx_addr, uint8_t *txbuffer, uint8_t pktlen, uint8_t tx_retries);
void CC1101_sent_acknowledge(uint8_t my_addr, uint8_t tx_addr);

uint8_t CC1101_check_acknowledge(uint8_t *rxbuffer, uint8_t pktlen, uint8_t sender, uint8_t my_addr);

int8_t CC1101_rssi_convert(uint8_t Rssi);
uint8_t CC1101_check_crc(uint8_t lqi);
uint8_t CC1101_lqi_convert(uint8_t lqi);
uint8_t CC1101_get_temp(uint8_t *ptemp_Arr);

void CC1101_set_myaddr(uint8_t addr);
void CC1101_set_channel(uint8_t channel);
void CC1101_set_ISM(uint8_t ism_freq);
void CC1101_set_mode(uint8_t mode);
void CC1101_set_output_power_level(int8_t dbm);
void CC1101_set_patable(uint8_t *patable_arr);
void CC1101_set_fec(uint8_t cfg);
void CC1101_set_data_whitening(uint8_t cfg);
void CC1101_set_modulation_type(uint8_t cfg);
void CC1101_set_preamble_len(uint8_t cfg);
void CC1101_set_manchester_encoding(uint8_t cfg);
void CC1101_set_sync_mode(uint8_t cfg);
void CC1101_set_datarate(uint8_t mdmcfg4, uint8_t mdmcfg3, uint8_t deviant);
void CC1101_voidSetFrequency(u32 Frequency);
void CC1101_voidSetSynchWord(u8 PSynchWord,u8 PLength,u8 CCSynch[2], u8 CCSynchLength, u8 PreambleLength);
u8* CC1101_u8PtrReadFrame(u32* ReceivedLength);
void CC1101_voidInit(u32 Frequency, u8 ModulationType,f32 ModulationIndex,u32 DataRate);


#endif // CC1101_INTERFACE_H_INCLUDED
