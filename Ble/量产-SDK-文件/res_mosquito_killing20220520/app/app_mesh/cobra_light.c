/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_light.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2021/3/23

	Description :   
       mesh 灯的控制

	Changed Log	:

		Liujian  2021/3/23			- Creation

*****************************************************************************/
#include "cobra_light.h"
#include "cobra_pcrm.h"
#include "cobra_pinmux.h"
#include "cobra4A_spwm.h"
#include "mesh_api.h"
#include "cobra_soft_timer.h"
#include "rdw_gpio.h"
#include "cobra_timer.h"
#include "rwip_config.h"  
#include "cobra4A_mpwm.h"
#include "cobra_system.h"
#include "cobra4A_spwm.h"
#include "app_tulip_comm.h"
#include "app_mesh_tscns.h"

#define BOARD_IS_TULIPX    1 

#if ((BLE_APP_MESH == 1) && (BLE_APP_MESH_TULIPE_LIGHT == 1))

//////////////////////////////////////////////////////////////////////////////////////
MCPWM_CHANNEL_CFG_Type  led_channel_setup[3];

#define BLDC_PERIOD    		  255                   //LED 的灯的周期
#define BLDC_STOP_DUTY      BLDC_PERIOD - 1       //
#define BLDC_NORMAL_DUTY    200


#define LEVEL_UNIT_1P2	   (65535/100/2)


#if (BOARD_IS_TULIPX == 1)
#define LED_LIGHT_POLAR   MCPWM_CHANNEL_PASSIVE_LO
#else
#define LED_LIGHT_POLAR   MCPWM_CHANNEL_PASSIVE_HIGH
#endif


static uint32_t  mesh_scens_timer_delay_id=0xff;


static  unsigned char soft_pwm_duty;    
static  unsigned char soft_pwm_base;
static  unsigned char soft_pwm_flag;



void app_ctl_warm_init(void)
{
    	soft_pwm_duty = 0;
	    soft_pwm_base = 0;
	    soft_pwm_flag = 0;
}

void app_ctl_set_warm_duty(unsigned char duty)
{
#if (SOFT_PWM == 1)
	  soft_pwm_duty = duty;
#else	
				cobra_map_pinmodeset(PIN8_INDEX,  PIN20_SPWM_OUT);
				cobra_map_pinmodeset(PIN28_INDEX,  PIN28_SPWM_OUT_N);
			  app_cobra_spwm_set_ctl(36000, duty);
	
#endif		
}

void app_ctl_warm(void)
{
#if (SOFT_PWM == 1)		

		soft_pwm_base++;
	
		if(soft_pwm_base > 100)
		{
			  soft_pwm_base = 0;
		}
		
	
		if((soft_pwm_base == 0) && soft_pwm_duty)
		{
#if COBRA_EMU_BOARD
			app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_LOW);
#else			
			app_m3_gpio_write_pin(GPIO_PIN_8,GPIO_HIGH );
#endif			
		}
		else if(soft_pwm_base == soft_pwm_duty)
		{
#if COBRA_EMU_BOARD
			app_m3_gpio_write_pin(GPIO_PIN_8,GPIO_HIGH );
#else			
			app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_LOW);
#endif			
		}

		
#endif
}


// CT 模式下 lightness 转换
uint8_t  app_cobra_light_level_2_lum(uint16_t level)
{
	uint16_t lightness = level;
	uint32_t fix_1p2 = 0;
	
	if(lightness)
	{	// fix decimals
		
		if(lightness < LEVEL_UNIT_1P2 + 2)
		{     // +2 to fix accuracy missing
			 lightness = LEVEL_UNIT_1P2 * 2;		// make sure lum is not zero when light on.
		}
		
		fix_1p2 = LEVEL_UNIT_1P2;
	}
	
	
	
//	return (((lightness + fix_1p2)*100)/65535);
    return (((lightness)*100)/65535);
	

	
}

