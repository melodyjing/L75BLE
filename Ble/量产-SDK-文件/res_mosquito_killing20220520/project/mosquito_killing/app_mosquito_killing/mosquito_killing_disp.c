#include "mosquito_killing_disp.h"

#if (APP_MOSQUITO_KILLING_HD_I2C_ENABLE == 1)
#include "cobra_i2c.h"
#endif

#if (APP_MOSQUITO_KILLING_ENABLE == 1)
#include "mosquito_killing_data.h"

static void app_mosquito_killing_disp_comm_init(void)
{
    cobra_map_pinmodeset(APP_MOSQUITO_KILLING_CLK_PIN_INDEX, APP_MOSQUITO_KILLING_CLK_PIN_MODE);
    cobra_pin_drain_source_enable(APP_MOSQUITO_KILLING_CLK_PIN_INDEX, 1);
//    cobra_pin_pullup_enable(APP_MOSQUITO_KILLING_CLK_PIN_INDEX, 1);
    cobra_pin_pullup_enable(APP_MOSQUITO_KILLING_CLK_PIN_INDEX, 0);
    app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_CLK_PIN_NUMBER, GPIO_OUTPUT);

    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_CLK_PIN_NUMBER, GPIO_HIGH);


    cobra_map_pinmodeset(APP_MOSQUITO_KILLING_DAT_PIN_INDEX, APP_MOSQUITO_KILLING_DAT_PIN_MODE);
    cobra_pin_drain_source_enable(APP_MOSQUITO_KILLING_DAT_PIN_INDEX, 1);
//    cobra_pin_pullup_enable(APP_MOSQUITO_KILLING_DAT_PIN_INDEX, 1);
    cobra_pin_pullup_enable(APP_MOSQUITO_KILLING_DAT_PIN_INDEX, 0);
    app_m3_gpio_set_direction(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER, GPIO_OUTPUT);

    app_m3_gpio_write_pin(APP_MOSQUITO_KILLING_DAT_PIN_NUMBER, GPIO_HIGH);

}

