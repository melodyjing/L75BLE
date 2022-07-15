/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_wm8978.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/3/16

	Description :   


	Changed Log	:

		Liujian  2020/3/16			- Creation

*****************************************************************************/
 
#include "cobra_wm8978.h"
#include "cobra_i2c.h"

#if(COBRA_WM8978_WIRELESS_EN)

static const uint16_t WM8978_DEFAULT[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0100, 0x00FF,
    0x00FF, 0x0000, 0x012C, 0x002C, 0x002C, 0x002C, 0x002C, 0x0000,
    0x0032, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0038, 0x000B, 0x0032, 0x0000, 0x0008, 0x000C, 0x0093, 0x00E9,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0003, 0x0010, 0x0010, 0x0100,
    0x0100, 0x0002, 0x0001, 0x0001, 0x0039, 0x0039, 0x0039, 0x0039,
    0x0001, 0x0001
};

static uint16_t WM8978_REGVAL[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0100, 0x00FF,
    0x00FF, 0x0000, 0x012C, 0x002C, 0x002C, 0x002C, 0x002C, 0x0000,
    0x0032, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0038, 0x000B, 0x0032, 0x0000, 0x0008, 0x000C, 0x0093, 0x00E9,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0003, 0x0010, 0x0010, 0x0100,
    0x0100, 0x0002, 0x0001, 0x0001, 0x0039, 0x0039, 0x0039, 0x0039,
    0x0001, 0x0001
};

static uint8_t cobra_wm8978_write_reg(uint8_t reg,uint16_t val)
{
		uint8_t stat = 0;
		uint8_t writeDataBuf[2];
	  writeDataBuf[0] = (uint8_t)(reg<<1)|((uint8_t)((val>>8)&0x01));;
	  writeDataBuf[1] = (uint8_t)val;
	
		if(cobra_i2c_write_byte(M3_I2C0, RDW_I2C_TX_START|RDW_I2C_TX_STOP ) < 0)
			stat = 1;
		if(cobra_i2c_write_byte(M3_I2C0, 3) < 0)   //3 byte
			stat = 1;
		if(cobra_i2c_write_byte(M3_I2C0, CODEC_ADDRESS|RDW_I2C_WRITE) < 0)
			stat = 1;
		if(cobra_i2c_write_byte(M3_I2C0, writeDataBuf[0]) < 0)
			stat = 1;
			
		if(cobra_i2c_write_byte(M3_I2C0, writeDataBuf[1]) < 0)
			stat = 1;
		
		while(cobra_i2c_get_status(M3_I2C0)&RDW_I2C_NACK)
		{
				
		}
		
		WM8978_REGVAL[reg] = val;
		
		return stat;
}

static uint16_t cobra_wm8978_read_reg(uint8_t reg)
{  
	return WM8978_REGVAL[reg];	
}

//WM8978 MIC增益设置(不包括BOOST的20dB,MIC-->ADC输入部分的增益)
//gain:0~63,对应-12dB~35.25dB,0.75dB/Step
void cobra_wm8978_mic_gain(uint8_t gain)
{
		uint16_t regval;
	
		gain &= 0x3F;
	
	  regval = cobra_wm8978_read_reg(REG_RIGHT_PGA_GAIN);
		regval &= ~0x3f;	
		cobra_wm8978_write_reg(REG_RIGHT_PGA_GAIN, (regval | gain ));					//R46,右通道PGA设置
	
		regval = cobra_wm8978_read_reg(REG_LEFT_PGA_GAIN);
		regval &= ~0x3f;		
		cobra_wm8978_write_reg(REG_LEFT_PGA_GAIN, (regval | gain | INPPGAUPDATE));							//R45,左通道PGA设置 
			
}


//WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
//#define L2_2BOOSTVOL_N12DB              (1 << 4)
//#define L2_2BOOSTVOL_N9DB               (2 << 4)
//#define L2_2BOOSTVOL_N6DB               (3 << 4)
//#define L2_2BOOSTVOL_N3DB               (4 << 4)
//#define L2_2BOOSTVOL_0DB                (5 << 4)
//#define L2_2BOOSTVOL_3DB                (6 << 4)
//#define L2_2BOOSTVOL_6DB                (7 << 4)

void cobra_wm8978_linein_gain(uint8_t gain)
{
		uint16_t regval;
   // gain &= 0x07;
    regval = cobra_wm8978_read_reg(REG_LEFT_ADC_BOOST);
    regval &= ~(L2_2BOOSTVOL_MASK << L2_2BOOSTVOL_POS);
    cobra_wm8978_write_reg(REG_LEFT_ADC_BOOST, regval | gain);
	
    regval=cobra_wm8978_read_reg(REG_RIGHT_ADC_BOOST);
    regval&=~(R2_2BOOSTVOL_MASK << R2_2BOOSTVOL_POS);
    cobra_wm8978_write_reg(REG_RIGHT_ADC_BOOST, regval | gain);
	
} 

//WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
//#define AUXL2BOOSTVOL_N12DB             (1)
//#define AUXL2BOOSTVOL_N9DB              (2)
//#define AUXL2BOOSTVOL_N6DB              (3)
//#define AUXL2BOOSTVOL_N3DB              (4)
//#define AUXL2BOOSTVOL_0DB               (5)
//#define AUXL2BOOSTVOL_3DB               (6)
//#define AUXL2BOOSTVOL_6DB               (7)

//#define AUXR2BOOSTVOL_N12DB             (1)
//#define AUXR2BOOSTVOL_N9DB              (2)
//#define AUXR2BOOSTVOL_N6DB              (3)
//#define AUXR2BOOSTVOL_N3DB              (4)
//#define AUXR2BOOSTVOL_0DB               (5)
//#define AUXR2BOOSTVOL_3DB               (6)
//#define AUXR2BOOSTVOL_6DB               (7)
void cobra_wm8978_aux_gain(uint8_t gain)
{
		uint16_t regval;

    regval = cobra_wm8978_read_reg(REG_LEFT_ADC_BOOST);
    regval &= ~(AUXL2BOOSTVOL_MASK << AUXL2BOOSTVOL_POS);
    cobra_wm8978_write_reg(REG_LEFT_ADC_BOOST, (regval | gain));
	
		 
    regval = cobra_wm8978_read_reg(REG_RIGHT_ADC_BOOST);
    regval &= ~(AUXR2BOOSTVOL_MASK << AUXR2BOOSTVOL_POS);
    cobra_wm8978_write_reg(REG_RIGHT_ADC_BOOST, regval | gain);
}  

//WM8978 DAC/ADC配置
//adcen:adc使能(1)/关闭(0)
//dacen:dac使能(1)/关闭(0)

void cobra_wm8978_adda_cfg(uint8_t dacen,uint8_t adcen)
{
		uint16_t regval;
    regval=cobra_wm8978_read_reg(REG_POWER_MANAGEMENT3);
    if(dacen)
        regval |= (DACENL | DACENR);
    else
        regval &= ~(DACENL | DACENR);
    cobra_wm8978_write_reg(REG_POWER_MANAGEMENT3,regval);
		
    regval=cobra_wm8978_read_reg(REG_POWER_MANAGEMENT2);
    if(adcen)
        regval |= (ADCENR | ADCENL);
    else
        regval &= ~(ADCENR | ADCENL);
    cobra_wm8978_write_reg(REG_POWER_MANAGEMENT2, regval);
}


//WM8978 输入通道配置 
//micen:MIC开启(1)/关闭(0)
//lineinen:Line In开启(1)/关闭(0)
//auxen:aux开启(1)/关闭(0) 
void cobra_wm8978_input_cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
		uint16_t regval;
    regval=cobra_wm8978_read_reg(REG_POWER_MANAGEMENT2);
    if(micen)
        regval |= (INPPGAENL | INPPGAENR);
    else
        regval &= ~(INPPGAENL | INPPGAENR);
		
    cobra_wm8978_write_reg(REG_POWER_MANAGEMENT2, regval);
		
    regval=cobra_wm8978_read_reg(REG_INPUT);
    if(micen)
        regval |= (LIP2INPVGA | LIN2INPVGA | RIP2INPVGA | RIN2INPVGA);
    else
        regval &= ~(LIP2INPVGA | LIN2INPVGA | RIP2INPVGA | RIN2INPVGA);
		
    cobra_wm8978_write_reg(REG_INPUT,regval);
		
    if(lineinen)
        cobra_wm8978_linein_gain(L2_2BOOSTVOL_0DB);
    else
        cobra_wm8978_linein_gain(L2_2BOOSTVOL_DISABLED);
		
    if(auxen)
        cobra_wm8978_aux_gain(AUXR2BOOSTVOL_0DB);
    else
        cobra_wm8978_aux_gain(AUXL2BOOSTVOL_N12DB);
}


void cobra_wm8978_MIC_mute(uint8_t mute_en)
{
		uint16_t regval;
    regval=cobra_wm8978_read_reg(REG_LEFT_PGA_GAIN);
		if(mute_en)
				regval |= INPPGAMUTEL;
		else
				regval &= ~INPPGAMUTEL;
		cobra_wm8978_write_reg(REG_LEFT_PGA_GAIN, regval);
		
		regval=cobra_wm8978_read_reg(REG_RIGHT_PGA_GAIN);
		if(mute_en)
				regval |= INPPGAMUTEL;
		else
				regval &= ~INPPGAMUTEL;
		
		regval |= INPPGAUPDATE;
		cobra_wm8978_write_reg(REG_RIGHT_PGA_GAIN, regval);

}

