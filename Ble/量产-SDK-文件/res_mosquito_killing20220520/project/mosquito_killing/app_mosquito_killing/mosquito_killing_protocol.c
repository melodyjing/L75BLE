#include "mosquito_killing_protocol.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "cobra_multi_timer.h"
#include "cobra_dualtimer.h"
#include "cobra_timer.h"

#include "mosquito_killing_data.h"

extern uint8_t app_mosquito_killing_curr_month;
extern uint8_t app_mosquito_killing_work_state;
extern uint16_t app_mosquito_killing_month_count[];
extern uint8_t  app_mosquito_killing_password[];
extern uint8_t  app_mosquito_killing_name[];
extern uint8_t app_mosquito_killing_statistics_flag;
extern uint32_t app_mosquito_killing_bt_connect_count;

extern uint8_t app_mosquito_killing_zero_flag;

extern void app_mosquito_killing_control(void);
extern void app_mosquito_killing_work_mode_zero_clear(void);
extern void app_mosquito_killing_soft_reset(void);

static mosquito_killing_date_t  app_mosquito_killing_date;

static mosquito_killing_data_t app_mosquito_killing_data_bt;

static uint8_t app_mosquito_killing_buffer_send[APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX];
static uint8_t app_mosquito_killing_buffer_rec[APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX];

/*{1, 0, 1, 22, 1, 17} v1.0.1, 22-1-17*/
const static uint8_t app_mosquito_killing_rel_sw_info[] = {1, 0, 1, 22, 1, 24};

static uint32_t bt_receive_time_count =0;
static uint8_t bt_receive_data_len = 0;
static uint32_t bt_receive_time_interval = APP_MOSQUITO_KILLING_PROTOCOL_CMD_EXCUTE_INTERVAL_MAX;  /*当接到当次的最后三个字节为结束符的，将其设置为1，立即执行，否则设置大些，防止漏包*/
static void app_mosquito_killing_receive_bt_data_1ms_period(void)
{
    if(bt_receive_data_len > 0) /*有接收到数据后才计数*/
    {
        if(bt_receive_time_count > bt_receive_time_interval)  /*指定时间内没有接收到数据视为一次接收完成*/
        {
            __direct_printf("count[%d], interval[%d], len[%d]\n", bt_receive_time_count, bt_receive_time_interval, bt_receive_data_len);
            app_mosquito_killing_bt_data_receive((uint8_t *)&app_mosquito_killing_buffer_rec, bt_receive_data_len);
            bt_receive_data_len = 0;
        }
        else
        {
            bt_receive_time_count++;
        }
    }
}

static cobra_timer mosquito_killing_receive_bt_data_soft_timer;
void app_mosquito_killing_receive_bt_data_softtimer_init(void)
{
    cobra_softtimer_init(&mosquito_killing_receive_bt_data_soft_timer, app_mosquito_killing_receive_bt_data_1ms_period, 1,  1);
    cobra_softtimer_start(&mosquito_killing_receive_bt_data_soft_timer);
}

void app_mosquito_killing_bt_data_assemble(unsigned char *buffer, unsigned int length)
{
    bt_receive_time_count = 0;

    if((bt_receive_data_len + length) < APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX)
    {
        memcpy((uint8_t *)&app_mosquito_killing_buffer_rec[bt_receive_data_len], buffer, length);
        bt_receive_data_len += length;
    }
    else
    {
        __direct_printf("receive data beyond max[%d]. [%d][%d]\n", APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX, bt_receive_data_len, length);
    }

    if(bt_receive_data_len>=APP_MOSQUITO_KILLING_PROTOCOL_NUM_MIN)
    {
        if((app_mosquito_killing_buffer_rec[bt_receive_data_len -1] == APP_MOSQUITO_KILLING_RAIL_3)
                && (app_mosquito_killing_buffer_rec[bt_receive_data_len -2] == APP_MOSQUITO_KILLING_RAIL_2)
                && (app_mosquito_killing_buffer_rec[bt_receive_data_len -3] == APP_MOSQUITO_KILLING_RAIL_1))
        {
            bt_receive_time_interval = 1; /*立即执行当前接收数据*/

            return;
        }
    }

    bt_receive_time_interval = APP_MOSQUITO_KILLING_PROTOCOL_CMD_EXCUTE_INTERVAL_MAX;
}

