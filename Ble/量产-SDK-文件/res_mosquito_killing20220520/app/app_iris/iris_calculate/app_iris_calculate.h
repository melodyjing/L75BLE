/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_iris_calculate.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/12/17

	Description :   
	  Test the csem rf module to transter data 				

	Changed Log	:

		Liujian  2019/12/17			- Creation


*****************************************************************************/
#ifndef __APP_IRIS_CALCULATE_H__
#define __APP_IRIS_CALCULATE_H__
#if(BLE_DIANBIAO_CALCUALTE_ENABLE  && DIANBIAO_PROTOCOL_ZHENGTAI_ENABLE)
#define 	CS556_REG_MODE      					0x00
#define   CS565_REG_MODE_2              0x01
#define   CS565_REG_FOURFSK_CODING      0x02
#define   CS565_REG_DATAWHITE_BTLE      0x03
#define   CS565_REG_MOD_TX              0x04
#define   CS565_REG_CARRIER_RECOVERY    0x05
#define   CS565_REG_TAU_ROUGH_RECOV     0x06
#define   CS565_REG_TAU_PHASE_RECOV     0x07
#define   CS565_REG_TAU_CLK_RECOV       0x08
#define   CS565_REG_TAU_DATARATE_RECOV  0x09
#define   CS565_REG_DATARATE_OFFSET     0x0a
#define   CS565_REG_FIFO                0x0c
#define   CS565_REG_FIFO_2              0x0d
#define   CS565_REG_IRQ_CONF            0x0e
#define   CS565_REG_PAD_CONF_1          0x0f
#define   CS565_REG_PAD_CONF_2          0x10
#define   CS565_REG_PAD_CONF_3          0x11
#define   CS565_REG_PAD_CONF_4          0x12
#define   CS565_REG_PAD_CONF_5          0x13
#define   CS565_REG_MAC_CONF            0x14
#define   CS565_REG_RX_MAC_TIMER        0x15
#define   CS565_REG_TX_MAC_TIMER        0x16
#define   CS565_REG_BANK                0x17
#define   CS565_REG_CENTER_FREQ         0x18
#define   CS565_REG_CHANNEL             0x1c
#define   CS565_REG_MOD_INFO_TX         0x1d
#define   CS565_REG_MOD_INFO_RX         0x1e
#define   CS565_REG_CODING              0x1f
#define   CS565_REG_CHANNELS_1          0x20
#define   CS565_REG_CHANNELS_2          0x22
#define   CS565_REG_PACKET_HANDLING     0x23
#define   CS565_REG_PACKET_LENGTH       0x24
#define   CS565_REG_PACKET_LENGTH_OPTS  0x25
#define   CS565_REG_PREAMBLE            0x26
#define   CS565_REG_PREAMBLE_LENGTH     0x27
#define   CS565_REG_ADDRESS             0x28
#define   CS565_REG_ADDRESS_BROADCAST   0x2a

#define   CS565_REG_PATTERN             0x2c   //SYNC


#define   CS565_REG_ADDRESS_CONF        0x30
#define   CS565_REG_PACKET_EXTRA        0x31
#define   CS565_REG_CONV_CODES_CONF     0x32
#define   CS565_REG_CONV_CODES_POLY     0x33
#define   CS565_REG_CRC_POLYNOMIAL      0x34
#define   CS565_REG_CRC_RST             0x38
#define   CS565_REG_CONV_CODES_PUNCT    0x3c
#define   CS565_REG_FRAC_CONF           0x3e
#define   CS565_REG_RX_FRAC_CONF        0x3f
#define   CS565_REG_TX_FRAC_CONF        0x40
#define   CS565_REG_TX_MULT             0x41
#define   CS565_REG_FRONTEND            0x42
#define   CS565_REG_FRONTEND_2          0x43
#define   CS565_REG_TX_PULSE_SHAPE_1    0x44
#define   CS565_REG_TX_PULSE_SHAPE_2    0x48
#define   CS565_REG_TX_PULSE_SHAPE_3    0x4c
#define   CS565_REG_TX_PULSE_SHAPE_4    0x50
#define   CS565_REG_RX_PULSE_SHAPE      0x54
#define   CS565_REG_FILTER_GAIN         0x58

#define   CS565_REG_CARRIER_RECOVERY_EXTRA 	0x59

