#include <stdint.h>
#include <stdbool.h>
#include "light.h"
#include "cobra4A_spwm.h"
#include "cobra4a_mpwm.h"
#include "cobra_m4_gpio.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "rwip.h"
#include "arch.h" 
#include "cobra_pcrm.h"
#include "mesh_api.h"


#if (LIGHT_TYPE_SEL == LIGHT_TYPE_CT_HSL)
const light_res_hw_t light_res_hw[LIGHT_CNT][4] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G, RES_HW_PWM_B, RES_HW_PWM_W}, // vc can't use "[0]="
};
#elif (LIGHT_TYPE_SEL == LIGHT_TYPE_HSL || LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
const light_res_hw_t light_res_hw[LIGHT_CNT][3] = {
	/*[0] = */{RES_HW_PWM_B, RES_HW_PWM_G, RES_HW_PWM_R},
};
#elif (LIGHT_TYPE_SEL == LIGHT_TYPE_CT)
	#if XIAOMI_MODULE_ENABLE
const light_res_hw_t light_res_hw[LIGHT_CNT][3] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G, RES_HW_PWM_W},
};
#else
const light_res_hw_t light_res_hw[LIGHT_CNT][2] = {
	/*[0] = */{RES_HW_PWM_R, RES_HW_PWM_G},
};
#endif
#endif

#if (LIGHT_TYPE_HSL_EN)
typedef struct{
    float h;
    float s;
    float l;
}HSL_set;

typedef struct{
    uint8_t r;
    uint8_t g;
    uint8_t b;
}RGB_set;

extern mesh_ind_status ind_status;

float Hue_2_RGB(float v1,float v2,float vH){
    if(vH < 0){
        vH+=1;
    }
    if(vH > 1){
        vH-=1;
    }
    if((6.0*vH)<1){
        return (v1+(v2-v1)*6.0f*vH);
    }
    if((2.0*vH)<1){
        return (v2);
    }
    if((3.0*vH)<2){
        return (v1+(v2-v1)*((2.0f/3.0f)-vH)*6.0f);
    } 
    return(v1);
}

void HslToRgb(HSL_set hsl, RGB_set *rgb)
{
	float m1,m2;
	if(hsl.s==0){	
	    rgb->r=rgb->g=rgb->b=(uint8_t)(hsl.l*255);
	}
	else
	{
	    if(hsl.l<0.5){
		    m2=hsl.l*(1+hsl.s);
	    }
	    else{
		    m2=hsl.l+hsl.s-hsl.l*hsl.s;   	
	    }
	    m1=2*hsl.l-m2;
//	    rgb->g=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h+(1.0f/3.0f)));
//	    rgb->b=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h));
//	    rgb->r=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h-(1.0f/3.0f)));
	    rgb->r=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h+(1.0f/3.0f)));
	    rgb->g=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h));
	    rgb->b=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h-(1.0f/3.0f)));
//	    rgb->r=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h+(1.0f/3.0f)));
//	    rgb->g=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h));
//	    rgb->b=(uint8_t)(255*Hue_2_RGB(m1,m2,hsl.h-(1.0f/3.0f)));
	}	
}
#endif 


int16_t lum2level(uint8_t lum)
{
	if(lum > 100){
		lum  = 100;
	}
	return (-32768 + division_round(65535*lum,100));
}

uint8_t level2lum(int16_t level)
{
	uint16_t lightness = level + 32768;
	uint32_t fix_1p2 = 0;
	if(lightness){	// fix decimals
		#define LEVEL_UNIT_1P2	(65535/100/2)
		if(lightness < LEVEL_UNIT_1P2 + 2){     // +2 to fix accuracy missing
			lightness = LEVEL_UNIT_1P2 * 2;		// make sure lum is not zero when light on.
		}
		fix_1p2 = LEVEL_UNIT_1P2;
	}
	return (((lightness + fix_1p2)*100)/65535);
}


