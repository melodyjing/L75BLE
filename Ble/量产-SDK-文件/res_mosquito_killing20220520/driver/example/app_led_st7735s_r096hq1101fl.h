
#ifndef _APP_LED_ST7735S_R096HQ1101FL_H
#define _APP_LED_ST7735S_R096HQ1101FL_H

#include "app_config.h"

#if (APP_LCD_ST7735S_R096HQ1101FL_ENABLE)
#include "cobra4_aon_top.h"
#include "cobra4_aon_wk.h"
#include "cobra_pcrm.h"
#include "cobra_irda.h"
#include "cobra_cache_ctrl.h"
#include "cobra4A_spwm.h"
#include "cobra4_common.h"
#include "cobra_udma.h"
#include "rdw_dma.h"
#include "rdw_mcu.h"
#include "cobra4_aes.h"
#include "ARMCM4.h"
#include "string.h"

void  app_cobra_lcd_st7735s_init(void);
#endif 

#endif  /* _APP_LED_ST7735S_R096HQ1101FL_H */


