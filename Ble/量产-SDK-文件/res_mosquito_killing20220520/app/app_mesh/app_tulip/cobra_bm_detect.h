#ifndef _COBRA_BM_DETECT_H
#define _COBRA_BM_DETECT_H
#include "stdint.h"
#include "stdbool.h"
#include "stdlib.h"
#include "reg_access.h"

#if (BM_DETECT)
#define     PINMUX_BASE                 0x40089000UL
#define     AON_PD_BASE                 0x4008B000UL
#define     COBRA_UMCU_BASE			        0x40083000UL

#define		AON_CONFIG_PROTECT			0x000UL

#define		AON_HF_CLK_CFG	            0x10UL
#define 	AON_AHB_CLK_CFG_MASK		(0x1f<<0)

#define AON_ANA_CFG0					0x0040
#define D2A_VDAC_EN_MASK			(1<<0)
#define D2A_VDAC_EN_POS				0
#define D2A_COMPA_EN_MASK			(1<<1)
#define D2A_COMPA_EN_POS				1
#define D2A_COMPB_EN_MASK			(1<<2)
#define D2A_COMPB_EN_POS				2
#define D2A_VDD3_ADC_EN_MASK			(1<<3)
#define D2A_VDD3_ADC_EN_POS				3


#define AON_ANA_CFG1					0x0044
#define XO32K_GMTRIM_MASK			(0<<0x0f)
#define XO32K_GMTRIM_POS			0
#define XO32K_CL_MASK					(4<<0x0f)
#define XO32K_CL_POS					4
#define RCO24M_FTUNE_MASK			(8<<0x7f)
#define RCO24M_FTUNE_POS			8
#define D2A_BTGPADC_MASK			(0x3FFF<<13)
#define D2A_BTGPADC_POS				13
#define D2A_BTGPADC_ADCGAIN_MASK	(7<<13)
#define D2A_BTGPADC_ADCGAIN_POS		13
#define D2A_BTGPADC_VPIN_SEL_MASK	(7<<16)
#define D2A_BTGPADC_VPIN_SEL_POS		16
#define D2A_BTGPADC_DIFF_MASK		(0x0f<<19)
#define D2A_BTGPADC_DIFF_POS		19
#define D2A_BTGAPADC_EN_GPADC_MASK		(1<<23)
#define D2A_BTGAPADC_EN_GPADC_POS			23
#define D2A_BTGPADC_EN_T2VC_MASK		(1<<24)
#define D2A_BTGPADC_EN_T2VC_POS			24
#define D2A_BTGPADC_MODE_MASK		(3<<25)
#define D2A_BTGPADC_MODE_POS		25
#define COMPB_DIVIER_MASK			(3<<27)
#define COMPB_DIVIER_POS			27

#define AON_ANA_CFG2					0x0048
#define D2A_VR3V3TO1V8_LDOADC_EN_MASK		(1<<0)
#define D2A_VR3V3TO1V8_LDOADC_EN_POS		0
#define D2A_VR3V3TO1V8_1V2REF_SEL_MASK	(1<<1)
#define D2A_VR3V3TO1V8_1V2REF_SEL_POS		1
#define D2A_BTGPADC_ADCCAL_MASK		(3<<6)
#define D2A_BTGPADC_ADCCAL_POS		6
#define D2A_BTGPADC_32K_MODE_MASK	(1<<8)
#define D2A_BTGPADC_32K_MODE_POS		8
#define D2A_CPA_NEG_SEL_MASK	(0x0f<<9)
#define D2A_CPA_NEG_SEL_POS		9
#define D2A_CPA_POS_SEL_MASK	(0x07<<13)
#define D2A_CPA_POS_SEL_POS		13
#define D2A_VDAC_SETX_MASK		(0x3F<<16)
#define D2A_VDAC_SETX_POS			16
#define D2A_VDAC_SETY_MASK		(0x3F<<22)
#define D2A_VDAC_SETY_POS			22



#define AON_ANA_CFG3					0x004C
#define D_I_COB3_VDAC_SETZ_MASK		0x7F
#define D_I_COB3_VDAC_SETZ_POS		0
#define D2A_CPB_NEG_SEL_MASK	(0x03<<7)
#define D2A_CPB_NEG_SEL_POS		7
#define D2A_CPB_POS_SEL_MASK	(0x0f<<9)
#define D2A_CPB_POS_SEL_POS		9

void bm_detect(void);
void save_data_before_ota(void);
#endif
#endif

