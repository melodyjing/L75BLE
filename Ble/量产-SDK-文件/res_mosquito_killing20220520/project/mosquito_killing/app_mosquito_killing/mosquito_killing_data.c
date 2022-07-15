#include "mosquito_killing_data.h"
#include "mosquito_killing_protocol.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)

mosquito_killing_timing_t app_mosquito_killing_data_timing[MAX_TIMING_NUM];
/*月统计计数，从基准开始，第一个数表示第一个月*/
uint16_t  app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX];
mosquito_killing_date_t  app_mosquito_killing_save_date;
/*记录设置蓝牙连接的次数,不做清除操作，只连接一次加1，当大于0xffff时停止计数，保持0xffff，返回app保持在0xffff*/
uint32_t app_mosquito_killing_bt_connect_count = 0;

mosquito_killing_timing_setting_t app_mosquito_killing_timing_setting;
mosquito_killing_bak_t  app_mosquito_killing_setting_bak;
mosquito_killing_year_base_t app_mosquito_killing_year_base;

/*v1.4 缺省为每天晚上20:00开启，第二天6:00关,没有定时设置*/
void app_mosquito_killing_setting_data_default_setting(void)
{
    memset((uint8_t *)app_mosquito_killing_month_count, 0x00, STATISTICS_MONTH_SZIE);
//    memset((uint8_t *)&app_mosquito_killing_setting_bak, 0x00, SETTING_BAK_SIZE); /*因reserve用于记录watchdog复位次数，不清除此数据*/
    memset((uint8_t *)&app_mosquito_killing_year_base, 0x00, YEAR_BASE_SIZE);

    app_mosquito_killing_setting_bak.bak_time_sync_finished = 0;
    app_mosquito_killing_setting_bak.bak_work_state = 0;



#if 0 /*only for test*/
    for(int i=0; i< APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX; i++)
    {
        app_mosquito_killing_month_count[i] = i;
    }
#endif

    app_mosquito_killing_data_timing[0].week_day = 0x00; /*每天*/
//    app_mosquito_killing_data_timing[0].week_day = 0x80; /*每天*/
//    app_mosquito_killing_data_timing[0].week_day = 0xFF; /*每天*/
    app_mosquito_killing_data_timing[0].hour_begin = 20;
    app_mosquito_killing_data_timing[0].min_begin = 0;
    app_mosquito_killing_data_timing[0].hour_end = 6;
    app_mosquito_killing_data_timing[0].min_end = 0;

    app_mosquito_killing_save_date.year = 2021;  /*0x07E5*/
    app_mosquito_killing_save_date.month = 9;
    app_mosquito_killing_save_date.day = 8;
    app_mosquito_killing_save_date.hour = 7;
    app_mosquito_killing_save_date.min = 6;
    app_mosquito_killing_save_date.sec = 5;
    app_mosquito_killing_save_date.week = 2; /*周三*/
}

