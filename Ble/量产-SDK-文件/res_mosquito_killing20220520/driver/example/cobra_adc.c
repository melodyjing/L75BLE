#include <stdio.h>
#include <stdint.h>         // Standard Integer
#include "cobra_adc.h"
#include "cobra4_aon_top.h"
#include "cobra4_aon_wk.h"
/*
    pd0_sel	0: 1.0v by ldo_perm with bgp_lp	
            1: 1.2v by ldo_dig0 with bgp_ln	
    ref_sel	0: ldo_adc ref from bgp_lp	
            1: ldo_adc ref from bgp_ln	

*/
#if 0//(COBRA_ADC_EN_VERSION2)
void set_adc_power(int pd0_sel,int ref_sel)
{
//    cobra4_aon_pd_top_protection_en(1);
//    REG_PL_WR(COBRA4_AON_PD_TOP_BASE+AON_CONFIG_PROTECT,1);
        
    //1. switch on sw_adc3v3
    cobra4_aon_pd_top_d2a_vdd3_adc_en(1);
        
    //2. configure for ldoadc and gpadc work mode,set d2a_vr3v3to1v8_1v2ref_sel=ref_sel
    cobra4_aon_pd_top_d2a_vr3v3to1v8_ldoadc_en(1);
    cobra4_aon_pd_top_d2a_vr3v3to1v8_1v2ref_sel(ref_sel);
//    REG_PL_WR(AON_PD_BASE+AON_ANA_CFG2,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG2)&(~0x03))|0x01|(ref_sel<<1));
//    REG_PL_WR(AON_PD_BASE+AON_CONFIG_PROTECT,0); 
        
    //3. configure pd0 source, set pd0_sw_ldo0_man = pd0_sel        
//    REG_PL_WR(AON_WK_BASE+AON_PROTECT,1);
    cobra4_aon_wk_pd0_sw_selection(PD0_LDO_PERM);
    cobra4_aon_wk_pd0_sw_mode(PD0_POWER_MANUAL);
    cobra4_aon_wk_pd0_sw_perm_manu_enable(1);
    cobra4_aon_wk_pd0_sw_ldo0_manu_enable(pd0_sel);
//    REG_PL_WR(AON_WK_BASE+AONWK_PD0_CFG,(REG_PL_RD(AON_WK_BASE+AONWK_PD0_CFG)&(~0x30101))|0x10000|(pd0_sel<<17));
//    REG_PL_WR(AON_WK_BASE+AON_PROTECT,0);
//    cobra4_aon_pd_top_protection_en(0);
}


/*
    clk_src     0:RCO  			
                1:XO			
    adc_rate	0:8M   	clk_src 1: XO48M   div=6 to get 8M		
                        clk_src 0: RCO24M   div=3 to get 8M		
                1:32K	clk_src 1: XO32K		
                        clk_src 0: RCO32K	
*/

void set_adc_clk(int clk_src,int adc_rate)
{	
//    REG_PL_WR(AON_PD_BASE+AON_CONFIG_PROTECT,1);
        
    //1. configure aon_hclk divider to div=1
    cobra4_aon_pd_top_aon_hclk_divider(0);
//    REG_PL_WR(AON_PD_BASE+AON_HF_CLK_CFG,(REG_PL_RD(AON_PD_BASE+AON_HF_CLK_CFG)&(~AON_AHB_CLK_CFG_MASK)));
        
    //2. configure for ldoadc and gpadc work mode
    cobra4_aon_pd_top_gpadc_32k_enable(adc_rate);
    //REG_PL_WR(AON_PD_BASE+AON_ANA_CFG2,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG2)&(~0x100)|(adc_rate<<8)));
        
    //3. if(adc_rate==0) configure 8M clock source
    if(clk_src == 1)
    {
        cobra4_aon_pd_top_gpadc_clock_selection(GPADC_XTAL48M);
    }
    else
    {
        cobra4_aon_pd_top_gpadc_clock_selection(GPADC_RC_OSC24M);
    }  
        
    //4. else/*if(adc_rate==1)*/configure 32K clock source
    cobra4_aon_pd_top_an_clk_gpadc_32k_gate_en(1);
    //REG_PL_WR(AON_PD_BASE+AON_CLK_GATE_CFG,(REG_PL_RD(AON_PD_BASE+AON_CLK_GATE_CFG)&(~0x8)|(0x8)));
//    REG_PL_WR(AON_PD_BASE+AON_CONFIG_PROTECT,0);         
               
//    REG_PL_WR(AON_WK_BASE+AON_PROTECT,1);
//    REG_PL_WR(AON_WK_BASE+AON_CONTROL_EN,(REG_PL_RD(AON_WK_BASE+AON_CONTROL_EN)&(~AON_CSEM_CLK_CFG))|(AON_CSEM_CLK_CFG));
//    REG_PL_WR(AON_WK_BASE+AON_PROTECT,0); 
    cobra4_aon_wk_aon_csem_clk_selection(CSEM_CLK_XTAL32K);
}
/*
    gpadc_mode	0:TM			
                1:BM			
                2:gpadc			
	diff	    0:single end mode			
                1:differential mode			
					
	gain	    0:1x gain			
                6:4x gain			
                5:2x gain			
                2:divide by 4     			
                1:divide by 2			
	pin1	from 27 to 31, pos input pin in differential mode, or input pin in single end mode			
	pin2	from 27 to 31, neg input pin in differential mode, or not used in single end mode			
*/			