// use for driver pwm, 0--100 is absolute value, not related to temp range
uint8_t app_cobra_temp_2_temp100(uint16_t temp) 
{
	 if(temp < CTL_TEMP_MIN)
	 {
		  temp = CTL_TEMP_MIN;
	 }
	
	 if(temp > CTL_TEMP_MAX)
	 {
		  temp = CTL_TEMP_MAX;
	 }
	 
	 uint32_t fix_1p2 = (CTL_TEMP_MAX - CTL_TEMP_MIN)/100/2;	// fix decimals
	
	 return (((temp - CTL_TEMP_MIN + fix_1p2)*100)/(CTL_TEMP_MAX - CTL_TEMP_MIN));   // temp100 can be zero.
	
}



void app_color_hsl_to_rgb(COLOR_HSL * hsl, COLOR_RGB * rgb)
{
	    float h, s, l;
	    float q, p, Tr, Tg, Tb;
	    float COLOR_R, COLOR_G, COLOR_B;
	    
	    h = hsl->h;
      s = hsl->s;
      l = hsl->l;
	
	    if(s == 0)
			{
				  rgb->r = rgb->g = rgb->b = hsl->l * 255;
			}
			
		 if (l < 0.5) 
		 {
        q = l * (1 + s);
     }
		 else
		 {
        q = l + s - (l * s);
     }
		 
     p = 2 * l - q;
     Tr = h + 1/3.0;
     Tg = h;
     Tb = h - 1/3.0;
    
     if (Tr < 0) 
		 {
        Tr = Tr + 1.0;
     }
		 else if (Tr > 1) 
		 {
        Tr = Tr - 1.0;
     }
     if (Tg < 0) 
		 {
        Tg = Tg + 1.0;
     }
		 else if (Tg > 1) 
		 {
        Tg = Tg - 1.0;
     }
     if (Tb < 0) 
		 {
        Tb = Tb + 1.0;
     } 
		 else if (Tb > 1) 
		 {
        Tb = Tb - 1.0;
     }
    
    if (Tr < 1/6.0) 
		{
        COLOR_R = p + ((q - p) * 6 * Tr);
    }
		else if (Tr >= 1/6.0 && Tr < 1/2.0) 
		{
        COLOR_R = q;
    }
		else if (Tr >= 1/2.0 && Tr < 2/3.0) 
		{
        COLOR_R = p + ((q - p) * 6 * (2/3.0 - Tr));
    }
		else
		{
        COLOR_R = p;
    }
    
    if (Tg < 1/6.0) 
		{
        COLOR_G = p + ((q - p) * 6 * Tg);
    }
		else if (Tg >= 1/6.0 && Tg < 1/2.0) {
        COLOR_G = q;
    }
		else if (Tg >= 1/2.0 && Tg < 2/3.0) 
		{
        COLOR_G = p + ((q - p) * 6 * (2/3.0 - Tg));
    }
		else
		{
        COLOR_G = p;
    }
    
    if (Tb < 1/6.0) 
		{
        COLOR_B = p + ((q - p) * 6 * Tb);
    }
		else if (Tb >= 1/6.0 && Tb < 1/2.0) 
		{
        COLOR_B = q;
    }
		else if (Tb >= 1/2.0 && Tb < 2/3.0) 
		{
        COLOR_B = p + ((q - p) * 6 * (2/3.0 - Tb));
    }
		else
		{
        COLOR_B = p;
    }
	
	  rgb->r = COLOR_R*255;
		rgb->g = COLOR_G*255;
		rgb->b = COLOR_B*255; 
	
}

/*
 duty 是占空比      0 - 255
 frequency  是 SPWM 的频率, 单位HZ
*/
static void app_cobra_spwm_set_red(unsigned int frequency, unsigned int duty)
{
#if (SOFT_PWM == 1)		
	  unsigned int apb_frequency = cobra_system_get_m3_apb1_clk();
	
	  unsigned int count = apb_frequency/frequency;
	  
    duty = duty%256;
	
		cobra_spwm_pwm_period_width(count-1);
	  
	  cobra_spwm_pwm_pulse_width(count*duty/256);	   
#endif	
}