/*ok==return 1; otherwise==return 0*/
uint8_t  app_mosquito_killing_setting_data_read(uint32_t startAddr)
{
    int i =0;

    uint32_t _u32Head = 0;
    uint32_t _u32Crc = 0;
    uint32_t _u32tmpCrc = 0;

    uint32_t * _u32Buf = (uint32_t * )(APP_MOSQUITO_KILLING_FLASH_READ_ADDR_OFFSET + startAddr);
    uint8_t * _u8Buf = (uint8_t * )(APP_MOSQUITO_KILLING_FLASH_READ_ADDR_OFFSET + startAddr + FLASH_SETTING_CRC_HEADER_SZIE);	/*定时数据*/

    _u32Head = _u32Buf[0];
    _u32Crc = _u32Buf[1];

    __direct_printf_flash("\n addr [0x%x],read flash data:\n", startAddr);
    for(i=0; i<FLASH_SETTING_SIZE; i++)
    {
        _u32tmpCrc += _u8Buf[i];

        __direct_printf_flash("%03d[0x%02x] ", i, _u8Buf[i]);
        if((i+1)%8 == 0)
            __direct_printf_flash("\n");
    }
    __direct_printf_flash("\n");

    __direct_printf("addr[0x%x],check crc Read[0x%x], Cal[0x%x], read magic num[0x%x]\n",startAddr, _u32Crc, _u32tmpCrc, _u32Head);

    if((_u32Head == FLASH_SETTING_MAGIC_NUMBER) && (_u32tmpCrc == _u32Crc))
    {
        memcpy((uint8_t *)&app_mosquito_killing_data_timing, _u8Buf, TIMING_SETTING_SIZE);

        memcpy((uint8_t *)app_mosquito_killing_month_count, _u8Buf+TIMING_SETTING_SIZE, STATISTICS_MONTH_SZIE);

        memcpy((uint8_t *)&app_mosquito_killing_save_date, _u8Buf+TIMING_SETTING_SIZE+STATISTICS_MONTH_SZIE, SAVE_DATE_SIZE);

        memcpy((uint8_t *)&app_mosquito_killing_bt_connect_count, _u8Buf+TIMING_SETTING_SIZE+STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE, SAVE_BT_CONNECT_COUNT_SIZE);

        memcpy((uint8_t *)&app_mosquito_killing_setting_bak, _u8Buf+TIMING_SETTING_SIZE+STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE+SAVE_BT_CONNECT_COUNT_SIZE, SETTING_BAK_SIZE);

		memcpy((uint8_t *)&app_mosquito_killing_year_base, _u8Buf+TIMING_SETTING_SIZE+STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE+SAVE_BT_CONNECT_COUNT_SIZE+SETTING_BAK_SIZE, YEAR_BASE_SIZE);
    }
    else
    {
		return 0;
//        app_mosquito_killing_setting_data_default_setting();
    }

	return 1;
}

/*先读取主存储区，有错误再读backup存储区，如都有错误，则只恢复内存数据，不需要写入eflash保存(缺省数据没有保存意义)*/
void app_mosquito_killing_setting_data_load(void)
{
	uint8_t _u8Res = 0;

	_u8Res = app_mosquito_killing_setting_data_read(FLASH_SETTING_DATA_START_ADDR);
	if(_u8Res == 0)
	{
		_u8Res = app_mosquito_killing_setting_data_read(FLASH_SETTING_DATA_BACKUP_START_ADDR);

		if(_u8Res == 0)
		{
			app_mosquito_killing_setting_data_default_setting();
		}
		else
		{
			app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);  /*当主存储区有误，而backup存储区正常时，需要写回主存储区*/
		}
	}
	
}

