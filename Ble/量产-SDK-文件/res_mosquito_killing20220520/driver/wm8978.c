#include <cobra_config.h>
#include "wm8978.h"
#include "cobra_uart.h"

#if COBRA_WM8978_EN

static uint16_t wm8978_RegCash[] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0100, 0x00FF,
    0x00FF, 0x0000, 0x012C, 0x002C, 0x002C, 0x002C, 0x002C, 0x0000,
    0x0032, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0038, 0x000B, 0x0032, 0x0000, 0x0008, 0x000C, 0x0093, 0x00E9,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0003, 0x0010, 0x0010, 0x0100,
    0x0100, 0x0002, 0x0001, 0x0001, 0x0039, 0x0039, 0x0039, 0x0039,
    0x0001, 0x0001
};
//#define I2C_DMA

#ifdef I2C_DMA
#include "dma.h"
bool WM8978_i2c_Write_Reg (volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address, uint8_t reg, uint8_t data)
{
    uint8_t cmd_data[5] = {0};
    cmd_data[0] = RDW_I2C_TX_START|RDW_I2C_TX_STOP;
    cmd_data[1] = 2+1;
    cmd_data[2] = slave_address | RDW_I2C_WRITE;
    cmd_data[3] = reg & 0xff;
    cmd_data[4] = data& 0xff;
    dma_set_channel_control(CHANNEL13_CTRL,CHANNEL13_I2C1_TX);
    dma_transfer_bytes((uint32_t )(cmd_data),(uint32_t)(&(i2c->TX_DATA)),DMA_DATA_INC_BYTE,DMA_DATA_INC_NO,5,EACH_1_DMA_TRANSFER_ARBITRATES,CHANNEL13,DMA_BASIC);
    wait_dma_channel_complete(CHANNEL13,PRI_SELECT);
}
#endif
#ifndef I2C_DMA
bool WM8978_i2c_Write_Reg (volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address, uint8_t reg, uint8_t data)
{

    bool stat = 0;
    stat = rdw_i2c_write_byte (i2c, RDW_I2C_TX_START|RDW_I2C_TX_STOP );
    if(stat!=0)
        return stat;
    stat = rdw_i2c_write_byte (i2c, 2+1);
    if(stat!=0)
        return stat;
    stat = rdw_i2c_write_byte (i2c, (slave_address | RDW_I2C_WRITE));
    if(stat!=0)
        return stat;
    stat = rdw_i2c_write_byte (i2c, reg & 0xff);
    if(stat!=0)
        return stat;
    stat = rdw_i2c_write_byte (i2c, data& 0xff);
    if(stat!=0)
        return stat;
    return stat;
}
#endif
bool WM8978_Write_Reg (volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,	uint8_t reg, uint16_t data)
{
    bool stat = 0;
    uint8_t RegAddr;
    uint8_t RegValue;
    RegAddr=(reg<<1)|((uint8_t)((data>>8)&0x01));
    RegValue=(uint8_t)data&0xFF;
    stat = WM8978_i2c_Write_Reg (i2c, slave_address,RegAddr, RegValue);
    wm8978_RegCash[reg] = data;
    return stat;
}

void WM8978_Reset(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address)
{
    uint8_t i;
    for(i=0; i<52; i++)
    {
        WM8978_i2c_Write_Reg (i2c, slave_address,i, wm8978_RegCash[i]);
    }
}

uint16_t WM8978_Read_Reg ( uint8_t reg)
{
    return wm8978_RegCash[reg];
}

/*DAC/ADC enable
 *dacen:1,enable;0,disable
 *adcen:1,enable;0,disable */
void WM8978_ADDA_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t dacen,uint8_t adcen)
{
    uint16_t regval;
    regval=WM8978_Read_Reg(3);
    if(dacen)
        regval|=3<<0;
    else
        regval&=~(3<<0);
    WM8978_Write_Reg(i2c,slave_address,3,regval);
    regval=WM8978_Read_Reg(2);
    if(adcen)
        regval|=3<<0;
    else
        regval&=~(3<<0);
    WM8978_Write_Reg(i2c,slave_address,2,regval);
}

void WM8978_LINEIN_Gain(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t gain)
{
    uint16_t regval;
    gain&=0X07;
    regval=WM8978_Read_Reg(47);
    regval&=~(7<<4);
    WM8978_Write_Reg(i2c,slave_address,47,regval|gain<<4);
    regval=WM8978_Read_Reg(48);
    regval&=~(7<<4);
    WM8978_Write_Reg(i2c,slave_address,48,regval|gain<<4);
}

