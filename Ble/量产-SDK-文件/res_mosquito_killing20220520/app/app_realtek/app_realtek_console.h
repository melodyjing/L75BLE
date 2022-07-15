/************************************************************************
  Copyright (C)  RadiaWave Technology(Shenzhen) Co., Ltd.
  Project Name:      
  File name:   
Description:

Author:   LuFeng    						Date: 2021-January-18
************************************************************************/

/**************************** Edit History **********************************
DATE           	NAME             	DESCRIPTION                    *
2021-01-18     	LuFeng            Create.         
************************************************************************/

#ifndef __APP_REAL_CONSOLE_H__
#define __APP_REAL_CONSOLE_H__
#include "cobra_system.h"
#include "app_network.h"
#include "rwip_config.h"
#include "rdw_queue.h"
#include "rdw_console.h"
#include "rdw_command.h"
#include "ke_event.h"
#include "device1_connection.h"
#include "device2_connection.h"
#include "device3_connection.h"
#include "app_host2_uart.h"
#include "app_host_uart.h"
#include "device1_scan.h"
#include "host_advertiser1.h"
#include "app_autoconnect.h"
#include "app_report_adv.h"
#if(DIANBIAO_PROTOCOL_REALTEK_ENABLE == 1)
enum UART_COMMAND_TYPE
{
		UART_CMD_TRANS = 1,
	  UART_CMD_WRITE,
	  UART_CMD_READ,
	  UART_CMD_REPORT,
	
	  UART_ANSWER    = 0x84,
};
enum MODUL_INFO
{
		MODUL_RESET = 1,
	  MODUL_RESTORE,
	  MODUL_SLEEP,
};
enum UART_COMMAND_INDEX
{
		CMD_DEV_PAIR_MODE = 0xF20B0000,
	  CMD_DEV_MAC_ADDR,
	  CMD_BLE_PARAMS,
	  CMD_BLE_CONN_INFO,
		CMD_BLE_DATA_MODE   = 0x00000006,
	  CMD_MODULE_INFO,
	  CMD_FIRMWARE_VERSION,
	  CMD_SCAN,
	  CMD_FILTER_ADDR,
	  CMD_FILTER_TYPE,
	  CMD_GET_FILTER_INFO,
	  CMD_UART_PARAMS,
	  CMD_ADV_DATA,
	  CMD_RESP_DATA,
	  CMD_ADV_INFO_REPORT,
	  CMD_CLEAR_BOND_INFO,
	  CMD_CLEAR_ALL_BOND_INFO,
		CMD_DEV_NAME,
		CMD_TX_POWER,
		CMD_ADV_INTERVAL,
		CMD_DEV_ADDR,
		CMD_DEV_PAIR_MM,
		CMD_DEV_PAIR_VALUE,
		CMD_LINK_CMD,
		CMD_ADV,
		CMD_FIRMWARE_BUILD_TIME,
		CMD_LINK_RSSI,
	  CMD_PD            = 0xF20B0201,
};
#if BLE_DATA_RECEIVE_MODE
enum BLE_DATA_RECE_MODE
{
		UNDEF_DAT_RECE_MODE,
		CACHE_DAT_RECE_MODE,
		DIRECT_DAT_RECE_MODE,
		MAX_DAT_RECE_MODE,
};
#endif
enum DEVICE_OBJECT
{
		HOST_DEV      = 1<<0,
	  SLAVE_DEV1    = 1<<1,
	  SLAVE_DEV2    = 1<<2,
	  SLAVE_DEV3    = 1<<3,
};
typedef struct uart_cmd_frame{
		UCHAR length_l;
		UCHAR length_h;
	
		UCHAR mac_address[6];
		enum UART_COMMAND_TYPE type;
		UCHAR reserve[4];
		UCHAR control_cmd[4];  //?? ?
		UCHAR user_data[1024];
		USHORT checksum;
		USHORT user_length;
}UART_CMD_FRAME;

typedef struct rdw_pin_code{
	
	  UCHAR   pin_length;  
			
    UCHAR   pin_name[6];
}RDW_PIN_CODE;

enum UART_COMMAND_STATE
{
		UART_CMD_IDLE,
		UART_CMD_LOOK_PREMBLE,
		UART_CMD_LOOK_LENGTH_L,
		UART_CMD_LOOK_LENGTH_H,
		UART_CMD_LOOK_ADDRESS,
		UART_CMD_LOOK_TYPE,    //5
		UART_CMD_LOOK_RESERVE,
		UART_CMD_LOOK_SECOND_BEGIN,
		UART_CMD_LOOK_CONTROL_CMD,
		UART_CMD_LOOK_USER_DATA,
		UART_CMD_LOOK_CHECKSUM,  //10
		UART_CMD_LOOK_END,
	
};
enum UART_CMD_ERROR_CODE
{
  CMD_ERROR_NO,
	CMD_ERROR_UNDEF,
	CMD_ERROR_NO_ADDRESS,
	CMD_ERROR_CONNECT_FAIL,
	CMD_ERROR_TRANS_FAIL,
	CMD_ERROR_LINE_OVER_LIMIT,
	CMD_ERROR_REPORT_PARA_FAIL,
	CMD_ERROR_NO_PAIR_INFO          = 16,
	CMD_ERROR_NO_RAM,
	CMD_ERROR_OTHER                 =0xFF,
};
void iris_dianbiao_slave_number_rsp(void);