void app_mosquito_killing_bt_data_receive(unsigned char *buffer, unsigned int length)
{
    int i = 0;
    uint8_t _u8CheckSum = 0;

    memset(&app_mosquito_killing_data_bt, 0x00, sizeof(app_mosquito_killing_data_bt));

    if(length < APP_MOSQUITO_KILLING_PROTOCOL_NUM_MIN)
    {
        __direct_printf("data len error!!!![%d]\n", length);

        return;
    }

    memcpy(&app_mosquito_killing_data_bt, buffer, length - 3);  /*复制除帧尾的三个字节前面的数据*/

    app_mosquito_killing_data_bt.rail[2] = buffer[length - 1];
    app_mosquito_killing_data_bt.rail[1] = buffer[length - 2];
    app_mosquito_killing_data_bt.rail[0] = buffer[length - 3];

    for(i=0; i<length - 6 - 1; i++)
    {
        _u8CheckSum ^= buffer[3+i];
    }

#if 1  /*only for test*/
    for(i = 0; i < length; i++)
    {
        __direct_printf("[0x%02x]  ", buffer[i]);
        if((i+1)%10 == 0)
            __direct_printf("\n");
    }
    __direct_printf("\nhead1[0x%02x] 2[0x%02x] 3[0x%02x]\n", app_mosquito_killing_data_bt.head[0], app_mosquito_killing_data_bt.head[1], app_mosquito_killing_data_bt.head[2]);
    __direct_printf("rail1[0x%02x] 2[0x%02x] 3[0x%02x]\n", app_mosquito_killing_data_bt.rail[0], app_mosquito_killing_data_bt.rail[1], app_mosquito_killing_data_bt.rail[2]);
    __direct_printf("checksum[0x%02x], cmd type[0x%02x], product type[0x%02x], cmd[0x%02x]\n", app_mosquito_killing_data_bt.checksum, app_mosquito_killing_data_bt.cmd_type, app_mosquito_killing_data_bt.product_type,app_mosquito_killing_data_bt.cmd);
#endif

    if((app_mosquito_killing_data_bt.head[0] == APP_MOSQUITO_KILLING_HEAD_1)
            && (app_mosquito_killing_data_bt.head[1] == APP_MOSQUITO_KILLING_HEAD_2)
            && (app_mosquito_killing_data_bt.head[2] == APP_MOSQUITO_KILLING_HEAD_3)
            && (app_mosquito_killing_data_bt.rail[0] == APP_MOSQUITO_KILLING_RAIL_1)
            && (app_mosquito_killing_data_bt.rail[1] == APP_MOSQUITO_KILLING_RAIL_2)
            && (app_mosquito_killing_data_bt.rail[2] == APP_MOSQUITO_KILLING_RAIL_3)
            /*&& (app_mosquito_killing_data_bt.checksum == _u8CheckSum)*/
            && (app_mosquito_killing_data_bt.product_type == APP_MOSQUITO_KILLING_PRODUCT_TYPE))
    {
        app_mosquito_killing_bt_data_process(app_mosquito_killing_data_bt.cmd, app_mosquito_killing_data_bt.cmd_type, &buffer[7], length - 10);
    }
	else
	{
		__direct_printf("!!!input cmd error....\n");
	}

}

