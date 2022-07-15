/********************************************************************************************************
 * @file     light.h 
 *
 * @brief    for TLSR chips
 *
 * @author	 Radiawave
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Radiawave Technologies  (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Radiawave 
 * 		     Technologies  (shenzhen) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Radiawave Technologies  (shenzhen) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "light_config.h"
#include "cobra_pinmux.h"
#include "mesh_config.h"


/** @addtogroup Mesh_Common
  * @{
  */
  
/** @defgroup Light
  * @brief Light Code.
  * @{
  */

#define LIGHT_TYPE_HSL_EN		1

#if (TULIPX_MESH)
#define	SW_PWM_EN								1
#define LIGHT_TYPE_LC_EN		1
#endif

#define RES_HW_PWM_R    {PWM_R,1}
#define RES_HW_PWM_G    {PWM_G,1}
#define RES_HW_PWM_B    {PWM_B,1}
#define RES_HW_PWM_W    {PWM_W,0}


#define LED_INDICATE_VAL    (rgb_lumen_map[100])
#define LED_INDICATE_LUM_VAL LED_INDICATE_VAL
#define CLOCK_SYS_CLOCK_HZ  (48000000UL)

#if(CLOCK_SYS_CLOCK_HZ == 48000000)
#define PWM_CLK_DIV_LIGHT   (1)
#else
#define PWM_CLK_DIV_LIGHT   (0)
#endif

#ifndef PWM_FREQ
#define PWM_FREQ	        (600)   // unit: Hz
#endif
#define PWM_MAX_TICK        ((CLOCK_SYS_CLOCK_HZ / (PWM_CLK_DIV_LIGHT + 1)) / PWM_FREQ)
#define PMW_MAX_TICK		PWM_MAX_TICK

#define LED_MASK							0x07
#define	config_led_event(on,off,n,sel)		(on | (off<<8) | (n<<16) | (sel<<24))

#define	LED_EVENT_FLASH_4HZ_10S				config_led_event(2,2,40,LED_MASK)
#define	LED_EVENT_FLASH_STOP				config_led_event(1,1,1,LED_MASK)
#define	LED_EVENT_FLASH_2HZ_2S				config_led_event(4,4,4,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_1S				config_led_event(8,8,1,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_2S				config_led_event(8,8,2,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_3S				config_led_event(8,8,3,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_4S				config_led_event(8,8,4,LED_MASK)
#define	LED_EVENT_FLASH_4HZ					config_led_event(2,2,0,LED_MASK)
#define	LED_EVENT_FLASH_1HZ					config_led_event(8,8,0,LED_MASK)
#define	LED_EVENT_FLASH_4HZ_1T				config_led_event(2,2,1,LED_MASK)
#define	LED_EVENT_FLASH_4HZ_2T				config_led_event(2,2,2,LED_MASK)
#define	LED_EVENT_FLASH_4HZ_3T				config_led_event(2,2,3,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_1T				config_led_event(1,15,1,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_3T				config_led_event(8,8,3,LED_MASK)
#define	LED_EVENT_FLASH_2HZ_1T				config_led_event(4,4,1,LED_MASK)
#define	LED_EVENT_FLASH_2HZ_2T				config_led_event(4,4,2,LED_MASK)

#if HOMEKIT_EN
//////////////////////////////////////////////////////////////////////
//    LED
//////////////////////////////////////////////////////////////////////
#define LED_EVENT_FLASH_STOP                config_led_event(1,1,1,LED_MASK)

//#define LED_EVENT_FLASH_1HZ                 config_led_event(100,100,250,LED_MASK)
#define LED_EVENT_FLASH_1HZ_2TIMES          config_led_event(8,8,2*2,LED_MASK)//config_led_event(100,100,2*2,LED_MASK)
#define LED_EVENT_FLASH_1HZ_3TIMES          config_led_event(8,8,3*2,LED_MASK)
#define LED_EVENT_FLASH_1HZ_4TIMES          config_led_event(8,8,4*2,LED_MASK)
#define LED_EVENT_FLASH_1HZ_5TIMES          config_led_event(8,8,5*2,LED_MASK)
#define LED_EVENT_FLASH_1HZ_10TIMES         config_led_event(8,8,10*2,LED_MASK)
#define LED_EVENT_FLASH_1HZ_1_SECOND        config_led_event(8,8,1*2*1,LED_MASK)
#define LED_EVENT_FLASH_1HZ_2_SECOND        config_led_event(8,8,2*2*1,LED_MASK)
#define LED_EVENT_FLASH_1HZ_3_SECOND        config_led_event(8,8,3*2*1,LED_MASK)
#define LED_EVENT_FLASH_1HZ_4_SECOND        config_led_event(8,8,4*2*1,LED_MASK)
#define LED_EVENT_FLASH_1HZ_5_SECOND        config_led_event(8,8,5*2*1,LED_MASK)