#if (SOFT_PWM == 0)
static void app_cobra_spwm_set_ctl(unsigned int frequency, unsigned int duty)
{
	  uint8_t lum_100 = app_cobra_light_get_lum();
	
	  unsigned int apb_frequency = cobra_system_get_cpu_frequency();
	
	  unsigned int count = apb_frequency/frequency;
	  
	
		cobra_spwm_pwm_period_width(count-1);
	  
	  cobra_spwm_pwm_pulse_width(count*duty/100+(100-lum_100)*1333/2/100);
	
		cobra_spwm_pwm_deadband_width((100-lum_100)*1333/2/100);
}
#endif

static void app_cobra_light_init_red_light(void)
{
#if (SOFT_PWM == 1)		
	    cobra_map_pinmodeset(PIN5_INDEX, PIN5_SPWM_OUT);
	
	    cobra_spwm_pwm_a_enable(1);
#if (COBRA_EMU_BOARD)	
			cobra_spwm_pola_a_set(PASSIVE_HIGH_ACTIVE_LOW);
#else			
			cobra_spwm_pola_a_set(PASSIVE_LOW_ACTIVE_HIGH);
#endif
		  app_cobra_spwm_set_red(12000, 0);
	
	    cobra_spwm_pwm_en(1);
#endif	
}