//WM8978 输出配置 
//dacen:DAC输出(放音)开启(1)/关闭(0)
//bpsen:Bypass输出(录音,包括MIC,LINE IN,AUX等)开启(1)/关闭(0)
void cobra_wm8978_output_cfg(uint8_t dacen,uint8_t bpsen)
{
		uint16_t regval;
	
    regval=cobra_wm8978_read_reg(REG_LEFT_MIXER);
	
    if(dacen)
        regval |= (DACL2LMIX);
		else
				regval &= ~(DACL2LMIX);
		
    if(bpsen)
    {
        regval |= (BYPL2LMIX | BYPLMIXVOL_0DB);
    }
		else
		{
			  regval &= ~(BYPL2LMIX);
		}
    cobra_wm8978_write_reg(REG_LEFT_MIXER,regval);
		
		regval=cobra_wm8978_read_reg(REG_RIGHT_MIXER);
		
		if(dacen)
        regval |= (DACR2RMIX);
		else
				regval &= ~(DACR2RMIX);
		
		if(bpsen)
    {
        regval |= (AUXR2RMIX | BYPRMIXVOL_0DB);
    }
		else
		{
			  regval &= ~(AUXR2RMIX);
		}
		
    cobra_wm8978_write_reg(REG_RIGHT_MIXER,regval);
}


//设置耳机左右声道音量
//voll:左声道音量(0~63)
//volr:右声道音量(0~63)
void cobra_wm8978_hpvol_set(uint8_t voll,uint8_t volr)
{
		uint16_t regval;
	
    regval=cobra_wm8978_read_reg(REG_LOUT1_VOL);	
		regval &= ~0x3f;
		voll &= 0x3f;
	  if(voll == 0) 
			voll |= LOUT1MUTE;
		
		cobra_wm8978_write_reg(REG_LOUT1_VOL, regval|voll);
	
		regval=cobra_wm8978_read_reg(REG_ROUT1_VOL);
		regval &= ~0x3f;
    volr &= 0x3F;
    if(volr==0) 
			volr |= ROUT1MUTE;
	
    cobra_wm8978_write_reg(REG_ROUT1_VOL,volr|regval|(HPVU));
	
}


//设置喇叭音量
//voll:左声道音量(0~63)
void cobra_wm8978_spk_vol_set(uint8_t volx)
{
		uint16_t regval;
	
    regval=cobra_wm8978_read_reg(REG_LOUT2_VOL);	
	  regval &= ~0x3f;
		volx &= 0x3f;//限定范围
		if(volx==0)
		{
			volx |= LOUT2MUTE;									//音量为0时,直接mute 
		}
		
		cobra_wm8978_write_reg(REG_LOUT2_VOL,volx|regval);							//R54,喇叭左声道音量设置
		
		regval=cobra_wm8978_read_reg(REG_ROUT2_VOL);	
		regval &= ~0x3f;
		volx &= 0x3f;//限定范围
		if(volx == 0)
		{
			volx |= ROUT2MUTE;									//音量为0时,直接mute 
		}
		
		cobra_wm8978_write_reg(REG_ROUT2_VOL, volx|regval|(SPKVU));				//R55,喇叭右声道音量设置,同步更新(SPKVU=1)
}

//设置I2S工作模式
//fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
//len:0,16位;1,20位;2,24位;3,32位;

//len
//#define WL_16BITS                               (0)
//#define WL_20BITS                               (1 << 5)
//#define WL_24BITS                               (2 << 5)        // Default value
//#define WL_32BITS                               (3 << 5)

//fmt
//#define FMT_RIGHT_JUSTIFIED             				(0)
//#define FMT_LEFT_JUSTIFIED              				(1 << 3)
//#define FMT_I2S                                 (2 << 3)        // Default value
//#define FMT_PCM                                 (3 << 3)
//#define DACLRSWAP                               (1 << 2)

//#define STERO_EN				1
//#define MONO_EN					0

void cobra_wm8978_i2s_cfg(uint8_t fmt,uint8_t len, uint8_t stero)
{
		uint16_t regval;
	
		regval=cobra_wm8978_read_reg(REG_AUDIO_INTERFACE);
	
		regval &= ~(0x03 << 3);
		regval &= ~(0x03 << 5);

		if(stero)
		{
				regval |= fmt;
				regval |= len;
				regval &= ~MONO;
				//cobra_wm8978_write_reg(REG_AUDIO_INTERFACE, regval);	//R4,WM8978工作模式设置	立体声
		}
		else
		{
				regval |= fmt;
				regval |= len; 
				regval |= MONO;										//单声道		
													
		}
		cobra_wm8978_write_reg(REG_AUDIO_INTERFACE, regval);	
	
}

