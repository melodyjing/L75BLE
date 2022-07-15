/**@file			cobra4A_pinmux.h
* @brief            
* @details          
* @author      			
* @date				2020-03-27
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/03/27  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __COBRA_PINMUX_H__
#define __COBRA_PINMUX_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_PINMUX_EN

typedef struct cobra_pinmux_type
{
	__attribute__((aligned(4))) uint32_t PIN0_MUX; //0x00
	__attribute__((aligned(4))) uint32_t PIN1_MUX;
	__attribute__((aligned(4))) uint32_t PIN2_MUX;
	__attribute__((aligned(4))) uint32_t PIN3_MUX;

	__attribute__((aligned(4))) uint32_t PIN4_MUX; //0x10
	__attribute__((aligned(4))) uint32_t PIN5_MUX;
	__attribute__((aligned(4))) uint32_t PIN6_MUX;
	__attribute__((aligned(4))) uint32_t PIN7_MUX;

	__attribute__((aligned(4))) uint32_t PIN8_MUX; //0x20
	__attribute__((aligned(4))) uint32_t PIN9_MUX;

	__attribute__((aligned(4))) uint32_t PIN10_MUX;
	__attribute__((aligned(4))) uint32_t PIN11_MUX;

	__attribute__((aligned(4))) uint32_t PIN12_MUX; //0x30
	__attribute__((aligned(4))) uint32_t PIN13_MUX;
	__attribute__((aligned(4))) uint32_t PIN14_MUX;
	__attribute__((aligned(4))) uint32_t PIN15_MUX;

	__attribute__((aligned(4))) uint32_t PIN16_MUX; //0x40
	__attribute__((aligned(4))) uint32_t PIN17_MUX;
	__attribute__((aligned(4))) uint32_t PIN18_MUX;
	__attribute__((aligned(4))) uint32_t PIN19_MUX;

	__attribute__((aligned(4))) uint32_t PIN20_MUX; //0x50
	__attribute__((aligned(4))) uint32_t PIN21_MUX;
	__attribute__((aligned(4))) uint32_t PIN22_MUX;
	__attribute__((aligned(4))) uint32_t PIN23_MUX;

	__attribute__((aligned(4))) uint32_t PIN24_MUX; //0x60
	__attribute__((aligned(4))) uint32_t PIN25_MUX;
	__attribute__((aligned(4))) uint32_t PIN26_MUX;
	__attribute__((aligned(4))) uint32_t PIN27_MUX;

	__attribute__((aligned(4))) uint32_t PIN28_MUX; //0x70
	__attribute__((aligned(4))) uint32_t PIN29_MUX;

	__attribute__((aligned(4))) uint32_t PIN30_MUX;
	__attribute__((aligned(4))) uint32_t PIN31_MUX;

	__attribute__((aligned(4))) uint32_t AON_PIN0_MUX; //0x80
	__attribute__((aligned(4))) uint32_t AON_PIN1_MUX;
	__attribute__((aligned(4))) uint32_t AON_PIN2_MUX;
	__attribute__((aligned(4))) uint32_t AON_PIN3_MUX;
	__attribute__((aligned(4))) uint32_t AON_PIN4_MUX;

} COBRA_PINMUX_TypeDef;

typedef enum pinmux_index
{
	PIN0_INDEX,
	PIN1_INDEX,
	PIN2_INDEX,
	PIN3_INDEX,
	PIN4_INDEX,
	PIN5_INDEX,
	PIN6_INDEX,
	PIN7_INDEX,
	PIN8_INDEX,
	PIN9_INDEX,
	PIN10_INDEX,
	PIN11_INDEX,
	PIN12_INDEX,
	PIN13_INDEX,
	PIN14_INDEX,
	PIN15_INDEX,

	PIN16_INDEX,
	PIN17_INDEX,
	PIN18_INDEX,
	PIN19_INDEX,
	PIN20_INDEX,
	PIN21_INDEX,
	PIN22_INDEX,
	PIN23_INDEX,
	PIN24_INDEX,
	PIN25_INDEX,
	PIN26_INDEX,
	PIN27_INDEX,
	PIN28_INDEX,
	PIN29_INDEX,
	PIN30_INDEX,
	PIN31_INDEX

} PINMUX_INDEX;

typedef enum pinmode
{
	PIN_MODE_0,
	PIN_MODE_1,
	PIN_MODE_2,
	PIN_MODE_3,
	PIN_MODE_4,
	PIN_MODE_5,
	PIN_MODE_6,
	PIN_MODE_7,
	PIN_MODE_8,
	PIN_MODE_9,
	PIN_MODE_10,
	PIN_MODE_11,
	PIN_MODE_12,
	PIN_MODE_13,
	PIN_MODE_14,
	PIN_MODE_15,
	PIN_MODE_16,
	PIN_MODE_17,
	PIN_MODE_18,
	PIN_MODE_19,
	PIN_MODE_20,
	PIN_MODE_21,
	PIN_MODE_22,
	PIN_MODE_23,
	PIN_MODE_24,
	PIN_MODE_25,
	PIN_MODE_26,
	PIN_MODE_27,
	PIN_MODE_28,
	PIN_MODE_29,
	PIN_MODE_30,
	PIN_MODE_31,
	PIN_MODE_32,
	PIN_MODE_33,
	PIN_MODE_34,
	PIN_MODE_35,
	PIN_MODE_36,
	PIN_MODE_37,
	PIN_MODE_38,
	PIN_MODE_39,
	PIN_MODE_40,
	PIN_MODE_41,
	PIN_MODE_42,
	PIN_MODE_43,
	PIN_MODE_44,
	PIN_MODE_45,
	PIN_MODE_46,
	PIN_MODE_47,
	PIN_MODE_48,
	PIN_MODE_49,
	PIN_MODE_50,
} PIN_MODE;

/*
|	0	
|	DPIN0	
|	PDUW0204CDG	
|		
|		
|		
|	va_pdm_left_data_in_pin 	
|	ODIO<i2c1_sda_in,i2c1_sda_oe>    	
|	mpwm_MCI2
|	IO<i2s1_rx_sclk_i,i2s1_rx_sclk_o,i2s1_rx_sclk_oe>         
|		
|	rmt0_tx_key[30] 	
|	btbb_spi_cs                                               
|	IO<spif_cs_n_in,spif_cs_n_out,spif_cs_n_out_enable>  
|	uart0_cts_n	
|	m0_uart0_cts_n                          
|	spi0_rx_chip_select_n
|		
|	C_0	
|	C_1	
*/
//#define PIN0_JTAG_TDO PIN_MODE_0 //M3 JTAG
#define PIN0_VA_PDM_LEFT_DATA_IN        PIN_MODE_1
#define PIN0_I2C1_SDA                   PIN_MODE_2
#define PIN0_MPWM_MCI2                  PIN_MODE_3
#define PIN0_I2S1_RX_SCLK               PIN_MODE_4
//#define PIN0_SCAN_IN0                   PIN_MODE_5
#define PIN0_RMT0_TX_KEY30              PIN_MODE_6
#define PIN0_BTBB_SPI_CS                PIN_MODE_7
#define PIN0_SPIF_CS                    PIN_MODE_8 //IO<i2s1_rx_mclk_i,i2s1_rx_mclk_o,i2s1_rx_mclk_oe>
#define PIN0_UART0_CTS                  PIN_MODE_9
#define PIN0_M0_UART0_CTS               PIN_MODE_10 //cobra4 UART1_TX
#define PIN0_SPI0_RX_CS                 PIN_MODE_11
#define PIN0_IO_OUTPUT_0                PIN_MODE_14
#define PIN0_IO_OUTPUT_1                PIN_MODE_15
#define PIN0_IO_HIGH_Z_STATE            PIN_MODE_13

/*
|	1	
|	DPIN1	
|	PDUW0204CDG	
|		
|		
|	ODIO<i2c0_sda_in,i2c0_sda_oe>    	
|	va_pdm_right_data_in_pin	
|	btco_wlan_tx                     
|	spwm_mci0	
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>            
|		
|	rmt0_tx_key[4]  	
|	btbb_spi_mosi                                             
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>            
|	IO<flash_tst_pi[7],flash_tst_po[7],flash_tst_poe[7]>	
|	IO<spif_so_in,spif_so_out,spif_so_out_enable>	
|	spi0_tx_data                
|	IO<flash_tst_pi[3],flash_tst_po[3],flash_tst_poe[3]>	
|	C_0	
|	C_1
|		
*/
#define PIN1_I2C0_SDA                           PIN_MODE_0 //M3 JTAG
#define PIN1_VA_PDM_RIGHT_DATA_IN_PIN           PIN_MODE_1
#define PIN1_BTCO_WLAN_TX                       PIN_MODE_2
#define PIN1_SPWM_MCI0                          PIN_MODE_3
#define PIN1_I2S0_TX_MCLK                       PIN_MODE_4 //IO<i2s1_tx_sclk_i,i2s1_tx_sclk_o,i2s1_tx_sclk_oe>
//#define PIN1_SCAN_IN1                 PIN_MODE_5
#define PIN1_RMT0_TX_KEY4                      PIN_MODE_6
#define PIN1_BTBB_SPI_MOSI                     PIN_MODE_7
#define PIN1_I2S0_RX_MCLK                      PIN_MODE_8 //IO<i2s1_rx_sclk_i,i2s1_rx_sclk_o,i2s1_rx_sclk_oe>
#define PIN1_FLASH_TST_PIN_7                    PIN_MODE_9
#define PIN1_SPIF_SO                           PIN_MODE_10 //UART10 TXD
#define PIN1_SPI0_TX_DATA                      PIN_MODE_11
#define PIN1_FLASH_TST_PIN3                    PIN_MODE_12

#define PIN1_IO_OUTPUT_0              PIN_MODE_14
#define PIN1_IO_OUTPUT_1              PIN_MODE_15
#define PIN1_IO_HIGH_Z_STATE          PIN_MODE_13


/*
|	2	
|	DPIN2	
|	PDUW0204CDG	
|		
|		
|	ODIO<i2c0_scl_in,i2c0_scl_oe>    
|	va_pdm_left_clk_pin 	
|	btco_ble_sync                   
|	spwm_mci2	
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>          
|		
|	rmt0_tx_key[6]  	
|	btbb_rxclk                                                
|	IO<spif_wp_n_in,spif_wp_n_out,spif_wp_n_out_enable>  
|	         
|		
|		
|	IO<flash_tst_pi[5],flash_tst_po[5],flash_tst_poe[5]>	
|	C_0
|	C_1
|	
*/
#define PIN2_I2C0_SCL                            PIN_MODE_0 //M3 JTAG
#define PIN2_VA_PDM_LEFT_CLK_PIN                 PIN_MODE_1
#define PIN2_BTCO_BLE_SYNC                       PIN_MODE_2
#define PIN2_SPWM_MCI2                           PIN_MODE_3
#define PIN2_I2S0_RX_MCLK                        PIN_MODE_4 //IO<i2s1_rx_mclk_i,i2s1_rx_mclk_o,i2s1_rx_mclk_oe>
//#define PIN2_SCAN_IN2                            PIN_MODE_5
#define PIN2_RMT0_TX_KEY6                        PIN_MODE_6
#define PIN2_BTBB_RX_CLK                         PIN_MODE_7
#define PIN2_SPIF_WP                             PIN_MODE_8

