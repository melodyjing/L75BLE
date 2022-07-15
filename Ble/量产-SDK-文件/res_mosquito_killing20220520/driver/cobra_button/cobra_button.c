/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#include "cobra_button.h"
#include "cobra_multi_timer.h"
#include "cobra4A_linked_softtimer.h"

#if (COBRA_BUTTON_EN == 1)  // && ((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1)))

#define EVENT_CB(ev)   if(handle->cb[ev])handle->cb[ev]((Button*)handle)

#if (COBRA_MULTI_TIMER_ENABLE == 1)
static cobra_timer button_soft_timer;
#endif

#if ((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))
static COBRA_SOFTWARE_TIMER * button_soft_timer;
#endif
//button handle list head.
static struct Button* head_handle = NULL;

/**
  * @brief  Initializes the button struct handle.
  * @param  handle: the button handle strcut.
  * @param  pin_level: read the HAL GPIO of the connet button level.
  * @param  active_level: pressed GPIO level.
  * @retval None
  */
void button_init(struct Button* handle, uint8_t(*pin_level)(), uint8_t active_level)
{
	memset(handle, 0, sizeof(struct Button));
	handle->event = (uint8_t)NONE_PRESS;
	handle->hal_button_Level = pin_level;
	handle->button_level = handle->hal_button_Level();
	handle->active_level = active_level;
}

/**
  * @brief  Attach the button event callback function.
  * @param  handle: the button handle strcut.
  * @param  event: trigger event type.
  * @param  cb: callback function.
  * @retval None
  */
void button_attach(struct Button* handle, PressEvent event, BtnCallback cb)
{
	handle->cb[event] = cb;
}

/**
  * @brief  Inquire the button event happen.
  * @param  handle: the button handle strcut.
  * @retval button event.
  */
PressEvent get_button_event(struct Button* handle)
{
	return (PressEvent)(handle->event);
}

#if (APP_AIRCLEAN_ENABLE == 1)
/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle strcut.
  * @retval None
  */