void get_adc_data_set(int gpadc_mode,int diff,int gain,int pin1,int pin2)
{
    uint32_t reg_tmp = 0,*reg_addr;
    reg_addr = (uint32_t *)((0x40089000UL) + (pin1) * 4);
    reg_tmp = *reg_addr;
    reg_tmp &= (~0x1800);
    reg_tmp &= (~0x09f);
    if(pin1 == 24 || pin1 == 25)	
        reg_tmp +=2;
    else if(pin1 == 26)
        reg_tmp +=3;
    else if(pin1==30 || pin1==31)
        reg_tmp +=1;
    *reg_addr = reg_tmp;
    if(pin2!=0)
    {
        reg_tmp = 0;
        reg_addr = (uint32_t *)((0x40089000UL) + (pin2) * 4);
        reg_tmp = *reg_addr;
        reg_tmp &= (~0x1800);
        reg_tmp &= (~0x09f);
        if(pin2 == 24 || pin2 == 25)	
            reg_tmp +=2;
        else if(pin2 == 26)
            reg_tmp +=3;
        else if(pin2==30 || pin2==31)
            reg_tmp +=1;
        *reg_addr = reg_tmp;
    }
        
//    REG_PL_WR(AON_PD_BASE+AON_CONFIG_PROTECT,1);
    //9. config gpadc mode, set adcgain to 1x gain
    cobra4_aon_pd_top_d2a_btgpadc_adcgain(gain);   
    cobra4_aon_pd_top_d2a_btgpadc_vpin_sel((31-pin1));
    if(pin2)
    {
        cobra4_aon_pd_top_d2a_btgpadc_diff((31-pin2+(diff<<3)));
    }else
    {
        cobra4_aon_pd_top_d2a_btgpadc_diff(((diff<<3)));
    }
    cobra4_aon_pd_top_d2a_btgapadc_en_gpadc(1);
    cobra4_aon_pd_top_d2a_btgpadc_en_t2vc((gpadc_mode==0));
    cobra4_aon_pd_top_d2a_btgpadc_mode(gpadc_mode);
//    REG_PL_WR(AON_PD_BASE+AON_ANA_CFG3,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG3)&(~D2A_BTGPADC_ADCGAIN_MASK))|(gain<<D2A_BTGPADC_ADCGAIN_POS));
//    REG_PL_WR(AON_PD_BASE+AON_ANA_CFG1,(REG_PL_RD(AON_PD_BASE+AON_ANA_CFG1)&(~D2A_BTGPADC_MASK))|((31-pin2+(diff<<3))<<19)|((31-pin1)<<16)|(1<<23)|((gpadc_mode==0)<<24)|(gpadc_mode<<25));
        
//    REG_PL_WR(AON_PD_BASE+AON_CONFIG_PROTECT,0);
    //10. enable adc data captureed to fifo
//    REG_PL_WR(0x40083000UL+0x010,(REG_PL_RD(0x40083000UL+0x010)&(~0x03))|(0x3<<0));
//    while(1)
//    {
//        //11. check if gpadc fifo is empty
//        if(!(REG_PL_RD(0x40083000UL+0x048)&((1<<2))))
//        //12. if not empty, cpu repeat read data from gpadc fifo. 
//        tmbm_data[i++] = REG_PL_RD(0x40083000UL+0x080)&0x7FFF ;
//        //13.cpu write the data read from fifo to SRAM.Repeat step 6 and step 8 until the dedicated number arrived(decided bt parameter len)
//        if(i==len)break;
//    }
//    //14. disable adc data captureed to fifo
//    REG_PL_WR(0x40083000UL+0x010,(REG_PL_RD(0x40083000UL+0x010)&(~0x03)));
//    printf("\n");
//    for(i=8;i<len;i++)
//    {
//        printf("%03x\n",tmbm_data[i]);
//    } 
}

