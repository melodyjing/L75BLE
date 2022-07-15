/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_m0_sysctrl.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/3/27

	Description :   from cobra digital document
	
		if (reg_addr[11:5] == 7'h00) begin
         case(reg_addr[4:2])
          3'b000: read_mux_le ={{31{1'b0}}, reg_remap} ;     // m0 remap
          3'b001: read_mux_le ={{31{1'b0}}, reg_pmuenable} ; //no use
          3'b010: read_mux_le ={{31{1'b0}}, reg_lockupreset} ; // Enable reset if lockup

          // reset reason of the last time
          // reg_resetinfo[0] : SYSRESETREQ;
          // reg_resetinfo[1] : WDOGRESETREQ;
          // reg_resetinfo[2] : (reg_lockupreset & LOCKUP);
          3'b100: read_mux_le ={{29{1'b0}}, reg_resetinfo[2:0]} ;
          3'b101: read_mux_le ={{22{1'b0}}, btbb_connect_pad,reg_deflt_dma_rstn,reg_icytrx_connect_pad, reg_bt_wakeup_req, reg_bt_clk_sel[5:0]} ;
          3'b110: read_mux_le ={{31{1'b0}}, reg_rfm0_bus_pwr_req} ;
          3'b011,3'b111: read_mux_le = {32{1'b0}};
          default: read_mux_le = {32{1'bx}};
         endcase
      end
	  
	Changed Log	:

		Liujian  2019/3/27			- Creation


*****************************************************************************/

#ifndef __COBRA_M0_SYSCTRL__
#define __COBRA_M0_SYSCTRL__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_M0_SYSCTRL_EN

typedef struct cobra_m0_sysctrl_t {
	__attribute__ ((aligned (4))) unsigned int m0_remap; 
	
	__attribute__ ((aligned (4))) unsigned int pmu_enable;   //no use
	
	__attribute__ ((aligned (4))) unsigned int lockup_reset;
	
	__attribute__ ((aligned (4))) unsigned int reserver0;
	
	__attribute__ ((aligned (4))) unsigned int reset_info;
	
	//btbb_connect_pad,
	//reg_deflt_dma_rstn,
	//reg_icytrx_connect_pad, 
	//reg_bt_wakeup_req, 
	//reg_bt_clk_sel[5:0]
	
	__attribute__ ((aligned (4))) unsigned int bt_control;
	
	__attribute__ ((aligned (4))) unsigned int rfm0_bus_pwr_req;
	
		
}COBRA_M0_SYSCTRL_T;

void cobra_m0_sysctrl_remap_enable(unsigned int en);
void cobra_m0_sysctrl_bt_clock_sel(unsigned int clock_sel);
void cobra_m0_sysctrl_icytrx_connect_to_pad(unsigned int en);
void cobra_m0_sysctrl_wakeup_the_bt(unsigned int en);
#define COBRA_M0_SYSCTRL_BASE				(0x40111000UL)

void cobra_m0_sysctrl_init(void);
#endif   //COBRA_M0_SYSCTRL_EN



#endif