void app_mosquito_killing_bt_data_send(uint8_t cmd, uint8_t cmdType, uint8_t *pdata, uint8_t len)
{
    int i;
    int index = 0;
    uint8_t _u8CheckSum = 0;
    uint8_t _u8Temp = 0;
    uint8_t _u8SendDataBytes = 0;

    uint32_t _u32TotalTemp = 0;

    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_HEAD_1;
    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_HEAD_2;
    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_HEAD_3;

//    app_mosquito_killing_buffer_send[index++] = _u8CheckSum;
    index++;  /*预留位置给checksum,后面计算完了才赋值, 固定在index = 3*/

    app_mosquito_killing_buffer_send[index++] = cmdType;
    _u8CheckSum ^= cmdType;
    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_PRODUCT_TYPE;
    _u8CheckSum ^= APP_MOSQUITO_KILLING_PRODUCT_TYPE;
    app_mosquito_killing_buffer_send[index++] = cmd;
    _u8CheckSum ^= cmd;

    /*如果是统计数据， 需要加上年、月*/
    if(cmdType == APP_MOSQUITO_KILLING_CMD_TYPE_RETURN)
    {
        if(cmd == APP_MOSQUITO_KILLING_CMD_COUNT)
        {
            /*直接返回总数*/
            for(i=0; i< APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX; i++)
            {
                _u32TotalTemp += app_mosquito_killing_month_count[i];
            }

            _u8Temp = (uint8_t)(_u32TotalTemp>>8)&0xFF;
            app_mosquito_killing_buffer_send[index++] = _u8Temp;
            _u8CheckSum ^= _u8Temp;
            _u8Temp = (uint8_t)( _u32TotalTemp&0xFF);
            app_mosquito_killing_buffer_send[index++] = _u8Temp;
            _u8CheckSum ^= _u8Temp;

#if 0
            if(app_mosquito_killing_zero_flag == 1) /*发送清零数据*/
            {
                app_mosquito_killing_zero_flag = 0;

                _u8Temp = 0xFF;
                app_mosquito_killing_buffer_send[index++] = _u8Temp;
                _u8CheckSum ^= _u8Temp;

                _u8Temp = 0xFF;
                app_mosquito_killing_buffer_send[index++] = _u8Temp;
                _u8CheckSum ^= _u8Temp;

                _u8Temp = 0x00;
                app_mosquito_killing_buffer_send[index++] = _u8Temp;
                _u8CheckSum ^= _u8Temp;
                _u8Temp = 0x00;
                app_mosquito_killing_buffer_send[index++] = _u8Temp;
                _u8CheckSum ^= _u8Temp;
            }
            else
            {
                _u8Temp = app_mosquito_killing_curr_month/12 + 1;  /*1~10 year*/
                app_mosquito_killing_buffer_send[index++] = _u8Temp;
                __direct_printf("m[%d], d[0x%x]\n", app_mosquito_killing_curr_month, _u8Temp);
                _u8CheckSum ^= _u8Temp;

                _u8Temp = app_mosquito_killing_curr_month%12 + 1;  /*1~12 month*/
                app_mosquito_killing_buffer_send[index++] = _u8Temp;
                __direct_printf("m[%d], d[0x%x]\n", app_mosquito_killing_curr_month, _u8Temp);
                _u8CheckSum ^= _u8Temp;

                app_mosquito_killing_buffer_send[index++] = pdata[1];
                _u8CheckSum ^= pdata[1];
                app_mosquito_killing_buffer_send[index++] = pdata[0];
                _u8CheckSum ^= pdata[0];
            }

#endif
        }
        else if(cmd == APP_MOSQUITO_KILLING_CMD_ON_OFF)
        {
            app_mosquito_killing_buffer_send[index++] = pdata[0];
            _u8CheckSum ^= pdata[0];
        }
		else if(cmd == APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING)
        {
            app_mosquito_killing_buffer_send[index++] = pdata[0];
            _u8CheckSum ^= pdata[0];
        }

//        for(i=0; i< len; i++)
//        {
//            __direct_printf("%02d[%x]\n", i, pdata[i]);
//        }

    }
    else if(cmdType == APP_MOSQUITO_KILLING_CMD_TYPE_READ)
    {
        for(i=0; i<len; i++)
        {
            app_mosquito_killing_buffer_send[index++] = pdata[i];
            _u8CheckSum ^= pdata[i];
        }
    }
    else if(cmdType == APP_MOSQUITO_KILLING_CMD_TYPE_WRITE)
    {
        /*其它指令不需要返回参数*/
        /*不需要返回参数时，直接将数据设置为NULL, 且长度设置为0，可以不再区分其指令*/
//        if((cmd == APP_MOSQUITO_KILLING_CMD_ON_OFF) || (cmd == APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING))
        {
            for(i=0; i<len; i++)
            {
                app_mosquito_killing_buffer_send[index++] = pdata[i];
                _u8CheckSum ^= pdata[i];
            }
        }
    }

    app_mosquito_killing_buffer_send[3] = _u8CheckSum;

    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_RAIL_1;
    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_RAIL_2;
    app_mosquito_killing_buffer_send[index++] = APP_MOSQUITO_KILLING_RAIL_3;

    if(cmd == APP_MOSQUITO_KILLING_CMD_IP_WR)  /*向串口回应写入IP地址结果或读取结果*/
    {
        for(i=0; i<index; i++)
        {
            cobra_uart_write(M3_UART0, app_mosquito_killing_buffer_send[i]);
        }
    }

#if 1
    /*only for test*/
    __direct_printf("send data len[%d]:\n",index);
    for(i=0; i<index; i++)
    {
        __direct_printf("[0x%02x] ", app_mosquito_killing_buffer_send[i]);
        if((i+1)%8 ==0)
            __direct_printf("\n");
    }
    __direct_printf("\n");
#endif

    /*20 bytes one time*/
    if(index > APP_MOSQUITO_KILLING_SEND_MTU)
    {
        _u8Temp = (index+APP_MOSQUITO_KILLING_SEND_MTU-1)/APP_MOSQUITO_KILLING_SEND_MTU;

        for(i=0; i<_u8Temp; i++)
        {
            if((index -  _u8SendDataBytes) > APP_MOSQUITO_KILLING_SEND_MTU)
            {
                app_wechat_send_data(&app_mosquito_killing_buffer_send[i*APP_MOSQUITO_KILLING_SEND_MTU], APP_MOSQUITO_KILLING_SEND_MTU);
                _u8SendDataBytes += APP_MOSQUITO_KILLING_SEND_MTU;
            }
            else if((index - _u8SendDataBytes)>0)
            {
                app_wechat_send_data(&app_mosquito_killing_buffer_send[i*APP_MOSQUITO_KILLING_SEND_MTU], index - _u8SendDataBytes);
            }
        }
    }
    else
    {
        app_wechat_send_data(app_mosquito_killing_buffer_send, index);
    }
}