void WM8978_AUX_Gain(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t gain)
{
    uint16_t regval;
    gain&=0X07;
    regval=WM8978_Read_Reg(47);
    regval&=~(7<<0);
    WM8978_Write_Reg(i2c,slave_address,47,regval|gain<<0);
    regval=WM8978_Read_Reg(48);
    regval&=~(7<<0);
    WM8978_Write_Reg(i2c,slave_address,48,regval|gain<<0);
}


/*Input channel
 * micen:1,MIC enable;0,MIC disable
 * lineinen:1,LINE IN enable;0,LINE IN MIC disable
 * auxen:1,AUX enable;0,AUX disable*/
void WM8978_Input_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
    uint16_t regval;
    regval=WM8978_Read_Reg(2);
    if(micen)
        regval|=3<<2;
    else
        regval&=~(3<<2);
    WM8978_Write_Reg(i2c,slave_address,2,regval);
    regval=WM8978_Read_Reg(44);
    if(micen)
        regval|=3<<4|3<<0;
    else
        regval&=~(3<<4|3<<0);
    WM8978_Write_Reg(i2c,slave_address,44,regval);
    if(lineinen)
        WM8978_LINEIN_Gain(i2c,slave_address,5);
    else
        WM8978_LINEIN_Gain(i2c,slave_address,0);
    if(auxen)
        WM8978_AUX_Gain(i2c,slave_address,7);
    else
        WM8978_AUX_Gain(i2c,slave_address,0);
}

/*Out channel
 * dacen:1,DAC output enable;0,DAC output disable
 * bpsen:1,bypass output enable;0,bypass output disable*/
void WM8978_Output_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t dacen,uint8_t bpsen)
{
    uint16_t regval=0;
    if(dacen)
        regval|=1<<0;
    if(bpsen)
    {
        regval|=1<<1;
        regval|=5<<2;
    }
    WM8978_Write_Reg(i2c,slave_address,50,regval);
    WM8978_Write_Reg(i2c,slave_address,51,regval);
}

void WM8978_HPvol_Set(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t voll,uint8_t volr)
{
    voll&=0X3F;
    volr&=0X3F;
    if(voll==0)voll|=1<<6;
    if(volr==0)volr|=1<<6;
    WM8978_Write_Reg(i2c,slave_address,52,voll);
    WM8978_Write_Reg(i2c,slave_address,53,volr|(1<<8));
}

void WM8978_SPKvol_Set(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t volx)
{
    volx&=0X3F;
    if(volx==0)volx|=1<<6;
    WM8978_Write_Reg(i2c,slave_address,54,volx);
    WM8978_Write_Reg(i2c,slave_address,55,volx|(1<<8));
}

/*Set I2S work mode
 * fmt:0,MSB;1,LSB;2,Philip;3,PCM/DSP
 * len:0,16bit;1,20bit;2,24bit;3,32bit*/
void WM8978_I2S_Cfg(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t fmt,uint8_t len)
{
    fmt&=0x03;
    len&=0x03;
    WM8978_Write_Reg(i2c,slave_address,4,(fmt<<3)|(len<<5)|0x0);
//	WM8978_Write_Reg(i2c,slave_address,6,0x00B);
}

void WM8978_MIC_Gain(volatile COBRA_I2C_TypeDef *i2c,uint8_t slave_address,uint8_t gain)
{
    gain&=0X3F;
    WM8978_Write_Reg(i2c,slave_address,45,gain);
    WM8978_Write_Reg(i2c,slave_address,46,gain|1<<8);
}

