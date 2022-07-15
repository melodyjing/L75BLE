//liujian add
//2018/8/6
//PDM driver
#include "rdw_pdm.h"
#include "rdw_gpio.h"
#include <math.h>

#define IIR_1FS_COEFF0  0.998689944140625*pow(2,15)
#define IIR_1FS_COEFF1   -0.99737548828125*pow(2,15)

volatile RDW_PDM_T   *p_rdw_pdm;

void rdw_init_pdm(void)
{
		p_rdw_pdm = (RDW_PDM_T   *)PDM_BASE;
		gpio_set_direction(RDW_PDM_GPIO, GPIO_OUTPUT);
	  gpio_write_pin(RDW_PDM_GPIO, GPIO_LOW);
	
}


void rdw_pdm_disable(void)
{
	p_rdw_pdm->CFG_PDM_CTRL &= ~(CFG_LEFT_EN|CFG_RIGHT_EN|CFG_CLK_PDM_ENABLE);
	p_rdw_pdm->UPFIFO_CTRL &= ~(CFG_UPFIFO_DMA_EN|CFG_UPFIFO_EN);
}

void rdw_pdm_enable(void)
{
	p_rdw_pdm->CFG_PDM_CTRL |= (CFG_LEFT_EN|CFG_RIGHT_EN|CFG_CLK_PDM_ENABLE);
	p_rdw_pdm->UPFIFO_CTRL  |= (CFG_UPFIFO_DMA_EN|CFG_UPFIFO_EN);
}

void rdw_init_sys_pdm(char pdm_type)
{
		if(pdm_type == 0)
		apb_en_ctrl(PDM0_EN,ENABLE);
	else
		apb_en_ctrl(PDM1_EN,ENABLE);
}

void rdw_pdm_init(uint8_t dma_en,uint8_t osr,float pclk_kHz)
{
	//float pcm_clk;
	int16_t iir_1fs_coeff0,iir_1fs_coeff1,delay_cnt;
	p_rdw_pdm->CFG_PDM_CTRL |= CFG_LEFT_PDM_TO_CIC_CLKEDGE_SEL|CFG_RIGHT_PDM_TO_CIC_CLKEDGE_SEL|CFG_PDM_LEFT_TWO_WIRE_IS_STEREO|CFG_CLK_PDM_ENABLE;
	p_rdw_pdm->CFG_BYPASS = CFG_PDM_DATA_DELAY_BYPASS;
	p_rdw_pdm->CFG_CLK_GEN = ((int)((pclk_kHz/(osr*16))+0.5))<< CFG_PDM_RATIO_BY_PCLK_POS;						//osr = 128  8			osr = 64  16
	p_rdw_pdm->LOG2_CIC_DECIMATION = log2(osr/4);																								//osr = 128  5			osr = 64  4
	iir_1fs_coeff0 = IIR_1FS_COEFF0;
	iir_1fs_coeff1 = IIR_1FS_COEFF1;
	p_rdw_pdm->HP_IIR_COFF0 |= iir_1fs_coeff0 << CFG_HP_IIR_S0_POS;
	p_rdw_pdm->HP_IIR_COFF0 |= iir_1fs_coeff1 << CFG_HP_IIR_1ORDER_A1_POS;
	p_rdw_pdm->CFG_GAIN_STEPS = INCREASE_12DB;
	p_rdw_pdm->CFG_PDM_CTRL1 = CFG_CIC_ENABLE|CFG_I2S_ENABLE|CFG_HP_IIR_ENABLE|(PCM_BITWIDTH_16BIT<<CFG_PCM_BITWIDTH_POS);//|CFG_OSR_128_EN;
	if(osr==128)
		p_rdw_pdm->CFG_PDM_CTRL1 |= CFG_OSR_128_EN;
	p_rdw_pdm->CFG_SAMPLING =  CFG_SAMPLING_DISABLE;//CFG_SAMPLING_ENABLE|(CIC_OUT<<CFG_SAMPLING_OBJECT_SEL_POS);//
	for(delay_cnt=16000;delay_cnt>0;delay_cnt--);				//delay 1ms
	p_rdw_pdm->UPFIFO_CTRL |= CFG_UPFIFO_EN;
	if(dma_en)
	{
		p_rdw_pdm->UPFIFO_CTRL |= CFG_UPFIFO_DMA_EN;
	}
//	p_rdw_pdm->CFG_PDM_CTRL |= CFG_LEFT_EN|CFG_RIGHT_EN;
//	pcm_clk = pclk_kHz/(((int)((pclk_kHz/(osr*16))+0.5))*osr);
//	return pcm_clk;
}