//#define PIN2_M0_UART0_RX                         PIN_MODE_10 //UART10 TXD
//#define PIN2_SPI0_RX_DATA_SLAVE                  PIN_MODE_11
#define PIN2_FLASH_TST_PIN5                      PIN_MODE_12
#define PIN2_IO_OUTPUT_0                         PIN_MODE_14
#define PIN2_IO_OUTPUT_1                         PIN_MODE_15
#define PIN2_IO_HIGH_Z_STATE                     PIN_MODE_13

/*
|	3	
|	DPIN3	
|	PDUW0204CDG	
|		
|		
|	jtag_tdo                     
|		
|	ODIO<i2c0_scl_in,i2c0_scl_oe>   
|	mpwm_MCABORT	
|	IO<i2s1_tx_mclk_i,i2s1_tx_mclk_o,i2s1_tx_mclk_oe>          
|	scan_in[0]                                   	
|	rmt0_tx_key[27] 	
|	btbb_txd                                                 
|	IO<i2s1_rx_mclk_i,i2s1_rx_mclk_o,i2s1_rx_mclk_oe>        
|	uart0_txd  
|	m0_uart0_tx                            
|	spi0_rx_clk
|	btco_wlan_tx	
|	C_0	
|	C_1	
|	IO<spif_cs_n_in,spif_cs_n_out,spif_cs_n_out_enable>  
*/

#define PIN3_JTAG_TDO                        PIN_MODE_0
//#define PIN3_AO_PDM_LEFT_DATA_IN             PIN_MODE_1
#define PIN3_I2C0_SCL                       PIN_MODE_2
#define PIN3_MPWM_MCABORT                   PIN_MODE_3
#define PIN3_I2S1_TX_MCLK                   PIN_MODE_4 //IO<i2s1_rx_sclk_i,i2s1_rx_sclk_o,i2s1_rx_sclk_oe>
#define PIN3_SCAN_IN0                       PIN_MODE_5
#define PIN3_RMT0_TX_KEY27                  PIN_MODE_6
#define PIN3_BTBB_TXD                       PIN_MODE_7
#define PIN3_I2S1_RX_MCLK                   PIN_MODE_8
#define PIN3_UART0_TX                       PIN_MODE_9
#define PIN3_M0_UART0_TX                    PIN_MODE_10 //UART10 TXD
#define PIN3_SPI0_RX_CLK                    PIN_MODE_11
#define PIN3_BTCO_WLAN_TX                   PIN_MODE_12
#define PIN3_IO_OUTPUT_0                    PIN_MODE_14
#define PIN3_IO_OUTPUT_1                    PIN_MODE_15
#define PIN3_IO_HIGH_Z_STATE                PIN_MODE_13
#define PIN3_SPIF_CS                        PIN_MODE_16

/*
|	4	
|	DPIN4	
|	PDUW0204CDG
|		
|		
|	jtag_nTRST0              
|	ao_mclk_to_pin       
|	ODIO<i2c0_sda_in,i2c0_sda_oe>   
|	mpwm_MCI0	
|	IO<i2s1_tx_sclk_i,i2s1_tx_sclk_o,i2s1_tx_sclk_oe>          
|	scan_in[1]                           
|	rmt0_tx_key[28] 	
|	btbb_txdv                                            
|	IO<i2s1_rx_sclk_i,i2s1_rx_sclk_o,i2s1_rx_sclk_oe>          
|	uart0_rts_n	
|	m0_uart0_rts_n                       
|	spi0_rx_data_master	
|	IO<flash_tst_pi[0],flash_tst_po[0],flash_tst_poe[0]>	
|	C_0	
|	C_1
|	IO<spif_so_in,spif_so_out,spif_so_out_enable>
*/

#define PIN4_JTAG_NTRST0                          PIN_MODE_0 //M0 JTAG
#define PIN4_AO_MCLK_TO_PIN                    PIN_MODE_1
#define PIN4_I2C0_SDA                          PIN_MODE_2
#define PIN4_MPWM_MCI0                         PIN_MODE_3
#define PIN4_I2S1_TX_SCLK                      PIN_MODE_4 //IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>
#define PIN4_SCAN_IN1                          PIN_MODE_5
#define PIN4_RMT0_TX_KEY28                      PIN_MODE_6
#define PIN4_BTBB_TXDV                         PIN_MODE_7
#define PIN4_I2S1_RX_SCLK                      PIN_MODE_8 //IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>
#define PIN4_UART0_RTS_N                       PIN_MODE_9
#define PIN4_M0_UART0_RTS_N                      PIN_MODE_10
#define PIN4_SPI0_RX_DATA_MASTER                    PIN_MODE_11
#define PIN4_FLASH_TST_PIN0                    PIN_MODE_12
#define PIN4_IO_OUTPUT_0                       PIN_MODE_14
#define PIN4_IO_OUTPUT_1                       PIN_MODE_15
#define PIN4_IO_HIGH_Z_STATE                   PIN_MODE_13
#define PIN4_SPIF_SO                           PIN_MODE_16

/*
|	5	
|	DPIN5	
|	PDUW0204CDG	
|		
|		
|	mpwm_MCOA1       
|	test_clk_out         
|	btco_wlan_rx            
|	spwm_mci1	
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>        
|	scan_clk                            
|	rmt0_tx_key[5]  
|	btbb_rxd                            
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>        
|	va_pdm_right_data_in_pin	
|	ODIO<i2c1_sda_in,i2c1_sda_oe>
|	spi0_tx_clk           
|	IO<flash_tst_pi[4],flash_tst_po[4],flash_tst_poe[4]>	
|	C_0	
|	C_1	
|	uart0_txd      
|	uart0_rts_n    
|	uart0_rxd           
|	uart0_cts_n  
|	spwm_out	
|	spi0_tx_data	
|	spi0_tx_clk	
|	spi1_tx_data
|	spi1_tx_clk	
|	ODIO<i2c0_sda_in,i2c0_sda_oe>   
|	ODIO<i2c0_scl_in,i2c0_scl_oe>
|	m0_uart0_tx	
|	m0_uart0_rts_n	
|	m0_uart0_rx	
|	m0_uart0_cts_n	
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>     
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>     
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>        
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>     
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
|	i2spcm0_tx_bitout                               
|	i2spcm0_rx_bitin                            
|	rmt0_txd	
|	rmt0_rxd	
|	spi0_rx_clk
|	spi0_rx_data_master
|	spi0_rx_data_slave	
|	spi0_rx_chip_select_n
|	spi1_rx_clk	
|	spi1_rx_data_master	
|	spi1_rx_data_slave	
|	spi1_rx_chip_select_n	
*/

#define PIN5_MPWM_MCOA1                PIN_MODE_0 //M3 JTAG
#define PIN5_TEST_CLK_OUT              PIN_MODE_1
#define PIN5_BTCO_WLAN_RX              PIN_MODE_2
#define PIN5_SPWM_MCI1                 PIN_MODE_3
#define PIN5_I2S0_TX_SCLK              PIN_MODE_4 //	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>
#define PIN5_SCAN_CLK                  PIN_MODE_5
#define PIN5_RMT0_TX_KEY5              PIN_MODE_6
#define PIN5_BTBB_RXD                  PIN_MODE_7
#define PIN5_I2S0_RX_SCLK              PIN_MODE_8 //IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>
#define PIN5_VA_PDM_RIGHT_DATA_IN_PIN  PIN_MODE_9
#define PIN5_I2C1_SDA                PIN_MODE_10
#define PIN5_SPI0_TX_CLK             PIN_MODE_11
#define PIN5_FLASH_TST_PIN4          PIN_MODE_12

#define PIN5_IO_OUTPUT_0             PIN_MODE_14
#define PIN5_IO_OUTPUT_1             PIN_MODE_15
#define PIN5_IO_HIGH_Z_STATE         PIN_MODE_13

#define PIN5_UART0_TXD               PIN_MODE_16
#define PIN5_UART0_RTS               PIN_MODE_17
#define PIN5_UART0_RXD               PIN_MODE_18
#define PIN5_UART0_CTS               PIN_MODE_19

#define PIN5_SPWM_OUT                PIN_MODE_20
#define PIN5_SPI0_TX_DATA            PIN_MODE_21
//#define PIN5_SPI0_TX_CLK               PIN_MODE_22
#define PIN5_SPI1_TX_DATA            PIN_MODE_23
#define PIN5_SPI1_TX_CLK             PIN_MODE_24
#define PIN5_I2C0_SDA                PIN_MODE_25
#define PIN5_I2C0_SCL                PIN_MODE_26
#define PIN5_M0_UART0_TX               PIN_MODE_27
#define PIN5_M0_UART0_RTS               PIN_MODE_28   //m0_uart0_rts_n
#define PIN5_M0_UART0_RX               PIN_MODE_29   //m0_uart0_rx
#define PIN5_M0_UART0_CTS               PIN_MODE_30     //m0_uart0_cts_n
#define PIN5_I2S0_TX_MCLK               PIN_MODE_31   //i2s0_tx_mclk_i
//#define PIN5_I2S0_TX_SCLK               PIN_MODE_32   //i2s0_tx_sclk_i
#define PIN5_I2S0_RX_MCLK               PIN_MODE_33   //i2s0_rx_mclk_i
//#define PIN5_I2S0_RX_SCLK               PIN_MODE_34   //i2s0_rx_sclk_i
#define PIN5_I2SPCM0_TX_OE               PIN_MODE_35   //i2spcm0_tx_oe_bit_vld
#define PIN5_I2SPCM0_RX_OE               PIN_MODE_36   //i2spcm0_rx_oe_bit_vld
#define PIN5_I2SPCM0_TX                  PIN_MODE_37   //i2spcm0_tx_bitout
#define PIN5_I2SPCM0_RX                 PIN_MODE_38   //i2spcm0_rx_bitin
#define PIN5_RMT0_TXD                   PIN_MODE_39   //rmt0_txd
#define PIN5_RMT0_RXD                   PIN_MODE_40   //rmt0_rxd
#define PIN5_SPI0_RX_CLK                       PIN_MODE_41   //spi0_rx_clk
#define PIN5_SPI0_RX_DATA_MASTER               PIN_MODE_42   //|	spi0_rx_data_master
#define PIN5_SPI0_RX_DATA_SLAVE              PIN_MODE_43   //|	spi0_rx_data_slave	
#define PIN5_SPI0_RX_CHIP_SELECT               PIN_MODE_44   //|	spi0_rx_chip_select_n
#define PIN5_SPI1_RX_CLK                    PIN_MODE_45   //|	spi1_rx_clk	
#define PIN5_SPI1_RX_DATA_MASTER               PIN_MODE_46   //|	spi1_rx_data_master	
#define PIN5_SPI1_RX_DATA_SLAVE               PIN_MODE_47   //|	spi1_rx_data_slave	
#define PIN5_SPI1_RX_CHIP_SELECT               PIN_MODE_48   //|	spi1_rx_chip_select_n	


