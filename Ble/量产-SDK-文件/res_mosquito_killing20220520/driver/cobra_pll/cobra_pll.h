#ifndef _COBRA_PLL_H
#define _COBRA_PLL_H
#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include <cobra_config.h>

#if COBRA4_PLL_EN
int cobra4_pll_set_frenquence(double pll_target);

void cobra_pcrm_d2a_pll_pfd_en(unsigned int en);

#endif

#endif
