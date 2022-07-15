//liujian add 
//2019/5/10

#ifndef __APP_SLEEP_H__
#define __APP_SLEEP_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "ke_mem.h"
#include "ke_msg.h"
#include "rwble_hl.h"
#include "hci.h"
#include "co_bt.h"


#ifdef __cplusplus
extern "C"
{
#endif

struct hci_sleep_cmd
{
    unsigned char sleep_mode;
};

unsigned int  app_cobra_sleep(void);
	
void app_sleep_key_setting(void);	
	
void app_cobra_enter_deep_sleep(void);

void app_sleep_require_cmd(const uint8_t sleep_mode);

int hci_cobra_sleep_cmd_cmp_evt_handler(uint16_t opcode, struct hci_basic_cmd_cmp_evt const *event);


void app_test_clear_deep_sleep_require(void);

void cobra_standby_wakeup_interrupt(void);

unsigned int cobra_standby_mode_get(void);
void cobra_standby_mode_clear(void);

void app_cobra_enter_standby_retention(void);
#ifdef __cplusplus
}
#endif

#endif 