// 0-100%  (pwm's value index: this is pwm compare value, and the pwm cycle is 255*256)
const uint16_t rgb_lumen_map[101] = {
  0,2*256+128,2*256+192,3*256,3*256+128,4*256,4*256+128,5*256,5*256+128,6*256,  7*256,
      8*256,  9*256, 10*256, 11*256, 12*256, 13*256, 14*256, 15*256, 16*256, 17*256,
     18*256, 19*256, 21*256, 23*256, 25*256, 27*256, 29*256, 31*256, 33*256, 35*256,
     37*256, 39*256, 41*256, 43*256, 45*256, 47*256, 49*256, 51*256, 53*256, 55*256,
     57*256, 59*256, 61*256, 63*256, 65*256, 67*256, 69*256, 71*256, 73*256, 75*256,
     78*256, 81*256, 84*256, 87*256, 90*256, 93*256, 96*256, 99*256,102*256,105*256, 
    108*256,111*256,114*256,117*256,120*256,123*256,126*256,129*256,132*256,135*256,
    138*256,141*256,144*256,147*256,150*256,154*256,158*256,162*256,166*256,170*256,
    174*256,178*256,182*256,186*256,190*256,194*256,198*256,202*256,206*256,210*256,
    214*256,218*256,222*256,226*256,230*256,235*256,240*256,245*256,250*256,255*256,// must less or equal than (255*256)
};


void light_pwm_init()
{
    cobra_pcrm_mpwm_active_mode_clock_enable(1);
		cobra_pcrm_mpwm_sleep_mode_clock_enable(1);
		cobra_pcrm_mpwm_deep_sleep_mode_clock_enable(1);

    cobra_pcrm_spwm_active_mode_clock_enable(1);
		cobra_pcrm_spwm_sleep_mode_clock_enable(1);
		cobra_pcrm_spwm_deep_sleep_mode_clock_enable(1);
	
		cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_20);        //MCOA0    WARM  SOFTWARE
    cobra_map_pinmodeset(PIN5_INDEX,PIN_MODE_20);        //SPWM_OUT    R
    cobra_map_pinmodeset(PIN29_INDEX,PIN_MODE_12);        //MCOB2  COL
    cobra_map_pinmodeset(PIN27_INDEX,PIN_MODE_12);        //MCOB1  G  
    cobra_map_pinmodeset(PIN15_INDEX,PIN_MODE_0);        //MCOB0   B
    mpwm_close(MPWM_CHANNEL0_EN|MPWM_CHANNEL1_EN|MPWM_CHANNEL2_EN); 
    set_tc0(0);
    set_tc1(0);
    set_tc2(0);
    set_lim0(PMW_MAX_TICK);
    set_lim1(PMW_MAX_TICK);
    set_lim2(PMW_MAX_TICK);
    set_mat0(1000);
    set_mat1(1000);
    set_mat2(1000);  
    set_dt0(0);
    set_dt1(0);
    set_dt2(0);
		mpwm_en(MPWM_CHANNEL0_EN|MPWM_CHANNEL1_EN|MPWM_CHANNEL2_EN); 
		
		cobra_spwm_pwm_en(0);
		cobra_spwm_pwm_period_width(PMW_MAX_TICK);
		cobra_spwm_pwm_pulse_width(1000);
		cobra_spwm_pwm_deadband_width(0);
		cobra_spwm_pwm_en(1);
		cobra_spwm_pwm_a_enable(1);
}


uint32_t get_pwm_cmp(uint8_t val, uint8_t lum){
    if(lum >= ARRAY_SIZE(rgb_lumen_map) - 1){
        lum = ARRAY_SIZE(rgb_lumen_map) - 1;
    }
    uint32_t val_lumen_map = rgb_lumen_map[lum];
    
    return ((uint32_t)val * val_lumen_map) / 255;
}


void pwm_set_lum (int id, uint16_t y, int pol)
{
#if (IS_VC_PROJECT)
	#if DEBUG_MESH_DONGLE_IN_VC_EN
	extern int DebugMeshLed(int id, uint16_t y, int pol);
	DebugMeshLed(id, y, pol);
	#endif
#else
    uint32_t level = ((uint32_t)y * PWM_MAX_TICK) / (255*256);
		
#if DEBUG_CFG			
	printf("pwm_set_lum:%x,%x,%x,%x\n",level,id,PMW_MAX_TICK,pol);
#endif
		if(pol)
			level = PMW_MAX_TICK - level;
		if(id==0)
			set_mat0(level);
		else if(id==1)
			set_mat1(level);
		else if(id==2)
			set_mat2(level);		
#endif
}

