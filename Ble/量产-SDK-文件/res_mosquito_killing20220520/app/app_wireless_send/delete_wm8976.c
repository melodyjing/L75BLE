/* 
 liujian add
 2018/7/2
 version 1.0 
 
WM8976 code driver
*/
#include "app_voice_buffer.h"
#include "delete_wm8976.h"     // SW configuration
#include "cobra_i2c.h"
#include "cobra4A_system.h"
#include <stdio.h>

#if((USE_WM8976_SAMPLE) && (BLE_APP_RDW_VOICE))

#define WM8976_ADDRESS		0x34
void WM8978_MIC_Gain(uint8_t gain);
static uint16_t WM8978_REGVAL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 


uint8_t WM8978_Write_Reg(uint8_t reg,uint16_t val)
{
	
		uint8_t stat = 0;
		uint8_t writeDataBuf[2];
	    writeDataBuf[0] = (uint8_t)(reg<<1)|((uint8_t)((val>>8)&0x01));;
	    writeDataBuf[1] = (uint8_t)val;
#if 1	
	
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
		
		while(cobra_i2c_get_status(M3_I2C0)&RDW_I2C_NACK);
		
#else
		
		RDW_I2C_write_byte(RDW_I2C_TX_START|RDW_I2C_TX_STOP );
		RDW_I2C_write_byte(3);
		RDW_I2C_write_byte(WM8976_ADDRESS|RDW_I2C_WRITE);
		RDW_I2C_write_byte(writeDataBuf[0]);
		RDW_I2C_write_byte(writeDataBuf[1]);
		while(cobra_i2c_get_status(M3_I2C0)&RDW_I2C_NACK);

#endif
		
		WM8978_REGVAL[reg] = val;
		
		return stat;
}


void WM8978_uninit(void)
{
		
		cobra_i2c_write_byte(M3_I2C0, RDW_I2C_TX_STOP_ON_NACK );
		cobra_i2c_write_byte(M3_I2C0, 3);
		cobra_i2c_write_byte(M3_I2C0, WM8976_ADDRESS|RDW_I2C_WRITE);
		cobra_i2c_write_byte(M3_I2C0, 0x00);
		cobra_i2c_write_byte(M3_I2C0, 0x00);
		while(cobra_i2c_get_status(M3_I2C0)&RDW_I2C_NACK);
}

uint16_t WM8978_Read_Reg(uint8_t reg)
{  
	return WM8978_REGVAL[reg];	
} 