void app_mosquito_killing_bt_data_process(uint8_t cmd, uint8_t cmdType, uint8_t *pdata, uint8_t len)
{
    int i=0;
    uint8_t _u8Res = 0;
    uint8_t _u8Temp[APP_MOSQUITO_KILLING_PROTOCOL_DATA_MAX];
    uint32_t u32Temp = 0;

    if(cmdType == APP_MOSQUITO_KILLING_CMD_TYPE_READ)
    {
        switch(cmd)
        {
            case APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING:
                _u8Temp[0] = (uint8_t)((app_mosquito_killing_bt_connect_count>>8) & 0xFF);
                _u8Temp[1] = (uint8_t)((app_mosquito_killing_bt_connect_count) & 0xFF);

				memcpy(&_u8Temp[2], app_mosquito_killing_rel_sw_info, sizeof(app_mosquito_killing_rel_sw_info));
				
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING,APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, 2+sizeof(app_mosquito_killing_rel_sw_info));
                break;

            case APP_MOSQUITO_KILLING_CMD_DATE_TIME:
            {
                COBRA_RTC_TIME_DATE_T time;
                cobra_rtc_get_current_time_date(&time);

                app_mosquito_killing_date.year = 2000+time.YEAR;
                app_mosquito_killing_date.month = time.MONTH;
                app_mosquito_killing_date.day = time.DAY;
                app_mosquito_killing_date.hour = time.HOUR;
                app_mosquito_killing_date.min = time.MIN;
                app_mosquito_killing_date.sec = time.SEC;

                app_mosquito_killing_date.week = 0;

				app_mosquito_killing_date.year = ((app_mosquito_killing_date.year<<8)&0xFF00)|((app_mosquito_killing_date.year>>8)&0xFF);

                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_DATE_TIME, APP_MOSQUITO_KILLING_CMD_TYPE_READ, &app_mosquito_killing_date, sizeof(app_mosquito_killing_date));

                break;
            }

            case APP_MOSQUITO_KILLING_CMD_ON_OFF:
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_READ, &app_mosquito_killing_work_state, 1);
                break;

            case APP_MOSQUITO_KILLING_CMD_COUNT:
            {

                COBRA_RTC_TIME_DATE_T time;
                cobra_rtc_get_current_time_date(&time);
#if 0
                /*取消全部返回，还是改为先前的按参数年差值来返回一年的计数*/
//				if((len == 1) && (pdata[0] == 0x0A))
                if((len == 1) /*&& (pdata[0] == 0x0A)*/)  /*测试发现app参数为0，先不判断此参数*/
                {
                    /*220107:改为全部数据返回,加上基准年份*/
                    _u8Temp[0] = (uint8_t)((app_mosquito_killing_year_base.year_base>>8)&0xFF);
                    _u8Temp[1] = (uint8_t)((app_mosquito_killing_year_base.year_base)&0xFF);
                    for(i=0; i<APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-2; i++)
                    {
                        _u8Temp[i*2+2] = (uint8_t)((app_mosquito_killing_month_count[i]>>8)&0xFF);
                        _u8Temp[i*2+3] = (uint8_t)(app_mosquito_killing_month_count[i]&0xFF);
                    }

                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, 120*2+2); /*120个月+两个字节年份基准*/
                }
                else
                {
                    __direct_printf("r count len[%d], ", len);
                    if(len>=1)
                    {
                        __direct_printf("param[0x%x] (right=0x0A)\n", pdata[0]);
                    }
                }
#endif