void spwm_set_lum (uint16_t y, int pol)
{
#if (IS_VC_PROJECT)
	#if DEBUG_MESH_DONGLE_IN_VC_EN
	extern int DebugMeshLed(int id, uint16_t y, int pol);
	DebugMeshLed(id, y, pol);
	#endif
#else
    uint32_t level = ((uint32_t)y * PWM_MAX_TICK) / (255*256);
		
#if DEBUG_CFG			
	printf("spwm_set_lum:%x,%x\n",level,pol);
#endif	
		if(pol)
			level = PMW_MAX_TICK - level;
		cobra_spwm_pwm_pulse_width(level);
#endif
}


/**
 * @brief  Control lights on hardware.
 * @param  idx: 1st index of light_res_hw array, also means index 
 *   of LIGHT_CNT.
 * @param  idx2: The 2nd index of light_res_hw array.
 * @param  val: Value.
 * @retval None
 */
void light_dim_set_hw(int idx, uint16_t val)
{
//    if((idx < ARRAY_SIZE(light_res_hw)))
		{
		const light_res_hw_t *p_hw =(const light_res_hw_t *)&light_res_hw[0][idx]; 
		#if FEATURE_LOWPOWER_EN
		led_onoff_gpio(p_hw->gpio, 0 != val);
		#else
        pwm_set_lum(idx, val,p_hw->invert);
        #endif
    }
}


uint8_t light_soft_ctl(mesh_ind_status ind_status)
{
				uint16_t ctr_temp=0;
				uint8_t ct_100=0;
				uint8_t lum_100 = level2lum(ind_status.light_hsl_l);
					if(ind_status.light_ctr_temp<CT_MI_MIN){
							ctr_temp =0;
					}else if (ind_status.light_ctr_temp > CT_MI_MAX){
							ctr_temp = 100;
					}else {
							ctr_temp = ((ind_status.light_ctr_temp-CT_MI_MIN)*100)/(CT_MI_MAX-CT_MI_MIN);
					}
					//printf("light_hsl 2:%d,%d,%d\n",ind_status.light_ctr_temp,ctr_temp,lum_100);
					return ctr_temp*lum_100/100;
}

void scen_default_hsl(void)
{
		switch(ind_status.scens_id)
		{
			case SCENE_GOODNIGHT:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 199*65535/360;
				ind_status.light_hsl_s_target = 0.66*65535;
				ind_status.light_hsl_l_target = 0.79*65535;
#else
				ind_status.light_hsl_h = 0.199*65535;
				ind_status.light_hsl_s = 0.66*65535;
				ind_status.light_hsl_l = 0.79*65535;
#endif			
				break;
			case SCENE_READING:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 34*65535/360;
				ind_status.light_hsl_s_target = 1.0*65535;
				ind_status.light_hsl_l_target = 0.81*65535;
#else
				ind_status.light_hsl_h = 0.34*65535;
				ind_status.light_hsl_s = 1.0*65535;
				ind_status.light_hsl_l = 0.81*65535;
#endif			
				break;
			case SCENE_WORKING:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 41*65535/360;
				ind_status.light_hsl_s_target = 1.0*65535;
				ind_status.light_hsl_l_target = 0.5*65535;
#else
				ind_status.light_hsl_h = 0.41*65535;
				ind_status.light_hsl_s = 1.0*65535;
				ind_status.light_hsl_l = 0.5*65535;
#endif			
				break;
			case SCENE_LEISURE:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 45*65535/360;
				ind_status.light_hsl_s_target = 0.03*65535;
				ind_status.light_hsl_l_target = 0.73*65535;
#else
				ind_status.light_hsl_h = 0.45*65535;
				ind_status.light_hsl_s = 0.03*65535;
				ind_status.light_hsl_l = 0.73*65535;
#endif			
				break;
			case SCENE_SOFT:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 160*65535/360;
				ind_status.light_hsl_s_target = 0.96*65535;
				ind_status.light_hsl_l_target = 0.78*65535;
#else
				ind_status.light_hsl_h = 0.16*65535;
				ind_status.light_hsl_s = 0.96*65535;
				ind_status.light_hsl_l = 0.78*65535;
#endif			
				break;
			case SCENE_DAZZING:
#if (LIGHT_TYPE_LC_EN)				
				if(ind_status.scens_daz_id%3==0)
				{
					ind_status.light_hsl_h_target = 0;
					ind_status.light_hsl_s_target = 65535;
					ind_status.light_hsl_l_target = 0.5*65535;
				}
				if(ind_status.scens_daz_id%3==1)
				{
					ind_status.light_hsl_h_target = 127*65535/360;
					ind_status.light_hsl_s_target = 65535;
					ind_status.light_hsl_l_target = 0.5*65535;
				}
				if(ind_status.scens_daz_id%3==2)
				{				
					ind_status.light_hsl_h_target = 234*65535/360;
					ind_status.light_hsl_s_target = 65535;
					ind_status.light_hsl_l_target = 0.5*65535;
				}
#else
				if(ind_status.scens_daz_id%3==0)
				{
					ind_status.light_hsl_h = 0;					
					ind_status.light_hsl_s = 65535;
					ind_status.light_hsl_l = 0.5*65535;
				}
				if(ind_status.scens_daz_id%3==1)
					{
					ind_status.light_hsl_h = 0.127*65535;
					ind_status.light_hsl_s = 65535;
					ind_status.light_hsl_l = 0.5*65535;
					}
				if(ind_status.scens_daz_id%3==2)
				{				
					ind_status.light_hsl_h = 0.234*65535;
					ind_status.light_hsl_s = 65535;
					ind_status.light_hsl_l = 0.5*65535;
				}
#endif			
				ind_status.scens_daz_id++;
				break;
			case SCENE_CUSTOM1:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 161*65535/360;
				ind_status.light_hsl_s_target = 65535;
				ind_status.light_hsl_l_target = 0.5*65535;
#else
				ind_status.light_hsl_h = 0.161*65535;
				ind_status.light_hsl_s = 65535;
				ind_status.light_hsl_l = 0.5*65535;
#endif			
				break;
			case SCENE_CUSTOM2:
#if (LIGHT_TYPE_LC_EN)				
				ind_status.light_hsl_h_target = 281*65535/360;
				ind_status.light_hsl_s_target = 65535;
				ind_status.light_hsl_l_target = 0.5*65535;
#else
				ind_status.light_hsl_h = 0.281*65535;
				ind_status.light_hsl_s = 65535;
				ind_status.light_hsl_l = 0.5*65535;
#endif			
				break;
		}
}

