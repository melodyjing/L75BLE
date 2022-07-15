/******************************************************************************

	Cobra Drivers

	File Name	:	cobra4_pll.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/2/25

	Description :   
	
	Changed Log	:

		Liujian  2020/2/25		- Creation



*****************************************************************************/

#ifndef __COBRA4_PLL_H__
#define __COBRA4_PLL_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>
#include "cobra_pcrm.h"
#include "cobra4_aon_wk.h"
#include "cobra_timer.h"

#if COBRA4_PLL_EN

int cobra4_pll_set_frenquence(double pll_target);

void cobra_pcrm_d2a_pll_pfd_en(unsigned int en);

#endif
#endif