static uint8_t stCommArray[APP_MOSQUITO_KILLING_TM1650_COMM_LEN_MAX];
/*0~9对应的写入指令数据, 7段*/
static uint8_t stDigitalValue[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
/*每位对应地址*/
static uint8_t stDigitalAddr[] = {0x68, 0x6A, 0x6C, 0x6E};
/*中横线0x40=='-'；0x79=='E'; 0x77=='R'/'A'*/
static uint8_t stCharValue[] = {0x40, 0x79, 0x77};


#if (APP_MOSQUITO_KILLING_HD_I2C_ENABLE == 1)
static void app_mosquito_killing_disp_i2c0_init(void)
{
    __direct_printf("cobra_i2c0[0x%x], cobra_i2c1[0x%x], apb0 clk[%d]\n", cobra_i2c0, cobra_i2c1, cobra_system_get_m3_apb0_clk());

    cobra_map_pinmodeset(PIN1_INDEX, PIN_MODE_2);
    cobra_map_pinmodeset(PIN2_INDEX, PIN_MODE_2);

    cobra_map_pinmodeset(PIN3_INDEX, PIN_MODE_2);
    cobra_map_pinmodeset(PIN4_INDEX, PIN_MODE_2);

    cobra_pin_input_enable(PIN3_INDEX, 1);
    cobra_pin_input_enable(PIN4_INDEX, 1);

    cobra_pin_pullup_enable(PIN3_INDEX, 1);
    cobra_pin_pullup_enable(PIN4_INDEX, 1);

    cobra_pcrm_i2c0_active_mode_clock_enable(1);


//    rdw_i2c_set_bit_rate(cobra_i2c0, cobra_system_get_m3_apb0_clk(), 100000);
//    rdw_i2c_set_bit_rate(cobra_i2c0, cobra_system_get_m3_apb0_clk(), 50000);
    rdw_i2c_set_bit_rate(cobra_i2c0, cobra_system_get_m3_apb0_clk(), 10000);
    rdw_i2c_master_enable(cobra_i2c0);

//    cobra_soft_delay_ms(100);

//	__direct_printf("finished \n");
}

bool app_mosquito_killing_disp_write_cmd(volatile COBRA_I2C_TypeDef *i2c, /*uint8_t slave_address, */uint8_t reg, uint8_t data)
{

    __direct_printf("hd i2c w[0x%x], [0x%x]\n", reg, data);

    bool stat = 0;
    stat = rdw_i2c_write_byte (i2c, RDW_I2C_TX_START|RDW_I2C_TX_STOP|RDW_I2C_ACK_LAST );
    if(stat!=0)
        return stat;
//    stat = rdw_i2c_write_byte (i2c, 2+1);
    stat = rdw_i2c_write_byte (i2c, 2);
    if(stat!=0)
        return stat;
//    stat = rdw_i2c_write_byte (i2c, (slave_address | RDW_I2C_WRITE));
//    if(stat!=0)
//        return stat;
    stat = rdw_i2c_write_byte (i2c, reg & 0xff);
    if(stat!=0)
        return stat;
    stat = rdw_i2c_write_byte (i2c, data& 0xff);
    if(stat!=0)
        return stat;
    return stat;
}

static void app_mosquito_killing_disp_comm_write(uint8_t *pData, uint8_t len)
{
    uint8_t res = 0;
    res = app_mosquito_killing_disp_write_cmd(cobra_i2c0, pData[0], pData[1]);

    if(res!= 0)
    {
        __direct_printf("i2c failed*****************, res[%d]\n", res);
    }
}
#else

#if 0
static void app_mosquito_killing_disp_comm_write(uint8_t *pData, uint8_t len)
{
    uint8_t i=0,j=0,value=0;
    uint8_t count = 0;

    __direct_printf("wdata[0x%x][0x%x]\n", pData[0], pData[1]);

    GLOBAL_INT_DISABLE();

    /*start*/
    SET_CLK_HIGH;
    SET_DAT_HIGH;
    COMM_DELAY;
    SET_DAT_LOW;

    COMM_DELAY;
    SET_CLK_LOW;  /*ready input data*/
    COMM_DELAY;

    for(j=0; j<len; j++)
    {
        count = 0;
        value = pData[j];

        for(i=0; i<8; i++)
        {

            if(value & 0x80)
            {
                SET_DAT_HIGH;
                COMM_DELAY;
                SET_CLK_HIGH;
                COMM_DELAY;
                SET_CLK_LOW;

                COMM_DELAY;
                SET_DAT_LOW;
                COMM_DELAY;
            }
            else
            {
                SET_DAT_LOW;
                COMM_DELAY;
                SET_CLK_HIGH;
                COMM_DELAY;
                SET_CLK_LOW;

                COMM_DELAY;
                SET_DAT_LOW;
                COMM_DELAY;
            }
            value =  value<<1;
        }

        SET_DAT_HIGH;  /*先把数据置高后再设置为输入状态, 等待外围拉低ACK*/
        SET_DAT_INPUT;
        COMM_DELAY;
//		GET_DAT_STATUS;
//		GET_DAT_STATUS;
        while(GET_DAT_STATUS == GPIO_HIGH) /*wait ACK*/
        {
            if(count < 100)
            {
                count++;
                COMM_DELAY;
            }
            else
            {
                printf("no ACK [0x%02x]\n",pData[j]);
                break;
            }
        }
        SET_DAT_OUTPUT;

        SET_CLK_HIGH;  /*release DATA*/
        COMM_DELAY;
        SET_CLK_LOW;   /*ready input data*/
        COMM_DELAY;

//		printf("ACK count[%d]\n", count);

    }

    SET_DAT_LOW;
    SET_CLK_HIGH;
    COMM_DELAY;
    SET_DAT_HIGH;

    GLOBAL_INT_RESTORE();
}

#else
static void app_mosquito_killing_disp_comm_write(uint8_t *pData, uint8_t len)
{
    uint8_t i=0,j=0,value=0;
    uint8_t count = 0;

//	__direct_printf("wdata[0x%x][0x%x]\n", pData[0], pData[1]);

    /*start*/
    SET_CLK_HIGH;
    SET_DAT_HIGH;
    COMM_DELAY;
    SET_DAT_LOW;

    COMM_DELAY;
    SET_CLK_LOW;  /*ready input data*/

    for(j=0; j<len; j++)
    {
        count = 0;
        value = pData[j];

        for(i=0; i<8; i++)
        {
            COMM_DELAY;

            if(value & 0x80)
            {
                SET_DAT_HIGH;
                COMM_DELAY;
                COMM_DELAY;
                SET_CLK_HIGH;
                COMM_DELAY;
                COMM_DELAY;
                COMM_DELAY;
                COMM_DELAY;
                COMM_DELAY;
                SET_CLK_LOW;
            }
            else
            {
                SET_DAT_LOW;
                COMM_DELAY;
                COMM_DELAY;
                SET_CLK_HIGH;
                COMM_DELAY;
                COMM_DELAY;
                COMM_DELAY;
                COMM_DELAY;
                COMM_DELAY;
                SET_CLK_LOW;
            }
            value =  value<<1;
        }

        SET_DAT_HIGH;
        SET_DAT_INPUT;
        COMM_DELAY;
//		GET_DAT_STATUS;
//		GET_DAT_STATUS;
        while(GET_DAT_STATUS == GPIO_HIGH) /*wait ACK*/
        {
            if(count < 100)
            {
                count++;
                COMM_DELAY;
            }
            else
            {
                printf("no ACK [0x%02x]\n",pData[j]);
                break;
            }
        }

        SET_CLK_HIGH;  /*release DATA*/
        COMM_DELAY;
        COMM_DELAY;
        COMM_DELAY;
        COMM_DELAY;
        COMM_DELAY;
        SET_CLK_LOW;
        COMM_DELAY;

//		printf("ACK count[%d]\n", count);
        SET_DAT_OUTPUT;
        COMM_DELAY;


    }

    SET_DAT_LOW;
    SET_CLK_HIGH;
    COMM_DELAY;
    SET_DAT_HIGH;

    COMM_DELAY;  /*following new add*/
    SET_CLK_LOW;
    SET_DAT_LOW;

}
#endif

#endif

static void app_mosquito_killing_tm1650_init(uint8_t paraBrightness, uint8_t paramSeg, uint8_t paramWorkMode, uint8_t paramOnOffMode)
{
    uint8_t _u8Setting = 0;
    static uint8_t _u8SettingBak = 0;

    _u8Setting |= ((paraBrightness<<4)&0xF0);
    _u8Setting |= ((paramSeg<<3)&0x08);
    _u8Setting |= ((paramWorkMode<<2)&0x04);
    _u8Setting |= ((paramOnOffMode)&0x01);


    if(_u8SettingBak != _u8Setting)
    {
        _u8SettingBak = _u8Setting;
        __direct_printf("tm1650 setting [0x%02x]\n", _u8Setting);
    }

    stCommArray[0] = 0x48;
    stCommArray[1] = _u8Setting;

    app_mosquito_killing_disp_comm_write(stCommArray, 2);
}

void app_mosquito_killing_disp_init(void)
{
    app_mosquito_killing_disp_comm_init();

#if (APP_MOSQUITO_KILLING_HD_I2C_ENABLE == 1)
    app_mosquito_killing_disp_i2c0_init();
#endif

    /*每次写入显示数据后后会重新设置其setting*/
//    app_mosquito_killing_tm1650_init(APP_MOSQUITO_KILLING_TM1650_DEF_BRIGHTNESS, APP_MOSQUITO_KILLING_TM1650_DEF_SEG, APP_MOSQUITO_KILLING_TM1650_DEF_WORK_MODE, APP_MOSQUITO_KILLING_TM1650_DEF_ONOFF_MODE);
}

void app_mosquito_killing_tm1650_open(void)
{
//    app_mosquito_killing_tm1650_init(APP_MOSQUITO_KILLING_TM1650_DEF_BRIGHTNESS, APP_MOSQUITO_KILLING_TM1650_DEF_SEG, 1, 1);
    app_mosquito_killing_tm1650_init(APP_MOSQUITO_KILLING_TM1650_DEF_BRIGHTNESS, APP_MOSQUITO_KILLING_TM1650_DEF_SEG, 0, 1);
}
void app_mosquito_killing_tm1650_close(void)
{
//    app_mosquito_killing_tm1650_init(APP_MOSQUITO_KILLING_TM1650_DEF_BRIGHTNESS, APP_MOSQUITO_KILLING_TM1650_DEF_SEG, 1, 0);
    app_mosquito_killing_tm1650_init(APP_MOSQUITO_KILLING_TM1650_DEF_BRIGHTNESS, APP_MOSQUITO_KILLING_TM1650_DEF_SEG, 0, 0);
}

#if 1

#if 0
/*最大显示3位数，即999*/
void app_mosquito_killing_tm1650_disp(uint32_t paramDataDisp)
{
    __direct_printf("display data[%d]\n", paramDataDisp);

    static uint8_t res00_cur = 0xFF;
    static uint8_t res01_cur = 0xFF;
    static uint8_t res02_cur = 0xFF;

    res00_cur = paramDataDisp%10;
    res01_cur = (paramDataDisp%100)/10;
    res02_cur = (paramDataDisp%1000)/100;

//    GLOBAL_INT_DISABLE();

    stCommArray[0] = stDigitalAddr[1];
    stCommArray[1] = stDigitalValue[res00_cur];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    if((res01_cur > 0) || (res02_cur > 0))
    {
        stCommArray[0] = stDigitalAddr[2];
        stCommArray[1] = stDigitalValue[res01_cur];
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }
    else
    {
        stCommArray[0] = stDigitalAddr[2];
        stCommArray[1] = 0x00;
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }

    if(res02_cur > 0)
    {
        stCommArray[0] = stDigitalAddr[3];
        stCommArray[1] = stDigitalValue[res02_cur];
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }
    else
    {
        stCommArray[0] = stDigitalAddr[3];
        stCommArray[1] = 0x00;
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }

//	GLOBAL_INT_RESTORE();

    app_mosquito_killing_tm1650_open();
}
#endif

#if 1
/*最大显示3位数，即999*/
void app_mosquito_killing_tm1650_disp(uint32_t paramDataDisp)
{
    __direct_printf("dd[%d]\n", paramDataDisp);

    static uint8_t res00_pre = 0xFF;
    static uint8_t res00_cur = 0xFF;
    static uint8_t res01_pre = 0xFF;
    static uint8_t res01_cur = 0xFF;
    static uint8_t res02_pre = 0xFF;
    static uint8_t res02_cur = 0xFF;

    res00_cur = paramDataDisp%10;
    res01_cur = (paramDataDisp%100)/10;
    res02_cur = (paramDataDisp%1000)/100;


    stCommArray[0] = stDigitalAddr[1];
    stCommArray[1] = stDigitalValue[(res00_cur)];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);


    if(res01_cur != res01_pre)
    {
        res01_pre = res01_cur;

        if((res01_cur>0) || (res02_cur>0))
        {
            stCommArray[0] = stDigitalAddr[2];
            stCommArray[1] = stDigitalValue[(res01_cur)];
            app_mosquito_killing_disp_comm_write(stCommArray, 2);
        }
        else
        {
            stCommArray[0] = stDigitalAddr[2];
            stCommArray[1] = 0x00;
            app_mosquito_killing_disp_comm_write(stCommArray, 2);
        }
    }
    else if((res01_cur == 0) && (res02_cur != 0))
    {
        stCommArray[0] = stDigitalAddr[2];
        stCommArray[1] = stDigitalValue[(res01_cur)];
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }
    else  if((res01_cur == 0) && (res02_cur == 0))
    {
        stCommArray[0] = stDigitalAddr[2];
        stCommArray[1] = 0x00;
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }


    if(res02_cur != res02_pre)
    {
        res02_pre = res02_cur;

        if(res02_cur>0)
        {
            stCommArray[0] = stDigitalAddr[3];
            stCommArray[1] = stDigitalValue[(res02_cur)];
            app_mosquito_killing_disp_comm_write(stCommArray, 2);
        }
        else
        {
            stCommArray[0] = stDigitalAddr[3];
            stCommArray[1] = 0x00;
            app_mosquito_killing_disp_comm_write(stCommArray, 2);
        }
    }
	else if(res02_cur == 0)
	{
        stCommArray[0] = stDigitalAddr[3];
        stCommArray[1] = 0x00;
        app_mosquito_killing_disp_comm_write(stCommArray, 2);		
	}

    app_mosquito_killing_tm1650_open();
}
#endif