#define LED_EVENT_FLASH_2HZ                 config_led_event(4,4,250,LED_MASK)
#define LED_EVENT_FLASH_2HZ_2TIMES          config_led_event(4,4,2*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_3TIMES          config_led_event(4,4,3*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_4TIMES          config_led_event(4,4,4*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_5TIMES          config_led_event(4,4,5*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_10TIMES         config_led_event(4,4,10*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_1_SECOND        config_led_event(4,4,1*2*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_2_SECOND        config_led_event(4,4,2*2*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_3_SECOND        config_led_event(4,4,3*2*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_4_SECOND        config_led_event(4,4,4*2*2,LED_MASK)
#define LED_EVENT_FLASH_2HZ_5_SECOND        config_led_event(4,4,5*2*2,LED_MASK)
#define	LED_EVENT_FLASH_1HZ_HK				config_led_event(8,8,250,LED_MASK)
#define	LED_EVENT_FLASH_4HZ_HK				config_led_event(2,2,250,LED_MASK)

//#define LED_EVENT_FLASH_4HZ                 config_led_event(25,25,250,LED_MASK)
#define LED_EVENT_FLASH_4HZ_5_SECOND        config_led_event(2,2,5*2*4,LED_MASK)

#define LED_EVENT_FLASH_5HZ                 config_led_event(2,2,250,LED_MASK)//config_led_event(20,20,250,LED_MASK)
#define LED_EVENT_FLASH_5HZ_3TIMES          config_led_event(2,2,3*2,LED_MASK)
#define LED_EVENT_FLASH_5HZ_5_SECOND        config_led_event(2,2,5*2*5,LED_MASK)

#endif

#define LEVEL_MIN      			(-32767)
#define LEVEL_MAX      			(32767)
#if (0 == LIGHT_PAR_USER_EN)
#define LIGHTNESS_MIN      		(1)			// can not set 0
#define LIGHTNESS_MAX      		(0xFFFF)
#define CTL_TEMP_MIN			(0x0320)	// 800
#define CTL_TEMP_MAX			(0x4E20)	// 20000
#define CTL_D_UV_MIN			(-32768)
#define CTL_D_UV_MAX			(32767)
#define HSL_HUE_MIN				(0)
#define HSL_HUE_MAX				(0xFFFF)
#define HSL_SAT_MIN				(0)
#define HSL_SAT_MAX				(0xFFFF)
#define XYL_X_MIN				(0)
#define XYL_X_MAX				(0xFFFF)
#define XYL_Y_MIN				(0)
#define XYL_Y_MAX				(0xFFFF)
#define LC_DEFAULT_MIN	(0x1000)

#define ONOFF_DEFAULT   	    (1)
#define LIGHTNESS_DEFAULT   	(LIGHTNESS_MAX)
#define CTL_TEMP_DEFAULT   		(CTL_TEMP_MAX)
#define CTL_D_UV_DEFAULT   		(0)
#define HSL_HUE_DEFAULT   		(HSL_HUE_MAX)
#define HSL_SAT_DEFAULT   		(HSL_SAT_MAX)
#define XYL_X_DEFAULT   		(XYL_X_MAX)
#define XYL_Y_DEFAULT   		(XYL_Y_MAX)
#endif

#define LEVEL_OFF				(-32768)
#define LUM_OFF					(0)

#define CT_MI_MIN		2700
#define CT_MI_MAX		6500


#define GPIO_LED	PWM_R

//--------------------------------------- node composition data
#ifndef LIGHT_CNT
#if (LIGHT_TYPE_SEL == LIGHT_TYPE_PANEL)
#define LIGHT_CNT                       (3)     // means instance count
#else
#define LIGHT_CNT                       (1)     // means instance count
#endif
#endif
#define ELE_CNT                         (LIGHT_CNT * ELE_CNT_EVERY_LIGHT)


