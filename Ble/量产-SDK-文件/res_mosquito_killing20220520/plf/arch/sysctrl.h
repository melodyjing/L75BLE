#include <stdbool.h>
#ifndef _SYSCTRL_H_
#define _SYSCTRL_H_
#if 0
//----------------- const area -------------------------------
#define  RFM0_SYS_CLK_16M		16			
#define  RFM0_SYS_CLK_8M		8
//---------------- register area  -----------------------------
#define RFM0_SYSCTRL_BASE		0x40111000

#define RFM0_REMAP_REG			RFM0_SYSCTRL_BASE + 0x000

#define RFM0_LOCKUP_RESET_REG	RFM0_SYSCTRL_BASE + 0x008

#define RFM0_RESET_INFO_REG		RFM0_SYSCTRL_BASE + 0x010
#define SYSRESET_INFO			1
#define WDOGRESET_INFO			2
#define LOCKUPRESET_INFO		4

#define RFM0_BT_CTRL_REG		RFM0_SYSCTRL_BASE + 0x014
#define RFM0_BT_WAKUP_MASK		0x40
#define RFM0_BT_WAKUP_POS		6
#define RFM0_BT_CLK_MASK		0x3f
#define RFM0_BT_CLK_POS			0

#define RFM0_PWR_REG			RFM0_SYSCTRL_BASE + 0x018

void rfm0_clk_set(char value);
void rfm0_remap(bool en);

#endif
#endif

