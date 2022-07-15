/******************************************************************************
	File Name	:	wm8976_drv.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/07

	Description :   

	Changed Log	:
		Liujian  2020/03/07			- Creation

*****************************************************************************/

#include "wm8976_drv.h"
#include "cobra_i2c.h"
#include "cobra_i2s.h"
#include "cobra_system.h"
#include "rdw_dma.h"

#include <stdio.h>
#if BLE_APP_RECEIVE_VOICE

void wm8978_aux_gain(uint8_t gain);
void wm8978_linein_gain(uint8_t gain);

#define WM8976_ADDRESS		0x34

//static uint16_t WM8978_REGVAL[58]=
//{
//	0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000,
//	0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0100, 0x00FF,
//	0X00FF,	0X0000,	0X012C,	0X002C,	0X002C,	0X002C,	0X002C,	0X0000,
//	0X0032,	0X0000,	0X0000,	0X0000,	0X0000,	0X0000,	0X0000,	0X0000,
//	0X0038,	0X000B,	0X0032,	0X0000,	0X0008,	0X000C,	0X0093,	0X00E9,
//	0X0000,	0X0000,	0X0000,	0X0000,	0X0003,	0X0010,	0X0010,	0X0100,
//	0X0100,	0X0002,	0X0001,	0X0001,	0X0039,	0X0039,	0X0039,	0X0039,
//	0X0001,	0X0001
//}; 

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


uint8_t wm8978_write_reg(uint8_t reg,uint16_t val)
{
		uint8_t stat = 0;
		uint8_t writeDataBuf[2];
	  writeDataBuf[0] = (uint8_t)(reg<<1)|((uint8_t)((val>>8)&0x01));;
	  writeDataBuf[1] = (uint8_t)val;
	
		if(cobra_i2c_write_byte(M3_I2C0, RDW_I2C_TX_START|RDW_I2C_TX_STOP ) < 0)
			stat = 1;
		if(cobra_i2c_write_byte(M3_I2C0, 3) < 0)   //3 byte
			stat = 1;
		if(cobra_i2c_write_byte(M3_I2C0, WM8976_ADDRESS|RDW_I2C_WRITE) < 0)
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

uint16_t wm8978_read_reg(uint8_t reg)
{  
	return WM8978_REGVAL[reg];	
} 


//WM8978 DAC/ADC����
//adcen:adcʹ��(1)/�ر�(0)
//dacen:dacʹ��(1)/�ر�(0)

void wm8978_adda_cfg(uint8_t dacen,uint8_t adcen)
{
		uint16_t regval;
    regval=wm8978_read_reg(3);
    if(dacen)
        regval|=3<<0;
    else
        regval&=~(3<<0);
    wm8978_write_reg(3,regval);
    regval=wm8978_read_reg(2);
    if(adcen)
        regval|=3<<0;
    else
        regval&=~(3<<0);
    wm8978_write_reg(2,regval);
}

//WM8978 ����ͨ������ 
//micen:MIC����(1)/�ر�(0)
//lineinen:Line In����(1)/�ر�(0)
//auxen:aux����(1)/�ر�(0) 
void wm8978_input_cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
		uint16_t regval;
    regval=wm8978_read_reg(2);
    if(micen)
        regval|=3<<2;
    else
        regval&=~(3<<2);
    wm8978_write_reg(2,regval);
    regval=wm8978_read_reg(44);
    if(micen)
        regval|=3<<4|3<<0;
    else
        regval&=~(3<<4|3<<0);
    wm8978_write_reg(44,regval);
    if(lineinen)
        wm8978_linein_gain(5);
    else
        wm8978_linein_gain(0);
    if(auxen)
        wm8978_aux_gain(7);
    else
        wm8978_aux_gain(0);
}


//WM8978 MIC��������(������BOOST��20dB,MIC-->ADC���벿�ֵ�����)
//gain:0~63,��Ӧ-12dB~35.25dB,0.75dB/Step
void wm8978_mic_gain(uint8_t gain)
{
		gain &= 0x3F;
		
		wm8978_write_reg(45,gain);							//R45,��ͨ��PGA���� 
		wm8978_write_reg(46,gain|1<<8);					//R46,��ͨ��PGA����
	
}


//WM8978 L2/R2(Ҳ����Line In)��������(L2/R2-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void wm8978_linein_gain(uint8_t gain)
{
		uint16_t regval;
    gain&=0X07;
    regval=wm8978_read_reg(47);
    regval&=~(7<<4);
    wm8978_write_reg(47,regval|gain<<4);
    regval=wm8978_read_reg(48);
    regval&=~(7<<4);
    wm8978_write_reg(48,regval|gain<<4);
} 


//WM8978 AUXR,AUXL(PWM��Ƶ����)��������(AUXR/L-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void wm8978_aux_gain(uint8_t gain)
{
		uint16_t regval;
    gain&=0X07;
    regval=wm8978_read_reg(47);
    regval&=~(7<<0);
    wm8978_write_reg(47,regval|gain<<0);
    regval=wm8978_read_reg(48);
    regval&=~(7<<0);
    wm8978_write_reg(48,regval|gain<<0);
}  

//WM8978 ������� 
//dacen:DAC���(����)����(1)/�ر�(0)
//bpsen:Bypass���(¼��,����MIC,LINE IN,AUX��)����(1)/�ر�(0)
void wm8978_output_cfg(uint8_t dacen,uint8_t bpsen)
{
		 uint16_t regval=0;
    if(dacen)
        regval|=1<<0;
    if(bpsen)
    {
        regval|=1<<1;
        regval|=5<<2;
    }
    wm8978_write_reg(50,regval);
    wm8978_write_reg(51,regval);
}