#else
/*最大显示3位数，即999*/
void app_mosquito_killing_tm1650_disp(uint32_t paramDataDisp)
{
    __direct_printf("display data[%d]\n", paramDataDisp);

    /*发现有时随机的会关屏，也没有操作close*/
//	app_mosquito_killing_tm1650_open();

    /*没有第四位，不需要*/
//    stCommArray[0] = stDigitalAddr[0];
//    stCommArray[1] = 0x00;
//	app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[1];
    stCommArray[1] = stDigitalValue[(paramDataDisp%10)];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    if(((paramDataDisp%100)/10 > 0) || ((paramDataDisp%1000)/100 > 0))
    {
        stCommArray[0] = stDigitalAddr[2];
        stCommArray[1] = stDigitalValue[(paramDataDisp%100)/10];
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }
    else
    {
        stCommArray[0] = stDigitalAddr[2];
        stCommArray[1] = 0x00;
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }

    if((paramDataDisp%1000)/100 > 0)
    {
        stCommArray[0] = stDigitalAddr[3];
        stCommArray[1] = stDigitalValue[(paramDataDisp%1000)/100];/*取百位*/
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }
    else
    {
        stCommArray[0] = stDigitalAddr[3];
        stCommArray[1] = 0x00;
        app_mosquito_killing_disp_comm_write(stCommArray, 2);
    }

    /*发现有时随机的会关屏，也没有操作close*/
//	app_mosquito_killing_tm1650_open();
}
#endif