void app_cobra_light_init(void)
{
	   cobra_pcrm_mpwm_active_mode_clock_enable(1);
		 cobra_pcrm_mpwm_sleep_mode_clock_enable(1);
		 cobra_pcrm_mpwm_deep_sleep_mode_clock_enable(1);

     cobra_pcrm_spwm_active_mode_clock_enable(1);
		 cobra_pcrm_spwm_sleep_mode_clock_enable(1);
		 cobra_pcrm_spwm_deep_sleep_mode_clock_enable(1);
	
	   cobra_map_pinmodeset(PIN15_INDEX,  PIN15_MPWM_MCOB0);	
	
	   cobra_map_pinmodeset(PIN27_INDEX,  PIN27_MPWM_MCOB1);
	
#if (SOFT_PWM == 1)		
	   cobra_map_pinmodeset(PIN29_INDEX,  PIN29_MPWM_MCOB2);
	
	   app_m3_gpio_set_direction(GPIO_PIN_8, GPIO_OUTPUT);
	
     app_m3_gpio_write_pin(GPIO_PIN_8, GPIO_LOW);
	
	   app_cobra_light_init_red_light();
	
#else       ////// //SOFT_PWM == 0
	    cobra_map_pinmodeset(PIN29_INDEX,  PIN29_MPWM_MCOB2);
	    cobra_map_pinmodeset(PIN8_INDEX,   PIN20_SPWM_OUT);
	    cobra_map_pinmodeset(PIN28_INDEX,  PIN28_SPWM_OUT_N);
			
#if (COBRA_EMU_BOARD)
			cobra_spwm_pola_a_set(PASSIVE_HIGH_ACTIVE_LOW);
			cobra_spwm_pola_b_set(PASSIVE_HIGH_ACTIVE_LOW);
#else
			cobra_spwm_pola_a_set(PASSIVE_LOW_ACTIVE_HIGH);   //PASSIVE_HIGH_ACTIVE_LOW
			cobra_spwm_pola_b_set(PASSIVE_LOW_ACTIVE_HIGH);
#endif
		 	
	    cobra_spwm_pwm_a_enable(1);

	    cobra_spwm_pwm_b_enable(1);
		  app_cobra_spwm_set_red(12000, 0);
	
	    cobra_spwm_pwm_en(1);
	
		  cobra_spwm_pwm_deadband_en(1);
			
#endif	
	

	   app_cobra_mcpwm_Init();

//soft pwm	
     app_ctl_warm_init();
		 
#if (SOFT_PWM == 1)			 
	   app_cobra_init_timer1(10000);
		 
		 app_cobra_timer1_set_callback(app_ctl_warm);
		
		 app_cobra_timer1_enable(1);
		 
		 app_ctl_set_warm_duty(0);
		 
		 app_cobra_light_set_cool_duty(0);
#else
		app_cobra_light_disable_white_light();
#endif		 
		 
///////////////////////////////////////////////////		
		 
//=====================================================================================
	   led_channel_setup[MCPWM_CHANNEL_0].channelType = MCPWM_CHANNEL_EDGE_MODE;
	   led_channel_setup[MCPWM_CHANNEL_0].channelPolarity = LED_LIGHT_POLAR;
	   led_channel_setup[MCPWM_CHANNEL_0].channelDeadtimeEnable = DISABLE;
	   led_channel_setup[MCPWM_CHANNEL_0].channelDeadtimeValue = 0;
	
	   led_channel_setup[MCPWM_CHANNEL_0].channelUpdateEnable = ENABLE;
	   led_channel_setup[MCPWM_CHANNEL_0].channelTimercounterValue = 0;
	   led_channel_setup[MCPWM_CHANNEL_0].channelPeriodValue = BLDC_PERIOD;
	   led_channel_setup[MCPWM_CHANNEL_0].channelPulsewidthValue = BLDC_STOP_DUTY;
//======================================================================================
	   led_channel_setup[MCPWM_CHANNEL_1].channelType = MCPWM_CHANNEL_EDGE_MODE;
	   led_channel_setup[MCPWM_CHANNEL_1].channelPolarity = LED_LIGHT_POLAR;
	   led_channel_setup[MCPWM_CHANNEL_1].channelDeadtimeEnable = DISABLE;
	   led_channel_setup[MCPWM_CHANNEL_1].channelDeadtimeValue = 0;
	
	   led_channel_setup[MCPWM_CHANNEL_1].channelUpdateEnable = ENABLE;
	   led_channel_setup[MCPWM_CHANNEL_1].channelTimercounterValue = 0;
	   led_channel_setup[MCPWM_CHANNEL_1].channelPeriodValue = BLDC_PERIOD;
	   led_channel_setup[MCPWM_CHANNEL_1].channelPulsewidthValue = BLDC_STOP_DUTY;
//=======================================================================================
	   led_channel_setup[MCPWM_CHANNEL_2].channelType = MCPWM_CHANNEL_EDGE_MODE;
	   led_channel_setup[MCPWM_CHANNEL_2].channelPolarity = LED_LIGHT_POLAR;
	   led_channel_setup[MCPWM_CHANNEL_2].channelDeadtimeEnable = DISABLE;
	   led_channel_setup[MCPWM_CHANNEL_2].channelDeadtimeValue = 0;
	
	   led_channel_setup[MCPWM_CHANNEL_2].channelUpdateEnable = ENABLE;
	   led_channel_setup[MCPWM_CHANNEL_2].channelTimercounterValue = 0;
	   led_channel_setup[MCPWM_CHANNEL_2].channelPeriodValue = BLDC_PERIOD;
	   led_channel_setup[MCPWM_CHANNEL_2].channelPulsewidthValue = BLDC_STOP_DUTY;
//=======================================================================================	 
	  
	   app_cobra_mcpwm_config_channel(MCPWM_CHANNEL_0, &led_channel_setup[MCPWM_CHANNEL_0]);
	   app_cobra_mcpwm_config_channel(MCPWM_CHANNEL_1, &led_channel_setup[MCPWM_CHANNEL_1]);
		 app_cobra_mcpwm_config_channel(MCPWM_CHANNEL_2, &led_channel_setup[MCPWM_CHANNEL_2]);
		 
		
}