/*
|	6	
|	DPIN6
|	PDUW0204CDG	
|	
|	
|	jtag_tdi             
|	ao_pdm_data_source   
|	ODIO<i2c1_scl_in,i2c1_scl_oe>   
|	mpwm_MCI1
|	IO<i2s1_rx_mclk_i,i2s1_rx_mclk_o,i2s1_rx_mclk_oe>         
|	scan_in[2]                       
|	rmt0_tx_key[29] 	
|	btbb_spi_clk            
|	va_pdm_left_clk_pin	
|	uart0_rxd  
|	m0_uart0_rx             
|	spi0_rx_data_slave	
|	IO<flash_tst_pi[1],flash_tst_po[1],flash_tst_poe[1]>
|	C_0	
|	C_1	
|	IO<spif_wp_n_in,spif_wp_n_out,spif_wp_n_out_enable> 
*/

#define PIN6_JTAG_TDI                         PIN_MODE_0 //M0 JTAG
#define PIN6_AO_PDM_DATA_SOURCE               PIN_MODE_1
#define PIN6_I2C1_SCL                         PIN_MODE_2
#define PIN6_MPWM_MCI1                        PIN_MODE_3
#define PIN6_I2S1_RX_MCLK                     PIN_MODE_4 //IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>
#define PIN6_SCAN_IN2                         PIN_MODE_5
#define PIN6_RMT0_TX_KEY29                    PIN_MODE_6
#define PIN6_BTBB_SPI_CLK                     PIN_MODE_7
#define PIN6_VA_PDM_LEFT_CLK                  PIN_MODE_8
#define PIN6_UART0_RXD                        PIN_MODE_9
#define PIN6_M0_UART0_RX                      PIN_MODE_10
#define PIN6_SPI0_RX_DATA_SLAVE               PIN_MODE_11
#define PIN6_FLASH_TST_PIN1                   PIN_MODE_12
#define PIN6_IO_OUTPUT_0                      PIN_MODE_14
#define PIN6_IO_OUTPUT_1                      PIN_MODE_15
#define PIN6_IO_HIGH_Z_STATE                  PIN_MODE_13
#define PIN6_SPIF_WP                          PIN_MODE_16

/*
|	7	
|	DPIN7	
|	PDUW0204CDG	
|		
|		
|	uart0_txd              
|	va_pdm_right_clk_pin	
|	btco_ble_in_process      
|	spi0_rx_clk
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>       
|	scan_compress_mode                    
|	rmt0_tx_key[7]  
|	btbb_rxsync           
|	                      
|		
|	
|		
|	IO<flash_tst_pi[6],flash_tst_po[6],flash_tst_poe[6]>	
|	C_0	
|	C_1	
|	uart0_txd          
|	uart0_rts_n    
|	uart0_rxd        
|	uart0_cts_n  
|	spwm_out	
|	spi0_tx_data
|	spi0_tx_clk	
|	spi1_tx_data	
|	spi1_tx_clk
|	ODIO<i2c0_sda_in,i2c0_sda_oe>   	
|	ODIO<i2c0_scl_in,i2c0_scl_oe>	
|	m0_uart0_tx
|	m0_uart0_rts_n
|	m0_uart0_rx	
|	m0_uart0_cts_n
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>     
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>        
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>     
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>   
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
|	i2spcm0_tx_bitout                                       
|	i2spcm0_rx_bitin                                      
|	rmt0_txd	
|	rmt0_rxd	
|	spi0_rx_clk	
|	spi0_rx_data_master
|	spi0_rx_data_slave
|	spi0_rx_chip_select_n	
|	spi1_rx_clk
|	spi1_rx_data_master
|	spi1_rx_data_slave
|	spi1_rx_chip_select_n	
*/
#define PIN7_UART0_TXD                        PIN_MODE_0
#define PIN7_VA_PDM_RIGHT_CLK                 PIN_MODE_1
#define PIN7_BTCO_BLE_IN_PROCESS              PIN_MODE_2
#define PIN7_SPI0_RX_CLK                      PIN_MODE_3
#define PIN7_I2S0_RX_SCLK                     PIN_MODE_4 //IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>
#define PIN7_SCAN_COMPRESS_MODE               PIN_MODE_5
#define PIN7_RMT0_TX_KEY7                     PIN_MODE_6
#define PIN7_BTBB_RXSYNC                      PIN_MODE_7


#define PIN7_FLASH_TST_PIN6                   PIN_MODE_12
#define PIN7_IO_OUTPUT_0                      PIN_MODE_14
#define PIN7_IO_OUTPUT_1                      PIN_MODE_15
#define PIN7_IO_HIGH_Z_STATE                  PIN_MODE_13

//#define PIN7_UART0_TXD                  PIN_MODE_16   //  uart0_txd          
#define PIN7_UART0_RTS                 PIN_MODE_17   // |	uart0_rts_n    
#define PIN7_UART0_RXD                  PIN_MODE_18   // |	uart0_rxd        
#define PIN7_UART0_CTS                  PIN_MODE_19   // |	uart0_cts_n  
#define PIN7_SPWM_OUT                  PIN_MODE_20   // |	spwm_out	
#define PIN7_SPI0_TX_DATA                  PIN_MODE_21   // |	spi0_tx_data
#define PIN7_SPI0_TX_CLK                  PIN_MODE_22   // |	spi0_tx_clk	

#define PIN7_SPI1_TX_DATA                  PIN_MODE_23   // |	spi1_tx_data	
#define PIN7_SPI1_TX_CLK                  PIN_MODE_24   // |	spi1_tx_clk

#define PIN7_I2C0_SDA                  PIN_MODE_25   // |	ODIO<i2c0_sda_in,i2c0_sda_oe>   	
#define PIN7_I2C0_SCL                  PIN_MODE_26   // |	ODIO<i2c0_scl_in,i2c0_scl_oe>	
#define PIN7_M0_UART0_TX                  PIN_MODE_27   // |	m0_uart0_tx
#define PIN7_M0_UART0_RTS                  PIN_MODE_28   // |	m0_uart0_rts_n
#define PIN7_M0_UART0_RX                  PIN_MODE_29   // |	m0_uart0_rx	
#define PIN7_M0_UART0_CTS                  PIN_MODE_30   // |	m0_uart0_cts_n

#define PIN7_I2S0_TX_MCLK                 PIN_MODE_31   // |	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>     
#define PIN7_I2S0_TX_SCLK                  PIN_MODE_32   // |	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>        
#define PIN7_I2S0_RX_MCLK                  PIN_MODE_33   // |	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>     
//#define PIN7_I2S0_RX_SCLK                  PIN_MODE_34   // |	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>   
#define PIN7_I2SPCM0_TX_OE                  PIN_MODE_35   // |	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
#define PIN7_I2SPCM0_RX_OE                  PIN_MODE_36   // |	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	

#define PIN7_I2SPCM0_TX                     PIN_MODE_37   // |	i2spcm0_tx_bitout                                       
#define PIN7_I2SPCM0_RX                     PIN_MODE_38   // |	i2spcm0_rx_bitin                                      
#define PIN7_RMT0_TXD                       PIN_MODE_39   // |	rmt0_txd	
#define PIN7_RMT0_RXD                      PIN_MODE_40   // |	rmt0_rxd	

//#define PIN7_SPI0_RX_CLK                   PIN_MODE_41   // |	spi0_rx_clk	
#define PIN7_SPI0_RX_DATA_MASTER                  PIN_MODE_42   // |	spi0_rx_data_master
#define PIN7_SPI0_RX_DATA_SLAVE                   PIN_MODE_43   // |	spi0_rx_data_slave
#define PIN7_SPI0_RX_CHIP_SELECT                  PIN_MODE_44   // |	spi0_rx_chip_select_n	
#define PIN7_SPI1_RX_CLK                          PIN_MODE_45   // |	spi1_rx_clk
#define PIN7_SPI1_RX_DATA_MASTER                  PIN_MODE_46   // |	spi1_rx_data_master
#define PIN7_SPI1_RX_DATA_SLAVE                   PIN_MODE_48   // |	spi1_rx_data_slave
#define PIN7_SPI1_RX_CHIP_SELECT                  PIN_MODE_49   // |	spi1_rx_chip_select_n	

/*
|	8
|	DPIN8	
|	PDUW0204CDG	
|		
|		
|	uart0_rts_n      
|	rmt_rxd_ana	
|	btco_ble_tx                   
|	spi0_rx_data_master
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>
|	scan_enable                
|	rmt0_tx_key[8]  
|	btbb_spi_miso       
|	                           
|		
|	spwm_mci0	
|	        
|	IO<flash_tst_pi[2],flash_tst_po[2],flash_tst_poe[2]>
|	C_0	
|	C_1	
|	uart0_txd      
|	uart0_rts_n      
|	uart0_rxd          
|	uart0_cts_n 
|	spwm_out	
|	spi0_tx_data
|	spi0_tx_clk	
|	spi1_tx_data
|	spi1_tx_clk	
|	ODIO<i2c0_sda_in,i2c0_sda_oe>   
|	ODIO<i2c0_scl_in,i2c0_scl_oe>
|	m0_uart0_tx	
|	m0_uart0_rts_n	
|	m0_uart0_rx
|	m0_uart0_cts_n	
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>    
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>         
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>       
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>      
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
|	i2spcm0_tx_bitout                                       
|	i2spcm0_rx_bitin                                       
|	rmt0_txd
|	rmt0_rxd	
|	spi0_rx_clk
|	spi0_rx_data_master
|	spi0_rx_data_slave
|	spi0_rx_chip_select_n	
|	spi1_rx_clk	
|	spi1_rx_data_master
|	spi1_rx_data_slave
|	spi1_rx_chip_select_n
*/
#define PIN8_UART0_RTS                        PIN_MODE_0 //M0 JTAG
#define PIN8_RMT_RXD_DATA                     PIN_MODE_1
#define PIN8_BTCO_BLE_TX                      PIN_MODE_2
#define PIN8_SPI0_RX_DATA_MASTER              PIN_MODE_3
#define PIN8_I2S0PCM_TX_OE                    PIN_MODE_4 //IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>
#define PIN8_SCAN_ENABLE                      PIN_MODE_5
#define PIN8_RMT0_TX_KEY8                     PIN_MODE_6
#define PIN8_BTBB_SPI_MISO                    PIN_MODE_7
//#define  PIN8_SPIF_SCK		                	PIN_MODE_9    //IO<spif_sck_in,spif_sck_out,spif_sck_out_enable>
#define PIN8_SPWM_MCI0                        PIN_MODE_10
//#define  PIN8_UART0_RTS											PIN_MODE_11
#define PIN8_FLASH_TST_PIN2                   PIN_MODE_12
#define PIN8_IO_OUTPUT_0                      PIN_MODE_14
#define PIN8_IO_OUTPUT_1                      PIN_MODE_15
#define PIN8_IO_HIGH_Z_STATE                  PIN_MODE_13