void app_mosquito_killing_tm1650_disp_all_month_count(void)
{
	int i = 0;
	uint32_t _u32Temp = 0;

	for(i=0; i< APP_MOSQUITO_KILLING_STATISTICS_MONTH_MAX; i++)
	{
		_u32Temp += app_mosquito_killing_month_count[i];
	}

//	if(_u32Temp >APP_MOSQUITO_KILLING_TM1650_DISP_COUNT_MAX)
//	{
//		app_mosquito_killing_tm1650_disp(APP_MOSQUITO_KILLING_TM1650_DISP_COUNT_MAX);
//	}
//	else		
	{
		app_mosquito_killing_tm1650_disp(_u32Temp);  /*只显示后面三位数*/
	}
}

void app_mosquito_killing_tm1650_disp_clear(void)
{
    stCommArray[0] = stDigitalAddr[1];
    stCommArray[1] = 0x00;
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[2];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[3];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);
}

void app_mosquito_killing_tm1650_restore_disp(void)
{
    stCommArray[0] = stDigitalAddr[1];
    stCommArray[1] = stCharValue[0];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[2];
    stCommArray[1] = stCharValue[0];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[3];
    stCommArray[1] = stCharValue[0];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);
}

/*-0-*/
void app_mosquito_killing_tm1650_ota_disp(void)
{
    stCommArray[0] = stDigitalAddr[1];
    stCommArray[1] = stCharValue[0];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[2];
    stCommArray[1] = stDigitalValue[0];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);

    stCommArray[0] = stDigitalAddr[3];
    stCommArray[1] = stCharValue[0];
    app_mosquito_killing_disp_comm_write(stCommArray, 2);
}