unsigned int cobra_wm8978_init(void)
{
		unsigned int i;
		for(i = 0; i < sizeof(WM8978_DEFAULT)/sizeof(uint16_t); i++)
		{
				WM8978_REGVAL[i] = WM8978_DEFAULT[i];
		}
		
		if(cobra_wm8978_write_reg(0,0))						//software reset WM8978
		{
				printf("cobra_wm8978_init init fail .... \n");
				return 0;
		}

		//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
		cobra_wm8978_write_reg(REG_POWER_MANAGEMENT1, PLLEN|MICBEN|BIASEN|VMIDSEL_5K);			  //0x3B	
		
		//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
		cobra_wm8978_write_reg(REG_POWER_MANAGEMENT2, ROUT1EN|LOUT1EN|BOOSTENR|BOOSTENL);				//0x1B0
		
		//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable
		cobra_wm8978_write_reg(REG_POWER_MANAGEMENT3, LOUT2EN|ROUT2EN|RMIXEN|LMIXEN);			   //0x6C	

		cobra_wm8978_write_reg(REG_CLOCK_GEN, CLKSEL_PLL|BCLK_DIV1|MCLK_DIV1_5);					//MCLK Provide by outside.  0x120

//PLL setting
		cobra_wm8978_write_reg(REG_PLL_N,  0x0c);					//add in 2018-5-16  16k
		cobra_wm8978_write_reg(REG_PLL_K1, 0);			//add in 2019-3-15
		cobra_wm8978_write_reg(REG_PLL_K2, 0);			//add in 2019-3-15
		cobra_wm8978_write_reg(REG_PLL_K3, 0);			//add in 2019-3-15

		cobra_wm8978_write_reg(REG_BEEP,  BEEPVOL_N3DB);				//INVROUT2 Anti- to.drive trumpet.
		
		cobra_wm8978_write_reg(REG_LEFT_ADC_BOOST, PGABOOSTL);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
		
		cobra_wm8978_write_reg(REG_RIGHT_ADC_BOOST, PGABOOSTR);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
		
		cobra_wm8978_write_reg(REG_OUTPUT, TSDEN);				//TSDEN,Lead hot protection enable.
		cobra_wm8978_write_reg(REG_DAC, DACOSR128);				//SOFTMUTE close,128x sample rate
		cobra_wm8978_write_reg(REG_ADC, ADCOSR128);
		
		
		return 1;
}


unsigned int cobra_wm8978_init_pll(void)
{
	
		if(cobra_wm8978_write_reg(0,0))						//software reset WM8978
		{
				printf("cobra_wm8978_init init fail .... \n");
				return 0;
		}
		
    cobra_wm8978_write_reg(REG_POWER_MANAGEMENT1, PLLEN|MICBEN|BIASEN|VMIDSEL_5K);							//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    cobra_wm8978_write_reg(REG_POWER_MANAGEMENT2, ROUT1EN|LOUT1EN|BOOSTENR|BOOSTENL);			//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    cobra_wm8978_write_reg(REG_POWER_MANAGEMENT3, LOUT2EN|ROUT2EN|RMIXEN|LMIXEN);			//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable

		cobra_wm8978_write_reg(REG_CLOCK_GEN, CLKSEL_PLL|BCLK_DIV1|MCLK_DIV1_5);		
//    cobra_wm8978_write_reg(6, 0x100);					//MCLK Provide by outside.  //0x120 1.5  0x100  1

	
    cobra_wm8978_write_reg(REG_PLL_N, 0x18);		//modify in 2018-12-9  pll_16k 0x18
    cobra_wm8978_write_reg(REG_PLL_K1, 0);			//add in 2019-3-15
    cobra_wm8978_write_reg(REG_PLL_K2, 0);			//add in 2019-3-15
    cobra_wm8978_write_reg(REG_PLL_K3, 0);			//add in 2019-3-15

    cobra_wm8978_write_reg(43, 1<<4);				//INVROUT2 Anti- to.drive trumpet.
		
    cobra_wm8978_write_reg(REG_LEFT_ADC_BOOST,  PGABOOSTL);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    cobra_wm8978_write_reg(REG_RIGHT_ADC_BOOST, PGABOOSTR);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
		
    cobra_wm8978_write_reg(49, 1<<1);				//TSDEN,Lead hot protection enable.
    cobra_wm8978_write_reg(10, 1<<3);				//SOFTMUTE close,128x sample rate
    cobra_wm8978_write_reg(14, 1<<3);
		


    return 1;
}

#endif