#if 1
                _u8Res = pdata[0];

				if(_u8Res == 0xFF)  /*查询返回总数*/
				{
                    _u8Temp[0] = 0xFF;

                    u32Temp = 0;
                    /*读返回总数*/
                    for(i=0; i< APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX; i++)
                    {
                        u32Temp += app_mosquito_killing_month_count[i];
                    }

                    _u8Temp[1] = (uint8_t)(u32Temp>>8)&0xFF;
                    _u8Temp[2] = (uint8_t)( u32Temp&0xFF);

                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, 3);

                    return;
                }

                /*年的差值(1~10)，改为以第一次连接保存的基准年份来作为第一年，其值为当前时间年的差值+1;如当前是2023年，
                  如差值为1，就是当前2023年的数据，如差值为2，则是2022的数据，如超过基准年则返回错误*/
                if((app_mosquito_killing_year_base.valid != 0xAA55) || (_u8Res>((2000+time.YEAR) - app_mosquito_killing_year_base.year_base + 1)))
                {
                    _u8Temp[0] = 0x00;
                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, 1);

                    return;
                }

                if((_u8Res>0) && (_u8Res<=10))
                {
//                    _u8Res = (_u8Res-1)*12; /*将年全部转换为月份写入月统计数组中*/
                    _u8Res = ((2000+time.YEAR) - app_mosquito_killing_year_base.year_base + 1 - _u8Res)*12;
                }
                else
                {
                    _u8Res = 0;  /*出错时置0*/

                    _u8Temp[0] = 0x00;
                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, 1);

                    return;
                }

                _u8Temp[0] = pdata[0];  /*返回年，直接用接收到的数据*/

                __direct_printf("read count year[%d], [%d]\n", _u8Temp[0], _u8Res);
//				for(i=0; i<12;i++)
//				{
//					__direct_printf("%02d[0x%x]\n", _u8Res+i, app_mosquito_killing_month_count[_u8Res+i]);
//				}

                for(i=0; i<12; i++)
                {
                    _u8Temp[i*2 + 1] = (app_mosquito_killing_month_count[_u8Res+i]>>8) & 0xFF;
                    _u8Temp[i*2 + 2] = (app_mosquito_killing_month_count[_u8Res+i])& 0xFF;

//                    __direct_printf("%02d[0x%02x]  %02d[0x%02x]  ", i*2 + 1, _u8Temp[i*2 + 1], i*2 + 2, _u8Temp[i*2 + 2]);
                }
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, 12*2+1);
#endif


                break;
            }
            case APP_MOSQUITO_KILLING_CMD_TIMING:
#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
                /*只读取第一组timing设置，暂定只有一组设置*/
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_TIMING, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)&app_mosquito_killing_data_timing, sizeof(mosquito_killing_timing_t)-1);/*注意这里有一个占位的字节，需要减去*/
#endif
                break;

            case APP_MOSQUITO_KILLING_CMD_IP_WR:
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_IP_WR, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)app_mosquito_killing_bt_addr, 6);
                break;

            case APP_MOSQUITO_KILLING_CMD_VERSION:
                memcpy(&_u8Temp[0], __DATE__, sizeof(__DATE__));
                memcpy(&_u8Temp[sizeof(__DATE__)], __TIME__, sizeof(__TIME__));

#if 1
                _u8Res = 0;
                if(_u8Temp[0] == 'J')
                {
                    if(_u8Temp[1] == 'u')
                    {
                        if(_u8Temp[2] == 'n')
                        {
                            _u8Res = 6;
                        }
                        else if(_u8Temp[2] == 'l')
                        {
                            _u8Res = 7;
                        }
                    }
                    else
                    {
                        _u8Res = 1;
                    }
                }
                else if(_u8Temp[0] == 'F')
                {
                    _u8Res = 2;
                }
                else if(_u8Temp[0] == 'M')
                {

                    if(_u8Temp[2] == 'r')
                    {
                        _u8Res = 3;
                    }
                    else
                    {
                        _u8Res = 5;
                    }
                }
                else if(_u8Temp[0] == 'A')
                {
                    if(_u8Temp[1] == 'p')
                    {
                        _u8Res = 4;
                    }
                    else
                    {
                        _u8Res = 8;
                    }
                }
                else if(_u8Temp[0] == 'S')
                {
                    _u8Res = 9;
                }
                else if(_u8Temp[0] == 'O')
                {
                    _u8Res = 10;
                }
                else if(_u8Temp[0] == 'N')
                {
                    _u8Res = 11;
                }
                else if(_u8Temp[0] == 'D')
                {
                    _u8Res = 12;
                }

                _u8Temp[0] = 0x00;
                _u8Temp[1] = 0x00;
                _u8Temp[2] = _u8Res;