#if (FEATURE_LOWPOWER_EN)
#define ONPOWER_UP_SELECT       ONPOWER_UP_DEFAULT
#elif (LIGHT_TYPE_SEL == TYPE_TOOTH_BRUSH)
#define ONPOWER_UP_SELECT       ONPOWER_UP_OFF
#else
#define ONPOWER_UP_SELECT       ONPOWER_UP_DEFAULT // ONPOWER_UP_STORE // 
#endif

enum ST_TRANS_TYPE{
	ST_TRANS_LIGHTNESS  	= 0,	// share with power level
	#if (LIGHT_TYPE_CT_EN)
	ST_TRANS_CTL_TEMP,
	#endif
	#if (LIGHT_TYPE_HSL_EN)
	ST_TRANS_HSL_HUE,
	ST_TRANS_HSL_SAT,
	#endif
    #if (LIGHT_TYPE_SEL == LIGHT_TYPE_XYL)
	ST_TRANS_XYL_X,
	ST_TRANS_XYL_Y,
    #endif
    #if (LIGHT_TYPE_CT_EN)
	ST_TRANS_CTL_D_UV,              // no level model related, assign at the end should be better.
    #endif
	#if (MI_API_ENABLE)
	ST_TRANS_MI_VENDOR_STS,
	#endif
	ST_TRANS_MAX,
    ST_TRANS_PUB_ONOFF = ST_TRANS_MAX,  // just use for publish,
};

enum{
	ST_PUB_TRANS_IDLE  	= 0,    // must 0
	ST_PUB_TRANS_ING,
	ST_PUB_TRANS_ALL_OK,        //
	ST_PUB_TRANS_MAX,
};

enum{
	OP_LC_ONOFF_TYPE_NONE   = 0,    // must 0
	OP_LC_ONOFF_TYPE_ON,
	OP_LC_ONOFF_TYPE_OFF,
};

enum{
		SCENE_GOODNIGHT=1,
		SCENE_READING,
		SCENE_WORKING,
		SCENE_LEISURE,
		SCENE_SOFT,
		SCENE_DAZZING,
		SCENE_CUSTOM1,
		SCENE_CUSTOM2,
};

typedef struct{
    uint32_t gpio;
    uint8_t invert;
}light_res_hw_t;

typedef struct{
	uint8_t st[ST_TRANS_MAX + 1];    // + 1: for onoff publish flag
// follwing parmeters is for inputting.	
	uint8_t no_dim_refresh_flag;     // input parameter
	uint8_t hsl_set_cmd_flag;        //
}st_pub_list_t;  // st_level_set_pub_list_t

typedef struct{
	uint16_t min;
	uint16_t max;
}light_range_u16_t;

typedef struct{
	int16_t min;
	int16_t max;
}light_range_s16_t;

typedef struct{
    int16_t last;           // CT was save in level
	int16_t def;			// default, all value transfer into int16_t, and CT not related to range
	int16_t min;			// all value transfer into int16_t, and CT not related to range
	int16_t max;			// all value transfer into int16_t, and CT not related to range
    uint8_t onoff;			// save for on power up
	uint8_t rsv;
}sw_level_save_t;

typedef struct{
	int32_t step_1p32768;   // (1 / 32768 level unit)
	uint32_t remain_t_ms;	// unit ms: max 26bit: 38400*1000ms
	uint16_t delay_ms;		// unit ms
    int16_t present;        // all value transfer into level, include CT.
    int16_t present_1p32768;// (1 / 32768 level unit)
    int16_t target;
}st_transition_t;

typedef struct{
	sw_level_save_t level[ST_TRANS_MAX];
	// may add member here later
}light_res_sw_save_t;	// need save

typedef struct{
	st_transition_t trans[ST_TRANS_MAX];	// state transition
	uint16_t linear;
	uint8_t linear_set_flag;
	// may add member here later
}light_res_sw_trans_t;	// no need save