#define PIN8_UART0_TXD                        PIN_MODE_16    // |	uart0_txd      
//#define PIN8_UART0_RTS                        PIN_MODE_17    // |	uart0_rts_n      
#define PIN8_UART0_RXD                        PIN_MODE_18    // |	uart0_rxd          
#define PIN8_UART0_CTS                        PIN_MODE_19    // |	uart0_cts_n 
#define PIN8_SPWM_OUT                         PIN_MODE_20    // |	spwm_out	
#define PIN8_SPI0_TX_DATA                     PIN_MODE_21    // |	spi0_tx_data
#define PIN8_SPI0_TX_CLK                      PIN_MODE_22    // |	spi0_tx_clk	
#define PIN8_SPI1_TX_DATA                     PIN_MODE_23    // |	spi1_tx_data
#define PIN8_SPI1_TX_CLK                      PIN_MODE_24    // |	spi1_tx_clk	
#define PIN8_I2C0_SDA                         PIN_MODE_25    // |	ODIO<i2c0_sda_in,i2c0_sda_oe>   
#define PIN8_I2C0_SCL                         PIN_MODE_26    // |	ODIO<i2c0_scl_in,i2c0_scl_oe>
#define PIN8_M0_UART0_TX                      PIN_MODE_27    // |	m0_uart0_tx	
#define PIN8_M0_UART0_RTS                     PIN_MODE_28    // |	m0_uart0_rts_n	
#define PIN8_M0_UART0_RX                      PIN_MODE_29    // |	m0_uart0_rx
#define PIN8_M0_UART0_CTS                     PIN_MODE_30    // |	m0_uart0_cts_n	
#define PIN8_I2S0_TX_MCLK                     PIN_MODE_31    // |	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>    
#define PIN8_I2S0_TX_SCLK                     PIN_MODE_32    // |	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>         
#define PIN8_I2S0_RX_MCLK                     PIN_MODE_33    // |	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>       
#define PIN8_I2S0_RX_SCLK                     PIN_MODE_34    // |	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>      

#define PIN8_I2SPCM0_TX_OE                    PIN_MODE_35    // |	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
#define PIN8_I2SPCM0_RX_OE                    PIN_MODE_36    // |	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
#define PIN8_I2SPCM0_TX                       PIN_MODE_37    // |	i2spcm0_tx_bitout                                       
#define PIN8_I2SPCM0_RX                       PIN_MODE_38    // |	i2spcm0_rx_bitin   

#define PIN8_RMT0_TXD                         PIN_MODE_39    // |	rmt0_txd
#define PIN8_RMT0_RXD                         PIN_MODE_40    // |	rmt0_rxd	
#define PIN8_SPI0_RX_CLK                      PIN_MODE_41    // |	spi0_rx_clk
//#define PIN8_SPI0_RX_DATA_MASTER                  PIN_MODE_42    // |	spi0_rx_data_master
#define PIN8_SPI0_RX_DATA_SLAVE                   PIN_MODE_43    // |	spi0_rx_data_slave
#define PIN8_SPI0_RX_CHIP_SELECT                  PIN_MODE_44    // |	spi0_rx_chip_select_n	
#define PIN8_SPI1_RX_CLK                          PIN_MODE_45    // |	spi1_rx_clk	
#define PIN8_SPI1_RX_DATA_MASTER                  PIN_MODE_46    // |	spi1_rx_data_master
#define PIN8_SPI1_RX_DATA_SLAVE                   PIN_MODE_47    // |	spi1_rx_data_slave
#define PIN8_SPI1_RX_CHIP_SELECT                  PIN_MODE_48    // |	spi1_rx_chip_select_n

/*
|	9	
|	DPIN9
|	PDUW0204CDG	
|	
|		
|	uart0_rxd            
|	ODIO<i2c0_sda_in,i2c0_sda_oe>  
|	btco_ble_rx                    
|	spi0_rx_data_slave
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>
|	scan_out[0]                
|	rmt0_tx_key[9] 
|	IO<icytrx_datain[1],dataout[1],dataout_en[1]>	
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>
|	btbb_txd 
|	spwm_mci1	
|		
|	
|	C_0
|	C_1	
|	uart0_txd          
|	uart0_rts_n      
|	uart0_rxd          
|	uart0_cts_n  
|	spwm_out	
|	spi0_tx_data
|	spi0_tx_clk
|	spi1_tx_data	
|	spi1_tx_clk	
|	ODIO<i2c0_sda_in,i2c0_sda_oe>  
|	ODIO<i2c0_scl_in,i2c0_scl_oe>
|	m0_uart0_tx	
|	m0_uart0_rts_n	
|	m0_uart0_rx
|	m0_uart0_cts_n
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>    
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>       
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>      
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>   
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
|	i2spcm0_tx_bitout                                        
|	i2spcm0_rx_bitin                                    
|	rmt0_txd	
|	rmt0_rxd	
|	spi0_rx_clk	
|	spi0_rx_data_master
|	spi0_rx_data_slave	
|	spi0_rx_chip_select_n	
|	spi1_rx_clk	
|	spi1_rx_data_master
|	spi1_rx_data_slave	
|	spi1_rx_chip_select_n
*/

#define PIN9_UART0_RXD                       PIN_MODE_0
#define PIN9_I2C0_SDA                        PIN_MODE_1
#define PIN9_BTCO_BLE_RX                     PIN_MODE_2
#define PIN9_SPI0_RX_DATA_SLAVE              PIN_MODE_3
#define PIN9_I2SPCM0_RX_OE                   PIN_MODE_4 //IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>
#define PIN9_SCAN_OUT0                       PIN_MODE_5
#define PIN9_RMT0_TX_KEY9                    PIN_MODE_6
#define PIN9_ICYTRX_DATAIN_1                    PIN_MODE_7
#define PIN9_I2SPCM0_TX_OE                   PIN_MODE_8 //IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>
#define PIN9_BTBB_TXD                        PIN_MODE_9
#define PIN9_SPWM_MCI1                       PIN_MODE_10
#define PIN9_IO_OUTPUT_0                     PIN_MODE_14
#define PIN9_IO_OUTPUT_1                     PIN_MODE_15
#define PIN9_IO_HIGH_Z_STATE                 PIN_MODE_13

#define PIN9_UART0_TXD                 PIN_MODE_16     //  |	uart0_txd          
#define PIN9_UART0_RTS                 PIN_MODE_17     //  |	uart0_rts_n      
//#define PIN9_UART0_RXD                 PIN_MODE_18     //  |	uart0_rxd          
#define PIN9_UART0_CTS                 PIN_MODE_19     //  |	uart0_cts_n  
#define PIN9_SPWM_OUT                 PIN_MODE_20     //  |	spwm_out	
#define PIN9_SPI0_TX_DATA                    PIN_MODE_21     //  |	spi0_tx_data
#define PIN9_SPI0_TX_CLK                     PIN_MODE_22     //  |	spi0_tx_clk
#define PIN9_SPI1_TX_DATA                    PIN_MODE_23     //  |	spi1_tx_data	
#define PIN9_SPI1_TX_CLK                     PIN_MODE_24     //  |	spi1_tx_clk	
//#define PIN9_I2C0_SDA                 PIN_MODE_25     //  |	ODIO<i2c0_sda_in,i2c0_sda_oe>  
#define PIN9_I2C0_SCL                 PIN_MODE_26     //  |	ODIO<i2c0_scl_in,i2c0_scl_oe>
#define PIN9_M0_UART0_TX                PIN_MODE_27     //  |	m0_uart0_tx	
#define PIN9_M0_UART0_RTS                 PIN_MODE_28     //  |	m0_uart0_rts_n	
#define PIN9_M0_UART0_RX                 PIN_MODE_29     //  |	m0_uart0_rx
#define PIN9_M0_UART0_CTS                 PIN_MODE_30     //  |	m0_uart0_cts_n
#define PIN9_I2S0_TX_MCLK                 PIN_MODE_31     //  |	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>    
#define PIN9_I2S0_TX_SCLK                 PIN_MODE_32     //  |	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>       
#define PIN9_I2S0_RX_MCLK                 PIN_MODE_33     //  |	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>      
#define PIN9_I2S0_RX_SCLK                 PIN_MODE_34     //  |	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>   
//#define PIN9_I2SPCM0_TX_OE                 PIN_MODE_35     //  |	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
//#define PIN9_I2SPCM0_RX_OE                 PIN_MODE_36     //  |	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
#define PIN9_I2SPCM0_TX                          PIN_MODE_37     //  |	i2spcm0_tx_bitout                                        
#define PIN9_I2SPCM0_RX                          PIN_MODE_38     //  |	i2spcm0_rx_bitin                                    
#define PIN9_RMT0_TXD                            PIN_MODE_39     //  |	rmt0_txd	
#define PIN9_RMT0_RXD                            PIN_MODE_40     //  |	rmt0_rxd	
#define PIN9_SPI0_RX_CLK                         PIN_MODE_41     //  |	spi0_rx_clk	
#define PIN9_SPI0_RX_DATA_MASTER                 PIN_MODE_42     //  |	spi0_rx_data_master
//#define PIN9_SPI0_RX_DATA_SLAVE                PIN_MODE_43     //  |	spi0_rx_data_slave	
#define PIN9_SPI0_RX_CHIP_SELECT                 PIN_MODE_44     //  |	spi0_rx_chip_select_n	
#define PIN9_SPI1_RX_CLK                         PIN_MODE_45     //  |	spi1_rx_clk	
#define PIN9_SPI1_RX_DATA_MASTER                 PIN_MODE_46     //  |	spi1_rx_data_master
#define PIN9_SPI1_RX_DATA_SLAVE                  PIN_MODE_47     //  |	spi1_rx_data_slave	
#define PIN9_SPI1_RX_CHIP_SELECT                 PIN_MODE_48     //  |	spi1_rx_chip_select_n

/*
|	10	
|	DPIN10
|	PDUW0204CDG	
|		
|		
|	uart0_cts_n    
|		
|	
|	spi0_rx_chip_select_n
|	IO<i2spcm1_tx_oe_bit_vld,i2spcm1_tx_bitout_vld,i2s1_tx_ws_oe>
|	
|	rmt0_tx_key[10] 	
|	                           
|	IO<i2spcm1_rx_oe_bit_vld,i2spcm1_rx_bitout_vld,i2s1_rx_ws_oe>	
|	btbb_txdv 
|	spwm_mci2	
|	         
|	
|	C_0	
|	C_1
*/

#define PIN10_UART0_CTS                         PIN_MODE_0 //M3 JTAG
//#define  PIN10_GPIO1_PIN2                     PIN_MODE_1
#define PIN10_SPI0_RX_CHIP_SELECT               PIN_MODE_3
#define PIN10_I2SPCM1_TX_OE                     PIN_MODE_4 //IO<i2spcm1_tx_oe_bit_vld,i2spcm1_tx_bitout_vld,i2s1_tx_ws_oe>
//#define PIN10_SCAN_OUT1                         PIN_MODE_5
#define PIN10_RMT0_TX_KEY10                     PIN_MODE_6
#define PIN10_I2SPCM1_RX_OE                     PIN_MODE_8 //IO<i2spcm1_rx_oe_bit_vld,i2spcm1_rx_bitout_vld,i2s1_rx_ws_oe>
#define PIN10_BTBB_TXDV                         PIN_MODE_9
#define PIN10_SPWM_MCI2                         PIN_MODE_10
//#define  PIN10_UART0_CTS	           					PIN_MODE_11
#define PIN10_IO_OUTPUT_0                       PIN_MODE_14
#define PIN10_IO_OUTPUT_1                       PIN_MODE_15
#define PIN10_IO_HIGH_Z_STATE                   PIN_MODE_13