void app_cobra_light_enable (void)   
{   
   
    /* Enable the MCPWM DC mode and all output pins */   
   // app_cobra_mcpwm_dc_mode(ENABLE, DISABLE, ( 0 ));   
   
    /* Set the PWM output value */   
    led_channel_setup[0].channelPulsewidthValue = 100;   
   
    /* Write to the shadow register */   
    app_cobra_mcpwm_write_to_shadow(0, &led_channel_setup[0]);   
   
    /* Disable LIM0 and MAT0 interrupt flag */   
    app_cobra_mcpwm_int_config(MCPWM_INTFLAG_LIM0, DISABLE);   
    app_cobra_mcpwm_int_config(MCPWM_INTFLAG_MAT0, DISABLE);   
   
    /* Disable LIM1 and MAT1 interrupt flag */   
    app_cobra_mcpwm_int_config(MCPWM_INTFLAG_LIM1, DISABLE);   
    app_cobra_mcpwm_int_config(MCPWM_INTFLAG_MAT1, DISABLE);   
   
    /* Disable LIM2 and MAT2 interrupt flag */   
    app_cobra_mcpwm_int_config(MCPWM_INTFLAG_LIM2, DISABLE);   
    app_cobra_mcpwm_int_config(MCPWM_INTFLAG_MAT2, DISABLE);   
   
    /* Start up the MCPWM */   
    app_cobra_mcpwm_start(ENABLE, ENABLE, ENABLE);   
     
}   

void app_cobra_light_set_duty (unsigned int duty)   
{   
    /* Set the PWM output value */   
    led_channel_setup[0].channelPulsewidthValue = duty;   
    /* Write to the shadow register */   
    app_cobra_mcpwm_write_to_shadow(0, &led_channel_setup[0]);   
   
    /* Set the PWM output value */   
    led_channel_setup[1].channelPulsewidthValue = duty;   
    /* Write to the shadow register */   
    app_cobra_mcpwm_write_to_shadow(1, &led_channel_setup[1]);   
   
    /* Set the PWM output value */   
    led_channel_setup[2].channelPulsewidthValue = duty;   
    /* Write to the shadow register */   
    app_cobra_mcpwm_write_to_shadow(2, &led_channel_setup[2]);   
} 


//设置蓝光
void app_cobra_light_set_blue_duty(unsigned char duty)
{
	  if(duty > 0)
		{
			 cobra_map_pinmodeset(PIN15_INDEX,  PIN15_MPWM_MCOB0);	
	     /* Set the PWM output value */   
#if COBRA_EMU_BOARD
       led_channel_setup[0].channelPulsewidthValue = 255-duty;   
#else
       led_channel_setup[0].channelPulsewidthValue = duty;   
#endif
       /* Write to the shadow register */   
       app_cobra_mcpwm_write_to_shadow(0, &led_channel_setup[0]); 
		}
    else
    {
#if COBRA_EMU_BOARD
        cobra_map_pinmodeset(PIN15_INDEX,PIN_MODE_15);
#else			
			  app_m3_gpio_set_direction(GPIO_PIN_15, GPIO_OUTPUT);
        app_m3_gpio_write_pin(GPIO_PIN_15, GPIO_LOW);
#endif			
    }	
	
	  
}

//设置绿光
void app_cobra_light_set_green_duty(unsigned char duty)
{
	  if(duty > 0)
		{
			 cobra_map_pinmodeset(PIN27_INDEX,  PIN27_MPWM_MCOB1);
	     /* Set the PWM output value */   
#if COBRA_EMU_BOARD
       led_channel_setup[1].channelPulsewidthValue = 255-duty;   
#else			
       led_channel_setup[1].channelPulsewidthValue = duty;   
#endif			
       /* Write to the shadow register */   
       app_cobra_mcpwm_write_to_shadow(1, &led_channel_setup[1]);   
		}
		else
		{
#if COBRA_EMU_BOARD
        cobra_map_pinmodeset(PIN27_INDEX,PIN_MODE_15);
#else			
			  app_m3_gpio_set_direction(GPIO_PIN_27, GPIO_OUTPUT);
        app_m3_gpio_write_pin(GPIO_PIN_27, GPIO_LOW);
#endif			
		}
	
}

