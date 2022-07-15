/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_mesh_util.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	   2020/8/5

	Description :   一些简单的工具, 方便写 应用
	
*******************************************************************************/
#include "app_mesh_util.h"
#include "rdw_mcu.h"
#include "cobra_cache_ctrl.h"
#include "cobra4C_eflash_ctrl.h"
#include "cobra_soft_timer.h"
#include "app_bk_task.h"
#include "cobra_system.h"
#include <stdint.h>
#include <ke_msg.h>
#include "mesh_api_msg.h"
#include "mm_gens.h"
#include "mm_api_int.h"

#include "gapm_task.h"
#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))
#include "app_tulip_comm.h"
#endif

#if (APP_TULIP_BATTER_DETECT_VERSION2)
#include "app_tulip_battery.h"
#endif


#if (BLE_APP_MESH == 1)

#define MESH_MAGIC_TAG  0x4D455348

#define TIMES_MAX (6)

static uint32_t  mesh_util_timer_id, mesh_erase_flag_id;

static uint32_t mesh_util_buffer_address;

static uint32_t mesh_util_time_delay;	

static void  app_mesh_util_callback(uint32_t param);

static void  app_mesh_erase_flag_callback(uint32_t param);

static uint8_t repeat_reset = 0;   //1. need write 2. need erase

#if APP_FLASH_LIGHT_NUMBERS
#include "app_tulip_light.h"
static uint8_t jscs_add=0;
static uint8_t jscs_sub=0;
#endif

/* 每个月的天数 */

static const unsigned char g_day_per_mon[MONTH_PER_YEAR] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/*
 * 功能：
 *     判断是否是闰年
 * 参数：
 *     year：需要判断的年份数
 *
 * 返回值：
 *     闰年返回1，否则返回0
 */
static unsigned char app_mesh_util_dt_is_leap_year(unsigned short year)
{

    if ((year % 400) == 0) 
		{
        return 1;
    } 
		else if ((year % 100) == 0) 
		{
        return 0;
    } 
		else if ((year % 4) == 0) 
		{
        return 1;
    } 
		else 
		{
        return 0;
    }
}

/*
 * 功能：
 *     得到每个月有多少天
 * 参数：
 *     month：需要得到天数的月份数
 *     year：该月所对应的年份数
 *
 * 返回值：
 *     该月有多少天
 *
 */

static unsigned char app_mesh_util_dt_last_day_of_mon(unsigned char month, unsigned short year)
{
    if ((month == 0) || (month > 12)) 
		{
        return g_day_per_mon[1] + app_mesh_util_dt_is_leap_year(year);
    }

    if (month != 2) 
		{
        return g_day_per_mon[month - 1];
    } 
		else 
		{
        return g_day_per_mon[1] + app_mesh_util_dt_is_leap_year(year);
    }
}