typedef struct{
    uint32_t onoff;
    uint32_t level;
    uint32_t level_target;
    uint32_t light_ln;
    uint32_t light_ln_target;
		uint32_t light_hsl_h;
		uint32_t light_hsl_h_target;
		uint32_t light_hsl_s;
		uint32_t light_hsl_s_target;
		uint32_t light_hsl_l;
		uint32_t light_hsl_l_target;
		uint32_t light_ctr_ln;
		uint32_t light_ctr_ln_target;
		uint32_t light_ctr_temp;
		uint32_t light_ctr_temp_target;
		uint16_t scens_id;
		uint8_t  scens_daz_id;
		uint32_t seconds;
		uint32_t time1;
	  uint32_t trans_time_ms;
		uint16_t state_id;
		uint8_t  mdl_lid[MESH_MODEL_NB_MAX];
		uint32_t model_id[MESH_MODEL_NB_MAX];
		uint8_t  total_mdl;
		uint8_t  temp_cycle;
		uint8_t  flash_time;
}mesh_ind_status;


extern light_res_sw_save_t light_res_sw_save[LIGHT_CNT];
extern light_res_sw_trans_t light_res_sw[LIGHT_CNT];
extern mesh_ind_status ind_status;


#define ARRAY_SIZE(a) 			(sizeof(a) / sizeof(*a))

#define everyN(i, n) 			++(i); (i)=((i) < N ? (i) : 0); if(0 == (i))

#define U16_HI(a)    (((a) >> 8) & 0xFF)
#define U16_LO(a)    ((a) & 0xFF)

#define U32_BYTE0(a) ((a) & 0xFF)
#define U32_BYTE1(a) (((a) >> 8) & 0xFF)
#define U32_BYTE2(a) (((a) >> 16) & 0xFF)
#define U32_BYTE3(a) (((a) >> 24) & 0xFF)

#define foreach(i, n) 			for(unsigned int i = 0; i < (n); ++i)
#define foreach_range(i, s, e) 	for(unsigned int i = (s); i < (e); ++i)
#define foreach_arr(i, arr) 	for(unsigned int i = 0; i < ARRAY_SIZE(arr); ++i)
//  round robbin foreach,   ??????????,??,  h ????????????,??????????h ????n !!!
#define foreach_hint(i, n, h) 	for(int i = 0, ++h, h=h<n?h:0; i < n; ++h, h=h<n?h:0)


#define LL_FOREACH(head,el)                                                                    \
    LL_FOREACH2(head,el,next)

#define LL_FOREACH2(head,el,next)                                                              \
    for(el=head;el;el=(el)->next)

#define LL_FOREACH_SAFE(head,el,tmp)                                                           \
    LL_FOREACH_SAFE2(head,el,tmp,next)

#define LL_FOREACH_SAFE2(head,el,tmp,next)                                                     \
  for((el)=(head);(el) && (tmp = (el)->next, 1); (el) = tmp)

#define LL_SEARCH_SCALAR(head,out,field,val)                                                   \
    LL_SEARCH_SCALAR2(head,out,field,val,next)

#define LL_SEARCH_SCALAR2(head,out,field,val,next)                                             \
do {                                                                                           \
    LL_FOREACH2(head,out,next) {                                                               \
      if ((out)->field == (val)) break;                                                        \
    }                                                                                          \
} while(0) 

#define LL_SEARCH(head,out,elt,cmp)                                                            \
    LL_SEARCH2(head,out,elt,cmp,next)

#define LL_SEARCH2(head,out,elt,cmp,next)                                                      \
do {                                                                                           \
    LL_FOREACH2(head,out,next) {                                                               \
      if ((cmp(out,elt))==0) break;                                                            \
    }                                                                                          \
} while(0) 

#define LL_EXIST(head,el,out)                                                            		\
do {																							\
	LL_FOREACH(head,out) {																		\
		if (out == el) break;																	\
	} 																						 	\
} while(0) 

#define set_level_current_type(idx, type)	// do{light_res_sw[idx].level_current_type = type;}while(0)
#define get_level_current_type(idx) 		//(light_res_sw[idx].level_current_type)
#define get_light_idx_from_level_md_idx(model_idx)  (model_idx / ELE_CNT_EVERY_LIGHT)
#define get_trans_type_from_level_md_idx(model_idx) (model_idx % ELE_CNT_EVERY_LIGHT)
//----------------
static inline uint32_t division_round(uint32_t val, uint32_t dividend)
{
	return (val + dividend/2)/dividend;
}

static inline uint16_t s16_to_u16(int16_t val)
{
	return (val + 32768);
}

static inline int16_t u16_to_s16(uint16_t val)
{
	return (val - 32768);
}

