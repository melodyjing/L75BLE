#include <cobra_config.h>
//#include "ARMCM4.h"
#include "rdw_mcu.h"
#include "cobra_src.h"
#include "cobra_agc.h"
#include "math.h"
#include "cobra_udma.h"
#include "rdw_dma.h"
#include "cobra_i2s.h"
#include "cobra_i2c.h"
#include "wm8978.h"
#include "cobra_pcrm.h"
#include "math.h"
#include "cobra_pinmux.h"
#include "cobra_uart.h"
#include "cobra4_aon_wk.h"
#include "cobra_pll.h"

#if COBRA_SRC_EN

volatile COBRA_SRC_TypeDef *  cobra_src0 __attribute__((section("retention_area"),zero_init));

#if COBRA_AGC_EN
volatile COBRA_AGC_TypeDef *  cobra_agc0 __attribute__((section("retention_area"),zero_init));
#endif

void cobra_src_init(void)
{
	cobra_src0 = (COBRA_SRC_TypeDef *)SRC0_BASE;
}
#if COBRA_AGC_EN
void cobra_agc_init(void)
{
	cobra_agc0 = (COBRA_AGC_TypeDef *)AGC0_BASE;
}
#endif

unsigned int * cobra4_src_read_upfifo_address(void)
{
	return (unsigned int *)(&cobra_src0->UPFIFO_RDAT0);
}
void src_pdm_init(uint8_t clocksource,double _1fs_clk_kHz,uint8_t out_bitwidth,uint16_t osr,uint8_t dma_en,double pclk_kHz,uint8_t bypass_hbnum,uint8_t to_i2s)
{
    uint8_t cic_log2decimation = 0,cic_stage = 0,shrink = 0,pclk_div = 0;
    volatile COBRA_SRC_TypeDef   *src;

    src = cobra_src0;
    cic_log2decimation = log(osr/pow(2,(3-bypass_hbnum)))/log(2);
    cic_stage = 5;
    if((cic_log2decimation*cic_stage + 1 - out_bitwidth)>0)
    {
        shrink = (cic_log2decimation*cic_stage + 1 - out_bitwidth);
    } else
    {
        shrink = 0;
    }
//		shrink++;
		shrink-=4;
#ifdef PRINT_REG
    printf("cic_log2decimation = %d\n",cic_log2decimation);
    printf("cic_stage = %d\n",cic_stage);
    printf("shrink = %d\n",shrink);
#endif   

		if(clocksource == 1)
		{
#if (COBRA_PLL_EN)
				cobra4_aon_wk_protection_en(1);
				cobra4_aon_wk_d2a_ldoanaforpll_en(1);
				cobra4_aon_wk_protection_en(0);
				set_pll_frenquence(196.608);
#endif
		}
		
    /*CFG_CTRL*/
    //init agc_ram
//	set_cfg_agc_ram_clk_is_hclk(src,1);
//	//use agd need to init agc ram
//	set_cfg_agc_ram_clk_is_hclk(src,0);
//	//init fast_clk
//	//use agd need to init fast_clk
    set_cfg_fast_clk_is_hclk(src,1);
    if(clocksource == 0)
    {
        set_cfg_xtal1_pll0_sel(src,1);
    }
    else
    {
        set_cfg_xtal1_pll0_sel(src,0);
    }
    set_cfg_clk_en(src,1);

//	set_cfg_xtal1_pll0_sel(src,1);
    if(to_i2s)
    {
        set_cfg_i2s_enable(src,1);
    }
//    set_cfg_hp_iir_en(src,1);
//    set_cfg_left_right_swap(src,1);
//	set_cfg_audio_mute_en(src,0);
//	set_cfg_right_copy_left_en(src,0);
//	set_cfg_left_copy_right_en(src,0);
//	set_cfg_iir_group_en(src,0);
//	set_cfg_left_right_swap(src,0);
//	  set_cfg_right_en(src,1);
//	  set_cfg_left_en(src,1);
    set_cfg_adc0_pdm1(src,1);
//	set_cfg_intf_fifo_enable(src,1);
#ifdef PRINT_REG
    printf("0d10<<2 src->CFG_CTRL = 0x%x\n",src->CFG_CTRL);
#endif

    /*CFG_MCLK*/
    pclk_div = ((int)((pclk_kHz/(osr*_1fs_clk_kHz))+0.5));
    set_cfg_osr_by_mclk(osr,src);
    set_cfg_mclk_ratio(pclk_div,src);
#ifdef PRINT_REG
    printf("0d11<<2 src->CFG_MCLK = 0x%x\n",src->CFG_MCLK);
#endif

    /*CFG_PDM*/
    set_cfg_pdm_data_delay_phase(0,src);
    set_cfg_pdm_four_wire_is_2mono(src,0);
    set_cfg_pdm_left_two_wire_is_stereo(src,1);
    set_cfg_pdm_right_posedge(src,1);
    set_cfg_pdm_left_posedge(src,0);
#ifdef PRINT_REG
    printf("0d12<<2 src->CFG_PDM = 0x%x\n",src->CFG_PDM);
#endif

//    /*CFG_ADC_DATA_INTF*/
//    set_cfg_adc_data_msb_swap_r(src,1);
//    set_cfg_adc_data_msb_swap_l(src,1);
//    set_cfg_adc_raw_data_en(src,1);
//#ifdef PRINT_REG
//    printf("0d13<<2 src->CFG_ADC_DATA_INTF = 0x%x\n",src->CFG_ADC_DATA_INTF);
//#endif

    /*CFG_INTERRUPT_STICKY*/
//    set_cfg_sticky_int_clr(0,src);
    set_cfg_int_timeout2(0x3b,src);
    set_cfg_int_timeout1(0xc0,src);
#ifdef PRINT_REG
    printf("0d15<<2 src->CFG_INTERRUPT_STICKY = 0x%x\n",src->CFG_INTERRUPT_STICKY);
#endif

    /*CFG_IIR_GROUP*/
    set_cfg_iir_group_mode(5,src);
#ifdef PRINT_REG
    printf("0d16<<2 src->CFG_IIR_GROUP = 0x%x\n",src->CFG_IIR_GROUP);
#endif

    /*CFG_UPFIFO_ALMOST_THRESHOLD*/
    set_cfg_upfifo_almostempty_threshold(4,src);
    set_cfg_upfifo_almostfull_threshold(4,src);
#ifdef PRINT_REG
    printf("0d17<<2 src->CFG_UPFIFO_ALMOST_THRESHOLD = 0x%x\n",src->CFG_UPFIFO_ALMOST_THRESHOLD);
#endif

//    /*CFG_DMA_UPFIFO_CTRL*/
//    set_cfg_dma_req_by_empty(src,1);
//    set_cfg_upfifo_dma_en(src,1);
//    set_cfg_upfifo_en(src,1);
//#ifdef PRINT_REG
//    printf("0d18<<2 src->CFG_DMA_UPFIFO_CTRL = 0x%x\n",src->CFG_DMA_UPFIFO_CTRL);
//#endif

    /*CFG_CIC*/
    set_cfg_cicout_shrink(shrink,src);
    set_cfg_cic_stages(cic_stage,src);
    set_cfg_log2_cic_decimation(cic_log2decimation,src);
#ifdef PRINT_REG
    printf("0d19<<2 src->CFG_CIC = 0x%x\n",src->CFG_CIC);
#endif

    /*CFG_BYPASS*/
    set_cfg_high_speed_bypass(src,0x8);
    set_cfg_pdm_data_delay_bypass(src,1);
    set_cfg_v_s_tof_bypass(src,1);
#if COBRA_AGC_EN == 0
    set_cfg_v_h_bypass(src,0);
#else
    set_cfg_v_h_bypass(src,1);
#endif
//	set_cfg_iir_group_bypass(src,1);

    if(bypass_hbnum == 3)
    {
        set_cfg_hbf2_bypass(src,1);
        set_cfg_hbf1_bypass(src,1);
        set_cfg_hbf0_bypass(src,1);
    }
    else if(bypass_hbnum == 2)
    {
        set_cfg_hbf1_bypass(src,1);
        set_cfg_hbf0_bypass(src,1);
    }
    else if(bypass_hbnum == 1)
    {
        set_cfg_hbf0_bypass(src,1);
    } else
    {

    }



#ifdef PRINT_REG
    printf("0d20<<2 src->CFG_BYPASS = 0x%x\n",src->CFG_BYPASS);
#endif

    /*CFG_OUT_BITWIDTH*/
    set_cfg_i2s_out_bitwidth(3,src);
    set_cfg_dma_out_bitwidth(6,src);
#ifdef PRINT_REG
    printf("0d21<<2 src->CFG_OUT_BITWIDTH = 0x%x\n",src->CFG_OUT_BITWIDTH);
#endif

    /*CFG_WHO_TO_OUT*/
#if COBRA_AGC_EN == 0
	set_cfg_who_to_i2s_mux(0,src);
#else
    set_cfg_who_to_i2s_mux(5,src);
#endif
	set_cfg_who_to_dma_mux(0,src);
#ifdef PRINT_REG
    printf("0d23<2 src->CFG_WHO_TO_OUT = 0x%x\n",src->CFG_WHO_TO_OUT);
#endif

    /*CFG_ZERO_CROSSING_TIMEOUT_VALUE*/
    set_cfg_zero_crossing_timeout_value(0x1900,src);
#ifdef PRINT_REG
    printf("0d24<2 src->CFG_ZERO_CROSSING_TIMEOUT_VALUE = 0x%x\n",src->CFG_ZERO_CROSSING_TIMEOUT_VALUE);
#endif

    /*CFG_VOLUME_SLIDE_CTRL*/
    set_cfg_v_s_tof_decline_start_point(0x6,src);
    set_cfg_v_s_tof_climb_start_point(0x6,src);
    set_cfg_v_s_tof_interval(0xcf,src);
    set_cfg_v_s_tof_zero_crossing_en(src,1);
    set_cfg_v_s_tof_rch_en(src,1);
    set_cfg_v_s_tof_lch_en(src,1);
#ifdef PRINT_REG
    printf("0d25<2 src->CFG_VOLUME_SLIDE_CTRL = 0x%x\n",src->CFG_VOLUME_SLIDE_CTRL);
#endif

    /*CFG_HP_IIR_NUM*/
    set_cfg_hp_iir_b1(0x80c0,src);
    set_cfg_hp_iir_b0(0x7f40,src);
#ifdef PRINT_REG
    printf("0d30<2 src->CFG_HP_IIR_NUM = 0x%x\n",src->CFG_HP_IIR_NUM);
#endif

    /*CFG_HP_IIR_DEN*/
    set_cfg_hp_iir_a1(0x8180,src);
#ifdef PRINT_REG
    printf("0d31<2 src->CFG_HP_IIR_DEN = 0x%x\n",src->CFG_HP_IIR_DEN);
#endif

    /*CFG_CLK_SPEED*/
    set_cfg_cic_clkout_speed(0x3-bypass_hbnum,src);
    set_clk_hbf0_speed(0x3,src);
    set_clk_hbf1_speed(0x2,src);
    set_clk_hbf2_speed(0x1,src);
#ifdef PRINT_REG
    printf("0d33<2 src->CFG_CLK_SPEED = 0x%x\n",src->CFG_CLK_SPEED);
#endif

    /*CFG_MANUAL_GAIN_COARSE*/
    set_cfg_manual_gain_coarse_shift_r(0x04,src);
    set_cfg_manual_gain_coarse_shift_l(0x04,src);
#ifdef PRINT_REG
    printf("0d80<2 src->CFG_MANUAL_GAIN_COARSE = 0x%x\n",src->CFG_MANUAL_GAIN_COARSE);
#endif

    /*CFG_MANUAL_GAIN_FINE*/
    set_cfg_manual_gain_fine_mul_r(0x400,src);
    set_cfg_manual_gain_fine_mul_l(0x400,src);
#ifdef PRINT_REG
    printf("0d81<2 src->CFG_MANUAL_GAIN_FINE = 0x%x\n",src->CFG_MANUAL_GAIN_FINE);
#endif

    /*CFG_MANUAL_GAIN_MODE*/
    set_cfg_manual_gain_mode(0x2,src);
#ifdef PRINT_REG
    printf("0d82<2 src->CFG_MANUAL_GAIN_MODE = 0x%x\n",src->CFG_MANUAL_GAIN_MODE);
#endif

    /*CFG_CTRL*/
    set_cfg_fast_clk_is_hclk(src,0);
    set_cfg_hp_iir_en(src,1);
    set_cfg_left_right_swap(src,1);
    set_cfg_right_en(src,1);
    set_cfg_left_en(src,1);
#ifdef PRINT_REG
    printf("0d10<<2 src->CFG_CTRL = 0x%x\n",src->CFG_CTRL);
#endif

    /*CFG_ADC_DATA_INTF*/
    set_cfg_adc_data_msb_swap_r(src,1);
    set_cfg_adc_data_msb_swap_l(src,1);
    set_cfg_adc_raw_data_en(src,0);
#ifdef PRINT_REG
    printf("0d13<<2 src->CFG_ADC_DATA_INTF = 0x%x\n",src->CFG_ADC_DATA_INTF);
#endif

    /*CFG_DMA_UPFIFO_CTRL*/
    set_cfg_dma_req_by_almostempty(src,1);
    set_cfg_upfifo_dma_en(src,1);
    set_cfg_upfifo_en(src,1);
#ifdef PRINT_REG
    printf("0d18<<2 src->CFG_DMA_UPFIFO_CTRL = 0x%x\n",src->CFG_DMA_UPFIFO_CTRL);
#endif
}
#if COBRA_AGC_EN
void agc_init(void)
{
    volatile COBRA_AGC_TypeDef   *agc;
#ifdef PRINT_REG
    printf("agc_init\n");
#endif 
    agc = cobra_agc0;
    /*CFG_AGC_CTRL*/
    set_cfg_dgt_soft_agc_enable(agc,1);
    set_cfg_agc_position(0x4,agc);
    set_cfg_envelope_r_en(agc,1);
    set_cfg_envelope_l_en(agc,1);
    set_cfg_slidemean_en(agc,1);
    set_cfg_beginning_hold_enable(agc,1);
    set_cfg_agc_mute_flag_en(agc,1);
    set_cfg_agc_target_low_en(agc,1);
    set_cfg_agc_dgt_en(agc,1);
    set_cfg_agc_mute_type(agc,0);
//    set_cfg_right_agc_enable(agc,1);
//    set_cfg_left_agc_enable(agc,1);
#ifdef PRINT_REG
    printf("0d0<<2 agc->CFG_AGC_CTRL = 0x%x\n",agc->CFG_AGC_CTRL);
#endif  
    
    /*CFG_SLIDEMEAN_NUM*/
    set_cfg_slidemean_num(1,agc);
#ifdef PRINT_REG
    printf("0d1<<2 agc->CFG_SLIDEMEAN_NUM = 0x%x\n",agc->CFG_SLIDEMEAN_NUM);
#endif  

    /*CFG_NOISE_THRESHOLD*/
    set_cfg_noise_threshold(0x100,agc);
#ifdef PRINT_REG
    printf("0d5<<2 agc->CFG_NOISE_THRESHOLD = 0x%x\n",agc->CFG_NOISE_THRESHOLD);
#endif

    /*CFG_VAD_THRESHOLD*/
    set_cfg_vad_threshold(0x18,agc);
#ifdef PRINT_REG
    printf("0d6<<2 agc->CFG_VAD_THRESHOLD = 0x%x\n",agc->CFG_VAD_THRESHOLD);
#endif  

    set_cfg_noise_threshold_hysteresis(2,agc);
    /*CFG_NOISE_THRESHOLD_HYSTERESIS*/
#ifdef PRINT_REG
    printf("0d7<<2 agc->CFG_NOISE_THRESHOLD_HYSTERESIS = 0x%x\n",agc->CFG_NOISE_THRESHOLD_HYSTERESIS);
#endif  
    
    /*CFG_DGT_GAIN0*/
    set_cfg_init_dgt_gain(0x0,agc);
    set_cfg_hand_dgt_gain(0x0,agc);
#ifdef PRINT_REG
    printf("0d8<<2 agc->CFG_DGT_GAIN0 = 0x%x\n",agc->CFG_DGT_GAIN0);
#endif     
    
    /*CFG_INIT_PGA_GAIN*/
    set_cfg_init_r_pga_gain(0x0,agc);
    set_cfg_init_l_pga_gain(0x0,agc);
#ifdef PRINT_REG
    printf("0d9<<2 agc->CFG_INIT_PGA_GAIN = 0x%x\n",agc->CFG_INIT_PGA_GAIN);
#endif

    /*CFG_DB_ALLOCATION_TABLE*/
    set_cfg_db_allocaion_table_clear(agc,1);
    set_cfg_db_allocaion_table_en(agc,1);
#ifdef PRINT_REG
    printf("0d10<<2 agc->CFG_DB_ALLOCATION_TABLE = 0x%x\n",agc->CFG_DB_ALLOCATION_TABLE);
#endif

    /*CFG_FRAME_LENGTH*/
    set_cfg_frame_stat_length(0x200,agc);
    set_cfg_frame_length(0x254,agc);
#ifdef PRINT_REG
    printf("0d11<<2 agc->CFG_FRAME_LENGTH = 0x%x\n",agc->CFG_FRAME_LENGTH);
#endif    
   
    /*CFG_DETECTOR_CTRL*/   
    set_cfg_detector_start_position(0x54,agc); 
    set_cfg_detector_out_mode(0x1,agc);
    set_cfg_noise_renew_sel(0x3,agc);
    set_cfg_noise_mode(0x1,agc);
#ifdef PRINT_REG
    printf("0d2<<2 agc->CFG_DETECTOR_CTRL = 0x%x\n",agc->CFG_DETECTOR_CTRL);
#endif

    /*CFG_DETECTOR_BETA_FILTER0*/
    set_cfg_beta_filter0_in_mode(0x0,agc);
    set_cfg_beta_filter0_en(agc,1);
    set_cfg_beta_filter0_beta(0x1999,agc);//0x6666 //0x1999
#ifdef PRINT_REG
    printf("0d3<<2 agc->CFG_DETECTOR_BETA_FILTER0 = 0x%x\n",agc->CFG_DETECTOR_BETA_FILTER0);
#endif    
   
    /*CFG_DETECTOR_BETA_FILTER1*/
    set_cfg_beta_filter1_in_mode(0x0,agc); 
    set_cfg_beta_filter1_en(agc,1);
    set_cfg_beta_filter1_beta(0x6666,agc);//0xa3d //0x6666   
#ifdef PRINT_REG
    printf("0d4<<2 agc->CFG_DETECTOR_BETA_FILTER1 = 0x%x\n",agc->CFG_DETECTOR_BETA_FILTER1);
#endif

    /*CFG_SOFTGAIN_LIMIT*/
    set_cfg_min_gain(0x0,agc);
    set_cfg_max_gain(0x20,agc);
#ifdef PRINT_REG
    printf("0d12<<2 agc->CFG_SOFTGAIN_LIMIT = 0x%x\n",agc->CFG_SOFTGAIN_LIMIT);
#endif

    /*CFG_SOFTAGC_STEP*/
    set_cfg_decay_step_length(0x30,agc);
    set_cfg_attack_step_length(0x60,agc);
#ifdef PRINT_REG
    printf("0d13<<2 agc->CFG_SOFTAGC_STEP = 0x%x\n",agc->CFG_SOFTAGC_STEP);
#endif 

    /*CFG_SOFTAGC_TARGET_LOW*/
    set_cfg_agc_target_low(0x24,agc);
#ifdef PRINT_REG
    printf("0d14<<2 agc->CFG_SOFTAGC_TARGET_LOW = 0x%x\n",agc->CFG_SOFTAGC_TARGET_LOW);
#endif 

    /*CFG_SOFTAGC_TARGET_HIGH*/
    set_cfg_agc_target_high(0x48,agc);
#ifdef PRINT_REG
    printf("0d15<<2 agc->CFG_SOFTAGC_TARGET_HIGH = 0x%x\n",agc->CFG_SOFTAGC_TARGET_HIGH);
#endif

    /*CFG_SOFTAGC_AWAKE_STANDARD*/
    set_cfg_agc_awake_standard(0x20,agc);
#ifdef PRINT_REG
    printf("0d16<<2 agc->CFG_SOFTAGC_AWAKE_STANDARD = 0x%x\n",agc->CFG_SOFTAGC_AWAKE_STANDARD);
#endif

    /*CFG_SOFTAGC_MUTE_STANDARD*/
    set_cfg_agc_mute_standard(0x18,agc);
#ifdef PRINT_REG
    printf("0d17<<2 agc->CFG_SOFTAGC_MUTE_STANDARD = 0x%x\n",agc->CFG_SOFTAGC_MUTE_STANDARD);
#endif
    
    /*CFG_SOFTAGC_TIME*/
    set_cfg_decay_time(0x0,agc);
    set_cfg_attack_time(0x0,agc);
#ifdef PRINT_REG
    printf("0d18<<2 agc->CFG_SOFTAGC_TIME = 0x%x\n",agc->CFG_SOFTAGC_TIME);
#endif

    /*CFG_SOFTAGC_HOLDTIME*/
    set_cfg_silence_to_normal_hold_time(0x2,agc);
    set_cfg_normal_to_silence_hold_time(0x3,agc);
#ifdef PRINT_REG
    printf("0d19<<2 agc->CFG_SOFTAGC_HOLDTIME = 0x%x\n",agc->CFG_SOFTAGC_HOLDTIME);
#endif

    /*CFG_SOFTAGC_HOLDTIME1*/
    set_cfg_beginning_hold_time(0x2,agc);
#ifdef PRINT_REG
    printf("0d20<<2 agc->CFG_SOFTAGC_HOLDTIME1 = 0x%x\n",agc->CFG_SOFTAGC_HOLDTIME1);
#endif
    
    /*CFG_PGA*/
    set_cfg_pga_db_r(0x2,agc);
    set_cfg_pga_en_r(agc,1);
    set_cfg_pga_db_l(0x2,agc);
    set_cfg_pga_en_l(agc,1);
#ifdef PRINT_REG
    printf("0d21<<2 agc->CFG_PGA = 0x%x\n",agc->CFG_PGA);
#endif
    
    /*CFG_DGT_GAIN1*/
    set_cfg_dgt_gain_work_delay(0x3,agc);
    set_cfg_dgt_init_gain(0x180,agc);//0x180
    set_cfg_dgt_db_0_125_max(0x7f,agc);
    set_cfg_dgt_db_0_125_min(0x12,agc);//0x80
#ifdef PRINT_REG
    printf("0d22<<2 agc->CFG_DGT_GAIN1 = 0x%x\n",agc->CFG_DGT_GAIN1);
#endif

    /*REG0*/
    set_reg0(0xee920a9b,agc);
#ifdef PRINT_REG
    printf("0d40<<2 agc->REG0 = 0x%x\n",agc->REG0);
#endif

    /*REG1*/
    set_reg1(0xe614b02f,agc);
#ifdef PRINT_REG
    printf("0d41<<2 agc->REG1 = 0x%x\n",agc->REG1);
#endif

    /*REG2*/
    set_reg2(0xf6c531d3,agc);
#ifdef PRINT_REG
    printf("0d42<<2 agc->REG2 = 0x%x\n",agc->REG2);
#endif

    /*REG3*/
    set_reg3(0x7968f501,agc);
#ifdef PRINT_REG
    printf("0d43<<2 agc->REG3 = 0x%x\n",agc->REG3);
#endif

    /*REG4*/
    set_reg4(0x16614c3c,agc);
#ifdef PRINT_REG
    printf("0d44<<2 agc->REG4 = 0x%x\n",agc->REG4);
#endif

    /*REG5*/
    set_reg5(0xcf0f952e,agc);
#ifdef PRINT_REG
    printf("0d45<<2 agc->REG5 = 0x%x\n",agc->REG5);
#endif

    /*REG6*/
    set_reg6(0x50df1dc2,agc);
#ifdef PRINT_REG
    printf("0d46<<2 agc->REG6 = 0x%x\n",agc->REG6);
#endif

    /*REG7*/
    set_reg7(0xbaf4b1f0,agc);
#ifdef PRINT_REG
    printf("0d47<<2 agc->REG7 = 0x%x\n",agc->REG7);
#endif

    set_cfg_right_agc_enable(agc,1);
    set_cfg_left_agc_enable(agc,1);
#ifdef PRINT_REG
    printf("0d0<<2 agc->CFG_AGC_CTRL = 0x%x\n",agc->CFG_AGC_CTRL);
#endif 
}
#endif
void pdm_disable(void)
{
    volatile COBRA_SRC_TypeDef   *src;
    src = cobra_src0;
    set_cfg_clk_en(src,0);
    set_cfg_fast_clk_is_hclk(src,0);
    set_cfg_hp_iir_en(src,0);
    set_cfg_left_right_swap(src,0);
    set_cfg_right_en(src,0);
    set_cfg_left_en(src,0);
    set_cfg_dma_req_by_almostempty(src,0);
    set_cfg_upfifo_dma_en(src,0);
    set_cfg_upfifo_en(src,0);
}
void pdm_enable(void)
{
    volatile COBRA_SRC_TypeDef   *src;
    src = cobra_src0;
    set_cfg_clk_en(src,1);
    set_cfg_fast_clk_is_hclk(src,1);
    set_cfg_hp_iir_en(src,1);
    set_cfg_left_right_swap(src,1);
    set_cfg_right_en(src,1);
    set_cfg_left_en(src,1);
    set_cfg_dma_req_by_almostempty(src,1);
    set_cfg_upfifo_dma_en(src,1);
    set_cfg_upfifo_en(src,1);
}
#if COBRA_I2S_EN
void pdm_to_i2s_init(uint8_t clocksource,float pcm_clk,float pclk_kHz,uint8_t pdm_osr)
{

    uint8_t slave_address;
    volatile COBRA_I2C_TypeDef   *i2c;

    i2c = cobra_i2c0;
    
    slave_address = 0x34;
    cobra_pcrm_i2c0_active_mode_clock_enable(1);
    cobra_pcrm_i2s0_active_mode_clock_enable(1);
    cobra_map_pinmodeset(PIN1_INDEX,PIN_MODE_1);
    cobra_map_pinmodeset(PIN2_INDEX,PIN_MODE_1);
    cobra_map_pinmodeset(PIN3_INDEX,PIN_MODE_2);
    cobra_pin_pullup_enable(PIN3_INDEX,1);
    cobra_map_pinmodeset(PIN4_INDEX,PIN_MODE_2);
    cobra_pin_pullup_enable(PIN4_INDEX,1);
    rdw_i2c_set_bit_rate(i2c, pclk_kHz, 200);
    rdw_i2c_master_enable(i2c);
    if(clocksource == 0)
    {
        WM8978_Init_Send(i2c, slave_address,VM8978_I2S_MODE,VM8978_LEN_16,1);
        i2s_clk_mode_set(0,pcm_clk,pclk_kHz,16,4,64,I2S_TX,TX_MODULE_MASTER);
    }else
    {
        WM8978_Init_Send_PLL(i2c, slave_address,VM8978_I2S_MODE,VM8978_LEN_16,1);
        i2s_pll_mode_set(0,12,I2S_TX);
    }
    cobra_map_pinmodeset(PIN27_INDEX,PIN_MODE_9);
    cobra_map_pinmodeset(PIN5_INDEX,PIN_MODE_4);
    cobra_map_pinmodeset(PIN8_INDEX,PIN_MODE_4);
    cobra_map_pinmodeset(PIN12_INDEX,PIN_MODE_4);
    cobra_map_pinmodeset(PIN14_INDEX,PIN_MODE_4);
    
    cobra_i2s_tx_init(I2S0,I2S_MODE,SAMPLE_WORD_16BIT,SAMPLE_64BIT,0);
    cobra_i2s_src_tx_enable(I2S0,1);

}
#endif
uint32_t pdm_data[4][128]  __attribute__ ((aligned(16)));