/*
|	11	
|	DPIN11
|	PDUW0204CDG	
|		
|		
|	o_sen[0]         
|		
|	btco_ble_pti[1]          
|	spi0_tx_data             
|	IO<i2spcm1_rx_oe_bit_vld,i2spcm1_rx_bitout_vld,i2s1_rx_ws_oe>	
|	scan_out[2]                               
|	rmt0_tx_key[11]
|	IO<flash_tst_pi[0],flash_tst_po[0],flash_tst_poe[0]>	
|	btbb_spi_clk      
|	
|		
|		
|		
|	C_0
|	C_1
*/
#define PIN11_O_SEN0                            PIN_MODE_0
//#define  PIN11_GPIO1_PIN3                     PIN_MODE_1
#define PIN11_BTCO_BLE_PTI1                     PIN_MODE_2
#define PIN11_SPI0_TX_DATA                      PIN_MODE_3
#define PIN11_I2SPCM1_RX_OE                     PIN_MODE_4 //  IO<i2spcm1_rx_oe_bit_vld,i2spcm1_rx_bitout_vld,i2s1_rx_ws_oe>
#define PIN11_SCAN_OUT2                         PIN_MODE_5
#define PIN11_RMT0_TX_KEY11                     PIN_MODE_6
#define PIN11_FLASH_TST_PIN0                    PIN_MODE_7
#define PIN11_BTBB_SPI_CLK                      PIN_MODE_8
#define PIN11_IO_OUTPUT_0                       PIN_MODE_14
#define PIN11_IO_OUTPUT_1                       PIN_MODE_15
#define PIN11_IO_HIGH_Z_STATE                   PIN_MODE_13
/*
|	12	
|	DPIN12	
|	PDDW0204CDG	
|		
|	
|	o_sen[1]                   
|	IO<gpio1_in[4],gpio1_out[4],gpio1_ena[4]>     
|	btco_ble_pti[2]                  
|	spi0_tx_clk                  
|	i2spcm0_tx_bitout                                          
|	scan_out[3]                                  
|	rmt0_tx_key[12] 
|	IO<flash_tst_pi[1],flash_tst_po[1],flash_tst_poe[1]>	
|	btbb_spi_cs                                                 
|		
|	
|	
|	
|
*/
/*
|	12	
|	DPIN12	
|	PDUW0204CDG	
|		
|		
|	o_sen[1]        
|	
|	btco_ble_pti[2]         
|	spi0_tx_clk        
|	i2spcm0_tx_bitout                             
|	scan_out[3]                  
|	rmt0_tx_key[12] 	
|	IO<flash_tst_pi[1],flash_tst_po[1],flash_tst_poe[1]>
|	btbb_spi_cs                         
|
|		
|	
|	
|	C_0
|	C_1
*/

#define PIN12_O_SEN1                  PIN_MODE_0
#define PIN12_BTCO_BLE_PTI2           PIN_MODE_2
#define PIN12_SPI0_TX_CLK             PIN_MODE_3
#define PIN12_I2SPCM0_TX_BITOUT       PIN_MODE_4
#define PIN12_SCAN_OUT3               PIN_MODE_5
#define PIN12_RMT0_TX_KEY12           PIN_MODE_6
#define PIN12_FLASH_TST_PIN1          PIN_MODE_7
#define PIN12_BTBB_SPI_CS             PIN_MODE_8
#define PIN12_IO_OUTPUT_0             PIN_MODE_14
#define PIN12_IO_OUTPUT_1             PIN_MODE_15
#define PIN12_IO_HIGH_Z_STATE         PIN_MODE_13

/*
|	13	
|	DPIN13
|	PDUW0204CDG
|	
|		
|	o_sen[2]             
|	IO<spif_si_in,spif_si_out,spif_si_out_enable>	
|	btco_ble_pti[3]           
|	
|	i2spcm1_tx_bitout          
|	scan_out[4]           
|	rmt0_tx_key[13] 
|	IO<flash_tst_pi[2],flash_tst_po[2],flash_tst_poe[2]>	
|	btbb_spi_mosi               
|		
|		
|		
|		
|	C_0
|	C_1
*/
#define PIN13_O_SEN2                PIN_MODE_0
#define PIN13_SPFI_SI               PIN_MODE_1
#define PIN13_BTCO_BLE_PTI3         PIN_MODE_2
#define PIN13_I2SPCM1_TX_BITOUT     PIN_MODE_4
#define PIN13_SCAN_OUT4             PIN_MODE_5
#define PIN13_RMT0_TX_KEY13         PIN_MODE_6
#define PIN13_FLASH_TST_PIN2        PIN_MODE_7
#define PIN13_BTBB_SPI_MOSI         PIN_MODE_8
#define PIN13_IO_OUTPUT_0           PIN_MODE_14
#define PIN13_IO_OUTPUT_1           PIN_MODE_15
#define PIN13_IO_HIGH_Z_STATE       PIN_MODE_13
/*
|	14	
|	DPIN14	
|	PDUW0204CDG
|		
|		
|	o_sen[3]       
|	IO<spif_sck_in,spif_sck_out,spif_sck_out_enable>	
|	btco_ble_pti[0]            
|		
|	i2spcm0_rx_bitin      
|	scan_in[3]           
|	rmt0_tx_key[14] 	
|	IO<flash_tst_pi[3],flash_tst_po[3],flash_tst_poe[3]>	
|	                                                
|	
|	
|		
|		
|	C_0	
|	C_1	
*/
#define PIN14_O_SEN3               PIN_MODE_0
#define PIN14_SPIF_SCLK            PIN_MODE_1
#define PIN14_BTCO_BLE_PTI0        PIN_MODE_2
#define PIN14_I2SPCM0_RX_BITIN     PIN_MODE_4
#define PIN14_SCAN_IN3             PIN_MODE_5
#define PIN14_RMT0_TX_KEY14        PIN_MODE_6
#define PIN14_FLASH_TST_PIN3       PIN_MODE_7
#define PIN14_IO_OUTPUT_0          PIN_MODE_14
#define PIN14_IO_OUTPUT_1          PIN_MODE_15
#define PIN14_IO_HIGH_Z_STATE      PIN_MODE_13


/*
|	15	
|	DPIN15	
|	PDDW0204CDG	
|		
|		
|	mpwm_MCOB0       
|	m0_uart0_rx      
|	IO<spif_hold_n_in,spif_hold_n_out,spif_hold_n_out_enable>	
|	pad_icy_spi_miso
|	i2spcm1_rx_bitin         
|	scan_in[4]               
|	rmt0_tx_key[15] 	
|	IO<flash_tst_pi[4],flash_tst_po[4],flash_tst_poe[4]>
|	btbb_rxclk                                    
|	spi1_rx_clk                                
|	ODIO<i2c0_scl_in,i2c0_scl_oe>  
|	mpwm_MCABORT
|	va_pdm_left_data_in_pin
|	C_0	
|	C_1	
|	uart0_txd        
|	uart0_rts_n    
|	uart0_rxd       
|	uart0_cts_n  
|	spwm_out	
|	spi0_tx_data	
|	spi0_tx_clk
|	spi1_tx_data	
|	spi1_tx_clk	
|	ODIO<i2c0_sda_in,i2c0_sda_oe>   	
|	ODIO<i2c0_scl_in,i2c0_scl_oe>	
|	m0_uart0_tx
|	m0_uart0_rts_n
|	m0_uart0_rx	
|	m0_uart0_cts_n	
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe> 
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>        
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>       
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>    
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>
|	i2spcm0_tx_bitout                                     
|	i2spcm0_rx_bitin                              
|	rmt0_txd	
|	rmt0_rxd	
|	spi0_rx_clk	
|	spi0_rx_data_master
|	spi0_rx_data_slave	
|	spi0_rx_chip_select_n	
|	spi1_rx_clk
|	spi1_rx_data_master
|	spi1_rx_data_slave	
|	spi1_rx_chip_select_n	|
*/

#define PIN15_MPWM_MCOB0            PIN_MODE_0   // mpwm_MCOB0
#define PIN15_M0_UART0_RX           PIN_MODE_1   //m0_uart0_rx
#define PIN15_SPIF_HOLD             PIN_MODE_2   //spif_hold_n_in
#define PIN15_PAD_ICY_SPI_MISO      PIN_MODE_3   //pad_icy_spi_miso
#define PIN15_I2SPCM1_RX_BITIN      PIN_MODE_4   //i2spcm1_rx_bitin
#define PIN15_SCAN_IN4              PIN_MODE_5  //scan_in[4]      
#define PIN15_RMT0_TX_KEY15         PIN_MODE_6   //rmt0_tx_key[15] 	
#define PIN15_FLASH_TST_PIN4        PIN_MODE_7   //flash_tst_pi
#define PIN15_BTBB_RXCLK            PIN_MODE_8   //btbb_rxclk
#define PIN15_SPI1_RX_CLK    	      PIN_MODE_9   //spi1_rx_clk
#define PIN15_I2C0_SCL              PIN_MODE_10   //i2c0_scl_in
#define PIN15_MPWM_MCABORT          PIN_MODE_11    //mpwm_MCABORT
#define PIN15_VA_PDM_LEFT_DATA      PIN_MODE_12   //va_pdm_left_data_in_pin

#define PIN15_IO_OUTPUT_0           PIN_MODE_14
#define PIN15_IO_OUTPUT_1           PIN_MODE_15
#define PIN15_IO_HIGH_Z_STATE       PIN_MODE_13