//设置红光
void app_cobra_light_set_red_duty(unsigned char duty)
{
#if (SOFT_PWM == 1)			
	  if(duty < 0xff)
		{
			  cobra_map_pinmodeset(PIN5_INDEX, PIN5_SPWM_OUT);
			  app_cobra_spwm_set_red(12000, duty);
		}
		else
		{
#if COBRA_EMU_BOARD
        cobra_map_pinmodeset(PIN5_INDEX,PIN_MODE_14);
#else			
			  app_m3_gpio_set_direction(GPIO_PIN_5, GPIO_OUTPUT);
        app_m3_gpio_write_pin(GPIO_PIN_5, GPIO_HIGH);
#endif	
		}
#else
	  if(duty > 0)
		{
			 cobra_map_pinmodeset(PIN29_INDEX,  PIN29_MPWM_MCOB2);
	     /* Set the PWM output value */   
			#if COBRA_EMU_BOARD
       led_channel_setup[2].channelPulsewidthValue = 255-duty;
			#else
       led_channel_setup[2].channelPulsewidthValue = duty;
			#endif
       /* Write to the shadow register */   
       app_cobra_mcpwm_write_to_shadow(2, &led_channel_setup[2]);   
		}
		else
		{
#if COBRA_EMU_BOARD
        cobra_map_pinmodeset(PIN29_INDEX,PIN_MODE_15);
#else			
			  app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_OUTPUT);
        app_m3_gpio_write_pin(GPIO_PIN_29, GPIO_LOW);
#endif			
		}

#endif	
}

//设置冷光
void app_cobra_light_set_cool_duty(unsigned char duty)
{
#if (SOFT_PWM == 1)			
	  if(duty > 0)
		{
			 cobra_map_pinmodeset(PIN29_INDEX,  PIN29_MPWM_MCOB2);
	     /* Set the PWM output value */   
#if COBRA_EMU_BOARD	
       led_channel_setup[2].channelPulsewidthValue = 255-duty;   
#else
       led_channel_setup[2].channelPulsewidthValue = duty;   
#endif
       /* Write to the shadow register */   
       app_cobra_mcpwm_write_to_shadow(2, &led_channel_setup[2]);   
		}
		else
		{
#if COBRA_EMU_BOARD
        cobra_map_pinmodeset(PIN29_INDEX,PIN_MODE_15);
#else				
			  app_m3_gpio_set_direction(GPIO_PIN_29, GPIO_OUTPUT);
        app_m3_gpio_write_pin(GPIO_PIN_29, GPIO_LOW);
#endif	
		}
#endif
	
}

//关掉调色光
void app_cobra_light_disable_white_light(void)
{
#if (SOFT_PWM == 1)	
	 app_cobra_light_set_cool_duty(0);
	 app_ctl_set_warm_duty(0);
#else
	
#if COBRA_EMU_BOARD
   cobra_map_pinmodeset(PIN28_INDEX,PIN_MODE_15);
   cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_15);
#else				
   cobra_map_pinmodeset(PIN28_INDEX,PIN_MODE_14);
   cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_14);	
#endif	
	
#endif	
}

//关掉彩光
void app_cobra_light_disable_color_light(void)
{
	 app_cobra_light_set_red_duty(0);
	 app_cobra_light_set_green_duty(0);
	 app_cobra_light_set_blue_duty(0);
}


static uint32_t  app_mesh_tscns_time_id;

void app_cobra_tuiple_light_timer_callback(uint32_t param)
{
     app_mesh_tscns_inc_tai_seconds();	
}

void app_cobra_tuiple_light_timer_init(void)
{
	   app_mesh_tscns_time_id = cobra_soft_timer_create(app_cobra_tuiple_light_timer_callback);
	
	   cobra_soft_timer_start(app_mesh_tscns_time_id, COBRA_SOFT_TIMER_TICK, 1);
}

#endif

