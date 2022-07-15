#include "cobra_bm_detect.h"
#include "cobra_pinmux.h"
#include "cobra4A_spwm.h"
#include "cobra4A_mpwm.h"
#include "cobra_pcrm.h"
#include "app_tulip_comm.h"
#include "app_mesh_util.h"
#include "cobra_system.h"
#include "rdw_mcu.h"

#if (BM_DETECT)

#define BM_THRE            30
#define BM_COUNT           5
#if ( SOFT_PWM == 1)
#define BM_MV_THRE         3650
#else
#if (COBRA_EMU_BOARD)
#define BM_MV_THRE         3650
#else
#define BM_MV_THRE         3700		
#endif
#endif
static uint32_t  reset_timer_id;
void poweroff_reset(void);
void shutdown_all_light_in_poweroff(void);
#if (APP_FLASH_LIGHT_NUMBERS)
void read_power_on_nubmer(void);
#endif	
uint16_t bm_dataget(void)
{
        uint16_t tmbm_data[16] = {0};
        uint16_t tmbm_value = 0;
        int mode = 1 ;
        int pin = 27;
        int data_len = 8;
	      uint16_t i=0;
			int sum = 0,average = 0;
			//0. configure DPIN29 to neither input,nor output, and PE disable to get high Z

		  data_len +=8;

			 
			REG_PL_WR(AON_PD_BASE+AON_CONFIG_PROTECT,1);
			//1. tester tx 1 xxkHz sinewave to DPIN29 for gpadc test
			//2. configure aon_hclk divider to div=1
			REG_PL_WR(AON_PD_BASE+AON_HF_CLK_CFG,(REG_PL_RD(AON_PD_BASE+AON_HF_CLK_CFG)&(~AON_AHB_CLK_CFG_MASK)));
			//3. switch on sw_adc3v3
			REG_PL_WR(AON_PD_BASE+AON_ANA_CFG0,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG0)&(~D2A_VDD3_ADC_EN_MASK))|D2A_VDD3_ADC_EN_MASK);
			//4. configure for ldoadc and gpadc work mode
			REG_PL_WR(AON_PD_BASE+AON_ANA_CFG2,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG2)&(~0x03))|0x03);
			REG_PL_WR(AON_PD_BASE+AON_ANA_CFG2,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG2)&(~0x1C0)));
			//5. config gpadc mode
			REG_PL_WR(AON_PD_BASE+AON_ANA_CFG1,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG1)&(~D2A_BTGPADC_MASK))|((7-(pin-24))<<16)|(1<<23)|(mode<<25));
			//6. enable adc data captureed to fifo
			REG_PL_WR(COBRA_UMCU_BASE+0x010,(REG_PL_RD(COBRA_UMCU_BASE+0x010)&(~0x03))|(3<<0));
			//7. check if gpadc fifo is empty
			//8. if not empty, cpu repeat read data from gpadc fifo. 
			while(1)
			{
				if(!(REG_PL_RD(COBRA_UMCU_BASE+0x048)&((1<<2))))
				{
					tmbm_data[i++] = REG_PL_RD(COBRA_UMCU_BASE+0x080)&0x7FFF ;
				}
				if(i==data_len)break;
			}
			//10. disable adc data captureed to fifo
			REG_PL_WR(COBRA_UMCU_BASE+0x010,(REG_PL_RD(COBRA_UMCU_BASE+0x010)&(~0x03)));

			for(i=8;i<data_len;i++)
            {
                sum += tmbm_data[i];
            }
            average = (int)(sum*1.0/(data_len - 8)+0.5);
            tmbm_value = average;
            return tmbm_value;
}

void bm_detect(void)
{
	static uint16_t bm_last = 0,bm_last_last = 0;
	uint16_t bm_current = 0;
	static uint16_t count = 0;
    uint8_t appkey_flag = 0;
    uint8_t temp = MESH_APPKEY_OK_LENGTH;
	static uint8_t only_one=1;

	uint32_t reg = REG_BLE_RD(0x40089108);  //0xa000000 // 0xc000000   //0x1000000

	uint8_t repeat = get_repeat_reset();

	if(++count >= BM_COUNT  && ((reg != 0xc000000) || repeat))
	{
		count = 0;
		bm_current = bm_dataget();

		if((bm_last_last != 0) && (bm_last < bm_last_last) && (bm_current < bm_last) && (bm_current <= BM_MV_THRE) && only_one)
		{
			set_repeat_reset(0);
			only_one = 0;
//printf("bm_detect\n");



			GLOBAL_INT_STOP();

#if (COBRA_POWER_DOWN_SAVE_ENABLE==1)
			app_mesh_drop_count();
#endif			
			app_mesh_data_save();

			if(nvds_get(MESH_APPKEY_OK_FLAG, (uint8_t *)&temp, &appkey_flag) != NVDS_OK)
			{
				appkey_flag = 0;
			}
			
      //设备未配网时, 不需要保存配网信息
			if(appkey_flag)
				cobra_system_nvds_save_to_eflash();

			cobra_soft_delay_ms(300);
      shutdown_all_light_in_poweroff();
			REG_PL_WR(0x40089100,1);
			REG_PL_WR(0x40089104,(REG_PL_RD(0x40089104)&(~(0x20000)))|(0x20000));
			REG_PL_WR(0x40089100,0);
			REG_PL_WR(0x400E200C,1);
		}
		else 
		{
		   read_power_on_nubmer();
		}
    	bm_last_last = bm_last;
		  bm_last = bm_current;
	}	
}
void shutdown_all_light_in_poweroff(void)
{
	#if (SOFT_PWM == 0)
			cobra_spwm_pwm_a_enable(0);
			cobra_spwm_pwm_b_enable(0);
			cobra_spwm_pwm_deadband_en(0);
			cobra_pcrm_mpwm_active_mode_clock_enable(0);
			cobra_pcrm_spwm_active_mode_clock_enable(0);
			cobra_map_pinmodeset(PIN15_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN27_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN28_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN29_INDEX,PIN_MODE_14);
			app_m3_gpio_set_direction(GPIO_PIN_15, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_28, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_INPUT);
#else
			cobra_pcrm_mpwm_active_mode_clock_enable(0);
			cobra_pcrm_spwm_active_mode_clock_enable(0);
			cobra_map_pinmodeset(PIN5_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN15_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN27_INDEX,PIN_MODE_14);
			cobra_map_pinmodeset(PIN29_INDEX,PIN_MODE_14);

			app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_15, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_INPUT);
			app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_INPUT);

#endif
}
#if (COBRA_POWER_DOWN_SAVE_ENABLE==1)
void save_data_before_ota(void)
{
	   uint8_t appkey_flag = 0;
	   uint8_t temp = MESH_APPKEY_OK_LENGTH;
	  
	   unsigned char ch;
	   uint8_t *p_buffer;	
	  
	   cobra_flash_erase(M4_EFLASH_MESH_ADDRESS, M0_M3_NVDS_SIZE);	
	   ch = 0xff;
	   p_buffer = (uint8_t * )M4_EFLASH_COUNT_TIMEOUT_ADDRESS;
		 cobra_flash_write((unsigned int)p_buffer, sizeof(ch),	(unsigned char *)&ch);	
	
		 app_mesh_data_save();

		 if(nvds_get(MESH_APPKEY_OK_FLAG, (uint8_t *)&temp, &appkey_flag) != NVDS_OK)
		 {
			 appkey_flag = 0;
		 }
		 //设备未配网时, 不需要保存配网信息
		 if(appkey_flag)
		 {
			 cobra_system_nvds_save_to_eflash();	
		 }
}
#endif


#endif