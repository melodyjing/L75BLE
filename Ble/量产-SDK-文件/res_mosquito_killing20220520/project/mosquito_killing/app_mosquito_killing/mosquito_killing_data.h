
#ifndef _MOSQUITO_KILLING_DATA_H
#define _MOSQUITO_KILLING_DATA_H

#include "app_config.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "cobra_system.h"

#include "mosquito_killing_protocol.h"

/*eflash erase ��С��ԪΪ4K,���Էֶδ�ʱ����С��4K*/
/*��mac/ip addr ��pid��������Ҫ�޸ĵĲ������ڵ�һ�Σ����ò����ŵ��ڶ���*/

#define   M4_EFLASH_MAC_PID_ADDRESS  (0x50000 - 0x4000)
/*ע��nvds��ַ�����ã���Ҫ���ظ���*/
//#define   M4_EFLASH_NVDS_ADDRESS	(0x50000 - 0x2000)

#define  APP_MOSQUITO_KILLING_FLASH_MAC_PID_SIZE  0x1000

#define  APP_MOSQUITO_KILLING_FLASH_READ_ADDR_OFFSET  (0x00800000)

/*����120���µ�ͳ������,�ټ�һ�����û��ͬ��ʱ��ļ���������ʱ��ͬ����ֱ��д��ʱ���Ӧ���·�*/
/*������4�ֽڶ��룬���Լ��������ֶΣ�ÿ���ֽ������ֽ�*/
#define  APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX  (120+2)
//#define  APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX  (6+2)

#define  APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX  0xffff

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
#define FLASH_SETTING_DATA_START_ADDR  (M4_EFLASH_MAC_PID_ADDRESS + APP_MOSQUITO_KILLING_FLASH_MAC_PID_SIZE)
#define FLASH_SETTING_DATA_BACKUP_START_ADDR  (M4_EFLASH_MAC_PID_ADDRESS - 0x1000)

#define FLASH_SETTING_CRC_HEADER_SZIE  (8)    /*4bytes magic number, 4bytes crc code*/

#define FLASH_SETTING_MAGIC_NUMBER  0x118855AA
#define FLASH_SETTING_ALARM_DATA_ADDR  (FLASH_SETTING_DATA_START_ADDR + FLASH_SETTING_CRC_HEADER_SZIE)

/*ÿ���Զ����ض�ʱ����Ҫ8���ֽ�,��Ҫ��ʱ��ÿ��ʱ���һ��*/
#define MAX_TIMING_NUM       (1)
//#define TIMING_SETTING_SIZE  (8*MAX_TIMING_NUM)  /*ÿ���Զ����ض�ʱ����Ҫ8���ֽ�*/
#define TIMING_SETTING_SIZE  (sizeof(mosquito_killing_timing_t)*MAX_TIMING_NUM) 
#define STATISTICS_MONTH_SZIE (sizeof(uint16_t)*APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX)
#define SAVE_DATE_SIZE  (sizeof(mosquito_killing_date_t))
#define SAVE_BT_CONNECT_COUNT_SIZE  (sizeof(uint32_t))
#define SETTING_BAK_SIZE (sizeof(mosquito_killing_bak_t))
#define YEAR_BASE_SIZE (sizeof(mosquito_killing_year_base_t))

#define FLASH_SETTING_SIZE  (TIMING_SETTING_SIZE+STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE+SAVE_BT_CONNECT_COUNT_SIZE+SETTING_BAK_SIZE+YEAR_BASE_SIZE)


void app_mosquito_killing_setting_data_default_setting(void);
void app_mosquito_killing_setting_data_load(void);
uint8_t app_mosquito_killing_setting_data_read(uint32_t startAddr);
void app_mosquito_killing_setting_data_write(uint32_t startAddr);
void app_mosquito_killing_setting_data_erase(void);
#endif

extern uint32_t app_mosquito_killing_bt_connect_count;
extern uint8_t app_mosquito_killing_curr_month;
extern uint32_t app_mosquito_killing_PhaseValue ;
extern uint16_t app_mosquito_killing_month_count[];
extern mosquito_killing_date_t  app_mosquito_killing_save_date;
extern uint8_t app_mosquito_killing_bt_addr[];
extern uint8_t app_mosquito_killing_pid[];
extern uint8_t  app_mosquito_killing_password[];
extern uint8_t  app_mosquito_killing_name[];
extern mosquito_killing_bak_t  app_mosquito_killing_setting_bak;

extern uint8_t app_mosquito_killing_work_state;
extern mosquito_killing_year_base_t app_mosquito_killing_year_base;

void app_mosquito_killing_mac_pid_default_setting(void);
void app_mosquito_killing_mac_pid_load(void);
uint8_t app_mosquito_killing_mac_pid_write(void);
void app_mosquito_killing_mac_pid_erase(void);

#endif  /* APP_MOSQUITO_KILLING_ENABLE */

#endif  /* _MOSQUITO_KILLING_DATA_H */