void light_hsl()
{
				HSL_set HSL;
        RGB_set RGB;
				uint16_t ctr_temp=0;
				uint8_t ct_100=0;
//				if(ind_status.onoff==0)
//				{
//					ind_status.light_hsl_l = 0;
//					ind_status.light_hsl_s = 0;
//					ind_status.light_hsl_h = 0;					
//				}
//				else
//				{
//					if(ind_status.light_hsl_l + ind_status.light_hsl_s + ind_status.light_hsl_h == 0)
//					{
//						ind_status.light_hsl_h = HSL_HUE_DEFAULT;
//						ind_status.light_hsl_s = HSL_SAT_DEFAULT;
//						ind_status.light_hsl_l = LIGHTNESS_DEFAULT;
//					}
//				}
//#if DEBUG_CFG			
//				printf("light_hsl 1:%x,%x,%x\n",ind_status.light_hsl_h,ind_status.light_hsl_s,ind_status.light_hsl_l);
//#endif	
				if(ind_status.state_id >=MM_STATE_LIGHT_HSL_LN || ind_status.state_id ==MM_CFG_IDX_GENS_ONOFF )
				{	
					//light_dim_set_hw(0, get_pwm_cmp(0xff, level2lum(ind_status.level)));
					HSL.h = ((float)(ind_status.light_hsl_h)/65535.0f);
					HSL.s = ((float)(ind_status.light_hsl_s)/65535.0f);
					HSL.l = ((float)(ind_status.light_hsl_l)/65535.0f);
					HslToRgb(HSL,&RGB);
#if DEBUG_CFG			
					printf("light_hsl 2:%x,%x,%x\n",RGB.r,RGB.g,RGB.b);
#endif	
					light_dim_set_hw(0, get_pwm_cmp(0xff, RGB.b*100/255));
					light_dim_set_hw(1, get_pwm_cmp(0xff, RGB.g*100/255));
					//light_dim_set_hw(2, get_pwm_cmp(0xff, RGB.b*100/255));
					spwm_set_lum(get_pwm_cmp(0xff, RGB.r*100/255),1);
				}
				else
				{
					light_dim_set_hw(2, get_pwm_cmp(0xff, ind_status.temp_cycle));
				}
}