void app_mosquito_killing_tm1650_error(uint8_t param)
{
    switch(param)
    {
        case APP_MOSQUITO_KILLING_TM1650_ERROR_PROTECT_SWITCH:

            stCommArray[0] = stDigitalAddr[1];
            stCommArray[1] = stCharValue[2];
            app_mosquito_killing_disp_comm_write(stCommArray, 2);

            stCommArray[0] = stDigitalAddr[2];
            stCommArray[1] = stCharValue[2];
            app_mosquito_killing_disp_comm_write(stCommArray, 2);

            stCommArray[0] = stDigitalAddr[3];
            stCommArray[1] = stCharValue[1];
            app_mosquito_killing_disp_comm_write(stCommArray, 2);

            break;

        default:
            __direct_printf("error code display[0x%x]\n", param);
            break;
    }
}

void app_mosquito_killing_tm1650_disp_debug(uint8_t param)
{
    __direct_printf("debug display data[%d]\n", param);

    stCommArray[0] = stDigitalAddr[3];
    stCommArray[1] = param;
    app_mosquito_killing_disp_comm_write(stCommArray, 2);
}

void app_mosquito_killing_tm1650_brightness_debug(uint8_t param)
{
    __direct_printf("debug brightness data[%d], (0~7,0=max)\n", param);

    app_mosquito_killing_tm1650_init(param, APP_MOSQUITO_KILLING_TM1650_DEF_SEG, 0, 1);
}

#endif