uint8_t WM8978_Init(void)
{
	//I2C总线必须在调用该函数前完成初始化

    uint32_t i;
	uint8_t Res;
	Res=WM8978_Write_Reg(0,0);							//软复位WM8978
	if(Res)
	{
		printf("WM8978_Init fail .... \n");
		return 1;															//发送指令失败,WM8978异常
	}
	for(i = 0; i < 1000; i++);    //Delay Liujian add
	WM8978_Write_Reg(1,0X1B);								//R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)  PLLL off
//	WM8978_Write_Reg(1,0X3B);								//R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)  PLL ON
	WM8978_Write_Reg(2,0X1B0);							//R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
	WM8978_Write_Reg(3,0X6C);								//R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能	

	WM8978_Write_Reg(6,0);									//R6,MCLK由外部提供
	//WM8978_Write_Reg(6,0x100);		       //liujian add
	
	WM8978_Write_Reg(7, 0x6);   //liujian add 
	
	WM8978_Write_Reg(43,1<<4);							//R43,INVROUT2反向,驱动喇叭

	WM8978_Write_Reg(47,1<<8);							//R47设置,PGABOOSTL,左通道MIC获得20倍增益
	WM8978_Write_Reg(48,1<<8);							//R48设置,PGABOOSTR,右通道MIC获得20倍增益

	WM8978_Write_Reg(32, 0x1F8);                //ALC on

	WM8978_Write_Reg(49,1<<1);							//R49,TSDEN,开启过热保护 
	WM8978_Write_Reg(10,1<<3);							//R10,SOFTMUTE关闭,128x采样,最佳SNR 
	WM8978_Write_Reg(14,1<<3);							//R14,ADC 128x采样率
	
	//WM8978_I2S_Cfg(1, 0);		//LJF 16bit								//设置I2S接口模式，数据位数不需要设置，播放从设备不使用
	//放音设置
	WM8978_ADDA_Cfg(1,0);										//开启DAC
	WM8978_Input_Cfg(0,0,0);								//关闭输入通道
	WM8978_Output_Cfg(1,1);									//开启DAC输出  
	WM8978_HPvol_Set(40,40);
	WM8978_SPKvol_Set(50);
	WM8978_MIC_Gain(60);
	return 0;

}
//WM8978 DAC/ADC配置
//adcen:adc使能(1)/关闭(0)
//dacen:dac使能(1)/关闭(0)
void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen)
{
	uint16_t regval;
	regval=WM8978_Read_Reg(3);							//读取R3
	if(dacen)
		regval|=3<<0;													//R3最低2个位设置为1,开启DACR&DACL
	else 
		regval&=~(3<<0);											//R3最低2个位清零,关闭DACR&DACL.
	WM8978_Write_Reg(3,regval);							//设置R3
	regval=WM8978_Read_Reg(2);							//读取R2
	if(adcen)
		regval|=3<<0;			        						//R2最低2个位设置为1,开启ADCR&ADCL
	else 
		regval&=~(3<<0);				  						//R2最低2个位清零,关闭ADCR&ADCL.
	WM8978_Write_Reg(2,regval);							//设置R2	
}
//WM8978 输入通道配置 
//micen:MIC开启(1)/关闭(0)
//lineinen:Line In开启(1)/关闭(0)
//auxen:aux开启(1)/关闭(0) 
void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
	uint16_t regval;  
	regval=WM8978_Read_Reg(2);							//读取R2
	if(micen)
		regval|=3<<2;													//开启INPPGAENR,INPPGAENL(MIC的PGA放大)
	else 
		regval&=~(3<<2);											//关闭INPPGAENR,INPPGAENL.
 	WM8978_Write_Reg(2,regval);							//设置R2 
	regval=WM8978_Read_Reg(44);							//读取R44
	if(micen)
		regval|=3<<4|3<<0;										//开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	else 
		regval&=~(3<<4|3<<0);									//关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
	WM8978_Write_Reg(44,regval);						//设置R44


	if(lineinen)
		WM8978_LINEIN_Gain(5);								//LINE IN 0dB增益
	else 
		WM8978_LINEIN_Gain(0);								//关闭LINE IN
	if(auxen)
		WM8978_AUX_Gain(7);										//AUX 6dB增益
	else 
		WM8978_AUX_Gain(0);										//关闭AUX输入  
}
//WM8978 MIC增益设置(不包括BOOST的20dB,MIC-->ADC输入部分的增益)
//gain:0~63,对应-12dB~35.25dB,0.75dB/Step
void WM8978_MIC_Gain(uint8_t gain)
{
	gain&=0X3F;
	WM8978_Write_Reg(45,gain);							//R45,左通道PGA设置 
	WM8978_Write_Reg(46,gain|1<<8);					//R46,右通道PGA设置
}
//WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);							//读取R47
	regval&=~(7<<4);												//清除原来的设置 
 	WM8978_Write_Reg(47,regval|gain<<4);		//设置R47
	regval=WM8978_Read_Reg(48);							//读取R48
	regval&=~(7<<4);												//清除原来的设置 
 	WM8978_Write_Reg(48,regval|gain<<4);		//设置R48
} 
//WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);							//读取R47
	regval&=~(7<<0);												//清除原来的设置 
 	WM8978_Write_Reg(47,regval|gain<<0);		//设置R47
	regval=WM8978_Read_Reg(48);							//读取R48
	regval&=~(7<<0);												//清除原来的设置 
 	WM8978_Write_Reg(48,regval|gain<<0);		//设置R48
}  

//WM8978 输出配置 
//dacen:DAC输出(放音)开启(1)/关闭(0)
//bpsen:Bypass输出(录音,包括MIC,LINE IN,AUX等)开启(1)/关闭(0)
void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen)
{
	uint16_t regval=0;
	if(dacen)
		regval|=1<<0;													//DAC输出使能
	if(bpsen)
	{
		regval|=1<<1;													//BYPASS使能
		regval|=6<<2;													//0dB增益
	} 
	WM8978_Write_Reg(50,regval);						//R50设置
	WM8978_Write_Reg(51,regval);						//R51设置 
}
//设置耳机左右声道音量
//voll:左声道音量(0~63)
//volr:右声道音量(0~63)
void WM8978_HPvol_Set(uint8_t voll,uint8_t volr)
{
	voll&=0X3F;
	volr&=0X3F;															//限定范围
	if(voll==0)voll|=1<<6;									//音量为0时,直接mute
	if(volr==0)volr|=1<<6;									//音量为0时,直接mute 
	WM8978_Write_Reg(52,voll);							//R52,耳机左声道音量设置
	WM8978_Write_Reg(53,volr|(1<<8));				//R53,耳机右声道音量设置,同步更新(HPVU=1)
}
//设置喇叭音量
//voll:左声道音量(0~63)
void WM8978_SPKvol_Set(uint8_t volx)
{
	volx&=0X3F;//限定范围
	if(volx==0)volx|=1<<6;									//音量为0时,直接mute 
 	WM8978_Write_Reg(54,volx);							//R54,喇叭左声道音量设置
	WM8978_Write_Reg(55,volx|(1<<8));				//R55,喇叭右声道音量设置,同步更新(SPKVU=1)
}
//设置I2S工作模式
//fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
//len:0,16位;1,20位;2,24位;3,32位;
void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len, uint8_t stero)
{
	fmt&=0x03;
	len&=0x03;															//限定范围
  if(stero)
	{
			WM8978_Write_Reg(4,(fmt<<3)|(len<<5)|(1<<7));	//R4,WM8978工作模式设置	立体声
	}
	else
	{
		WM8978_Write_Reg(4,(fmt<<3)|(len<<5)|(0x01));  //单声道		
	//		WM8978_Write_Reg(4,(fmt<<3)|(len<<5)|(1<<7)|0x01);  //单声道		
	//	WM8978_Write_Reg(4,(fmt<<3)|(len<<5)|0x01);
		
	}
	
//	WM8978_Write_Reg(7, 0x001);
}