void button_handler(struct Button* handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level();

    //ticks counter working..
    if((handle->state) > 0) handle->ticks++;

    /*------------button debounce handle---------------*/
    if(read_gpio_level != handle->button_level)   //not equal to prev one
    {
        //continue read 3 times same new level change
        if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS)
        {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else     //leved not change ,counter reset.
    {
        handle->debounce_cnt = 0;
    }

    /*-----------------State machine-------------------*/
    switch (handle->state)
    {
        case 0:
            if(handle->button_level == handle->active_level)  	//start press down
            {
                handle->event = (uint8_t)PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->ticks = 0;
                handle->repeat = 1;
                handle->state = 1;
            }
            else
            {
                handle->event = (uint8_t)NONE_PRESS;
            }
            break;

        case 1:
            if(handle->button_level != handle->active_level)   //released press up
            {
                handle->event = (uint8_t)PRESS_UP;
                EVENT_CB(PRESS_UP);
                handle->ticks = 0;
                handle->state = 0;

            } 
            else if(handle->ticks%50==0)   /*按下每秒触发一次回调*/
            {
                handle->event = (uint8_t)PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->state = 1;
            }
            break;

			
    }
}

#elif (APP_RAINBOW_ENABLE == 1)
/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle strcut.
  * @retval None
  */
void button_handler(struct Button* handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level();

    //ticks counter working..
    if((handle->state) > 0) handle->ticks++;

    /*------------button debounce handle---------------*/
    if(read_gpio_level != handle->button_level)   //not equal to prev one
    {
        //continue read 3 times same new level change
        if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS)
        {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else     //leved not change ,counter reset.
    {
        handle->debounce_cnt = 0;
    }

    /*-----------------State machine-------------------*/
    switch (handle->state)
    {
        case 0:
            if(handle->button_level == handle->active_level)  	//start press down
            {
                handle->event = (uint8_t)PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->ticks = 0;
                handle->repeat = 1;
                handle->state = 1;
            }
            else
            {
                handle->event = (uint8_t)NONE_PRESS;
            }
            break;

        case 1:
            if(handle->button_level != handle->active_level)   //released press up
            {
                handle->event = (uint8_t)PRESS_UP;
                EVENT_CB(PRESS_UP);
                handle->ticks = 0;
                handle->state = 0;

            } 
            else if(handle->ticks%50==0)   /*按下每秒触发一次回调*/
            {
                handle->event = (uint8_t)PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->state = 1;
            }
            break;

			
    }
}
#elif (APP_MOSQUITO_KILLING_ENABLE == 1)
/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle strcut.
  * @retval None
  */
void button_handler(struct Button* handle)
{
    uint8_t read_gpio_level = handle->hal_button_Level();

    //ticks counter working..
    if((handle->state) > 0) handle->ticks++;

    /*------------button debounce handle---------------*/
    if(read_gpio_level != handle->button_level)   //not equal to prev one
    {
        //continue read 3 times same new level change
        if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS)
        {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else     //leved not change ,counter reset.
    {
        handle->debounce_cnt = 0;
    }

    /*-----------------State machine-------------------*/
    switch (handle->state)
    {
        case 0:
            if(handle->button_level == handle->active_level)  	//start press down
            {
                handle->event = (uint8_t)PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->ticks = 0;
                handle->repeat = 1;
                handle->state = 1;
            }
            else
            {
                handle->event = (uint8_t)NONE_PRESS;
            }
            break;

        case 1:
            if(handle->button_level != handle->active_level)   //released press up
            {
                handle->event = (uint8_t)PRESS_UP;
                EVENT_CB(PRESS_UP);
                handle->ticks = 0;
                handle->state = 0;

            } 
//            else if(handle->ticks%50==0)   /*按下每秒触发一次回调*/
			else if(handle->ticks%5==0)   /*按下每10ms触发一次回调*/
				
            {
                handle->event = (uint8_t)PRESS_DOWN;
                EVENT_CB(PRESS_DOWN);
                handle->state = 1;
            }
            break;

			
    }
}

#else

/**
  * @brief  Button driver core function, driver state machine.
  * @param  handle: the button handle strcut.
  * @retval None
  */
void button_handler(struct Button* handle)
{
	uint8_t read_gpio_level = handle->hal_button_Level();

	//ticks counter working..
	if((handle->state) > 0) handle->ticks++;

	/*------------button debounce handle---------------*/
	if(read_gpio_level != handle->button_level) { //not equal to prev one
		//continue read 3 times same new level change
		if(++(handle->debounce_cnt) >= DEBOUNCE_TICKS) {
			handle->button_level = read_gpio_level;
			handle->debounce_cnt = 0;
		}
	} else { //leved not change ,counter reset.
		handle->debounce_cnt = 0;
	}

	/*-----------------State machine-------------------*/
	switch (handle->state) {
	case 0:
		if(handle->button_level == handle->active_level) {	//start press down
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			handle->ticks = 0;
			handle->repeat = 1;
			handle->state = 1;
		} else {
			handle->event = (uint8_t)NONE_PRESS;
		}
		break;

	case 1:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			handle->ticks = 0;
			handle->state = 2;

		} else if(handle->ticks > LONG_TICKS) {
			handle->event = (uint8_t)LONG_RRESS_START;
			EVENT_CB(LONG_RRESS_START);
			handle->state = 5;
		}
		break;

	case 2:
		if(handle->button_level == handle->active_level) { //press down again
			handle->event = (uint8_t)PRESS_DOWN;
			EVENT_CB(PRESS_DOWN);
			handle->repeat++;			
			EVENT_CB(PRESS_REPEAT); // repeat hit
			handle->ticks = 0;
			handle->state = 3;
		} else if(handle->ticks > SHORT_TICKS) { //released timeout
			if(handle->repeat == 1) {
				handle->event = (uint8_t)SINGLE_CLICK;
				EVENT_CB(SINGLE_CLICK);
			} else if(handle->repeat == 2) {
				handle->event = (uint8_t)DOUBLE_CLICK;
				EVENT_CB(DOUBLE_CLICK); // repeat hit
			}
			handle->state = 0;
		}
		break;

	case 3:
		if(handle->button_level != handle->active_level) { //released press up
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			if(handle->ticks < SHORT_TICKS) {
				handle->ticks = 0;
				handle->state = 2; //repeat press
			} else {
				handle->state = 0;
			}
		}
		break;

	case 5:
		if(handle->button_level == handle->active_level) {
			//continue hold trigger
			handle->event = (uint8_t)LONG_PRESS_HOLD;
			EVENT_CB(LONG_PRESS_HOLD);

		} else { //releasd
			handle->event = (uint8_t)PRESS_UP;
			EVENT_CB(PRESS_UP);
			handle->state = 0; //reset
		}
		break;
	}
}
#endif

/**
  * @brief  Start the button work, add the handle into work list.
  * @param  handle: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
int button_start(struct Button* handle)
{
	struct Button* target = head_handle;
	while(target) {
		if(target == handle) return -1;	//already exist.
		target = target->next;
	}
	handle->next = head_handle;
	head_handle = handle;
	return 0;
}

/**
  * @brief  Stop the button work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval None
  */
void button_stop(struct Button* handle)
{
	struct Button** curr;
	for(curr = &head_handle; *curr; ) {
		struct Button* entry = *curr;
		if (entry == handle) {
			*curr = entry->next;
//			free(entry);
		} else
			curr = &entry->next;
	}
}

/**
  * @brief  background ticks, timer repeat invoking interval 5ms.
  * @param  None.
  * @retval None
  */
void button_ticks()
{
	struct Button* target;
	for(target=head_handle; target; target=target->next) {
		button_handler(target);
	}
}
#if ((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))
static void cobra4A_button_callback(COBRA_SOFTWARE_TIMER * param)
{
	    button_ticks();
}
#endif
void cobra4A_button_init(void)
{
#if ((COBRA_LINK_LIBRARY_ENABLE == 1) && (COBRA_LINKEN_SOFT_TIMER_ENABLE == 1))
		 button_soft_timer = cobra_linked_softtimer_create(NULL, cobra4A_button_callback, 20);   //20ms 检测一次按键
	   cobra_linked_softtimer_start(button_soft_timer);	
#endif
#if (COBRA_MULTI_TIMER_ENABLE == 1)

#if (APP_AIRCLEAN_ENABLE == 1)
	cobra_softtimer_init(&button_soft_timer, button_ticks, 20,  20);
#elif (APP_MOSQUITO_KILLING_ENABLE == 1)
	cobra_softtimer_init(&button_soft_timer, button_ticks, 20,  20);
#else
	cobra_softtimer_init(&button_soft_timer, button_ticks, 1,  1);
#endif
	cobra_softtimer_start(&button_soft_timer);
#endif	
}
#endif

