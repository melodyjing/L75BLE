#ifndef __APP_LIGHT_BREATHER_H__
#define __APP_LIGHT_BREATHER_H__

#include "app_tulip_comm.h"

#include <stdint.h>

#ifndef IN
#define IN 
#endif

#ifndef OUT
#define OUT
#endif

/// Control RGB calc max bright value
#define CTRL_RGB_BRIGHT_VALUE_MAX   100
/// Control RGB calc min bright value
#define CTRL_RGB_BRIGHT_VALUE_MIN   (CTRL_RGB_BRIGHT_VALUE_MAX * 0.01)

/// Control cw calc max bright value
#define CTRL_CW_BRIGHT_VALUE_MAX    1000
#define CTRL_CW_TEMPER_VALUE_MAX    1000

typedef enum {
    BREATH_INIT = 0,
    BREATH_WAIT_INIT_OK,
    BREATH_CHANGE,
    BREATH_OK
}BREATH_STEP_E;

typedef enum {
    LIGHT_OK = 0,
    LIGHT_COM_ERROR,
    LIGHT_INVALID_PARM,
    LIGHT_MALLOC_FAILED
}BREATH_RETURN_E;

typedef enum {
    DEF_COLOR_C = 0,
    DEF_COLOR_W,
    DEF_COLOR_R,
    DEF_COLOR_G,
    DEF_COLOR_B,
    DEF_COLOR_RGB,
    DEF_COLOR_MAX,
}CTRL_DEF_COLOR_E;

typedef struct {
	uint8_t  r;
	uint8_t  g;
	uint8_t  b;
}BREATH_COLOR_RGB;

typedef struct hl_gens_color_breath_env
{ 
		uint32_t uiBreathCnt;
	
    uint32_t timer_id;    //

    LIGHT_HSL_ELEMENT_RESUME hsl;
		
		LIGHT_CTL_ELEMENT_RESUME ctl;
	
	  uint8_t index;
	
	  uint8_t coolorcolor;
		
		COLOR_RGB rgb;
	
	  uint32_t transtime;
		
    unsigned int screen_index;	

    uint8_t state; // 0 not in color transition,1 in color transition
}hl_gens_color_breath_env_t;

CTRL_DEF_COLOR_E rdw_light_cfg_init_get_netcolor(void);

uint8_t rdw_light_cfg_init_get_colormin(void);
uint8_t rdw_light_cfg_init_get_cwmin(void);
void rdw_light_gradual_ctrl_targetset(uint16_t usBright,uint16_t usBright2);
void app_light_shade_ctrl_disable(void);
void rdw_light_gradual_ctrl_start(void);
void app_light_ctrl_breathing_start(void);

#endif