//                __direct_printf("APP_MOSQUITO_KILLING_CMD_VERSION ***[0x%x], [0x%x], [0x%x]\n", _u8Temp[0], _u8Temp[1], _u8Temp[2]);
#endif

                if(_u8Res != 0)
                {
                    for(i=0; i<sizeof(__DATE__)+sizeof(__TIME__); i++)
                    {
                        if((_u8Temp[i] >=0x30) && (_u8Temp[i] <=0x39))
                        {
                            _u8Temp[i] = _u8Temp[i] - 0x30;
                        }
                    }

                    _u8Temp[3] = 0xAA; /*作为分隔显示*/
                    _u8Temp[6] = 0xAA;
                    _u8Temp[11] = 0xAA;
                    _u8Temp[14] = 0xAA;
                    _u8Temp[17] = 0xAA;
                    _u8Temp[20] = 0xAA;
                }
				
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_VERSION, APP_MOSQUITO_KILLING_CMD_TYPE_READ, (uint8_t *)_u8Temp, sizeof(__DATE__)+sizeof(__TIME__));
                break;

            default:
                __direct_printf("r no cmd [0x%02x]\n", cmd);
                break;
        }
    }
    else if(cmdType == APP_MOSQUITO_KILLING_CMD_TYPE_WRITE)
    {
        if(app_mosquito_killing_timing_setting.conn_bt_flag == 0)
        {
            app_mosquito_killing_timing_setting.conn_bt_flag = 1;  /*只累加上电后第一次连接bt且用写入操作时*/

            if(app_mosquito_killing_bt_connect_count < 0xFFFF)
            {
                app_mosquito_killing_bt_connect_count++;
            }

            __direct_printf("connect bt count[%d]\n", app_mosquito_killing_bt_connect_count);

            app_mosquito_killing_statistics_flag = 1; /*有修改，需要保存*/
        }

        switch(cmd)
        {
            case APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING:

                _u8Res = app_mosquito_killing_restore_default_setting();

                _u8Res = 1; /*远程触发带参数2返回 */
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_RESET_DEFAULT_SETTING, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, &_u8Res, 1);
                break;

            case APP_MOSQUITO_KILLING_CMD_INFO:
                if(len < (4+16))
                {
                    __direct_printf(" info len error[%d]!!!\n", len);
                }
                else
                {
                    memcpy((uint8_t *)&app_mosquito_killing_password, pdata, 4);
                    memcpy((uint8_t *)&app_mosquito_killing_name, pdata+4, 16);

                    app_mosquito_killing_mac_pid_write();
                }
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_INFO, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, NULL, 0);
                break;

            case APP_MOSQUITO_KILLING_CMD_DATE_TIME:
            {
                COBRA_RTC_TIME_DATE_T time;
                time.SUBSEC = 0;
                time.SEC    = pdata[6];
                time.MIN    = pdata[5];
                time.HOUR   = pdata[4];
                time.DAY    = pdata[3];
                time.MONTH  = pdata[2];
                time.YEAR   = ((pdata[0]<<8) | pdata[1])-2000;

                if((time.YEAR>99) || (time.MONTH>12) || (time.DAY>31) || (time.HOUR>23) || (time.MIN>59) || (time.SEC>59))
                {

                    __direct_printf("error app write date [%d-%d-%d %d:%d:%d] \n",time.YEAR, time.MONTH, time.DAY, time.HOUR, time.MIN, time.SEC);
                    return;
                }

                cobra_rtc_timer_write_en(1);
                cobra_rtc_set_current_time_date(&time);
                cobra_rtc_timer_write_en(0);

                app_mosquito_killing_save_date.year = ((pdata[0]<<8) | pdata[1]);
                app_mosquito_killing_save_date.month = pdata[2];
                app_mosquito_killing_save_date.day =  pdata[3];
                app_mosquito_killing_save_date.hour = pdata[4];
                app_mosquito_killing_save_date.min =  pdata[5];
                app_mosquito_killing_save_date.sec =  pdata[6];
                app_mosquito_killing_save_date.week = 0;

                __direct_printf("app write date [%d-%d-%d %d:%d:%d] \n",time.YEAR, time.MONTH, time.DAY, time.HOUR, time.MIN, time.SEC);
                __direct_printf("save date [%d-%d-%d %d:%d:%d] w[%d] \n",app_mosquito_killing_save_date.year, app_mosquito_killing_save_date.month, app_mosquito_killing_save_date.day, app_mosquito_killing_save_date.hour, app_mosquito_killing_save_date.min, app_mosquito_killing_save_date.sec, app_mosquito_killing_save_date.week);

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
                /*当第一次同步时间操作后，需要将没有时间时计数到暂存区的计数移到对应月的计数中，将暂存区的计数清零*/
//                if(app_mosquito_killing_timing_setting.time_sync_finished == 0)
                if (1)  /*可以多次同步修改时间*/
                {
//                    _u8Res = time.YEAR - 21;  /*以21年为基准，计算其对应的月份*/

                    /*基准年份已保存，不需要再次同步基准年份*/
                    if(app_mosquito_killing_year_base.valid != 0xAA55)
                    {
                        /*以21年为起始基准，计算其对应的月份；改为存储的第一次同步时间的年份*/
                        app_mosquito_killing_year_base.valid = 0xAA55;
                        app_mosquito_killing_year_base.year_base = 2000 + time.YEAR;
                    }

                    _u8Res = time.YEAR - (uint8_t)(app_mosquito_killing_year_base.year_base - 2000);

                    if((_u8Res>=0) && (_u8Res<10))
                    {
                        app_mosquito_killing_curr_month = _u8Res*12+time.MONTH - 1;

                        if((app_mosquito_killing_month_count[app_mosquito_killing_curr_month] + app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX - 1]) < APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX)
                        {
                            app_mosquito_killing_month_count[app_mosquito_killing_curr_month] += app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX - 1];

                        }
                        else
                        {
                            app_mosquito_killing_month_count[app_mosquito_killing_curr_month] = APP_MOSQUITO_KILLING_STATISTICS_COUNT_MAX;
                        }

                        app_mosquito_killing_month_count[APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX - 1] = 0;
                    }
                    else
                    {
                        __direct_printf("app write date error[%d]\n", time.YEAR);
                    }

                    app_mosquito_killing_timing_setting.time_sync_finished = 1;
                }
