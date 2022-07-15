/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	  app_at_cmd.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/11/2

	Description :  AT command
	Changed Log	:

		Liujian  2020/11/2		- Creation
*****************************************************************************/

#include "app_at_cmd.h"
#include "cobra_uart.h"
#include "rdw_queue.h"
#include "cobra_watchdog.h"
#include "app.h"
#include "app_meter_common.h"

#if (COBRA_LOGGER_ENABLE == 1)	
#include <elog.h>
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#include "master_scan.h"
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))	
#include "app_host1.h"
#endif

#if((APP_AT_COMMAND_ENABLE == 1) && (APP_RDW_SHELL_ENABLE == 0))

static QUEUE64_TYPE   at_cmd_uart_buffer;

static struct cat_object  cobra_at;

/* working buffer */
static unsigned char work_buf[128];

static unsigned char ble_address[7];

static uint8_t channel;
static uint8_t ble_send_length;
static uint8_t ble_send_data[256];

static uint8_t scan_state;

static uint8_t disconnection_channel;

static const char ble_dev_name[5];

#if (APP_METER_AT_COMMAND == 1)

/////////////////////////////////////////////////////////////////////////////////////////
static int meter_reboot_run(const struct cat_command *cmd)
{
#if (COBRA_LOGGER_ENABLE == 1)	
	      log_i("+OK");
#endif	
	      app_cobra_watchdog_init(100, WATCHDOG_RESET);
	      while(1)
				{
					
				}
        return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////
//scan  function 


static struct cat_variable meter_scan_vars[] = {
        {
                .type = CAT_VAR_UINT_DEC,
                .data = &scan_state,
                .data_size = sizeof(scan_state),
                .name = "scan cmd"
        },
};


static int meter_scan_write(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, const size_t args_num)
{
	      if(scan_state)
				{
					  master_scan_set_state(1);
				}
				else
				{
					  master_scan_set_state(0);
				}
        return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////

static struct cat_variable meter_connect_vars[] = {
        {
                .type = CAT_VAR_BUF_HEX,
                .data = &ble_address,
                .data_size = sizeof(ble_address),
                .name = "connect cmd"
        },
};

/*
连接蓝牙设备
*/
static int meter_connect_write(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, const size_t args_num)
{
	     int i;
	
	     app_set_connect_device1_address(ble_address);
	     app_device_connect_to_duanluqi_1_enable(1);
	     printf("connect to \n");
	     for(i = 0; i < 6; i++)
	     {
				   printf("0x%02x ", ble_address[i]);
			 }
			 printf("\n");
	     return 0;
}

/*
查询连接信息
*/
static int meter_channel_info(const struct cat_command *cmd)
{
        app_device_show_connect_info();
        return 0;
}

static struct cat_variable meter_send_vars[] = {
        {
                .type = CAT_VAR_UINT_DEC,
                .data = &channel,
                .data_size = sizeof(channel),
                .name = "CHANNEL"
        },
        {
                .type = CAT_VAR_UINT_DEC,
                .data = &ble_send_length,
                .data_size = sizeof(ble_send_length),
                .name = "BLE_SEND_LENGTH"
        },
        {
                .type = CAT_VAR_BUF_STRING,
                .data = ble_send_data,
                .data_size = sizeof(ble_send_data),
                .name = "BLE_SEND_DATA"
        }
};

/*
发送数据

*/
static int meter_ble_send_data(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, const size_t args_num)
{
	     printf("channel = %d , length = %d \n", channel, ble_send_length);
	     printf("%s \n", ble_send_data);
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))	
	     app_send_data_to_device(channel, ble_send_length, ble_send_data);
#endif
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 1))	
      app_meter_send_data_to_master(ble_send_length, ble_send_data); 
#endif	
	
	     return 0;
}

static struct cat_variable meter_disconnection_vars[] = {
        {
                .type = CAT_VAR_UINT_DEC,
                .data = &disconnection_channel,
                .data_size = sizeof(disconnection_channel),
                .name = "connect cmd"
        },
};

/*
断开蓝牙连接
*/
static int meter_ble_disconnection(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, const size_t args_num)
{
	   app_disconnection_device(disconnection_channel);
	   return 0;
}