uint8_t WM8978_Init_Receive(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster)
{
    uint8_t Res;
    Res = WM8978_Write_Reg(i2c, slave_address,0,0);						//software reset WM8978
    while(Res!=0)
    {
        Res = WM8978_Write_Reg(i2c, slave_address,0,0);
        printf("I2C Error!\n");
    }
    WM8978_Write_Reg(i2c, slave_address,1,0X3B);				//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    WM8978_Write_Reg(i2c, slave_address,2,0X1B0);				//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    WM8978_Write_Reg(i2c, slave_address,3,0X6C);				//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable
    if(isMaster)
        WM8978_Write_Reg(i2c, slave_address,6,0x0);					//MCLK Provide by outside.
    else
        WM8978_Write_Reg(i2c, slave_address,6,0x10B);
    WM8978_Write_Reg(i2c, slave_address,36,0xC);					//add in 2018-5-16
    WM8978_Write_Reg(i2c, slave_address,43,1<<4);				//INVROUT2 Anti- to.drive trumpet.
    WM8978_Write_Reg(i2c, slave_address,47,1<<8);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,48,1<<8);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,49,1<<1);				//TSDEN,Lead hot protection enable.
    WM8978_Write_Reg(i2c, slave_address,10,1<<3);				//SOFTMUTE close,128x sample rate
    WM8978_Write_Reg(i2c, slave_address,14,1<<3);
    WM8978_I2S_Cfg(i2c, slave_address,2,0);							//Set I2S mode Philip ,16bit
    WM8978_ADDA_Cfg(i2c, slave_address,0,0);						//Close DAC close ADC.
    WM8978_Input_Cfg(i2c, slave_address,0,0,0);					//Close input channel
    WM8978_Output_Cfg(i2c, slave_address,0,0);					//Close output channel
    WM8978_HPvol_Set(i2c, slave_address,25,25);					//Set headphones volume
    WM8978_SPKvol_Set(i2c, slave_address,25);						//Set trumpet volume

    WM8978_ADDA_Cfg(i2c, slave_address,0,1);
    WM8978_Input_Cfg(i2c, slave_address,1,0,0);
    WM8978_Output_Cfg(i2c, slave_address,0,1);
    WM8978_LINEIN_Gain(i2c, slave_address,7);
    WM8978_MIC_Gain(i2c, slave_address,63);
    WM8978_I2S_Cfg(i2c, slave_address,mode,len);
    WM8978_HPvol_Set(i2c, slave_address,45,45);
    WM8978_SPKvol_Set(i2c, slave_address,25);

    return 0;
}

//void i2s_received_test(uint32_t *databuffer,uint32_t datanum)
//{
//    volatile RDW_I2S_T   *i2s;
//    uint8_t slave_address;
//    uint32_t	i ;
//    i2s = RDW_I2S0;
//    slave_address = 0x34;
//    rdw_i2c_set_bit_rate(cobra_i2c1, 16000000, 400000);
//    rdw_i2c_master_enable (cobra_i2c1);
//    WM8978_Init_Receive(cobra_i2c1, slave_address,VM8978_I2S_MODE,VM8978_LEN_32,1);
//    rdw_i2s_bits_per_word(i2s,RDW_I2S_TX_32_BIT|RDW_I2S_RX_32_BIT);
//    rdw_i2s_control(i2s,RDW_I2S_TX_ENABLE|RDW_I2S_TX_FIFO_FLUSH|RDW_I2S_RX_ENABLE|RDW_I2S_RX_FIFO_FLUSH|RDW_I2S_MASTER|RDW_I2S_SD_TX_MASK|RDW_I2S_MODE_PHILIPS);
//    for(i=0; i<datanum;)
//    {
//        if(!(rdw_i2s_status(i2s)&RDW_I2S_RX_EMPTY))
//        {
//            databuffer[i] = rdw_i2s_read_data(i2s);
//            i++;
//        }
//    }

//}

