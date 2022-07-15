/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 /******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Port
	
	Liujian  2020/11/2		- Creation
*****************************************************************************/

#include <elog.h>
#include <stdio.h>
#include "cobra_rtc.h"
#include "cobra_uart.h"

#if (COBRA_LOGGER_ENABLE == 1)
/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;


    return result;
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
 //    printf("%.*s", size, log);
    int i;
    for (i = 0; i < size; i++)
    {
		   cobra_uart_write(M3_UART0,*log++);
    }

}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    __disable_irq();
    
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    __enable_irq();
    
}

/**
 * get current time interface
 *
 * @return current time
 */
char timer_buffer[20];
const char *elog_port_get_time(void) {
    
    /* add your code here */
    #if((COBRA_RTC_EN == 1) && (APP_COBRA_RTC_EN == 1))
    
	  COBRA_RTC_TIME_DATE_T timer;
	  cobra_rtc_get_current_time_date(&timer);
	  sprintf(timer_buffer, "%d:%d:%d", timer.HOUR, timer.MIN, timer.SEC);
	  return (const char *)timer_buffer;
	  #else
	  return "10:08:12";
	  #endif
	
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    return "pid:1008";
    
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    return "tid:24";
    
}

#endif