void app_mosquito_killing_setting_data_write(uint32_t startAddr)
{
    int i=0;
    int j = 0;
    uint32_t _u32tmpCrc = 0;
    uint32_t _u32Head = FLASH_SETTING_MAGIC_NUMBER;

    uint8_t * _u8Buf;

    __direct_printf_flash("\nwrite flash data:\n");
    _u8Buf = (uint8_t *)&app_mosquito_killing_data_timing;
    for(i=0; i<TIMING_SETTING_SIZE; i++)
    {
        _u32tmpCrc +=  _u8Buf[i];
        __direct_printf_flash("%03d[0x%02x] ", j++, _u8Buf[i]);
        if((j)%8 == 0)
            __direct_printf_flash("\n");
    }


    _u8Buf = (uint8_t *)app_mosquito_killing_month_count;
    for(i=0; i< STATISTICS_MONTH_SZIE; i++)
    {
        _u32tmpCrc += _u8Buf[i];
        __direct_printf_flash("%03d[0x%02x] ", j++, _u8Buf[i]);
        if((j)%8 == 0)
            __direct_printf_flash("\n");
    }

    _u8Buf = (uint8_t *)&app_mosquito_killing_save_date;
    for(i=0; i< SAVE_DATE_SIZE; i++)
    {
        _u32tmpCrc += _u8Buf[i];
        __direct_printf_flash("%03d[0x%02x] ", j++, _u8Buf[i]);
        if((j)%8 == 0)
            __direct_printf_flash("\n");
    }

    _u8Buf = (uint8_t *)&app_mosquito_killing_bt_connect_count;
    for(i=0; i< SAVE_BT_CONNECT_COUNT_SIZE; i++)
    {
        _u32tmpCrc += _u8Buf[i];
        __direct_printf_flash("%03d[0x%02x] ", j++, _u8Buf[i]);
        if((j)%8 == 0)
            __direct_printf_flash("\n");
    }

    app_mosquito_killing_setting_bak.bak_work_state = app_mosquito_killing_work_state;
    app_mosquito_killing_setting_bak.bak_time_sync_finished = app_mosquito_killing_timing_setting.time_sync_finished;
    _u8Buf = (uint8_t *)&app_mosquito_killing_setting_bak;
    for(i=0; i< SETTING_BAK_SIZE; i++)
    {
        _u32tmpCrc += _u8Buf[i];
        __direct_printf_flash("%03d[0x%02x] ", j++, _u8Buf[i]);
        if((j)%8 == 0)
            __direct_printf_flash("\n");
    }

//	app_mosquito_killing_year_base.valid = 0xAA55; /*不能在此赋值，其它的保存也会写入*/
    _u8Buf = (uint8_t *)&app_mosquito_killing_year_base;
    for(i=0; i< YEAR_BASE_SIZE; i++)
    {
        _u32tmpCrc += _u8Buf[i];
        __direct_printf_flash("%03d[0x%02x] ", j++, _u8Buf[i]);
        if((j)%8 == 0)
            __direct_printf_flash("\n");
    }	
    __direct_printf_flash("\n");


    cobra_flash_erase(startAddr, FLASH_SETTING_CRC_HEADER_SZIE+FLASH_SETTING_SIZE);

    cobra_flash_write(startAddr, 4,(uint8_t *) &_u32Head);
    cobra_flash_write(startAddr + 4, 4,(uint8_t *) &_u32tmpCrc);

    cobra_flash_write(startAddr+FLASH_SETTING_CRC_HEADER_SZIE, TIMING_SETTING_SIZE,(uint8_t *) &app_mosquito_killing_data_timing);
    cobra_flash_write(startAddr+FLASH_SETTING_CRC_HEADER_SZIE + TIMING_SETTING_SIZE,STATISTICS_MONTH_SZIE, (uint8_t *)app_mosquito_killing_month_count);
    cobra_flash_write(startAddr+FLASH_SETTING_CRC_HEADER_SZIE + TIMING_SETTING_SIZE +STATISTICS_MONTH_SZIE,SAVE_DATE_SIZE, (uint8_t *)&app_mosquito_killing_save_date);
    cobra_flash_write(startAddr+FLASH_SETTING_CRC_HEADER_SZIE + TIMING_SETTING_SIZE +STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE, SAVE_BT_CONNECT_COUNT_SIZE, (uint8_t *)&app_mosquito_killing_bt_connect_count);
    cobra_flash_write(startAddr+FLASH_SETTING_CRC_HEADER_SZIE + TIMING_SETTING_SIZE +STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE+SAVE_BT_CONNECT_COUNT_SIZE, SETTING_BAK_SIZE, (uint8_t *)&app_mosquito_killing_setting_bak);
    cobra_flash_write(startAddr+FLASH_SETTING_CRC_HEADER_SZIE + TIMING_SETTING_SIZE +STATISTICS_MONTH_SZIE+SAVE_DATE_SIZE+SAVE_BT_CONNECT_COUNT_SIZE+SETTING_BAK_SIZE, YEAR_BASE_SIZE, (uint8_t *)&app_mosquito_killing_year_base);

    cobra_cache_invalidate_the_cache();

    __direct_printf("w addr[0x%x], crc[0x%x]\n", startAddr, _u32tmpCrc);
}

void app_mosquito_killing_setting_data_erase(void)
{
    cobra_flash_erase(FLASH_SETTING_DATA_START_ADDR, FLASH_SETTING_CRC_HEADER_SZIE+FLASH_SETTING_SIZE);
    cobra_cache_invalidate_the_cache();
}