void src_dma_callback(void)
{
    static uint32_t dma_flag = 0;
//    printf("%d\n",dma_flag);  	
    switch(dma_flag)
    {
        case 0 :
        {
            cobra_udma_set_channel_transfer(CHANNEL2 | UDMA_PRI_SELECT,
                           UDMA_MODE_PINGPONG,
                           (void *)&(cobra_src0->UPFIFO_RDAT0),
                           (void *)&pdm_data[2], 128);
            dma_flag = 1;	
            
            break;
        }
        case 1 :
        {
            cobra_udma_set_channel_transfer(CHANNEL2 | UDMA_ALT_SELECT,
                           UDMA_MODE_PINGPONG,
                           (void *)&(cobra_src0->UPFIFO_RDAT0),
                           (void *)&pdm_data[3], 128);
            dma_flag = 2;
                
            break;
        }
        case 2 :
        {
            cobra_udma_set_channel_transfer(CHANNEL2 | UDMA_PRI_SELECT,
                           UDMA_MODE_PINGPONG,
                           (void *)&(cobra_src0->UPFIFO_RDAT0),
                           (void *)&pdm_data[0], 128);
            
            dma_flag = 3;
            break;
        }
        case 3 :
        {
            cobra_udma_set_channel_transfer(CHANNEL2 | UDMA_ALT_SELECT,
                           UDMA_MODE_PINGPONG,
                           (void *)&(cobra_src0->UPFIFO_RDAT0),
                           (void *)&pdm_data[1], 128);

            dma_flag = 0;
            break;
        }
    }
    
}