#define PIN15_UART0_TXD             PIN_MODE_16    // |	uart0_txd        
#define PIN15_UART0_RTS             PIN_MODE_17    // |	uart0_rts_n    
#define PIN15_UART0_RXD             PIN_MODE_18    // |	uart0_rxd       
#define PIN15_UART0_CTS             PIN_MODE_19    // |	uart0_cts_n  
#define PIN15_SPWM_OUT              PIN_MODE_20    // |	spwm_out	
#define PIN15_SPI0_TX_DATA          PIN_MODE_21    // |	spi0_tx_data	
#define PIN15_SPI0_TX_CLK           PIN_MODE_22    // |	spi0_tx_clk
#define PIN15_SPI1_TX_DATA          PIN_MODE_23    // |	spi1_tx_data	
#define PIN15_SPI1_TX_CLK               PIN_MODE_24    // |	spi1_tx_clk	
#define PIN15_I2C0_SDA                  PIN_MODE_25    // |	ODIO<i2c0_sda_in,i2c0_sda_oe>   	
//#define PIN15_I2C0_SCL                PIN_MODE_26    // |	ODIO<i2c0_scl_in,i2c0_scl_oe>	
#define PIN15_M0_UART0_TX               PIN_MODE_27    // |	m0_uart0_tx
#define PIN15_M0_UART0_RTS              PIN_MODE_28    // |	m0_uart0_rts_n
//#define PIN15_M0_UART0_RX             PIN_MODE_29    // |	m0_uart0_rx	
#define PIN15_M0_UART0_CTS              PIN_MODE_30    // |	m0_uart0_cts_n	
#define PIN15_I2S0_TX_MCLK              PIN_MODE_31    // |	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe> 
#define PIN15_I2S0_TX_SCLK              PIN_MODE_32    // |	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>        
#define PIN15_I2S0_RX_MCLK              PIN_MODE_33    // |	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>       
#define PIN15_I2S0_RX_SCLK              PIN_MODE_34    // |	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>    
#define PIN15_I2SPCM0_TX_OE             PIN_MODE_35    // |	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>
#define PIN15_I2SPCM0_RX_OE             PIN_MODE_36    // |	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>
#define PIN15_I2SPCM0_TX                PIN_MODE_37    // |	i2spcm0_tx_bitout                                     
#define PIN15_I2SPCM0_RX                PIN_MODE_38    // |	i2spcm0_rx_bitin                              
#define PIN15_RMT0_TXD                  PIN_MODE_39    // |	rmt0_txd	
#define PIN15_RMT0_RXD                  PIN_MODE_40    // |	rmt0_rxd	
#define PIN15_SPI0_RX_CLK               PIN_MODE_41    // |	spi0_rx_clk	
#define PIN15_SPI0_RX_DATA_MASTER       PIN_MODE_42    // |	spi0_rx_data_master
#define PIN15_SPI0_RX_DATA_SLAVE        PIN_MODE_43    // |	spi0_rx_data_slave	
#define PIN15_SPI0_RX_CHIP_SELECT       PIN_MODE_44    // |	spi0_rx_chip_select_n	
//#define PIN15_SPI1_RX_CLK              PIN_MODE_45    // |	spi1_rx_clk
#define PIN15_SPI1_RX_DATA_MASTER       PIN_MODE_46    // |	spi1_rx_data_master
#define PIN15_SPI1_RX_DATA_SLAVE        PIN_MODE_47    // |	spi1_rx_data_slave	
#define PIN15_SPI1_RX_CHIP_SELECT       PIN_MODE_48    // |	spi1_rx_chip_select_n	|



/*
|	16	
|	DPIN16	
|	PDDW0204CDG	
|	
|		
|	
|	m0_uart0_tx      
|	
|	IO<spif_si_in,spif_si_out,spif_si_out_enable>	
|	
|	
|	rmt0_tx_key[16] 	
|		
|	btbb_rxsync       
|		
|	ODIO<i2c0_sda_in,i2c0_sda_oe>  
|	mpwm_MCI0	
|	spi1_rx_data_master             
|	C_0	
|	C_1
*/

#define PIN16_M4_UART1_TX                PIN_MODE_1
#define PIN16_SPIF_SI_IN                 PIN_MODE_3

#define PIN16_RMT0_TX_KEY16              PIN_MODE_6

#define PIN16_BTBB_RXSYNC                PIN_MODE_8

#define PIN16_I2C0_SDA                   PIN_MODE_10
#define PIN16_MPWM_MCI0                  PIN_MODE_11
#define PIN16_SPI1_RX_DATA_MASTER        PIN_MODE_12
#define PIN16_IO_OUTPUT_0                PIN_MODE_14
#define PIN16_IO_OUTPUT_1                PIN_MODE_15
#define PIN16_IO_HIGH_Z_STATE            PIN_MODE_13
/*
|	17	
|	DPIN17	
|	PDDW0204CDG
|	
|		
|	rmt0_rxd   
|	m0_uart0_cts_n      
|	timer0_extin       
|	IO<spif_sck_in,spif_sck_out,spif_sck_out_enable>	
|	
|	
|	rmt0_tx_key[17] 	
|	
|	btbb_spi_miso                 
|	 
|	ODIO<i2c1_scl_in,i2c1_scl_oe>   
|	mpwm_MCI1
|	spi1_rx_data_slave           
|	C_0	
|	C_1	
*/

#define PIN17_RMT0_RXD            PIN_MODE_0 //KIP<jtag_sel0>
#define PIN17_M0_UART0_CTS        PIN_MODE_1
#define PIN17_TIMER0_EXTIN        PIN_MODE_2
#define PIN17_SPIF_SCK            PIN_MODE_3

#define PIN17_RMT0_TX_KEY17       PIN_MODE_6

#define PIN17_BTBB_SPI_MISO       PIN_MODE_8

#define PIN17_I2C1_SCL            PIN_MODE_10
#define PIN17_MPWM_MCI1           PIN_MODE_11
#define PIN17_SPI1_RX_DATA_SLAVE  PIN_MODE_12

#define PIN17_IO_OUTPUT_0         PIN_MODE_14
#define PIN17_IO_OUTPUT_1         PIN_MODE_15
#define PIN17_IO_HIGH_Z_STATE     PIN_MODE_13

/*
|	18	
|	DPIN18	
|	PDDW0204CDG	
|	
|		
|	rmt0_txd   
|	m0_uart0_rts_n          
|	timer1_extin       
|	IO<spif_hold_n_in,spif_hold_n_out,spif_hold_n_out_enable>
|	
|	
|	rmt0_tx_key[18] 
|		
|	btbb_rxd             
|	         
|	ODIO<i2c1_sda_in,i2c1_sda_oe>  
|	mpwm_MCI2
|	spi1_rx_chip_select_n       
|	C_0
|	C_1
*/

#define PIN18_RMT0_TXD           PIN_MODE_0 
#define PIN18_UART1_RTS          PIN_MODE_1
#define PIN18_TIMER1_EXTIN       PIN_MODE_2
#define PIN18_SPIF_HOLD          PIN_MODE_3

#define PIN18_RMT0_TX_KEY18      PIN_MODE_6

#define PIN18_BTBB_RXD           PIN_MODE_8

#define PIN18_I2C1_SDA           PIN_MODE_10
#define PIN18_MPWM_MCI2          PIN_MODE_11
#define PIN18_SPI1_RX_CHIP_CS          PIN_MODE_12
#define PIN18_IO_OUTPUT_0        PIN_MODE_14
#define PIN18_IO_OUTPUT_1        PIN_MODE_15
#define PIN18_IO_HIGH_Z_STATE    PIN_MODE_13
/*
|	19	
|	DPIN19
|	PDUW0204CDG
|		
|	
|	mpwm_MCOA0           
|	
|	         
|		
|	IO<icytrx_datain[8],dataout[8],dataout_en[8]>
|		
|	rmt0_tx_key[19] 
|	
|		
|	IO<i2s1_tx_mclk_i,i2s1_tx_mclk_o,i2s1_tx_mclk_oe>    
|	IO<i2s1_rx_mclk_i,i2s1_rx_mclk_o,i2s1_rx_mclk_oe>      
|	
|		
|	C_0	
|	C_1
*/
#define PIN19_MPWM_MCOA0                     PIN_MODE_0

#define PIN19_ICYTRX_DATA8                   PIN_MODE_4 

#define PIN19_RMT0_TX_KEY19                   PIN_MODE_6

#define PIN19_I2S1_TX_MCLK             PIN_MODE_9
#define PIN19_I2S1_RX_MCLK             PIN_MODE_10
#define PIN19_IO_OUTPUT_0              PIN_MODE_14
#define PIN19_IO_OUTPUT_1              PIN_MODE_15
#define PIN19_IO_HIGH_Z_STATE          PIN_MODE_13
/*
|	20	
|	DPIN20	
|	PDUW0204CDG
|		
|		
|	IO<jtag_tms0,SWDO_0,SWDOEN_0>     
|	ODIO<i2c0_scl_in,i2c0_scl_oe> 
|	
|	pad_icy_spi_clk	
|	
|	scan_out[1]  	
|	rmt0_tx_key[20] 
|		
|	mpwm_MCOA0    
|	IO<i2s1_tx_sclk_i,i2s1_tx_sclk_o,i2s1_tx_sclk_oe>  
|	IO<i2s1_rx_sclk_i,i2s1_rx_sclk_o,i2s1_rx_sclk_oe>      
|	m0_uart0_cts_n	
|	ao_mclk_to_pin     
|	C_0	
|	C_1	
|	uart0_txd    
|	uart0_rts_n   
|	uart0_rxd       
|	uart0_cts_n 
|	spwm_out	
|	spi0_tx_data
|	spi0_tx_clk	
|	spi1_tx_data	
|	spi1_tx_clk	
|	ODIO<i2c0_sda_in,i2c0_sda_oe>
|	ODIO<i2c0_scl_in,i2c0_scl_oe>
|	m0_uart0_tx	
|	m0_uart0_rts_n	
|	m0_uart0_rx
|	m0_uart0_cts_n	
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>    
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>   
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>    
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>  
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>	
|	i2spcm0_tx_bitout                                
|	i2spcm0_rx_bitin                                 
|	rmt0_txd	
|	rmt0_rxd	
|	spi0_rx_clk
|	spi0_rx_data_master
|	spi0_rx_data_slave	
|	spi0_rx_chip_select_n
|	spi1_rx_clk	
|	spi1_rx_data_master
|	spi1_rx_data_slave	
|	spi1_rx_chip_select_n
*/

#define PIN20_JTAG_SDO              PIN_MODE_0
#define PIN20_IOC0_SCL              PIN_MODE_1
#define PIN20_PAD_ICY_SPI_CLK       PIN_MODE_3

#define PIN20_SCAN_OUT              PIN_MODE_5
#define PIN20_RMT0_TX_KEY20         PIN_MODE_6

#define PIN20_MPWM_MCOA0            PIN_MODE_8

#define PIN20_I2S1_TX_SCLK          PIN_MODE_9  
#define PIN20_I2S1_RX_SCLK          PIN_MODE_10 
#define PIN20_UART1_CTS             PIN_MODE_11	   

#define PIN20_AO_MCLK_TO_PIN        PIN_MODE_12
#define PIN20_IO_OUTPUT_0           PIN_MODE_14
#define PIN20_IO_OUTPUT_1           PIN_MODE_15
#define PIN20_IO_HIGH_Z_STATE       PIN_MODE_13

#define PIN20_UART0_TXD             PIN_MODE_16
#define PIN20_UART0_RTS             PIN_MODE_17
#define PIN20_UART0_RXD             PIN_MODE_18
#define PIN20_UART0_CTS             PIN_MODE_19
#define PIN20_SPWM_OUT              PIN_MODE_20

//#define PIN20_UART0_RTS             PIN_MODE_17
//#define PIN20_UART0_RXD             PIN_MODE_18
//#define PIN20_UART0_CTS             PIN_MODE_19