uint8_t WM8978_Init_Send(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster)
{
    WM8978_Write_Reg(i2c, slave_address,0,0);						//software reset WM8978
    WM8978_Write_Reg(i2c, slave_address,1,0X3B);				//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    WM8978_Write_Reg(i2c, slave_address,2,0X1B0);				//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    WM8978_Write_Reg(i2c, slave_address,3,0X6C);				//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable
    if(isMaster)
        WM8978_Write_Reg(i2c, slave_address,6,0x120);					//MCLK Provide by outside.  //0x120 1.5  0x100  1
    else
        WM8978_Write_Reg(i2c, slave_address,6,0x10B);

//	WM8978_Write_Reg(i2c, slave_address,36,0x8);		//add in 2019-3-20   44.1
//	WM8978_Write_Reg(i2c, slave_address,37,0);			//add in 2019-3-20
//	WM8978_Write_Reg(i2c, slave_address,38,0);			//add in 2019-3-20
//	WM8978_Write_Reg(i2c, slave_address,39,0);			//add in 2019-3-20

    WM8978_Write_Reg(i2c, slave_address,36,0xC);					//add in 2018-5-16  16k
    WM8978_Write_Reg(i2c, slave_address,37,0);			//add in 2019-3-15
    WM8978_Write_Reg(i2c, slave_address,38,0);			//add in 2019-3-15
    WM8978_Write_Reg(i2c, slave_address,39,0);			//add in 2019-3-15

    WM8978_Write_Reg(i2c, slave_address,43,1<<4);				//INVROUT2 Anti- to.drive trumpet.
    WM8978_Write_Reg(i2c, slave_address,47,1<<8);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,48,1<<8);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,49,1<<1);				//TSDEN,Lead hot protection enable.
    WM8978_Write_Reg(i2c, slave_address,10,1<<3);				//SOFTMUTE close,128x sample rate
    WM8978_Write_Reg(i2c, slave_address,14,1<<3);
    WM8978_I2S_Cfg(i2c, slave_address,2,0);							//Set I2S mode Philip ,16bit
    WM8978_ADDA_Cfg(i2c, slave_address,0,0);						//Close DAC close ADC.
    WM8978_Input_Cfg(i2c, slave_address,0,0,0);					//Close input channel
    WM8978_Output_Cfg(i2c, slave_address,0,0);					//Close output channel
    WM8978_HPvol_Set(i2c, slave_address,25,25);					//Set headphones volume
    WM8978_SPKvol_Set(i2c, slave_address,25);						//Set trumpet volume

    WM8978_ADDA_Cfg(i2c, slave_address,1,0);
    WM8978_Input_Cfg(i2c, slave_address,0,0,0);
    WM8978_Output_Cfg(i2c, slave_address,1,0);
//	WM8978_LINEIN_Gain(i2c, slave_address,7);
//	WM8978_MIC_Gain(i2c, slave_address,63);
    WM8978_I2S_Cfg(i2c, slave_address,mode,len);
    WM8978_HPvol_Set(i2c, slave_address,60,60);
    WM8978_SPKvol_Set(i2c, slave_address,0);

    return 0;
}

uint8_t WM8978_Init_Send_PLL(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster)
{
    WM8978_Write_Reg(i2c, slave_address,0,0);						//software reset WM8978
    WM8978_Write_Reg(i2c, slave_address,1,0X1B);				//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    WM8978_Write_Reg(i2c, slave_address,2,0X1B0);				//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    WM8978_Write_Reg(i2c, slave_address,3,0X6C);				//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable
    if(isMaster)
        WM8978_Write_Reg(i2c, slave_address,6,0x040);					//MCLK Provide by outside.  //0x120 1.5  0x100  1
    else
        WM8978_Write_Reg(i2c, slave_address,6,0x10B);

//	WM8978_Write_Reg(i2c, slave_address,36,0x8);		//add in 2019-3-20   44.1
//	WM8978_Write_Reg(i2c, slave_address,37,0);			//add in 2019-3-20
//	WM8978_Write_Reg(i2c, slave_address,38,0);			//add in 2019-3-20
//	WM8978_Write_Reg(i2c, slave_address,39,0);			//add in 2019-3-20

    WM8978_Write_Reg(i2c, slave_address,36,0x18);		//modify in 2018-12-9  pll_16k
    WM8978_Write_Reg(i2c, slave_address,37,0);			//add in 2019-3-15
    WM8978_Write_Reg(i2c, slave_address,38,0);			//add in 2019-3-15
    WM8978_Write_Reg(i2c, slave_address,39,0);			//add in 2019-3-15

    WM8978_Write_Reg(i2c, slave_address,43,1<<4);				//INVROUT2 Anti- to.drive trumpet.
    WM8978_Write_Reg(i2c, slave_address,47,1<<8);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,48,1<<8);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,49,1<<1);				//TSDEN,Lead hot protection enable.
    WM8978_Write_Reg(i2c, slave_address,10,1<<3);				//SOFTMUTE close,128x sample rate
    WM8978_Write_Reg(i2c, slave_address,14,1<<3);
    WM8978_I2S_Cfg(i2c, slave_address,2,0);							//Set I2S mode Philip ,16bit
    WM8978_ADDA_Cfg(i2c, slave_address,0,0);						//Close DAC close ADC.
    WM8978_Input_Cfg(i2c, slave_address,0,0,0);					//Close input channel
    WM8978_Output_Cfg(i2c, slave_address,0,0);					//Close output channel
    WM8978_HPvol_Set(i2c, slave_address,25,25);					//Set headphones volume
    WM8978_SPKvol_Set(i2c, slave_address,25);						//Set trumpet volume

    WM8978_ADDA_Cfg(i2c, slave_address,1,1);
    WM8978_Input_Cfg(i2c, slave_address,1,0,0);
    WM8978_Output_Cfg(i2c, slave_address,1,0);