void app_mesh_util_unix_time(long linux_time)
{
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;

    //将时间戳值转化成天数。通过天数可以比较方便地算出年、月、日。
    int days = linux_time / SEC_PER_DAY;
    //这个时间戳值的年。
    int yearTmp = 0;
    int dayTmp = 0;
    //使用夹逼法计算 days 天中包含的年数。
    for (yearTmp = UTC_BASE_YEAR; days > 0; yearTmp++) 
    {
        dayTmp = (DAY_PER_YEAR + app_mesh_util_dt_is_leap_year(yearTmp)); //这一年有多少天？
			
        if (days >= dayTmp) //条件成立，则 yearTmp 即是这个时间戳值所代表的年数。
        {
           days -= dayTmp;
        }
        else
        {
           break;
        }
    }
		
    year = yearTmp;

    //这个时间戳值的月
    int monthTmp = 0;
    for (monthTmp = 1; monthTmp < MONTH_PER_YEAR; monthTmp++) 
		{
       dayTmp = app_mesh_util_dt_last_day_of_mon(monthTmp, year);
			
       if (days >= dayTmp) 
			 {
           days -= dayTmp;
       }
       else
       {
           break;
       }
    }
    month = monthTmp;

    day = days + 1;

    //转化成秒。
    int secs = linux_time % SEC_PER_DAY;
    //这个时间戳值的小时数。
    hour = secs / SEC_PER_HOUR;
    //这个时间戳值的分钟数。
    secs %= SEC_PER_HOUR;
    minute = secs / SEC_PER_MIN;
    //这个时间戳的秒钟数。
    second = secs % SEC_PER_MIN;

    printf("%d-%d-%d %d:%d:%d\n\n", year, month, day, hour, minute, second);
}
#if (BLE_APP_MESH_TULIPE_LIGHT == 1)
#if (COBRA_POWER_DOWN_SAVE_ENABLE == 0)
/*
  根据开机上电的时间长度次数, 来决定后面的事件
  三端两长 就清掉保存的记录
*/
void app_mesh_util_init_mesh_eflash(void)
{
	  uint32_t i;
	  uint32_t ok_flag = 1;
	  volatile uint32_t * p_flag = (uint32_t * )M4_EFLASH_MESH_ADDRESS;
	  volatile uint8_t *  p_buffer;
	  unsigned char temp_array[5];
	  unsigned char ch;
	  uint32_t temp = MESH_MAGIC_TAG;
	  p_buffer = (uint8_t * )(M4_EFLASH_MESH_ADDRESS + 100);
	  
	  if((*p_flag != MESH_MAGIC_TAG) || (*p_buffer == 0x88))   //说明是上一次开机的时候, 工作超过了 12s, 要把这块地址
		{
		   cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);	
       cobra_flash_write(M4_EFLASH_MESH_ADDRESS, sizeof(temp),	(unsigned char *)&temp);		
	     cobra_cache_invalidate_the_cache();
		}
		
		mesh_util_buffer_address = 0x00000000;
		
		p_buffer = (uint8_t * )(M4_EFLASH_MESH_ADDRESS + 4);
		for(i = 0; i < 5 ; i++)
		{
			  if(*p_buffer == 0x55) {
					 temp_array[i] = *(p_buffer + 1);
					 p_buffer += 2;
           continue;					
				}
				else
				{
					 ch = 0x55;
					 cobra_flash_write((unsigned int)p_buffer, sizeof(ch),	(unsigned char *)&ch);	
					 mesh_util_buffer_address = (unsigned int)p_buffer + 1;
					 ok_flag = 0;
					 break;
				}
		}
		
		if(ok_flag)
		{
			  for(i = 0; i < 5; i++)
			  {
					 printf("0x%02x ", temp_array[i]);
				}
				
				if((temp_array[0] == 0xff) && (temp_array[1] == 0xff) && (temp_array[2] == 0xff) \
             	&& (temp_array[3] == 0xff) && (temp_array[4] == 0xff) )
        {
             cobra_flash_erase(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);
					   cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);	
             platform_reset(0x00);					
				}					
		}
		
		mesh_util_timer_id = cobra_soft_timer_create(app_mesh_util_callback);
		
	  cobra_soft_timer_start(mesh_util_timer_id, COBRA_SOFT_TIMER_TICK, 1);	
		
    mesh_util_time_delay = 0;

	  app_mesh_flash_to_scens();	
}

#else

uint8_t get_repeat_reset()
{
	return repeat_reset;
}
void set_repeat_reset(uint8_t value)
{
	repeat_reset = value;
}
void app_mesh_drop_count(void)
{
	
	#if (COBRA_FLASH_POWERON_CRASE == 1)
	uint8_t   p_count = get_poweronoff_number();
	uint8_t   p_flag = get_poweron_timeout();
	#else
	uint8_t *  p_count = (uint8_t * )M4_EFLASH_COUNT_ADDRESS;
	uint8_t *  p_flag = (uint8_t * )M4_EFLASH_COUNT_TIMEOUT_ADDRESS;
	#endif
	uint8_t count = 0;
	uint32_t temp = MESH_MAGIC_TAG;

	#if (COBRA_FLASH_POWERON_CRASE == 1)
	count = p_count;
	if(count == 0xff)	
	#else
	count = *p_count;
	if(count == 0xff || (*p_flag == 0x88))
	#endif
	{
		count = 1;
	}
	#if (COBRA_FLASH_POWERON_CRASE == 1)
	else if(p_flag == 0x88) // 解决超时不能马上复位，导致多计数一次的问题
	{
		count = 2;
	}		
	#endif
	else
	{
		count ++;
	}
	
	//PRINTF_LOG("app_mesh_drop_count count:%d", count);

	
	  #if (COBRA_FLASH_POWERON_CRASE == 1)
	  cobra_flash_write(M4_EFLASH_MESH_ADDRESS, sizeof(temp),  (unsigned char *)&temp);
	  #else
    cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);
    cobra_flash_write(M4_EFLASH_MESH_ADDRESS, sizeof(temp),  (unsigned char *)&temp); 
    #endif	
    cobra_flash_write(M4_EFLASH_COUNT_ADDRESS, sizeof(count),  (unsigned char *)&count);  

	
}