static inline uint8_t get_onoff_from_level(int16_t level)
{
	return (level != LEVEL_OFF) ? 1 : 0;
}

static inline uint16_t get_lightness_from_level(int16_t level)
{
	return (s16_to_u16(level));
}

static inline int16_t get_level_from_lightness(uint16_t lightness)
{
	return (u16_to_s16(lightness));
}

static inline uint16_t get_ctl_temp_from_level(int16_t level, uint16_t min, uint16_t max)
{
    if(max == min){
	    return max;
    }else{
        #if 0
        return min + ((level+32768)*(max-min))/65535;
        #else // use rounding, because CT value save in level,and should be restore to CT value accurately.
	    return min+division_round(((level+32768)*(max-min)),65535);
        #endif
	}
}

static inline int16_t get_level_from_ctl_temp(uint16_t temp, uint16_t min, uint16_t max)
{
    // have been make sure temp is range in min and max before.
    if(max == min){
	    return 0x7fff;
	}else{
	    #if 0
	    return ((temp-min)*65535)/(max-min) - 32768;
	    #else // use rounding, because CT value save in level,and should be restore to CT value accurately.
	    return division_round((temp-min)*65535,(max-min))-32768;
        #endif
	}
}

static inline int get_level_set_st(int level_set_st1, int level_set_st2)
{
	return (level_set_st2 > level_set_st1 ? level_set_st2 : level_set_st1);
}

extern uint8_t light_pub_trans_step;
extern uint8_t *light_pub_model_priority;
extern uint8_t ct_flag;

void light_pwm_init();

int16_t lum2level(uint8_t lum);
uint8_t level2lum(int16_t level);


// led
#define LGT_CMD_BLE_ADV					0xa0
#define LGT_CMD_BLE_CONN				0xa1

#define PROV_START_LED_CMD				0xc0
#define PROV_END_LED_CMD				0xc1

#define	LGT_CMD_SET_MESH_INFO           0xc5
#define	LGT_CMD_SET_DEV_ADDR            0xc6
#define	LGT_CMD_SET_SUBSCRIPTION        0xc7
#define	LGT_CMD_FRIEND_SHIP_OK          0xc8
#define	LGT_CMD_DUAL_MODE_MESH        	0xc9

#define LGT_CMD_SWITCH_POWERON 			0xd0
#define LGT_CMD_SWITCH_PROVISION		0xd1
#define LGT_CMD_SWITCH_CMD		        0xd2

#define LGT_CMD_PROV_SUC_EVE			LGT_CMD_SET_MESH_INFO

#define PWM_R       PIN5_INDEX		//red
#define PWM_G       PIN27_INDEX		//green
#define PWM_B       PIN15_INDEX		//blue
#define PWM_W       PIN29_INDEX		//white

// default PWM function
#ifndef PWM_FUNC_R
#define PWM_FUNC_R  PIN_MODE_20
#endif
#ifndef PWM_FUNC_G
#define PWM_FUNC_G  PIN_MODE_12
#endif
#ifndef PWM_FUNC_B
#define PWM_FUNC_B  PIN_MODE_0
#endif
#ifndef PWM_FUNC_W
#define PWM_FUNC_W  PIN_MODE_12
#endif

#define GET_PWMID(gpio, func)     ((gpio==PIN20_INDEX) ? PIN_MODE_8 : (  \
                     (gpio==PIN5_INDEX) ? PIN_MODE_0 : (  \
                     (gpio==PIN29_INDEX) ? PIN_MODE_12 : (  \
                     (gpio==PIN15_INDEX) ? PIN_MODE_12 : (  \
                    )))))


#define PWMID_R     (0)
#define PWMID_G     (0)
#define PWMID_B     (0)
#define PWMID_W     (0)
                    
#define PWM_INV_R   (0)
#define PWM_INV_G   (0)
#define PWM_INV_B   (0)
#define PWM_INV_W   (0)


void light_hsl();
void scen_default_hsl(void);

static inline uint16_t get_lightness_present(int light_idx)
{
	return (get_lightness_from_level(light_g_level_present_get(light_idx, ST_TRANS_LIGHTNESS)));
}

static inline uint16_t get_lightness_target(int light_idx)
{
	return (get_lightness_from_level(light_g_level_target_get(light_idx, ST_TRANS_LIGHTNESS)));
}

/**
  * @}
  */
    
/**
  * @}
  */