#define   CS565_REG_RX_IF_DIG           0x5a
#define   CS565_REG_DELAY_LINE_CONF     0x5c
#define   CS565_REG_FSK_FCR_AMP_1       0x5e
#define   CS565_REG_FSK_FCR_AMP_2       0x5f
#define   CS565_REG_FSK_FCR_AMP_3       0x60
#define   CS565_REG_DECISION            0x61
#define   CS565_REG_CORRECT_CFREQ_IF    0x62
#define   CS565_REG_RSSI_BANK           0x64
#define   CS565_REG_RSSI_BANK_ALT       0x65
#define   CS565_REG_CLK_CH_FILTER       0x66
#define   CS565_REG_PA_PWR              0x67
#define   CS565_REG_PLL_BANK            0x68
#define   CS565_REG_FILTER_BIAS         0x6a
#define   CS565_REG_PA_PWR_OFFSET       0x6c
#define   CS565_REG_RSSI_CTRL           0x6d
#define   CS565_REG_ATT_CTRL            0x6e
#define   CS565_REG_AGC_THR_LOW         0x6f
#define   CS565_REG_AGC_THR_HIGH        0x70
#define   CS565_REG_AGC_PEAK_DET        0x71
#define   CS565_REG_IEEE802154_OPTS     0x72
#define   CS565_REG_AGC_LUT_1           0x74
#define   CS565_REG_AGC_LUT_2           0x78
#define   CS565_REG_AGC_LUT_3           0x7c
#define   CS565_REG_AGC_LUT_4           0x80
#define   CS565_REG_AGC_LUT_5           0x84
#define   CS565_REG_DATA_STREAMS        0x85
#define   CS565_REG_TIMINGS_1           0x86
#define   CS565_REG_TIMINGS_2           0x87
#define   CS565_REG_AGC_ATT_1           0x88
#define   CS565_REG_AGC_ATT_2           0x8c
#define   CS565_REG_TIMINGS_3           0x8d
#define   CS565_REG_TIMINGS_4           0x8e
#define   CS565_REG_TIMINGS_5           0x8f
#define   CS565_REG_TIMING_FAST_RX      0x90

#define   CS565_REG_TIMEOUT             0x91
#define   CS565_REG_INTERFACE_CONF      0x92

#define   CS565_REG_BIAS_0                0x93
#define   CS565_REG_BIAS_1                0x94
#define   CS565_REG_BIAS_2                0x95
#define   CS565_REG_BIAS_3                0x96
#define   CS565_REG_BIAS_4                0x97
#define   CS565_REG_BIAS_5                0x98
#define   CS565_REG_BIAS_6                0x99
#define   CS565_REG_BIAS_7                0x9a
#define   CS565_REG_BIAS_8                0x9b
#define   CS565_REG_BIAS_9                0x9c
#define   CS565_REG_BIAS_10               0x9d
#define   CS565_REG_BIAS_11               0x9e
#define   CS565_REG_BIAS_12               0x9f

#define   CS565_REG_SD_MASH               0xa0
#define   CS565_REG_SD_MASH_MASK          0xa1
#define   CS565_REG_SWCAP_FSM             0xa2
#define   CS565_REG_CTRL_ADC              0xa3
#define   CS565_REG_BIAS_EN_1             0xa4
#define   CS565_REG_BIAS_EN_2             0xa6

#define   CS565_REG_CTRL_RX               0xa7
#define   CS565_REG_PLL_CTRL              0xa8
#define   CS565_REG_DLL_CTRL              0xac
#define   CS565_REG_RSSI_TUN_1            0xae
#define   CS565_REG_RSSI_TUN_2            0xb0

#define   CS565_REG_XTAL_TRIM              0xb1
#define   CS565_REG_ENABLES                0xb2
#define   CS565_REG_XTAL_CTRL              0xb4
#define   CS565_REG_PA_CONF                0xb8
#define   CS565_REG_SUBBAND_CONF           0xb9
#define   CS565_REG_SWCAP_LIM              0xba
#define   CS565_REG_REGULATOR              0xbb
#define   CS565_REG_SUBBAND_OFFSET         0xbc
#define   CS565_REG_PA_RAMPUP              0xbe
#define   CS565_REG_TXRX_CONF              0xbf
#define   CS565_REG_SUBBAND_CORR           0xc0
#define   CS565_REG_RSSI_DETECT            0xc1
#define  	CS565_REG_DEMOD_CTRL             0xc2
#define   CS565_REG_RSSI_DETECT_DIFF_THR   0xc3
#define   CS565_REG_RSSI_DETECT_ABS_THR    0xc4
#define   CS565_REG_SYNC_WORD_CORR         0xc5

#define   CS565_REG_SUBBAND_FLL            0xc6
#define   CS565_REG_PADS_PE_DS             0xc7
#define   CS565_REG_SPARES                 0xc8
#define   CS565_REG_AES_CONF               0xc9
#define   CS565_REG_TX_PRE_DIST            0xca
#define   CS565_REG_CLK_RECOVERY           0xcc

#define   CS565_REG_BLR_PREAMPLE_THRESHOLD	0xce
#define   CS565_REG_BLR_SYNC_THRESHOLD      0xcf


#define   CS565_REG_MISC                  	0xd0
#define   CS565_REG_PMU_TIMING            	0xd1
#define   CS565_REG_PMU_ENABLES_TRANSIENT   0xd2 
#define   CS565_REG_STAGING							    0xd3 
#define   CS565_REG_PMU_TRIM                0xd4
#define   CS565_REG_IQ_SPARES               0xd6
#define   CS565_REG_BLE_DTM                 0xd8

#define   CS565_REG_ASK_MOD                 0xda
#define		CS565_CHIP_ID_RM									0x0ff

#define	  CS565_REG_FSM_MODE                0x100
#define	  CS565_REG_FSM_STATUS              0x101
#define	  CS565_REG_TXFIFO_STATUS           0x102
#define	  CS565_REG_RXFIFO_STATUS           0x103