void print_pdm_data(uint8_t *p)
{
    uint16_t i = 0;
    for(i=0; i<512; i++)
    {
        cobra_uart_write(M3_UART0,*p++);
    }
}

void pdm_test(uint8_t clocksource)
{
//    set_pin_output_value(17,0);
//    if(clocksource == 1)
//    {
//        set_pll_frenquence(196.608);
//    }
    cobra_pcrm_apll_active_mode_clock_enable(1);
    cobra_pcrm_pdm0_active_mode_clock_enable(1);
    cobra_pcrm_udma_active_mode_clock_enable(1);
    pdm_disable();
#if COBRA_I2S_EN
    if(clocksource == 0)
    {
    	pdm_to_i2s_init(0,15.625,48000,PDM_OSR_128);
    }
    else
    {
    	pdm_to_i2s_init(1,16,48000,PDM_OSR_128);
    }
#endif
    app_rdw_udma_init();
    
    app_rdw_udma_get_channel(CHANNEL2, CH2_PDM0);
	
    cobra_udma_channel_attribute_disable(CHANNEL2,
                                    UDMA_ATTR_ALTSELECT | UDMA_ATTR_USEBURST |
                                    UDMA_ATTR_HIGH_PRIORITY |
                                    UDMA_ATTR_REQMASK);
	cobra_dma_chnl_reqmask_all();
	cobra_chnl_enable_all();
    app_rdw_udma_register_callback(CHANNEL2, src_dma_callback);
	cobra_udma_clear_interrupt(CHANNEL2);
    NVIC_EnableIRQ((IRQn_Type)DMADONE_IRQn);
    NVIC_SetPriority((IRQn_Type)DMADONE_IRQn, 3);
	cobra_udma_enable_interrupt(CHANNEL2);

    cobra_udma_set_channel_control(CHANNEL2 | UDMA_PRI_SELECT,
                              UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_32 |
                              UDMA_ARB_1);
    cobra_udma_set_channel_control(CHANNEL2 | UDMA_ALT_SELECT,
                              UDMA_SIZE_32 | UDMA_SRC_INC_NONE | UDMA_DST_INC_32 |
                              UDMA_ARB_1); 

    cobra_udma_set_channel_transfer(CHANNEL2 | UDMA_PRI_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *)&(cobra_src0->UPFIFO_RDAT0),
                               (void *)&pdm_data[0], 128);
                               
    cobra_udma_set_channel_transfer(CHANNEL2 | UDMA_ALT_SELECT,
                               UDMA_MODE_PINGPONG,
                               (void *)&(cobra_src0->UPFIFO_RDAT0),
                               (void *)&pdm_data[1], 128);
                               
	cobra_udma_enable_channel(CHANNEL2);
	cobra_udma_channel_attribute_disable(CHANNEL2,UDMA_ATTR_REQMASK);
    cobra_map_pinmodeset(PIN30_INDEX,PIN_MODE_10);
    cobra_map_pinmodeset(PIN28_INDEX,PIN_MODE_10);
    cobra_map_pinmodeset(PIN31_INDEX,PIN_MODE_10);
    cobra_map_pinmodeset(PIN29_INDEX,PIN_MODE_10);
    if(clocksource == 0)
    {
        src_pdm_init(0,15.625,12,PDM_OSR_128,1,48000,0,1);
    }else
    {
        src_pdm_init(1,16,12,PDM_OSR_128,1,24576,1,1);
    }
#if COBRA_AGC_EN
    agc_init();
#endif
//    while(1)
//    {
//    }
}

#endif