#if (APP_FLASH_LIGHT_NUMBERS)
void read_power_on_nubmer(void)
{
		uint8_t	number=3;
  #if (COBRA_FLASH_POWERON_CRASE == 1)
	uint8_t   count = get_poweronoff_number();
	uint32_t   p_flag = get_meshaddress_flag();

	if(((p_flag == MESH_MAGIC_TAG)  &&count >= TIMES_MAX) && (count != 0xff))// 	
	#else	
		uint32_t *p_flag = (uint32_t * )M4_EFLASH_MESH_ADDRESS;
		uint8_t *p_buffer = (uint8_t * )M4_EFLASH_COUNT_ADDRESS;
		uint8_t count = *p_buffer;
	
	if(((*p_flag == MESH_MAGIC_TAG)  &&count >= TIMES_MAX) && (count != 0xff))// 
	#endif	
	{
    printf("count=%x\n",count);
		cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);	
		cobra_flash_erase(APP_MESH_TULIPX_SCENS_DATA, M0_M3_NVDS_SIZE);
	
		#if APP_FLASH_LIGHT_NUMBERS
		cobra_flash_write(M4_EFLASH_FLASH_NUMBERS_ADDRESS, sizeof(number),  (unsigned char *)&number); 		
		#endif
		platform_reset(0x00);
	}	
}

#endif

void app_mesh_util_init_mesh_eflash(void)
{
  #if (COBRA_FLASH_POWERON_CRASE == 1)
	uint32_t p_flag = get_meshaddress_flag();	
	#else
	uint32_t *p_flag = (uint32_t * )M4_EFLASH_MESH_ADDRESS;
	uint8_t *p_buffer = (uint8_t * )M4_EFLASH_COUNT_ADDRESS;
	uint8_t count = *p_buffer;
	#endif
	uint32_t temp = MESH_MAGIC_TAG;
	if((uint32_t)(p_flag) == (uint32_t)MESH_MAGIC_TAG)	
	#if (APP_FLASH_LIGHT_NUMBERS == 0)

	//PRINTF_LOG("app_mesh_util_init_mesh_eflash count: %d", count);
	if((*p_flag == MESH_MAGIC_TAG) && (count >= TIMES_MAX) && (count != 0xff))
	{
		//PRINTF_LOG("M4_EFLASH_NVDS_ADDRESS ERASE");
		cobra_flash_erase(M4_EFLASH_NVDS_ADDRESS, M0_M3_NVDS_SIZE);
		cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);
		cobra_flash_erase(APP_MESH_TULIPX_SCENS_DATA, M0_M3_NVDS_SIZE);
		//cobra_flash_erase(M4_MESH_DATA_ADDR, M0_M3_NVDS_SIZE);	
		platform_reset(0x00);
	}
		
		
	if((uint32_t)(*p_flag) == (uint32_t)MESH_MAGIC_TAG)
#endif	
	{
		mesh_erase_flag_id = cobra_soft_timer_create(app_mesh_erase_flag_callback);

		cobra_soft_timer_start(mesh_erase_flag_id, COBRA_SOFT_TIMER_TICK, 1);	

		mesh_util_time_delay = 0;
	}	
	


	app_mesh_flash_to_scens();
	//check_device_is_provied();
	
}
#endif

static void  app_mesh_util_callback(uint32_t param)
{
	
	  mesh_util_time_delay++;
	
	  if((mesh_util_time_delay > 0) && (mesh_util_time_delay < 15))
		{
			  cobra_app_set_mesh_write_flag_task();
		}
		else if(mesh_util_time_delay > 15)
		{
			  cobra_app_set_mesh_erase_flag_task();
			  cobra_soft_timer_stop(mesh_util_timer_id);
		}
	
}
extern unsigned int test_screen_index;  
static void  app_mesh_erase_flag_callback(uint32_t param)
{
	
	  mesh_util_time_delay++;
	
		if(mesh_util_time_delay > 15)
		{
			  cobra_app_set_mesh_erase_flag_task();
			  cobra_soft_timer_stop(mesh_erase_flag_id);
			//  cobra_soft_timer_stop(mesh_util_timer_id);
		}
		if(mesh_util_time_delay == 2)
		{
        set_repeat_reset(1);
		}
	
}


void app_mesh_util_write_flag(void)
{
	  unsigned char ch;
	
	  if(mesh_util_buffer_address != 0x0000)
		{
			  ch = 0x00;
		//		cobra_flash_write((unsigned int)mesh_util_buffer_address, sizeof(ch),	(unsigned char *)&ch);	
			  mesh_util_buffer_address = 0x000;
		}
	
}