//	WM8978_LINEIN_Gain(i2c, slave_address,0);
	WM8978_MIC_Gain(i2c, slave_address,25);
    WM8978_I2S_Cfg(i2c, slave_address,mode,len);
    WM8978_HPvol_Set(i2c, slave_address,60,60);
    WM8978_SPKvol_Set(i2c, slave_address,0);

    return 0;
}

uint8_t WM8978_Init_Receive_Send(volatile COBRA_I2C_TypeDef *i2c, uint8_t slave_address,uint8_t mode,uint8_t len,bool isMaster)
{
    WM8978_Write_Reg(i2c, slave_address,0,0);						//software reset WM8978
    WM8978_Write_Reg(i2c, slave_address,1,0X1B);				//MICEN SET 1(MIC Enable)  BIASEN SET 1(Emulator Work) VMIDSEL[1:0] SET 11(5K)
    WM8978_Write_Reg(i2c, slave_address,2,0X1B0);				//ROUT2,LOUT2 output enable(Headphones Work) ,BOOSTENR,BOOSTENL tnable
    WM8978_Write_Reg(i2c, slave_address,3,0X6C);				//ROUT1,LOUT1 output enable(Trumpet Work) ,RMIX,LMIX tnable
    if(isMaster)
    {
        WM8978_Write_Reg(i2c, slave_address,6,0x000);					//MCLK Provide by outside.
    }
    else
    {
//		WM8978_Write_Reg(i2c, slave_address,6,0x125);				//46.875
        WM8978_Write_Reg(i2c, slave_address,6,0x1A5);					//16k
    }

    WM8978_Write_Reg(i2c, slave_address,36,0x8);					//add in 2018-5-16  16k
    WM8978_Write_Reg(i2c, slave_address,37,0);			//add in 2019-3-15
    WM8978_Write_Reg(i2c, slave_address,38,0);			//add in 2019-3-15
    WM8978_Write_Reg(i2c, slave_address,39,0);			//add in 2019-3-15

    WM8978_Write_Reg(i2c, slave_address,43,1<<4);				//INVROUT2 Anti- to.drive trumpet.
    WM8978_Write_Reg(i2c, slave_address,47,1<<8);				//PGABOOSTL,Left passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,48,1<<8);				//PGABOOSTR,Right passage MIC get 2000% a benefit by increase
    WM8978_Write_Reg(i2c, slave_address,49,1<<1);				//TSDEN,Lead hot protection enable.
    WM8978_Write_Reg(i2c, slave_address,10,1<<3);				//SOFTMUTE close,128x sample rate
    WM8978_Write_Reg(i2c, slave_address,14,1<<3);
    WM8978_I2S_Cfg(i2c, slave_address,2,0);							//Set I2S mode Philip ,16bit
    WM8978_ADDA_Cfg(i2c, slave_address,0,0);						//Close DAC close ADC.
    WM8978_Input_Cfg(i2c, slave_address,0,0,0);					//Close input channel
    WM8978_Output_Cfg(i2c, slave_address,0,0);					//Close output channel
    WM8978_HPvol_Set(i2c, slave_address,25,25);					//Set headphones volume
    WM8978_SPKvol_Set(i2c, slave_address,25);						//Set trumpet volume

    WM8978_ADDA_Cfg(i2c, slave_address,1,1);
    WM8978_Input_Cfg(i2c, slave_address,1,1,0);
    WM8978_Output_Cfg(i2c, slave_address,1,0);
//    WM8978_LINEIN_Gain(i2c, slave_address,3);
	  WM8978_MIC_Gain(i2c, slave_address,0x1C);
    WM8978_I2S_Cfg(i2c, slave_address,mode,len);
    WM8978_HPvol_Set(i2c, slave_address,60,60);
    WM8978_SPKvol_Set(i2c, slave_address,0);

    return 0;
}
#endif  //#if COBRA_I2C_EN

