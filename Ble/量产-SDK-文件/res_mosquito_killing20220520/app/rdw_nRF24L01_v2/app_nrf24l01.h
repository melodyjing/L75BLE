/*
liujian add
2019/6/22

for RF module
*/

#ifndef __APP_NRF24L01_H__
#define __APP_NRF24L01_H__

#if (CFG_NRF24L01_DEMO)
#include "cobra_spi.h"
#include "cobra_pinmux.h"
#include "cobra_pcrm.h"
#include "rdw_gpio.h"
#include "cobra_m0_gpio.h"
#include "cobra_timer.h"
#include "cobra_umcu.h"
#include "nrf24.h"

typedef int (*nrf24l01_interrupt_callback)(unsigned char state);

void app_cobra_nRF24L01_demo(void);

void app_cobra_nRF24L01_init(void);

void cobra_app_nrf24l01_task_loop(void);


void app_cobra_nRF24L01_init(void);


void cobra_app_nrf24l01_register_callback(nrf24l01_interrupt_callback callback);

void cobra_app_nrf24l01_unregister_callback(void);

#endif

#endif