/*
|	21	
|	DPIN21*B	
|	PDDW0204CDG	
|		
|		
|	mpwm_MCOA1        
|	IO<gpio1_in[7],gpio1_out[7],gpio1_ena[7]>    
|	uart1_rts_n                      
|	m0_uart1_rts_n	
|	IO<m0gpio_in[5],m0gpio_out[5],m0gpio_outen[5]>    
|	IO<icytrx_datain[2],dataout[2],dataout_en[2]>	
|	rmt0_tx_key[21] 	
|	IO<spif_so_in,spif_so_out,spif_so_out_enable>       
|		
|	IO<i2spcm1_tx_oe_bit_vld,i2spcm1_tx_bitout_vld,i2s1_tx_ws_oe>	
|	IO<i2spcm1_rx_oe_bit_vld,i2spcm1_rx_bitout_vld,i2s1_rx_ws_oe>	
|		
|		
|
*/
#define PIN21_MPWM_MCOA1 PIN_MODE_0
//#define  PIN21_GPIO1_PIN7                   	PIN_MODE_1
//#define  PIN21_UART1_RTS	                    PIN_MODE_2
//#define  PIN21_M0_UART1_RTS		  				      PIN_MODE_3
#define PIN21_ICYTRX_DATA2 PIN_MODE_4 //IO<m0gpio_in[5],m0gpio_out[5],m0gpio_outen[5]>
#define PIN21_SCAN_IN7 PIN_MODE_5
#define PIN21_RMT0_TX_KEY21 PIN_MODE_6
#define PIN21_SPIF_SO PIN_MODE_7		//  IO<spif_so_in,spif_so_out,spif_so_out_enable>
#define PIN21_I2SPCM1_TX_OE PIN_MODE_9	//	IO<i2spcm1_tx_oe_bit_vld,i2spcm1_tx_bitout_vld,i2s1_tx_ws_oe>
#define PIN21_I2SPCM1_RX_OE PIN_MODE_10 //  IO<i2spcm1_rx_oe_bit_vld,i2spcm1_rx_bitout_vld,i2s1_rx_ws_oe>
//#define  PIN21_SPIF_SO												PIN_MODE_11   //IO<spif_so_in,spif_so_out,spif_so_out_enable>
#define PIN21_AO_PDM_DATA_SOURCE PIN_MODE_12
#define PIN21_IO_OUTPUT_0 PIN_MODE_14
#define PIN21_IO_OUTPUT_1 PIN_MODE_15
#define PIN21_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	22	
|	DPIN22*B
|	PDDW0204CDG	
|		
|		
|	mpwm_MCOB1         
|	cpu0_TRACECLK                  
|	uart1_rxd                    
|	m0_uart1_tx	
|	IO<m0gpio_in[6],m0gpio_out[6],m0gpio_outen[6]>     
|	IO<icytrx_datain[3],dataout[3],dataout_en[3]>	
|	rmt0_tx_key[22] 	
|	IO<spif_si_in,spif_si_out,spif_si_out_enable>     
|	mpwm_MCOA1                   
|	i2spcm1_tx_bitout           
|	can0_rx                       
|	
|	
|
*/

#define PIN22_MPWM_MCOB1 PIN_MODE_0
//#define  PIN22_CPU0_TRACECLK                   	PIN_MODE_1
//#define  PIN22_UART1_RXD		                    PIN_MODE_2
//#define  PIN22_M0_UART1_TX			  				      PIN_MODE_3
#define PIN22_ICYTRX_DATA3 PIN_MODE_4 //IO<m0gpio_in[6],m0gpio_out[6],m0gpio_outen[6]>
#define PIN22_SCAN_IN8 PIN_MODE_5
#define PIN22_RMT0_TX_KEY22 PIN_MODE_6
#define PIN22_SPIF_SI PIN_MODE_7 //  IO<spif_si_in,spif_si_out,spif_si_out_enable>
#define PIN22_MPWM_MCOA1 PIN_MODE_8
#define PIN22_I2SPCM1_TX_BITOUT PIN_MODE_9
//#define  PIN22_CAN0_RX		  	                  PIN_MODE_10
//#define  PIN22_SPIF_CS												PIN_MODE_11   //IO<spif_cs_n_in,spif_cs_n_out,spif_cs_n_out_enable>
#define PIN22_VA_PDM_LEFT_DATA_IN PIN_MODE_12
#define PIN22_IO_OUTPUT_0 PIN_MODE_14
#define PIN22_IO_OUTPUT_1 PIN_MODE_15
#define PIN22_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	23	
|	DPIN23*B	
|	PDDW0204CDG	
|		
|		
|	mpwm_MCOA2  
|	cpu0_TRACEDATA[0]                 
|	uart1_cts_n                 
|	pdm_lclk	
|	IO<m0gpio_in[7],m0gpio_out[7],m0gpio_outen[7]>   
|	IO<icytrx_datain[4],dataout[4],dataout_en[4]>	
|	rmt0_tx_key[23] 
|	IO<spif_wp_n_in,spif_wp_n_out,spif_wp_n_out_enable>      
|	
|	i2spcm1_rx_bitin          
|	can0_tx               
|	va_pdm_clk
|	
|
*/
#define PIN23_MPWM_MCOA2 PIN_MODE_0
//#define  PIN23_CPU0_TRACEDATA0                  PIN_MODE_1
//#define  PIN23_UART1_CTS		                    PIN_MODE_2
#define PIN23_PDM_LCLK PIN_MODE_3
#define PIN23_ICYTRX_DATA4 PIN_MODE_4 // IO<m0gpio_in[7],m0gpio_out[7],m0gpio_outen[7]>
#define PIN23_SCAN_IN9 PIN_MODE_5
#define PIN23_RMT0_TX_KEY23 PIN_MODE_6
#define PIN23_SPIF_WP PIN_MODE_7 //  IO<spif_si_in,spif_si_out,spif_si_out_enable>
#define PIN23_I2SPCM1_RX_BITIN PIN_MODE_9
//#define  PIN23_CAN0_TX		  	                  PIN_MODE_10
//#define  PIN23_VA_PDM_CLK		  	                PIN_MODE_11
#define PIN23_VA_PDM_RIGHT_DATA_IN PIN_MODE_12
#define PIN23_IO_OUTPUT_0 PIN_MODE_14
#define PIN23_IO_OUTPUT_1 PIN_MODE_15
#define PIN23_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	24	
|	DPIN24*B	
|	PDDW0204CDG	
|		
|	
|	mpwm_MCOB2     
|	cpu0_TRACEDATA[1]        
|	dualtimers_TIMCLKEN1      
|	pdm_rclk	
|	IO<m0gpio_in[8],m0gpio_out[8],m0gpio_outen[8]>    
|	IO<icytrx_datain[5],dataout[5],dataout_en[5]>	
|	rmt0_tx_key[24] 
|	IO<spif_hold_n_in,spif_hold_n_out,spif_hold_n_out_enable> 	
|	mpwm_MCOA2                 
|	qdec0_b            
|		
|	va_pdm_clk
|		
|
*/

#define PIN24_MPWM_MCOB2 PIN_MODE_0
//#define  PIN24_CPU0_TRACEDATA1                  PIN_MODE_1
#define PIN24_DUALTIMERS_TIMCLKEN1 PIN_MODE_2
#define PIN24_PDM_RCLK PIN_MODE_3
#define PIN24_ICYTRX_DATA5 PIN_MODE_4 // IO<m0gpio_in[8],m0gpio_out[8],m0gpio_outen[8]>
//#define  PIN24_ICYTRX_DATA5				 							PIN_MODE_5
#define PIN24_RMT0_TX_KEY24 PIN_MODE_6
#define PIN24_SPIF_HOLD PIN_MODE_7 //  IO<spif_hold_n_in,spif_hold_n_out,spif_hold_n_out_enable>
#define PIN24_MPWM_MCOA2 PIN_MODE_8
#define PIN24_QDEC0_B PIN_MODE_9
#define PIN24_RMT_RXD_ANA PIN_MODE_10

//#define  PIN24_VA_PDM_CLK		  	                PIN_MODE_11
#define PIN24_TEST_CLK_OUT PIN_MODE_12
#define PIN24_IO_OUTPUT_0 PIN_MODE_14
#define PIN24_IO_OUTPUT_1 PIN_MODE_15
#define PIN24_IO_HIGH_Z_STATE PIN_MODE_13

/*
|	25
|	DPIN25*B	
|	PDDW0204CDG
|		
|	
|	spwm_out        
|	cpu0_TRACEDATA[2]          
|	dualtimers_TIMCLKEN2     
|	pdm0_pdm	
|	IO<m0gpio_in[9],m0gpio_out[9],m0gpio_outen[9]>    
|	IO<icytrx_datain[6],dataout[6],dataout_en[6]>
|	rmt0_tx_key[25] 
|	               
|	spwm_out_n         
|	qdec0_index      
|	rmt0_rxd                         	
|	va_pdm_ld	
|		
|
*/
#define PIN25_SPWM_OUT PIN_MODE_0
//#define  PIN25_CPU0_TRACEDATA2                  PIN_MODE_1
#define PIN25_DUALTIMERS_TIMCLKEN2 PIN_MODE_2
#define PIN25_PDM0_PDM PIN_MODE_3
#define PIN25_ICYTRX_DATA6 PIN_MODE_4 // IO<m0gpio_in[9],m0gpio_out[9],m0gpio_outen[9]>
//#define  PIN25_ICYTRX_DATA6				 							PIN_MODE_5
#define PIN25_RMT0_TX_KEY25 PIN_MODE_6
#define PIN25_SPWM_OUT_N PIN_MODE_8
#define PIN25_QDEC0_INDEX PIN_MODE_9
#define PIN25_RMT0_RXD PIN_MODE_10
#define PIN25_VA_PDM_LD PIN_MODE_11
#define PIN25_VA_PDM_LEFT_CLK PIN_MODE_12
#define PIN25_IO_OUTPUT_0 PIN_MODE_14
#define PIN25_IO_OUTPUT_1 PIN_MODE_15
#define PIN25_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	26	
|	DPIN26*B	
|	PDDW0204CDG
|		
|		
|	spwm_out_n     
|	cpu0_TRACEDATA[3]              
|	timer10_extin              
|	pdm1_pdm	
|	IO<m0gpio_in[10],m0gpio_out[10],m0gpio_outen[10]>     
|	IO<icytrx_datain[7],dataout[7],dataout_en[7]>	
|	rmt0_tx_key[26] 
|	                
|	
|	qdec0_a       
|	rmt0_txd           
|	va_pdm_rd	
|		
|
*/
#define PIN26_SPWM_OUT_N PIN_MODE_0
//#define  PIN26_CPU0_TRACEDATA3                  PIN_MODE_1
//#define  PIN26_TIMER10_EXTIN					          PIN_MODE_2
#define PIN26_PDM1_PDM PIN_MODE_3
#define PIN26_ICYTRX_DATA7 PIN_MODE_4 // IO<m0gpio_in[10],m0gpio_out[10],m0gpio_outen[10]>
//#define  PIN26_ICYTRX_DATA7				 							PIN_MODE_5
#define PIN26_RMT0_TX_KEY26 PIN_MODE_6
#define PIN26_QDEC0_A PIN_MODE_9
#define PIN26_RMT0_TXD PIN_MODE_10
#define PIN26_VA_PDM_RD PIN_MODE_11
#define PIN26_VA_PDM_RIGHT_CLK PIN_MODE_12
#define PIN26_IO_OUTPUT_0 PIN_MODE_14
#define PIN26_IO_OUTPUT_1 PIN_MODE_15
#define PIN26_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	27	
|	DPIN27	
|	PDUW0204CDG	
|		
|		
|	spi1_rx_clk          
|	
|	IO<flash_tst_pi[5],flash_tst_po[5],flash_tst_poe[5]>	
|	pad_icy_spi_mosi	
|	
|		
|	rmt0_tx_key[0]  
|	uart0_cts_n	
|	spi1_tx_clk       
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>	
|	pdm_lclk
|	spwm_out    
|	mpwm_MCOB1	
|	C_0
|	C_1	
|	uart0_txd    
|	uart0_rts_n    
|	uart0_rxd          
|	uart0_cts_n 
|	spwm_out
|	spi0_tx_data	
|	spi0_tx_clk	
|	spi1_tx_data	
|	spi1_tx_clk	
|	ODIO<i2c0_sda_in,i2c0_sda_oe> 
|	ODIO<i2c0_scl_in,i2c0_scl_oe>	
|	m0_uart0_tx	
|	m0_uart0_rts_n	
|	m0_uart0_rx
|	m0_uart0_cts_n	
|	IO<i2s0_tx_mclk_i,i2s0_tx_mclk_o,i2s0_tx_mclk_oe>          
|	IO<i2s0_tx_sclk_i,i2s0_tx_sclk_o,i2s0_tx_sclk_oe>         
|	IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>        
|	IO<i2s0_rx_sclk_i,i2s0_rx_sclk_o,i2s0_rx_sclk_oe>        
|	IO<i2spcm0_tx_oe_bit_vld,i2spcm0_tx_bitout_vld,i2s0_tx_ws_oe>	
|	IO<i2spcm0_rx_oe_bit_vld,i2spcm0_rx_bitout_vld,i2s0_rx_ws_oe>
|	i2spcm0_tx_bitout                           
|	i2spcm0_rx_bitin                     
|	rmt0_txd	
|	rmt0_rxd	
|	spi0_rx_clk	
|	spi0_rx_data_master
|	spi0_rx_data_slave	
|	spi0_rx_chip_select_n	
|	spi1_rx_clk	
|	spi1_rx_data_master	
|	spi1_rx_data_slave	
|	spi1_rx_chip_select_n	
*/
#define PIN27_SPI1_RX_CLK                      PIN_MODE_0
#define PIN15_FLASH_TST_PIN5                   PIN_MODE_2
#define PIN27_PAD_ICY_SPI_MOSI                 PIN_MODE_3 