//���ö���������������
//voll:����������(0~63)
//volr:����������(0~63)
void wm8978_hpvol_set(uint8_t voll,uint8_t volr)
{
		voll&=0X3F;
    volr&=0X3F;
    if(voll==0)voll|=1<<6;
    if(volr==0)volr|=1<<6;
    wm8978_write_reg(52,voll);
    wm8978_write_reg(53,volr|(1<<8));
}

//������������
//voll:����������(0~63)
void wm8978_spk_vol_set(uint8_t volx)
{
		volx &= 0x3F;//�޶���Χ
		if(volx==0)
			volx|=1<<6;									//����Ϊ0ʱ,ֱ��mute 
		
		wm8978_write_reg(54,volx);							//R54,������������������
		wm8978_write_reg(55,volx|(1<<8));				//R55,������������������,ͬ������(SPKVU=1)
}
//����I2S����ģʽ
//fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;
//len:0,16λ;1,20λ;2,24λ;3,32λ;
void wm8978_i2s_cfg(uint8_t fmt,uint8_t len, uint8_t stero)
{
	fmt &= 0x03;
	len &= 0x03;															//�޶���Χ
  if(stero)
	{
			wm8978_write_reg(4,(fmt<<3)|(len<<5)|(1<<7));	//R4,WM8978����ģʽ����	������
	}
	else
	{
		wm8978_write_reg(4,(fmt<<3)|(len<<5)|(0x01));  //������		
		
	//		WM8978_Write_Reg(4,(fmt<<3)|(len<<5)|(1<<7)|0x01);  //������		
	//	WM8978_Write_Reg(4,(fmt<<3)|(len<<5)|0x01);
		
	}
	
		fmt&=0x03;
    len&=0x03;
    wm8978_write_reg(4,(fmt<<3)|(len<<5)|0x0);
	
	
//	WM8978_Write_Reg(7, 0x001);
}
unsigned int wm8978_init(void)
{
		 //I2C���߱����ڵ��øú���ǰ��ɳ�ʼ��
#if 0	
			uint32_t i;
			uint8_t Res;
			Res=wm8978_write_reg(0,0);							//��λWM8978
			if(Res)
			{
				printf("wm8978 init fail .... \n");
				return 1;															//����ָ��ʧ��,WM8978�쳣
			}
			for(i = 0; i < 1000; i++);    //Delay Liujian add
			
			wm8978_write_reg(1, 0x1b);								//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)  PLLL off
		//	WM8978_Write_Reg(1,0X3B);								//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)  PLL ON
			wm8978_write_reg(2, 0x1b0);							//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
			wm8978_write_reg(3, 0x6c);								//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	

			wm8978_write_reg(6, 0);									//R6,MCLK���ⲿ�ṩ
			//WM8978_Write_Reg(6,0x100);		       //liujian add
			
			wm8978_write_reg(7,  0x6);   //liujian add 
			
			wm8978_write_reg(43, 1<<4);							//R43,INVROUT2����,��������

			wm8978_write_reg(47, 1<<8);							//R47����,PGABOOSTL,��ͨ��MIC���20������
			wm8978_write_reg(48, 1<<8);							//R48����,PGABOOSTR,��ͨ��MIC���20������

			wm8978_write_reg(32, 0x1f8);                //ALC on

			wm8978_write_reg(49, 1<<1);							//R49,TSDEN,�������ȱ��� 
			wm8978_write_reg(10, 1<<3);							//R10,SOFTMUTE�ر�,128x����,���SNR 
			wm8978_write_reg(14, 1<<3);							//R14,ADC 128x������
#else
			wm8978_write_reg(0,0);						//software reset WM8978
    wm8978_write_reg(1,0X3B);				//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    wm8978_write_reg(2,0X1B0);				//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    wm8978_write_reg(3,0X6C);				//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable

    wm8978_write_reg(6,0x120);					//MCLK Provide by outside.


    wm8978_write_reg(36,0xC);					//add in 2018-5-16  16k
    wm8978_write_reg(37,0);			//add in 2019-3-15
    wm8978_write_reg(38,0);			//add in 2019-3-15
    wm8978_write_reg(39,0);			//add in 2019-3-15

    wm8978_write_reg(43,1<<4);				//INVROUT2 Anti- to.drive trumpet.
    wm8978_write_reg(47,1<<8);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    wm8978_write_reg(48,1<<8);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
    wm8978_write_reg(49,1<<1);				//TSDEN,Lead hot protection enable.
    wm8978_write_reg(10,1<<3);				//SOFTMUTE close,128x sample rate
    wm8978_write_reg(14,1<<3);

//		WM8978_MIC_Gain(10);


#endif

			//WM8978_I2S_Cfg(1, 0);		//LJF 16bit								//����I2S�ӿ�ģʽ������λ������Ҫ���ã����Ŵ��豸��ʹ��
			//��������
//			wm8978_adda_cfg(1,0);										//����DAC
//			
//			wm8978_input_cfg(0,0,0);								//�ر�����ͨ��
//			
//			wm8978_output_cfg(1, 0);									//����DAC���  
//			
//			wm8978_hpvol_set(40, 40);
//			
//			wm8978_spk_vol_set(50);
//			
//			wm8978_mic_gain(60);

			return 0;
	
}






#endif