/*
设置蓝牙的名称
*/
static int meter_ble_set_name(const struct cat_command *cmd, const uint8_t *data, const size_t data_size, const size_t args_num)
{

	     printf("%s \n", ble_dev_name);
	     app_meter_set_adv_name(ble_dev_name);
	     appm_reset_adv_data();
	     return 0;
}

static struct cat_variable meter_ble_set_vars[] = {
        {
                .type = CAT_VAR_BUF_STRING,
                .data = &ble_dev_name,
                .data_size = sizeof(ble_dev_name),
                .name = "ble dev name"
        },
};

/* declaring commands array */

static struct cat_command meter_cmds[] = {
	
#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))	
	      {
                .name = "+SCAN",
                .description = "1 start and stop ble scan",
                .write = meter_scan_write,
                .var = meter_scan_vars,
                .var_num = sizeof(meter_scan_vars) / sizeof(meter_scan_vars[0]),
                .need_all_vars = true
        },
				{
					      .name = "+CONN",
                .description = "connect ble device",
                .write = meter_connect_write,
                .var = meter_connect_vars,
                .var_num = sizeof(meter_connect_vars) / sizeof(meter_connect_vars[0]),
                .need_all_vars = true
					
				},
				
				{
					      .name = "+CHINFO",
                .description = "get the channel info",
                .run = meter_channel_info,
                .need_all_vars = false
				},
				
				{
					      .name = "+LEDISC",
                .description = "Disconnection ble ",
                .write = meter_ble_disconnection,
                .var = meter_disconnection_vars,
                .var_num = sizeof(meter_disconnection_vars) / sizeof(meter_disconnection_vars[0]),
                .need_all_vars = true
					
				},
#endif
				{
					      .name = "+LESEND",
                .description = "ble send data ",
                .write = meter_ble_send_data,
                .var = meter_send_vars,
                .var_num = sizeof(meter_send_vars) / sizeof(meter_send_vars[0]),
                .need_all_vars = true
					
				},
				
				{
					      .name = "+NAME",
                .description = "set ble device name ",
                .write = meter_ble_set_name,
                .var = meter_ble_set_vars,
                .var_num = sizeof(meter_ble_set_vars) / sizeof(meter_ble_set_vars[0]),
                .need_all_vars = true
					
				},
				
        {
                .name = "+REBOOT",
                .run = meter_reboot_run
        },
				
};



static struct cat_command_group meter_cmd_group = {
        .cmd = meter_cmds,
        .cmd_num = sizeof(meter_cmds) / sizeof(meter_cmds[0]),
};


static struct cat_command_group *meter_cmd_desc[] = {
        &meter_cmd_group
};


static struct cat_descriptor desc = {
        .cmd_group = meter_cmd_desc,
        .cmd_group_num = sizeof(meter_cmd_desc) / sizeof(meter_cmd_desc[0]),

        .buf = work_buf,
        .buf_size = sizeof(work_buf)
};

#endif

/* custom target dependent input output handlers */
static int write_char(char ch)
{
	      cobra_uart_write(M3_UART0, ch);
        return 1;
}

static int read_char(char *ch)
{
	    uint8_t cChar;		
	
	    if (true == QueueOut(at_cmd_uart_buffer, cChar)) 
			{
          *ch = cChar; //getc(stdin);
          return 1;
			}
			else
			{
				  return 0;
			}
}

/* declaring input output interface descriptor for parser */
static struct cat_io_interface iface = {
        .read = read_char,
        .write = write_char
};


static void app_at_cmd_uart_receive_data(unsigned char ch)
{
	
	  QueueIn(at_cmd_uart_buffer, ch);
}

void app_at_cmd_init(void)
{
	  memset((uint8_t *)&at_cmd_uart_buffer, 0, sizeof(at_cmd_uart_buffer));
		 
		QueueInit(at_cmd_uart_buffer);
		 
		cobra4_uart_set_receive_callback(app_at_cmd_uart_receive_data);
	
	  cat_init(&cobra_at, &desc, &iface, NULL);
}


void app_at_cmd_loop(void)
{
	//  cat_service(&cobra_at);
	  while ((cat_service(&cobra_at) != 0)) {};
		
}

#endif