#endif
                app_mosquito_killing_statistics_flag = 1; /*有修改，需要保存*/

                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_DATE_TIME, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, NULL, 0);

                break;
            }

            case APP_MOSQUITO_KILLING_CMD_ON_OFF:
                app_mosquito_killing_work_state = pdata[0];
                app_mosquito_killing_control();

#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
                if(app_mosquito_killing_timing_setting.timing_auto_operate_flag != 0)  /*有定时操作*/
                {
                    app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 1;
                }
#endif

                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, &app_mosquito_killing_work_state, 1);
                break;

            case APP_MOSQUITO_KILLING_CMD_COUNT:
            {

                COBRA_RTC_TIME_DATE_T time;
                cobra_rtc_get_current_time_date(&time);
#if 0
                /*取消全部返回，还是改为先前的按参数年差值来返回一年的计数*/

                /*改为修改全部120个月的数据，附带有基准年份2个字节，只作为与存储的基准年份比较是否一致来验证使用，因为这个数据本身是程序上传给app的*/

                _u8Res = 0;
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, &_u8Res, 1);

                if(len<APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX)
                {
                    __direct_printf(" w scount len error[%d]!!!\n", len);
                }
                else
                {
                    u32Temp = (pdata[0]<<8)|pdata[1];
                    if(u32Temp == app_mosquito_killing_year_base.year_base)
                    {
                        for(i=0; i<APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX-2; i++)
                        {
                            app_mosquito_killing_month_count[i] = (pdata[i*2+2]<<8)|pdata[i*2+3];
                        }
                    }
                    else
                    {
                        __direct_printf(" w scount year base error[%d], right[%d]!!!\n", u32Temp, app_mosquito_killing_year_base.year_base);
                    }
                }

                _u8Res = 0;
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, &_u8Res, 1);
#endif

#if 1
//                _u8Res = pdata[0]; /*年的差值(1~10)，2021开始计数,第一年为1；后面紧接着第一月*/
//                if((_u8Res>0) && (_u8Res<=10))
//                {
//                    _u8Res = (_u8Res-1)*12; /*将年全部转换为月份写入月统计数组中*/
//                }
//                else
//                {
//                    _u8Res = 0;  /*出错时置0*/
//                }

                _u8Res = pdata[0];
                /*年的差值(1~10)，改为以第一次连接保存的基准年份来作为第一年，其值为当前时间年的差值+1;如当前是2023年，
                  如差值为1，就是当前2023年的数据，如差值为2，则是2022的数据，如超过基准年则返回错误*/
                if((app_mosquito_killing_year_base.valid != 0xAA55) || (_u8Res>((2000+time.YEAR) - app_mosquito_killing_year_base.year_base + 1)))
                {
                    _u8Temp[0] = 0xFF;
                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, (uint8_t *)_u8Temp, 1);

                    return;
                }

                if((_u8Res>0) && (_u8Res<=10))
                {
//                    _u8Res = (_u8Res-1)*12; /*将年全部转换为月份写入月统计数组中*/
                    _u8Res = ((2000+time.YEAR) - app_mosquito_killing_year_base.year_base +1 - _u8Res)*12; /*将年全部转换为月份写入月统计数组中,取值范围为0`10*/
                }
                else
                {
                    _u8Res = 0;  /*出错时置0*/

                    _u8Temp[0] = 0xFF;
                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, (uint8_t *)_u8Temp, 1);

                    return;
                }

                for(i=0; i<12; i++)
                {
                    app_mosquito_killing_month_count[_u8Res+i] = (pdata[i*2+1]<<8)| pdata[i*2+2];
                }