#define PIN27_RMT0_TX_KEY0                     PIN_MODE_6
#define PIN27_UART0_CTS_N                      PIN_MODE_7
#define PIN27_SPI1_TX_CLK                      PIN_MODE_8
#define PIN27_I2S0_TX_MCLK                     PIN_MODE_9
#define PIN27_PDM_LCLK               PIN_MODE_10
#define PIN27_SPWM_OUT 							 PIN_MODE_11
#define PIN27_MPWM_MCOB1             PIN_MODE_12
#define PIN27_IO_OUTPUT_0            PIN_MODE_14
#define PIN27_IO_OUTPUT_1            PIN_MODE_15
#define PIN27_IO_HIGH_Z_STATE        PIN_MODE_13

#define PIN27_UART0_TXD                      PIN_MODE_16

/*
|	28	
|	DPIN28
|	PDDW0204CDG	
|		
|		
|	spi1_rx_data_master    
|	IO<m0gpio_in[1],m0gpio_out[1],m0gpio_outen[1]> 
|	can0_rx                  
|	
|	IO<m0gpio_in[12],m0gpio_out[12],m0gpio_outen[12]>    
|	                       
|	rmt0_tx_key[1]  
|	              
|	spi1_rx_data_slave                  
|	uart1_rts_n                 
|	pdm_rclk	
|		
|	
|
*/
#define PIN28_SPI1_RX_DATA_MASTER PIN_MODE_0
//#define  PIN28_M0_GPIO_PIN1		                  PIN_MODE_1    //IO<m0gpio_in[1],m0gpio_out[1],m0gpio_outen[1]>
//#define  PIN28_CAN0_RX								          PIN_MODE_2
//#define  PIN28_M0_GPIO_PIN12		  				     	PIN_MODE_4    // IO<m0gpio_in[12],m0gpio_out[12],m0gpio_outen[12]>
#define PIN28_RMT0_TX_KEY1       PIN_MODE_6
#define PIN28_SPI1_RX_DATA_SLAVE PIN_MODE_8
#define PIN28_I2S0_RX_MCLK       PIN_MODE_9 //IO<i2s0_rx_mclk_i,i2s0_rx_mclk_o,i2s0_rx_mclk_oe>


#define PIN28_PDM_RCLK           PIN_MODE_10
#define PIN28_SPWM_OUT_N				 PIN_MODE_11
#define PIN28_MPWM_MCOA2         PIN_MODE_12
#define PIN28_IO_OUTPUT_0        PIN_MODE_14
#define PIN28_IO_OUTPUT_1        PIN_MODE_15
#define PIN28_IO_HIGH_Z_STATE    PIN_MODE_13
/*
|	29	
|	DPIN29	
|	PDUW0204CDG
|		
|		
|	spi1_rx_chip_select_n       
|	qdec0_b                    
|	IO<flash_tst_pi[7],flash_tst_po[7],flash_tst_poe[7]>
|	
|	IO<icytrx_datain[3],dataout[3],dataout_en[3]>	
|	                               
|	rmt0_tx_key[2]  
|	
|	
|	btco_ble_sync
|	pdm0_pdm
|	va_pdm_ld	
|	mpwm_MCOB2
|	C_0	
|	C_1	

*/
#define PIN29_SPI1_RX_CHIP_SELECT PIN_MODE_0
#define PIN29_QDEC0_B PIN_MODE_1
//#define  PIN29_CAN0_TX								          PIN_MODE_2
//#define  PIN29_M0_GPIO_PIN13		  				     	PIN_MODE_4    // IO<m0gpio_in[13],m0gpio_out[13],m0gpio_outen[13]>
#define PIN29_RMT0_TX_KEY2 PIN_MODE_6
//#define  PIN29_UART1_RXD				    						PIN_MODE_9

#define PIN29_SPIF_WP            PIN_MODE_8 //IO<spif_wp_n_in,spif_wp_n_out,spif_wp_n_out_enable>
#define PIN29_PDM0_PDM           PIN_MODE_10
#define PIN29_VA_PDM_LD          PIN_MODE_11
#define PIN29_MPWM_MCOB2         PIN_MODE_12
#define PIN29_IO_OUTPUT_0 PIN_MODE_14
#define PIN29_IO_OUTPUT_1 PIN_MODE_15
#define PIN29_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	30
|	DPIN30
|	PDDW0204CDG	
|		
|	
|	spi1_tx_clk   
|	qdec0_index              
|	rmt0_rxd               
|		
|	IO<m0gpio_in[14],m0gpio_out[14],m0gpio_outen[14]>      
|	IO<icytrx_datain[8],dataout[8],dataout_en[8]>	
|	rmt0_tx_key[3]  
|	            
|	
|	uart1_cts_n       
|	pdm1_pdm	
|	va_pdm_rd	
|	
|
*/

#define PIN30_SPI1_TX_CLK PIN_MODE_0
#define PIN30_QDEC0_INDEX PIN_MODE_1
#define PIN30_RMT0_RXD PIN_MODE_2
#define PIN30_ICYTRX_DATA8 PIN_MODE_4 // IO<m0gpio_in[14],m0gpio_out[14],m0gpio_outen[14]>
//#define  PIN30_ICYTRX_DATA8			              	PIN_MODE_5
#define PIN30_RMT0_TX_KEY3 PIN_MODE_6
#define PIN30_SPIF_SO PIN_MODE_8 //IO<spif_so_in,spif_so_out,spif_so_out_enable>
#define PIN30_UART1_CTS PIN_MODE_9
#define PIN30_PDM1_PDM PIN_MODE_10
#define PIN30_VA_PDM_RD PIN_MODE_11
#define PIN30_IO_OUTPUT_0 PIN_MODE_14
#define PIN30_IO_OUTPUT_1 PIN_MODE_15
#define PIN30_IO_HIGH_Z_STATE PIN_MODE_13
/*
|	31	
|	DPIN31	
|	PDDW0204CDG	
|		
|	
|	spi1_tx_data    
|	qdec0_a          
|	rmt0_txd       
|		
|	IO<m0gpio_in[15],m0gpio_out[15],m0gpio_outen[15]>      
|	IO<icytrx_datain[9],dataout[9],dataout_en[9]>	
|	rmt0_tx_key[31] 
|	               
|	
|	
|	pdm_lclk
|	va_pdm_clk	
|		
|
*/

#define PIN31_SPI1_TX_DATA PIN_MODE_0
#define PIN31_QDEC0_A PIN_MODE_1
#define PIN31_RMT0_TXD PIN_MODE_2
#define PIN31_ICYTRX_DATA9 PIN_MODE_4 // IO<m0gpio_in[15],m0gpio_out[15],m0gpio_outen[15]>
//#define  PIN31_ICYTRX_DATA9			              	PIN_MODE_5
#define PIN31_RMT0_TX_KEY31 PIN_MODE_6

#define PIN31_SPIF_CS PIN_MODE_8 //IO<spif_cs_n_in,spif_cs_n_out,spif_cs_n_out_enable>
#define PIN31_PDM_LCLK PIN_MODE_10
//#define  PIN31_VA_PDM_CLK			  	              PIN_MODE_11
#define PIN31_IO_OUTPUT_0 PIN_MODE_14
#define PIN31_IO_OUTPUT_1 PIN_MODE_15
#define PIN31_IO_HIGH_Z_STATE PIN_MODE_13

#define COBRA_PINMUX_BASE (0x40089000UL) //cobra4   2019/8/5

void cobra_pinmux_init(void);
void cobra_map_pinmodeset(PINMUX_INDEX pin, PIN_MODE mode);

void cobra_pin_input_enable(PINMUX_INDEX pin, unsigned int en);
void cobra_pin_pullup_enable(PINMUX_INDEX pin, unsigned int en);

void cobra_pin_level_detect(PINMUX_INDEX pin, unsigned int level);
void cobra_pin_level_detect_enable(PINMUX_INDEX pin, unsigned int en);
void cobra_pin_drain_enable(PINMUX_INDEX pin, unsigned int en);

void pinmux_debug_information(void);
void cobra_pin_drain_source_enable(PINMUX_INDEX  pin,  unsigned int enbale);

unsigned int cobra_pin_get_pin_mode(PINMUX_INDEX pin);

unsigned int cobra_pin_level_detect_get(PINMUX_INDEX pin);

#if (COBRA_LIGHT_WS2812B_ENABLE == 1)
inline void cobra_ws2812_set_high(void);
inline void cobra_ws2812_set_low(void);
void app_light_ws2812b_light_display(uint32_t data);
#endif	

#endif //COBRA_PINMUX_EN

#endif