void iris_dianbiao_master_number_rsp(void);

void iris_uart_task_callback(void);

void app_iris_up_collection_data(UCHAR connection_index, UCHAR length, UCHAR *p_buffer);


void iris_calibration_loop(void);

void app_iris_dianbiao_uart_output(UINT8 index, UINT8 length,  UINT8 *p_buffer);

void  iris_dianbiao_console_init(void);

UCHAR * iris_uart_get_address(void);

void  realtek_dianbiao_report_init(void);

void iris_auto_reply_ack_command(void);

void selfdef_adv_and_resq_data_init(void);

bool Copy_resq_data(UINT8 *data,UINT16 *length);

bool copy_adv_data(UINT8 *data,UINT16 *length);

UINT16 Get_Filter_Adv_Info_length(void);

void Print_Filter_Adv_Info(USHORT *checksum);

void realtek_dianBiao_set_rsp(enum UART_COMMAND_INDEX index,enum UART_CMD_ERROR_CODE);

void cmd_dev_mac_addr_read_handler(void);

void cmd_dev_pair_mode_read_handler(void);

void cmd_ble_params_read_handler(void);

void cmd_scan_handler(USHORT length,UCHAR *user_data);

BOOL get_scan_enable(void);

UINT8 get_reset_state(void);

UINT16 app_device_get_scan_interval(void);

void app_device_set_can_interval(UINT16 value);

void app_device_set_adv_interval(UINT16 value);

UINT16 app_device_get_adv_interval(void);

void cobra_app_set_reset_report_task(void);
#if BLE_DATA_RECEIVE_MODE
void cmd_ble_data_rece_mode_read_handler(void);
enum BLE_DATA_RECE_MODE app_get_ble_data_rece_mode(void);
void app_set_ble_data_rece_mode(enum BLE_DATA_RECE_MODE mode);
#endif
#if CMD_DEV_NAME_FUNCTION
void cmd_device_name_handler(USHORT length,UCHAR *user_data);
void cmd_device_name_read_handler(void);
void app_set_device_name(USHORT length,UCHAR *name);
USHORT app_get_device_name(UCHAR *name);
#endif
#if CMD_TX_POWER_FUNCTION
void cmd_tx_power_handler(USHORT length,UCHAR *user_data);
void cmd_tx_power_read_handler(void);
#endif
#if CMD_ADV_INTERVAL_FUNCTION
void cmd_adv_interval_handler(USHORT length,UCHAR *user_data);
void cmd_adv_interval_read_handler(void);
#endif
#if CMD_DEV_ADDR_FUNCTION
void cmd_dev_add_handler(USHORT length,UCHAR *user_data);
void cmd_dev_addr_read_handler(USHORT length,UCHAR *user_data);
#endif
#if CMD_DEV_PAIR_MODE_FUNCTION
void cmd_dev_pair_mm_handler(USHORT length,UCHAR *user_data);
void cmd_dev_pair_mm_read_handler(USHORT length,UCHAR *user_data);
#endif
#if CMD_DEV_PAIR_VALUE_FUNCTION
void cmd_dev_pair_value_handler(USHORT length,UCHAR *user_data);
void cmd_dev_pair_value_read_handler(USHORT length,UCHAR *user_data);
#endif
#if CMD_ADV_FUNCTION
void cmd_adv_handler(USHORT length,UCHAR *user_data);
BOOL get_adv_enable(void);
#endif
#if CMD_FIRMWARE_BUILD_TIME_FUNCTION
void cmd_fw_build_time_read_handler(void);
#endif
#if CMD_LINK_CMD_FUNCTION
void cmd_link_cmd_handler(USHORT length,UCHAR *user_data);
#endif
#if CMD_LINK_RSSI_FUNCTION
void cmd_link_rssi_read_handler(USHORT length,UCHAR *user_data);
#endif
#if BLE_DATA_RECEIVE_MODE

struct sch_realtek_rece_elt_tag
{
    /// List element for chaining in the realtek receive lists
    struct co_list_hdr hdr;

    /// max number of element in the list
    UINT8 conidx;
    /// min number of element in the list
    UINT8  rx_data[255];
	
	  UINT8  length;

};
struct sch_realtek_rece_env_tag
{
    /// List of pending element to program
    struct co_list elt_wait;
    /// element programmed
    struct sch_realtek_rece_elt_tag *elt_prog;
    /// List of element canceled
    struct co_list elt_canceled;
		
	  /// number of element in the list
    uint32_t cnt;
};
bool realtek_malloc_elt(void);
void realtek_client_rx_data_handle(UINT8 conidx,struct uart_client_rx_data_ind  CONST *param);
void realtek_slave_rx_data_handle(UINT8 conidx,struct device1_uart_rx_data  CONST *param);
#endif
#endif
#endif