#if 1
                /*only for test*/
                __direct_printf("sss:\n");
                for(i=0; i< 24+1; i++)
                {
                    __direct_printf("[0x%02x]", pdata[i]);
                    if((i+1)%8==0)
                        __direct_printf("\n");
                }
                __direct_printf("\n");

                __direct_printf("ddd:\n");
                for(i=0; i< 12; i++)
                {
//					__direct_printf("%02d[0x%x][%d]  ",_u8Res+i, app_mosquito_killing_month_count[_u8Res+i], app_mosquito_killing_month_count[_u8Res+i]);
                    __direct_printf("%02d[0x%x]  ",_u8Res+i, app_mosquito_killing_month_count[_u8Res+i]);
                    if((i+1)%8==0)
                        __direct_printf("\n");
                }
                __direct_printf("\n");
#endif
				_u8Temp[0] = pdata[0];  /*r返回收到的对应年差值*/
                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, (uint8_t *)_u8Temp, 1);
#endif

                app_mosquito_killing_tm1650_disp_all_month_count();

				app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, NULL, 0);
                app_mosquito_killing_statistics_flag = 1; /*有修改，需要保存*/

                break;
            }

            case APP_MOSQUITO_KILLING_CMD_TIMING:
#if (APP_MOSQUITO_KILLING_TIMING_ENABLE == 1)
//				memcpy((uint8_t *)&app_mosquito_killing_data_timing,pdata,sizeof(mosquito_killing_timing_t));
                memcpy((uint8_t *)&app_mosquito_killing_data_timing,pdata,len);
                __direct_printf("bt wrtie timing data len[%d]\n", len);

                app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_TIMING, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, NULL, 0);
#endif

                app_mosquito_killing_statistics_flag = 1; /*有修改，需要保存*/
                #if 1
						     app_mosquito_killing_timing_setting.timing_auto_operate_flag = 0;
                 app_mosquito_killing_timing_setting.timing_key_operatoe_flag = 0;
						    #endif//Ganxiaoping debug timer bug 20220322
                break;

            case APP_MOSQUITO_KILLING_CMD_IP_WR:
                if(len>=6)
                {
                    app_mosquito_killing_bt_addr[0] = pdata[0];
                    app_mosquito_killing_bt_addr[1] = pdata[1];
                    app_mosquito_killing_bt_addr[2] = pdata[2];
                    app_mosquito_killing_bt_addr[3] = pdata[3];
                    app_mosquito_killing_bt_addr[4] = pdata[4];
                    app_mosquito_killing_bt_addr[5] = pdata[5];

                    _u8Res = app_mosquito_killing_mac_pid_write(); /*1--OK*/

                    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_IP_WR, APP_MOSQUITO_KILLING_CMD_TYPE_WRITE, &_u8Res, 1);
                }
                else
                {
                    __direct_printf("enter addr len error!!![%d]\n", len);
                }
                break;

            case APP_MOSQUITO_KILLING_CMD_OTA:
				app_mosquito_killing_tm1650_open();
				app_mosquito_killing_tm1650_ota_disp();
				cobra_soft_delay_ms(200);
				
                __direct_printf("sofreset...\n");
				cobra_soft_delay_ms(100);
                app_mosquito_killing_soft_reset();
                break;

            default:
                __direct_printf("w no cmd [0x%02x]\n", cmd);
                break;
        }
    }
	else
	{
		__direct_printf("!!!error no cmd type[0x%02x], cmd[0x%02x]\n", cmdType, cmd);
	}
}

void app_mosquito_killing_bt_data_init(void)
{
    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_ON_OFF, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, &app_mosquito_killing_work_state, 1);
    app_mosquito_killing_bt_data_send(APP_MOSQUITO_KILLING_CMD_COUNT, APP_MOSQUITO_KILLING_CMD_TYPE_RETURN, (uint8_t *)&app_mosquito_killing_month_count[app_mosquito_killing_curr_month], 2);
}

#endif

