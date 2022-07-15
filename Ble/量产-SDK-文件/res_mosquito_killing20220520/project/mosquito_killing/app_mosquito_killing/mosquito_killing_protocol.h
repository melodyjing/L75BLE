#ifndef _MOSQUITO_KILLING_PROTOCOLC_H
#define _MOSQUITO_KILLING_PROTOCOLC_H



#include "app_config.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "cobra_system.h"

#define APP_MOSQUITO_KILLING_TIMING_ON  0x01
#define APP_MOSQUITO_KILLING_TIMING_OFF  0x00

/*read--send data to apk
 write--receive data from apk*/
#define  APP_MOSQUITO_KILLING_CMD_TYPE_READ      0x52
#define  APP_MOSQUITO_KILLING_CMD_TYPE_WRITE     0x57
#define  APP_MOSQUITO_KILLING_CMD_TYPE_RETURN    0x53

#define  APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING  0x00
#define  APP_MOSQUITO_KILLING_CMD_INFO  0x01
#define  APP_MOSQUITO_KILLING_CMD_DATE_TIME       0x02
#define  APP_MOSQUITO_KILLING_CMD_ON_OFF          0x03
#define  APP_MOSQUITO_KILLING_CMD_COUNT           0x05
#define  APP_MOSQUITO_KILLING_CMD_TIMING          0x40

#define  APP_MOSQUITO_KILLING_CMD_IP_WR          0xE0
#define  APP_MOSQUITO_KILLING_CMD_OTA          0xE1
#define  APP_MOSQUITO_KILLING_CMD_VERSION          0xE2

#define  APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX        0x64
//#define  APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX        0xFF

#define  APP_MOSQUITO_KILLING_HEAD_1   0xa5
#define  APP_MOSQUITO_KILLING_HEAD_2   0xaa
#define  APP_MOSQUITO_KILLING_HEAD_3   0xac

#define  APP_MOSQUITO_KILLING_RAIL_1   0xc5
#define  APP_MOSQUITO_KILLING_RAIL_2   0xcc
#define  APP_MOSQUITO_KILLING_RAIL_3   0xca

#define  APP_MOSQUITO_KILLING_PRODUCT_TYPE   0xc2

#define  APP_MOSQUITO_KILLING_SEND_MTU  20

/*10���ֽ�*/
#define  APP_MOSQUITO_KILLING_PROTOCOL_NUM_MIN 10
/*500ms*/
//#define  APP_MOSQUITO_KILLING_PROTOCOL_CMD_EXCUTE_INTERVAL_MAX 500
#define  APP_MOSQUITO_KILLING_PROTOCOL_CMD_EXCUTE_INTERVAL_MAX 1000

typedef struct
{
    uint8_t head[3];

    uint8_t checksum;
    uint8_t cmd_type;
    uint8_t product_type;
    uint8_t cmd;
    uint8_t param[50];

    uint8_t rail[3];

    uint8_t valid;
} mosquito_killing_data_t;


typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t week;
} mosquito_killing_date_t;

// To be used with
typedef struct
{
    uint16_t year;    // 2000+
    uint8_t month;    // 0-11
    uint8_t day;      // 0-30
    uint8_t seconds;  // 0-59
    uint8_t minutes;  // 0-59
    uint8_t hour;     // 0-23
}
UTCTimeStruct;

typedef enum
{
    MOn  = 0,
    Tues  = 1,
    Wed  = 2,
    Thur = 3,
    Fri  = 4,
    Sat  = 5,
    Sun  = 6
} DAY_OF_WEEK;

typedef struct
{
	uint8_t reserve;
	
	uint8_t mode;
	uint8_t week_day; /*������λ��Ӧһ�ܵ�ÿһ�죬0~6λΪ1��Ӧ��һ������, ���λ����λ��ʾ������*/
	
    uint8_t hour_begin;
    uint8_t min_begin;

    uint8_t hour_end;
    uint8_t min_end;

	uint8_t reserve01;  /*ռλ��4�ֽڶ��룬����eflash��д��,�ڷ��ض�ʱ����ʱҪ��ȥ���ռλ���ֽ�*/
} mosquito_killing_timing_t;

typedef struct
{
	uint8_t conn_bt_flag;  /*����һ����������д�����ʱ���ۼ�һ�μ���*/
	uint8_t time_sync_finished;  /*������ͬ��ʱ���־*/
	uint8_t timing_auto_operate_flag;  /*0=û��ʱ������1=��ʱ�����˿���2=��ʱ�����˹�*/
	uint8_t timing_key_operatoe_flag;  /*0=�ޣ�1=�� ��ʱ��Ч�ڼ��Ƿ��а��������������򱾴ζ�ʱ����ʧЧ*/
	uint8_t timing_cancel_flag_week_day;  /*0=��Ч��������λ��Ӧһ�ܵ�ÿһ�죬0~6λΪ1��Ӧ��һ������,��7λ���ڼ�¼������;��¼������Ϊ��ʱ��Ч�ڼ����ֶ���������Ҫȡ�����εĶ�ʱ����*/

	COBRA_RTC_TIME_DATE_T timging_start_time;  /*��¼��ʱ������ʱ��㣬���ڼ��㱾�ζ�ʱ��ȥ��ʱ��Σ��жϱ��ζ�ʱ�Ƿ����*/
	uint32_t timing_count_second;  /*��¼���ζ�ʱ��ʱ�䳤�ȣ�����Ϊ��λ������Ƚ�,������ʱʱ��������*/
} mosquito_killing_timing_setting_t;

/*���ڴ��watchdog��λ����Ҫϵͳ������Ϣ*/
typedef struct
{
	uint8_t bak_work_state;	
	uint8_t bak_time_sync_finished;

	uint8_t reserve;
	uint8_t reserve01;  /*ռλ��4�ֽڶ��룬����eflash��д��*/
} mosquito_killing_bak_t;

typedef struct
{
	uint16_t valid;	     /*��Ч���жϣ� 0xAA55Ϊ��Ч*/
	uint16_t year_base; /*ע�� 4�ֽڶ��룬����eflash��д��*/
} mosquito_killing_year_base_t;

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
typedef uint32_t UTCTime; /* used to store the second counts for RTC */

extern mosquito_killing_timing_t app_mosquito_killing_data_timing[];
extern mosquito_killing_timing_setting_t app_mosquito_killing_timing_setting;
#endif

extern uint8_t app_mosquito_killing_bt_addr[];

void app_mosquito_killing_bt_data_process(uint8_t cmd, uint8_t cmdType, uint8_t *pdata, uint8_t len);
void app_mosquito_killing_bt_data_receive(unsigned char *buffer, unsigned int length);
void app_mosquito_killing_bt_data_send(uint8_t cmd, uint8_t cmdType, uint8_t *pdata, uint8_t len);
void app_mosquito_killing_bt_data_init(void);
void app_mosquito_killing_receive_bt_data_softtimer_init(void);

extern uint8_t app_mosquito_killing_restore_default_setting(void);
#endif  /* APP_MOSQUITO_KILLING_ENABLE */

#endif  /* _MOSQUITO_KILLING_PROTOCOLC_H */