unsigned int wm8978_init(void)
{
		 //I2C总线必须在调用该函数前完成初始化
#if 0
			uint32_t i;
			uint8_t Res;
			Res=WM8978_Write_Reg(0,0);							//软复位WM8978
			if(Res)
			{
				printf("wm8978 init fail .... \n");
				return 1;															//发送指令失败,WM8978异常
			}
			for(i = 0; i < 1000; i++);    //Delay Liujian add
			
			WM8978_Write_Reg(1, 0x1b);								//R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)  PLLL off
		//	WM8978_Write_Reg(1,0X3B);								//R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)  PLL ON
			WM8978_Write_Reg(2, 0x1b0);							//R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
			WM8978_Write_Reg(3, 0x6c);								//R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能	

			WM8978_Write_Reg(6, 0);									//R6,MCLK由外部提供
			//WM8978_Write_Reg(6,0x100);		       //liujian add
			
			WM8978_Write_Reg(7,  0x6);   //liujian add 
			
			WM8978_Write_Reg(43, 1<<4);							//R43,INVROUT2反向,驱动喇叭

			WM8978_Write_Reg(47, 1<<8);							//R47设置,PGABOOSTL,左通道MIC获得20倍增益
			WM8978_Write_Reg(48, 1<<8);							//R48设置,PGABOOSTR,右通道MIC获得20倍增益

			WM8978_Write_Reg(32, 0x1f8);                //ALC on

			WM8978_Write_Reg(49, 1<<1);							//R49,TSDEN,开启过热保护 
			WM8978_Write_Reg(10, 1<<3);							//R10,SOFTMUTE关闭,128x采样,最佳SNR 
			WM8978_Write_Reg(14, 1<<3);							//R14,ADC 128x采样率
			
			//WM8978_I2S_Cfg(1, 0);		//LJF 16bit								//设置I2S接口模式，数据位数不需要设置，播放从设备不使用
			//放音设置
//			wm8978_adda_cfg(1,0);										//开启DAC
//			
//			wm8978_input_cfg(0,0,0);								//关闭输入通道
//			
//			wm8978_output_cfg(1, 0);									//开启DAC输出  
//			
//			wm8978_hpvol_set(40, 40);
//			
//			wm8978_spk_vol_set(50);
//			
//			wm8978_mic_gain(60);
#else
		WM8978_Write_Reg(0,0);						//software reset WM8978
    WM8978_Write_Reg(1,0X3B);				//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    WM8978_Write_Reg(2,0X1B0);				//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    WM8978_Write_Reg(3,0X6C);				//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable

    WM8978_Write_Reg(6,0x120);					//MCLK Provide by outside.


    WM8978_Write_Reg(36,0xC);					//add in 2018-5-16  16k
    WM8978_Write_Reg(37,0);			//add in 2019-3-15
    WM8978_Write_Reg(38,0);			//add in 2019-3-15
    WM8978_Write_Reg(39,0);			//add in 2019-3-15

    WM8978_Write_Reg(43,1<<4);				//INVROUT2 Anti- to.drive trumpet.
    WM8978_Write_Reg(47,1<<8);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(48,1<<8);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(49,1<<1);				//TSDEN,Lead hot protection enable.
    WM8978_Write_Reg(10,1<<3);				//SOFTMUTE close,128x sample rate
    WM8978_Write_Reg(14,1<<3);

		WM8978_MIC_Gain(10);
#endif
			return 0;
	
}


//void WM8976_init(void)
//{
////		unsigned int apb_i2c_clk;
////		
////		apb_i2c_clk = cobra_system_get_m3_apb1_clk();
////	
////		cobra_i2c_set_bit_rate(M3_I2C0, apb_i2c_clk, 200000);    //i2c 200Khz
////	
////		cobra_i2c_maste_enable(M3_I2C0);
//	
//		WM8978_Init();
//		
//}
#endif