#define	  CS565_REG_TXFIFO                  0x104
#define	  CS565_REG_RXFIFO                  0x108

#define	  CS565_REG_TXFIFO_COUNT            0x10c
#define	  CS565_REG_RXFIFO_COUNT            0x10d
#define	  CS565_REG_RSSI_AVG                0x10e
#define	  CS565_REG_DESER_STATUS            0x110
#define	  CS565_REG_BLE_AES_CCM             0x114
#define	  CS565_REG_IRQ_STATUS              0x118
#define	  CS565_REG_RSSI_MIN                0x11c
#define	  CS565_REG_RSSI_MAX                0x11e
#define	  CS565_REG_RSSI_PKT                0x120
#define	  CS565_REG_DR_ERR_IND              0x122
#define	  CS565_REG_RX_ATT_LEVEL            0x123
#define	  CS565_REG_FEI                     0x124
#define	  CS565_REG_FEI_PKT                 0x126
#define	  CS565_REG_FEI_AFC                 0x128
#define	  CS565_REG_LINK_QUAL               0x12a
#define	  CS565_REG_LINK_QUAL_PKT           0x12b
#define	  CS565_REG_SUBBAND_ERR             0x12c
#define	  CS565_REG_ANALOG_INFO             0x12d
#define	  CS565_REG_PEAK_DET_VAL            0x12e
#define	  CS565_REG_SAMPLE_RSSI             0x12f
#define	  CS565_REG_RSSI_THERM              0x130
#define	  CS565_REG_PROT_TIMER_CONF         0x140
#define	  CS565_REG_PT_DELTA_TS             0x144
#define	  CS565_REG_COMMANDS                0x1c0
#define	  CS565_REG_PROT_TIMER              0x1c1
#define	  CS565_REG_BLR_PKT_LEN             0x1c2
#define	  CS565_REG_SCAN_1                  0x1fc
#define	  CS565_REG_SCAN_2                  0x1fd
#define	  CS565_REG_MD5_REGS                0x1fe
#define	  CS565_REG_CHIP_ID                 0x1ff


typedef enum {
	CSEM_IRQ_TX_FLAG,
	CSEM_IRQ_RXSTOP_FLAG, 
	CSEM_IRQ_RECEIVED_FLAG,
	CSEM_IRQ_SYNC_FLAG,
	CSEM_IRQ_TXFIFO_FLAG,
	CSEM_IRQ_RXFIFO_FLAG
}CSEM_IRQ_TYPE;

//typedef enum {
//	PLUSE_ACTIVE = 0x00,
//	PLUSE_REACTIVE,
//	PLUSE_PER_SECOND,
//	PLUSE_FORWARD_HARMONICS,
//	PLUSE_INVERSE_HARMONICS,
//	PLUSE_DEMAND,
//	PLUSE_SWITCH_TIME,
//	PLUSE_ERROR = 0xFF,
//	
//}PLUSE_TYPE1;

typedef enum {
	PLUSE_PER_SECOND = 0x00,
	PLUSE_DEMAND,
	PLUSE_SWITCH_TIME,
	PLUSE_FORWARD_HARMONICS,
	PLUSE_INVERSE_HARMONICS,
	PLUSE_REACTIVE,
	PLUSE_ACTIVE,
	PLUSE_ERROR = 0xFF,
	
}PLUSE_TYPE;

typedef enum {
		CSEM_RF_IDLE_MODE,
		CSEM_RF_RX_MODE,
		CSEM_RF_TX_MODE,	
}APP_CSEM_RF_MODE;


typedef struct {
  unsigned char head;
  unsigned char length;
  unsigned char payload[256];		
}APP_CSEM_PROTOCOL;

#define SEND_DATA_HZ     500
/*
测量时需要知道有多少个频点和每个频点的频率
最多5个
*/
typedef struct {
	unsigned short frequecny[5];
	unsigned char  frequency_number;
	unsigned char  power_level;
}APP_CAL_FREQUENCY;

typedef void (*csem_tx_callback_t)(void);
typedef void (*csem_rx_callback_t)(void);

//void app_iris_calculate_send_task(APP_CAL_FREQUENCY *p_cal_param, unsigned int address, unsigned char pluse_type);

//void app_iris_calculate_receive_task(APP_CAL_FREQUENCY *p_cal_param, unsigned int address, unsigned char pluse_type);

int app_iris_calculate_init(unsigned int frequency);
void app_csem_pluse_calculate_init(void);
unsigned int app_iris_set_frequency(unsigned short frequency);
void app_iris_set_access_address(unsigned int address);
void app_iris_set_whitelist_value(unsigned short frequency);
void app_csem_set_receive_mode(void);
void app_meter_send_data(void);
void app_csem_receive_go(void);
void app_csem_idle_mode(void);
void app_csem_tx_go(void);
void app_csem_irq_enable(CSEM_IRQ_TYPE irq);
void app_meter_send_event(PLUSE_TYPE pluse_type, unsigned char level);

void app_csem_set_tx_callback(csem_tx_callback_t func);
void app_csem_set_rx_callback(csem_rx_callback_t func);

#endif

#endif