/**************************************************************************************/
/*cobra_nvds_init()执行时会读取ip地址来设置，如读取不成功会采用随机的方式来生成ip地址来填充*/
void app_mosquito_killing_mac_pid_default_setting(void)
{

}

void app_mosquito_killing_mac_pid_load(void)
{
    uint8_t * _u8Buf = (uint8_t * )(APP_MOSQUITO_KILLING_FLASH_READ_ADDR_OFFSET + M4_EFLASH_MAC_PID_ADDRESS );

    if((_u8Buf[0] == 0xAA) && (_u8Buf[1] == 0x55))
    {
        if(_u8Buf[2] == 0xA5)
        {
            memcpy((uint8_t*)&app_mosquito_killing_bt_addr, _u8Buf+3, 6);
        }

        if(_u8Buf[9] == 0xA5)
        {
            memcpy((uint8_t *)&app_mosquito_killing_pid, _u8Buf+10, 4);
        }

        if(_u8Buf[14] == 0xA5)
        {
            memcpy((uint8_t *)&app_mosquito_killing_password, _u8Buf+15, 4);
        }

        if(_u8Buf[19] == 0xA5)
        {
            memcpy((uint8_t *)&app_mosquito_killing_name, _u8Buf+20, 16);
        }
    }
    else
    {
        __direct_printf("mac pid read failed!!!\n");
        app_mosquito_killing_mac_pid_default_setting();
    }

    __direct_printf("read mac pid:\n");
    for(int i=0; i<= 35; i++)
    {
        __direct_printf("%02d[0x%02x] ", i, _u8Buf[i]);
        if((i+1)%8 == 0)
            __direct_printf("\n");
    }
    __direct_printf("\n");

}

/*return 0: failed;
         1: OK
*/      
uint8_t app_mosquito_killing_mac_pid_write(void)
{
    int i=0;
    int j=0;
    uint8_t _u8Temp[50];
	uint8_t _u8Res = 0;

    _u8Temp[i++]= 0xAA;
    _u8Temp[i++]= 0x55;

    _u8Temp[i++]= 0xA5;
    for(j=0; j<6; j++)
    {
        _u8Temp[i++]= app_mosquito_killing_bt_addr[j];
    }

    _u8Temp[i++]= 0xA5;
    for(j=0; j<4; j++)
    {
        _u8Temp[i++]= app_mosquito_killing_pid[j];
    }

    _u8Temp[i++]= 0xA5;
    for(j=0; j<4; j++)
    {
        _u8Temp[i++]= app_mosquito_killing_password[j];
    }

    _u8Temp[i++]= 0xA5;
    for(j=0; j<16; j++)
    {
        _u8Temp[i++]= app_mosquito_killing_name[j];
    }

//    cobra_flash_erase(M4_EFLASH_MAC_PID_ADDRESS, i);
    _u8Res = cobra_flash_erase(M4_EFLASH_MAC_PID_ADDRESS, APP_MOSQUITO_KILLING_FLASH_MAC_PID_SIZE);

	if(_u8Res == 1)
	{
	    _u8Res = cobra_flash_write(M4_EFLASH_MAC_PID_ADDRESS, i,(uint8_t *)&_u8Temp);
	}
	else
	{
		return 0;
	}

    cobra_cache_invalidate_the_cache();

    __direct_printf("write mac pid:\n");
    for(int j=0; j< i; j++)
    {
        __direct_printf("%02d[0x%02x] ", j, _u8Temp[j]);
        if((j+1)%8 == 0)
            __direct_printf("\n");
    }
    __direct_printf("\n");

	return _u8Res;

}

void app_mosquito_killing_mac_pid_erase(void)
{
}

void app_mosquito_killing_setting_data_write_debug(uint32_t param)
{
	app_mosquito_killing_setting_bak.reserve01 = param;
	app_mosquito_killing_setting_data_write(FLASH_SETTING_DATA_START_ADDR);
}

#endif

