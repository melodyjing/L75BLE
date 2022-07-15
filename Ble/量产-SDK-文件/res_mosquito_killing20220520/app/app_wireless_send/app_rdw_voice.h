
#ifndef __APP_RDW_VOICE_H__
#define __APP_RDW_VOICE_H__

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_RDW_VOICE)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"
#include "rdw_voice_task.h"
#include <string.h>
#include "app_rdw_voice.h"                  
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "rdw_voice_task.h"             
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions

#include "co_utils.h"
#include "co_math.h"
#include "ke_timer.h"
#include "ke_event.h"
#include "gapm_task.h"
#include "cobra_udma.h"
#include "cobra_i2s.h"
#include "cobra_pinmux.h"
#include "cobra_pcrm.h"
#include "cobra_pdm.h"
#include "cobra_uart.h"
#include "cobra4_audio.h"

#include "rdw_dma.h"
#include "voice_task.h"
#include "co_list.h"
#include "adpcm_code.h"

#include "app_voice_buffer.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */



extern struct app_env_tag app_env;


struct app_voice_env_tag
{
  
    uint16_t key_sample_time;
	
		uint16_t timer_enable;
	
		uint16_t sound_begin;
	
		uint16_t device_ready;  //表示蓝牙设备与主机端建立了连接

};

#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)



/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern struct app_voice_env_tag app_voice_env;

/// Table of message handlers
extern const struct app_subtask_handlers app_voice_handlers;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

void app_voice_init(void);


void app_voice_add_profile(void);


void app_voice_stop_timer (void);


void app_voice_enable_prf(uint8_t);


//void  rdw_void_init_i2s_dma(void);


void app_voice_update_param_again(void);

void app_audio_record_begin(void);
#endif 



#endif 