void app_mesh_util_erase_flag(void)
{
	   unsigned char ch;
	   uint8_t *p_buffer;
	   printf("app_mesh_util_erase_flag \n");
	//  cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);	


	   ch = 0x88;
	   p_buffer = (uint8_t * )M4_EFLASH_COUNT_TIMEOUT_ADDRESS;
		 cobra_flash_write((unsigned int)p_buffer, sizeof(ch),	(unsigned char *)&ch);	
		 mesh_util_buffer_address = 0x000;
}

#endif

void app_mesh_task_storage_config(void)
{		
		// Allocate the message
    struct m_api_storage_config_cmd * req = KE_MSG_ALLOC(MESH_API_CMD,
                                                prf_get_task_from_id(TASK_ID_MESH),
                                                TASK_APP,
                                                m_api_storage_config_cmd);
		req->cmd_code = M_API_STORAGE_CONFIG;
		
		req->config = 4;    //4 秒钟检测一次
		
    // Send the message
    ke_msg_send(req);	
}

#if (BLE_APP_MESH_TULIPE_LIGHT == 1 || APP_MESH_CWHSL_LIGHT_FUNTION == 1)
////////////////////////////////////////////
/*
添加 CTL MODULE

*/
void app_tulip_add_module_ctl(uint8_t elmt_idx, bool main)
{
	 // Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;


    {
        do
        {
            // Model local index for Light CTL Server model and for Light CTL Tempereture Server model
            // and for Light Lightness Server model
            m_lid_t ln_mdl_lid, ctl_mdl_lid, ctlt_mdl_lid;


            // Register Generic Level Server model
            status = mm_api_register_server(elmt_idx + 1, MM_CFG_IDX_GENS_LEVEL, 0);

            if (status != MESH_ERR_NO_ERROR)
            {
                break;
            }

            // Register the Light CTL Server model and associated models
            status = mm_lights_ctl_register(elmt_idx, &ctl_mdl_lid, &ctlt_mdl_lid);

            if (main && (status == MESH_ERR_NO_ERROR))
            {
                // Group local index
                m_lid_t grp_lid;

                // Create group and set Light CTL Server model as main model
                mm_tb_bind_add_group(3, elmt_idx, &grp_lid, ctl_mdl_lid,
                                     mm_lights_ctl_cb_grp_event, mm_lights_ctl_cb_trans_req);

                // Add Generic Level Server model to the group
                mm_api_grp_add_local(elmt_idx, MM_ID_GENS_LVL, grp_lid);
                // Add Generic OnOff Server model to the group
                mm_api_grp_add_local(elmt_idx, MM_ID_GENS_OO, grp_lid);
                // Add Light Lightness Server model to the group
                mm_tb_bind_group_add_mdl(grp_lid, ln_mdl_lid, MM_ID_LIGHTS_LN,
                                         mm_lights_ln_cb_grp_event, mm_lights_ln_cb_set_state);

                // Create group and set Light CTL Temperature Server model as main model
                mm_tb_bind_add_group(1, elmt_idx + 1, &grp_lid, ctlt_mdl_lid,
                                     mm_lights_ctl_cb_grp_event_temp, mm_lights_ctl_cb_trans_req_temp);

                // Add Generic Level Server model to the group
                mm_api_grp_add_local(elmt_idx + 1, MM_ID_GENS_LVL, grp_lid);
            }
        } while (0);
    }

}

#endif

void app_mesh_stop_advertising(void)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                      TASK_GAPM, TASK_APP,
                                                      gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = app_env.adv_actv_idx;

    // Send the message
    ke_msg_send(cmd);

}

void app_debug_print_memory_content(unsigned char * p_address, uint32_t total_number)
{
	   uint32_t i;
	   printf("\n****************************************************************************\n");
	   printf("address: 0x%x \n", (uint32_t)(p_address));
	
	   for(i = 0; i < total_number; i++)
	   {
			   if((i % 32) == 0)
			   {
					   printf("\n");
				 }
				 printf("0x%02x ", p_address[i]);
		 }
		 
		 printf("\n-----------------------------------------------------------------------------\n");

	
	
}



void app_fnd_confs_add_callback(void)
{
		#if APP_FLASH_LIGHT_NUMBERS
		{
					jscs_add++;
					if(jscs_add>=10)
					{
								jscs_add=0;
								jscs_sub=0;
								set_flash_light_number(3);
								app_mesh_prov_start_timer();
					}
		}
		#endif
}

void app_fnd_confs_del_callback(void)
{
	#if APP_FLASH_LIGHT_NUMBERS

			jscs_sub++;
			if(jscs_sub>=10)
			{
					jscs_add=0;
					jscs_sub=0;
					set_flash_light_number(3);
					app_mesh_prov_start_timer();
			}

	#endif		
}


#endif