void adc_init(void)
{
    cobra4_aon_pd_top_protection_en(1);
    cobra4_aon_wk_protection_en(1);
    set_adc_power(1,1);
    set_adc_clk(1,0);
    get_adc_data_set(2,0,0,27,0);//DP31 only
//    get_adc_data_set(2,1,0,29,30);//DP29 & DP30
		//get_adc_data_set(2,0,0,27,0);//DP27 only	adc1
		//get_adc_data_set(2,0,0,31,0);//DP31 only	adc0
    cobra4_aon_pd_top_protection_en(0);
    cobra4_aon_wk_protection_en(0);  
}

void adc_dpin_change(uint8_t adc_no)
{
    cobra4_aon_pd_top_protection_en(1);
    cobra4_aon_wk_protection_en(1);
		if(adc_no==0)
		{
				get_adc_data_set(2,0,0,31,0);//DP31 only
		}
		else if(adc_no==1)
		{
				get_adc_data_set(2,1,0,25,26);//DP27 only
		}		
		else if(adc_no==2)
		{
				get_adc_data_set(2,1,0,27,28);//DP27 only
		}
		else
		{
				get_adc_data_set(2,1,0,29,30);//DP29 & DP30
		}
    cobra4_aon_pd_top_protection_en(0);
    cobra4_aon_wk_protection_en(0);  
}

void get_adc_data(uint32_t *adc_val)
{
    uint16_t tmbm_data[128];
    int i = 0,len = 4;
    REG_PL_WR(0x40083000UL+0x010,(REG_PL_RD(0x40083000UL+0x010)&(~0x03))|(0x3<<0));
    while(1)
    {
        //11. check if gpadc fifo is empty
        if(!(REG_PL_RD(0x40083000UL+0x048)&((1<<2))))
        //12. if not empty, cpu repeat read data from gpadc fifo. 
            tmbm_data[i++] = REG_PL_RD(0x40083000UL+0x080)&0x7FFF;
//            printf("%03x\n",tmbm_data[i]);
        //13.cpu write the data read from fifo to SRAM.Repeat step 6 and step 8 until the dedicated number arrived(decided bt parameter len)
        if(i==len)break;
    }
    //14. disable adc data captureed to fifo
    REG_PL_WR(0x40083000UL+0x010,(REG_PL_RD(0x40083000UL+0x010)&(~0x03)));
		
		*adc_val = 0;
    for(i=0;i<len;i++)
    {
      //  printf("%03x\n",tmbm_data[i]);
			*adc_val += tmbm_data[i];
    }
		*adc_val /= len;
}
void demo_adc_send_task_loop(void)
{
	  uint32_t adc_value = 0;
		cobra_soft_delay_ms(100);
	  adc_dpin_change(0);
	  get_adc_data(&adc_value);
	  printf("adc31 0x%x\n",adc_value);
	  cobra_soft_delay_ms(100);
	  adc_dpin_change(1);
		get_adc_data(&adc_value);
		printf("adc25 adc26 0x%x\n",adc_value);
	  cobra_soft_delay_ms(100);
	  adc_dpin_change(2);
		get_adc_data(&adc_value);
		printf("adc27 adc28 0x%x\n",adc_value);
		cobra_soft_delay_ms(100);
	  adc_dpin_change(3);
		get_adc_data(&adc_value);
		printf("adc29 adc30 0x%x\n",adc_value);
}
#endif