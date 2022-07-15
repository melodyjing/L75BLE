#ifndef _SRC_H
#define _SRC_H
/*RDW SRC module,reference Cobra4_src.csv,time 2019 08 09 16:32:09*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cobra_config.h>
#if COBRA_SRC_EN
typedef struct RDW_SRC {
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED0[10];                          /*0x000-0x024*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_CTRL;                               /*0x028*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_MCLK;                               /*0x02c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_PDM;                                /*0x030*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_ADC_DATA_INTF;                      /*0x034*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DMA_UPFIFO_MASK;                    /*0x038*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_INTERRUPT_STICKY;                   /*0x03c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR_GROUP;                          /*0x040*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_UPFIFO_ALMOST_THRESHOLD;            /*0x044*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DMA_UPFIFO_CTRL;                    /*0x048*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_CIC;                                /*0x04c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_BYPASS;                             /*0x050*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_OUT_BITWIDTH;                       /*0x054*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DC_OFFSET;                          /*0x058*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_WHO_TO_OUT;                         /*0x05c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_ZERO_CROSSING_TIMEOUT_VALUE;        /*0x060*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_VOLUME_SLIDE_CTRL;                  /*0x064*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED1[4];                           /*0x068-0x074*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_HP_IIR_NUM;                         /*0x078*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_HP_IIR_DEN;                         /*0x07c*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED2;                              /*0x080*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_CLK_SPEED;                          /*0x084*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_HIGH_SPEED_FIR;                     /*0x088*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED3[15];                          /*0x08c-0x0c4*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR0_NUM;                           /*0x0c8*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR0_DEN;                           /*0x0cc*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR1_NUM;                           /*0x0d0*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR1_DEN;                           /*0x0d4*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR2_NUM;                           /*0x0d8*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR2_DEN;                           /*0x0dc*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR3_NUM;                           /*0x0e0*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR3_DEN;                           /*0x0e4*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR4_NUM;                           /*0x0e8*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR4_DEN;                           /*0x0ec*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR5_NUM;                           /*0x0f0*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR5_DEN;                           /*0x0f4*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR6_NUM;                           /*0x0f8*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR6_DEN;                           /*0x0fc*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR7_NUM;                           /*0x100*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR7_DEN;                           /*0x104*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR8_NUM;                           /*0x108*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR8_DEN;                           /*0x10c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR9_NUM;                           /*0x110*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR9_DEN;                           /*0x114*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR0_G0;                            /*0x118*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR1_G0;                            /*0x11c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR2_G0;                            /*0x120*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR3_G0;                            /*0x124*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR4_G0;                            /*0x128*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR5_G0;                            /*0x12c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR6_G0;                            /*0x130*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR7_G0;                            /*0x134*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR8_G0;                            /*0x138*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_IIR9_G0;                            /*0x13c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_MANUAL_GAIN_COARSE;                 /*0x140*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_MANUAL_GAIN_FINE;                   /*0x144*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_MANUAL_GAIN_MODE;                   /*0x148*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED4[13];                          /*0x14c-0x17c*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT0;                           /*0x180*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT1;                           /*0x184*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT2;                           /*0x188*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT3;                           /*0x18c*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT4;                           /*0x190*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT5;                           /*0x194*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT6;                           /*0x198*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_RDAT7;                           /*0x19c*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED5[4];                           /*0x1a0-0x1ac*/
    __attribute__ ((aligned (4))) volatile unsigned int INT_STATUS;                             /*0x1b0*/
    __attribute__ ((aligned (4))) volatile unsigned int VOLUME_SLIDE_STATUS;                    /*0x1b4*/
    __attribute__ ((aligned (4))) volatile unsigned int UPFIFO_STATUS;                          /*0x1b8*/
    __attribute__ ((aligned (4))) volatile unsigned int AUDIOADC_LEFT_CHANNEL_FIFO_STATUS;      /*0x1bc*/
    __attribute__ ((aligned (4))) volatile unsigned int AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS;     /*0x1c0*/
} COBRA_SRC_TypeDef;
#define SRC0_BASE        (0x400cc000)
//#define SRC0             ((volatile COBRA_SRC_TypeDef   *) SRC0_BASE )
extern volatile COBRA_SRC_TypeDef *  cobra_src0;
/*CFG_CTRL*/
#define CFG_HIGH_SPEED_FIRDATARAM_CLEAR                                       0x00400000UL
static inline void set_cfg_high_speed_firdataram_clear(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_HIGH_SPEED_FIRDATARAM_CLEAR;
    } else
    {
        src->CFG_CTRL &= ~CFG_HIGH_SPEED_FIRDATARAM_CLEAR;
    }
}
static inline bool read_cfg_high_speed_firdataram_clear(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_HIGH_SPEED_FIRDATARAM_CLEAR) == CFG_HIGH_SPEED_FIRDATARAM_CLEAR;
}
#define CFG_AGC_RAM_CLK_IS_HCLK                                               0x00200000UL
static inline void set_cfg_agc_ram_clk_is_hclk(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_AGC_RAM_CLK_IS_HCLK;
    } else
    {
        src->CFG_CTRL &= ~CFG_AGC_RAM_CLK_IS_HCLK;
    }
}
static inline bool read_cfg_agc_ram_clk_is_hclk(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_AGC_RAM_CLK_IS_HCLK) == CFG_AGC_RAM_CLK_IS_HCLK;
}
#define CFG_FAST_CLK_IS_HCLK                                                  0x00100000UL
static inline void set_cfg_fast_clk_is_hclk(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_FAST_CLK_IS_HCLK;
    } else
    {
        src->CFG_CTRL &= ~CFG_FAST_CLK_IS_HCLK;
    }
}
static inline bool read_cfg_fast_clk_is_hclk(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_FAST_CLK_IS_HCLK) == CFG_FAST_CLK_IS_HCLK;
}
#define CFG_CLK_EN                                                            0x00080000UL
static inline void set_cfg_clk_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_CLK_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_CLK_EN;
    }
}
static inline bool read_cfg_clk_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_CLK_EN) == CFG_CLK_EN;
}
#define CFG_XTAL1_PLL0_SEL                                                    0x00040000UL
static inline void set_cfg_xtal1_pll0_sel(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_XTAL1_PLL0_SEL;
    } else
    {
        src->CFG_CTRL &= ~CFG_XTAL1_PLL0_SEL;
    }
}
static inline bool read_cfg_xtal1_pll0_sel(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_XTAL1_PLL0_SEL) == CFG_XTAL1_PLL0_SEL;
}
#define CFG_I2S_ENABLE                                                        0x00001000UL
static inline void set_cfg_i2s_enable(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_I2S_ENABLE;
    } else
    {
        src->CFG_CTRL &= ~CFG_I2S_ENABLE;
    }
}
static inline bool read_cfg_i2s_enable(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_I2S_ENABLE) == CFG_I2S_ENABLE;
}
#define CFG_AUDIO_MUTE_EN                                                     0x00000800UL
static inline void set_cfg_audio_mute_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_AUDIO_MUTE_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_AUDIO_MUTE_EN;
    }
}
static inline bool read_cfg_audio_mute_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_AUDIO_MUTE_EN) == CFG_AUDIO_MUTE_EN;
}
#define CFG_RIGHT_COPY_LEFT_EN                                                0x00000400UL
static inline void set_cfg_right_copy_left_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_RIGHT_COPY_LEFT_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_RIGHT_COPY_LEFT_EN;
    }
}
static inline bool read_cfg_right_copy_left_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_RIGHT_COPY_LEFT_EN) == CFG_RIGHT_COPY_LEFT_EN;
}
#define CFG_LEFT_COPY_RIGHT_EN                                                0x00000200UL
#define CFG_IIR_GROUP_AS_DECIMATION_EN                                        0x00000100UL
static inline void set_cfg_iir_group_as_decimation_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_IIR_GROUP_AS_DECIMATION_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_IIR_GROUP_AS_DECIMATION_EN;
    }
}
static inline bool read_cfg_iir_group_as_decimation_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_IIR_GROUP_AS_DECIMATION_EN) == CFG_IIR_GROUP_AS_DECIMATION_EN;
}
#define CFG_HP_IIR_EN                                                         0x00000040UL
static inline void set_cfg_hp_iir_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_HP_IIR_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_HP_IIR_EN;
    }
}
static inline bool read_cfg_hp_iir_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_HP_IIR_EN) == CFG_HP_IIR_EN;
}
#define CFG_IIR_GROUP_EN                                                      0x00000020UL
static inline void set_cfg_iir_group_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_IIR_GROUP_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_IIR_GROUP_EN;
    }
}
static inline bool read_cfg_iir_group_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_IIR_GROUP_EN) == CFG_IIR_GROUP_EN;
}
#define CFG_LEFT_RIGHT_SWAP                                                   0x00000010UL
static inline void set_cfg_left_right_swap(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_LEFT_RIGHT_SWAP;
    } else
    {
        src->CFG_CTRL &= ~CFG_LEFT_RIGHT_SWAP;
    }
}
static inline bool read_cfg_left_right_swap(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_LEFT_RIGHT_SWAP) == CFG_LEFT_RIGHT_SWAP;
}
#define CFG_RIGHT_EN                                                          0x00000008UL
static inline void set_cfg_right_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_RIGHT_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_RIGHT_EN;
    }
}
static inline bool read_cfg_right_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_RIGHT_EN) == CFG_RIGHT_EN;
}
#define CFG_LEFT_EN                                                           0x00000004UL
static inline void set_cfg_left_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_LEFT_EN;
    } else
    {
        src->CFG_CTRL &= ~CFG_LEFT_EN;
    }
}
static inline bool read_cfg_left_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_LEFT_EN) == CFG_LEFT_EN;
}
#define CFG_ADC0_PDM1                                                         0x00000002UL
static inline void set_cfg_adc0_pdm1(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_ADC0_PDM1;
    } else
    {
        src->CFG_CTRL &= ~CFG_ADC0_PDM1;
    }
}
static inline bool read_cfg_adc0_pdm1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_ADC0_PDM1) == CFG_ADC0_PDM1;
}
#define CFG_INTF_FIFO_ENABLE                                                  0x00000001UL
static inline void set_cfg_intf_fifo_enable(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CTRL |= CFG_INTF_FIFO_ENABLE;
    } else
    {
        src->CFG_CTRL &= ~CFG_INTF_FIFO_ENABLE;
    }
}
static inline bool read_cfg_intf_fifo_enable(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CTRL & CFG_INTF_FIFO_ENABLE) == CFG_INTF_FIFO_ENABLE;
}

/*CFG_MCLK*/
#define CFG_OSR_BY_MCLK_POW                                                           16UL
#define CFG_OSR_BY_MCLK_MASK                                                  0x07ff0000UL
static inline void set_cfg_osr_by_mclk(uint32_t cfg_osr_by_mclk,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MCLK &= ~CFG_OSR_BY_MCLK_MASK;
    src->CFG_MCLK |= cfg_osr_by_mclk<<CFG_OSR_BY_MCLK_POW;
}
static inline uint32_t read_cfg_osr_by_mclk(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MCLK & CFG_OSR_BY_MCLK_MASK) >> CFG_OSR_BY_MCLK_POW;
}
#define CFG_MCLK_RATIO_POW                                                             0UL
#define CFG_MCLK_RATIO_MASK                                                   0x0000ffffUL
static inline void set_cfg_mclk_ratio(uint32_t cfg_mclk_ratio,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MCLK &= ~CFG_MCLK_RATIO_MASK;
    src->CFG_MCLK |= cfg_mclk_ratio<<CFG_MCLK_RATIO_POW;
}
static inline uint32_t read_cfg_mclk_ratio(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MCLK & CFG_MCLK_RATIO_MASK) >> CFG_MCLK_RATIO_POW;
}

/*CFG_PDM*/
#define CFG_PDM_DATA_DELAY_PHASE_POW                                                   4UL
#define CFG_PDM_DATA_DELAY_PHASE_MASK                                         0x000000f0UL
static inline void set_cfg_pdm_data_delay_phase(uint32_t cfg_pdm_data_delay_phase,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_PDM &= ~CFG_PDM_DATA_DELAY_PHASE_MASK;
    src->CFG_PDM |= cfg_pdm_data_delay_phase<<CFG_PDM_DATA_DELAY_PHASE_POW;
}
static inline uint32_t read_cfg_pdm_data_delay_phase(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_PDM & CFG_PDM_DATA_DELAY_PHASE_MASK) >> CFG_PDM_DATA_DELAY_PHASE_POW;
}
#define CFG_PDM_FOUR_WIRE_IS_2MONO                                            0x00000008UL
static inline void set_cfg_pdm_four_wire_is_2mono(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_PDM |= CFG_PDM_FOUR_WIRE_IS_2MONO;
    } else
    {
        src->CFG_PDM &= ~CFG_PDM_FOUR_WIRE_IS_2MONO;
    }
}
static inline bool read_cfg_pdm_four_wire_is_2mono(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_PDM & CFG_PDM_FOUR_WIRE_IS_2MONO) == CFG_PDM_FOUR_WIRE_IS_2MONO;
}
#define CFG_PDM_LEFT_TWO_WIRE_IS_STEREO                                       0x00000004UL
static inline void set_cfg_pdm_left_two_wire_is_stereo(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_PDM |= CFG_PDM_LEFT_TWO_WIRE_IS_STEREO;
    } else
    {
        src->CFG_PDM &= ~CFG_PDM_LEFT_TWO_WIRE_IS_STEREO;
    }
}
static inline bool read_cfg_pdm_left_two_wire_is_stereo(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_PDM & CFG_PDM_LEFT_TWO_WIRE_IS_STEREO) == CFG_PDM_LEFT_TWO_WIRE_IS_STEREO;
}
#define CFG_PDM_RIGHT_POSEDGE                                                 0x00000002UL
static inline void set_cfg_pdm_right_posedge(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_PDM |= CFG_PDM_RIGHT_POSEDGE;
    } else
    {
        src->CFG_PDM &= ~CFG_PDM_RIGHT_POSEDGE;
    }
}
static inline bool read_cfg_pdm_right_posedge(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_PDM & CFG_PDM_RIGHT_POSEDGE) == CFG_PDM_RIGHT_POSEDGE;
}
#define CFG_PDM_LEFT_POSEDGE                                                  0x00000001UL
static inline void set_cfg_pdm_left_posedge(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_PDM |= CFG_PDM_LEFT_POSEDGE;
    } else
    {
        src->CFG_PDM &= ~CFG_PDM_LEFT_POSEDGE;
    }
}
static inline bool read_cfg_pdm_left_posedge(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_PDM & CFG_PDM_LEFT_POSEDGE) == CFG_PDM_LEFT_POSEDGE;
}

/*CFG_ADC_DATA_INTF*/
#define CFG_ADC_DATA_MSB_SWAP_R                                               0x00000004UL
static inline void set_cfg_adc_data_msb_swap_r(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_ADC_DATA_INTF |= CFG_ADC_DATA_MSB_SWAP_R;
    } else
    {
        src->CFG_ADC_DATA_INTF &= ~CFG_ADC_DATA_MSB_SWAP_R;
    }
}
static inline bool read_cfg_adc_data_msb_swap_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_ADC_DATA_INTF & CFG_ADC_DATA_MSB_SWAP_R) == CFG_ADC_DATA_MSB_SWAP_R;
}
#define CFG_ADC_DATA_MSB_SWAP_L                                               0x00000002UL
static inline void set_cfg_adc_data_msb_swap_l(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_ADC_DATA_INTF |= CFG_ADC_DATA_MSB_SWAP_L;
    } else
    {
        src->CFG_ADC_DATA_INTF &= ~CFG_ADC_DATA_MSB_SWAP_L;
    }
}
static inline bool read_cfg_adc_data_msb_swap_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_ADC_DATA_INTF & CFG_ADC_DATA_MSB_SWAP_L) == CFG_ADC_DATA_MSB_SWAP_L;
}
#define CFG_ADC_RAW_DATA_EN                                                   0x00000001UL
static inline void set_cfg_adc_raw_data_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_ADC_DATA_INTF |= CFG_ADC_RAW_DATA_EN;
    } else
    {
        src->CFG_ADC_DATA_INTF &= ~CFG_ADC_RAW_DATA_EN;
    }
}
static inline bool read_cfg_adc_raw_data_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_ADC_DATA_INTF & CFG_ADC_RAW_DATA_EN) == CFG_ADC_RAW_DATA_EN;
}

/*CFG_DMA_UPFIFO_MASK*/
#define CFG_UPFIFO_ALMOSTEMPTY_INTERRUPT_MASK                                 0x00000020UL
static inline void set_cfg_upfifo_almostempty_interrupt_mask(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_MASK |= CFG_UPFIFO_ALMOSTEMPTY_INTERRUPT_MASK;
    } else
    {
        src->CFG_DMA_UPFIFO_MASK &= ~CFG_UPFIFO_ALMOSTEMPTY_INTERRUPT_MASK;
    }
}
static inline bool read_cfg_upfifo_almostempty_interrupt_mask(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_MASK & CFG_UPFIFO_ALMOSTEMPTY_INTERRUPT_MASK) == CFG_UPFIFO_ALMOSTEMPTY_INTERRUPT_MASK;
}
#define CFG_UPFIFO_ALMOSTFULL_INTERRUPT_MASK                                  0x00000010UL
static inline void set_cfg_upfifo_almostfull_interrupt_mask(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_MASK |= CFG_UPFIFO_ALMOSTFULL_INTERRUPT_MASK;
    } else
    {
        src->CFG_DMA_UPFIFO_MASK &= ~CFG_UPFIFO_ALMOSTFULL_INTERRUPT_MASK;
    }
}
static inline bool read_cfg_upfifo_almostfull_interrupt_mask(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_MASK & CFG_UPFIFO_ALMOSTFULL_INTERRUPT_MASK) == CFG_UPFIFO_ALMOSTFULL_INTERRUPT_MASK;
}
#define CFG_UPFIFO_EMPTY_INTERRUPT_MASK                                       0x00000008UL
static inline void set_cfg_upfifo_empty_interrupt_mask(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_MASK |= CFG_UPFIFO_EMPTY_INTERRUPT_MASK;
    } else
    {
        src->CFG_DMA_UPFIFO_MASK &= ~CFG_UPFIFO_EMPTY_INTERRUPT_MASK;
    }
}
static inline bool read_cfg_upfifo_empty_interrupt_mask(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_MASK & CFG_UPFIFO_EMPTY_INTERRUPT_MASK) == CFG_UPFIFO_EMPTY_INTERRUPT_MASK;
}
#define CFG_UPFIFO_OVERFLOW_INTERRUPT_MASK                                    0x00000004UL
static inline void set_cfg_upfifo_overflow_interrupt_mask(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_MASK |= CFG_UPFIFO_OVERFLOW_INTERRUPT_MASK;
    } else
    {
        src->CFG_DMA_UPFIFO_MASK &= ~CFG_UPFIFO_OVERFLOW_INTERRUPT_MASK;
    }
}
static inline bool read_cfg_upfifo_overflow_interrupt_mask(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_MASK & CFG_UPFIFO_OVERFLOW_INTERRUPT_MASK) == CFG_UPFIFO_OVERFLOW_INTERRUPT_MASK;
}
#define CFG_UPFIFO_UNDERFLOW_INTERRUPT_MASK                                   0x00000002UL
static inline void set_cfg_upfifo_underflow_interrupt_mask(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_MASK |= CFG_UPFIFO_UNDERFLOW_INTERRUPT_MASK;
    } else
    {
        src->CFG_DMA_UPFIFO_MASK &= ~CFG_UPFIFO_UNDERFLOW_INTERRUPT_MASK;
    }
}
static inline bool read_cfg_upfifo_underflow_interrupt_mask(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_MASK & CFG_UPFIFO_UNDERFLOW_INTERRUPT_MASK) == CFG_UPFIFO_UNDERFLOW_INTERRUPT_MASK;
}
#define CFG_UPFIFO_FULL_INTERRUPT_MASK                                        0x00000001UL
static inline void set_cfg_upfifo_full_interrupt_mask(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_MASK |= CFG_UPFIFO_FULL_INTERRUPT_MASK;
    } else
    {
        src->CFG_DMA_UPFIFO_MASK &= ~CFG_UPFIFO_FULL_INTERRUPT_MASK;
    }
}
static inline bool read_cfg_upfifo_full_interrupt_mask(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_MASK & CFG_UPFIFO_FULL_INTERRUPT_MASK) == CFG_UPFIFO_FULL_INTERRUPT_MASK;
}

/*CFG_INTERRUPT_STICKY*/
#define CFG_STCIKY_INT_EN                                                     0x40000000UL
static inline void set_cfg_stciky_int_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_INTERRUPT_STICKY |= CFG_STCIKY_INT_EN;
    } else
    {
        src->CFG_INTERRUPT_STICKY &= ~CFG_STCIKY_INT_EN;
    }
}
static inline bool read_cfg_stciky_int_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_INTERRUPT_STICKY & CFG_STCIKY_INT_EN) == CFG_STCIKY_INT_EN;
}
#define CFG_STICKY_INT_CLR_POW                                                        24UL
#define CFG_STICKY_INT_CLR_MASK                                               0x3f000000UL
static inline void set_cfg_sticky_int_clr(uint32_t cfg_sticky_int_clr,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_INTERRUPT_STICKY &= ~CFG_STICKY_INT_CLR_MASK;
    src->CFG_INTERRUPT_STICKY |= cfg_sticky_int_clr<<CFG_STICKY_INT_CLR_POW;
}
static inline uint32_t read_cfg_sticky_int_clr(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_INTERRUPT_STICKY & CFG_STICKY_INT_CLR_MASK) >> CFG_STICKY_INT_CLR_POW;
}
#define CFG_INT_TIMEOUT2_POW                                                          16UL
#define CFG_INT_TIMEOUT2_MASK                                                 0x00ff0000UL
static inline void set_cfg_int_timeout2(uint32_t cfg_int_timeout2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_INTERRUPT_STICKY &= ~CFG_INT_TIMEOUT2_MASK;
    src->CFG_INTERRUPT_STICKY |= cfg_int_timeout2<<CFG_INT_TIMEOUT2_POW;
}
static inline uint32_t read_cfg_int_timeout2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_INTERRUPT_STICKY & CFG_INT_TIMEOUT2_MASK) >> CFG_INT_TIMEOUT2_POW;
}
#define CFG_INT_TIMEOUT1_POW                                                           0UL
#define CFG_INT_TIMEOUT1_MASK                                                 0x0000ffffUL
static inline void set_cfg_int_timeout1(uint32_t cfg_int_timeout1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_INTERRUPT_STICKY &= ~CFG_INT_TIMEOUT1_MASK;
    src->CFG_INTERRUPT_STICKY |= cfg_int_timeout1<<CFG_INT_TIMEOUT1_POW;
}
static inline uint32_t read_cfg_int_timeout1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_INTERRUPT_STICKY & CFG_INT_TIMEOUT1_MASK) >> CFG_INT_TIMEOUT1_POW;
}

/*CFG_IIR_GROUP*/
#define CFG_IIR_GROUP_10IIR_FOR_WHICH_CH_SEL                                  0x00000020UL
static inline void set_cfg_iir_group_10iir_for_which_ch_sel(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_IIR_GROUP |= CFG_IIR_GROUP_10IIR_FOR_WHICH_CH_SEL;
    } else
    {
        src->CFG_IIR_GROUP &= ~CFG_IIR_GROUP_10IIR_FOR_WHICH_CH_SEL;
    }
}
static inline bool read_cfg_iir_group_10iir_for_which_ch_sel(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR_GROUP & CFG_IIR_GROUP_10IIR_FOR_WHICH_CH_SEL) == CFG_IIR_GROUP_10IIR_FOR_WHICH_CH_SEL;
}
#define CFG_IIR_GROUP_DECI_ONLY_USE_LAST_3IIR_EN                              0x00000010UL
static inline void set_cfg_iir_group_deci_only_use_last_3iir_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_IIR_GROUP |= CFG_IIR_GROUP_DECI_ONLY_USE_LAST_3IIR_EN;
    } else
    {
        src->CFG_IIR_GROUP &= ~CFG_IIR_GROUP_DECI_ONLY_USE_LAST_3IIR_EN;
    }
}
static inline bool read_cfg_iir_group_deci_only_use_last_3iir_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR_GROUP & CFG_IIR_GROUP_DECI_ONLY_USE_LAST_3IIR_EN) == CFG_IIR_GROUP_DECI_ONLY_USE_LAST_3IIR_EN;
}
#define CFG_IIR_GROUP_MODE_POW                                                         0UL
#define CFG_IIR_GROUP_MODE_MASK                                               0x0000000fUL
static inline void set_cfg_iir_group_mode(uint32_t cfg_iir_group_mode,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR_GROUP &= ~CFG_IIR_GROUP_MODE_MASK;
    src->CFG_IIR_GROUP |= cfg_iir_group_mode<<CFG_IIR_GROUP_MODE_POW;
}
static inline uint32_t read_cfg_iir_group_mode(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR_GROUP & CFG_IIR_GROUP_MODE_MASK) >> CFG_IIR_GROUP_MODE_POW;
}

/*CFG_UPFIFO_ALMOST_THRESHOLD*/
#define CFG_UPFIFO_ALMOSTEMPTY_THRESHOLD_POW                                           4UL
#define CFG_UPFIFO_ALMOSTEMPTY_THRESHOLD_MASK                                 0x000000f0UL
static inline void set_cfg_upfifo_almostempty_threshold(uint32_t cfg_upfifo_almostempty_threshold,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_UPFIFO_ALMOST_THRESHOLD &= ~CFG_UPFIFO_ALMOSTEMPTY_THRESHOLD_MASK;
    src->CFG_UPFIFO_ALMOST_THRESHOLD |= cfg_upfifo_almostempty_threshold<<CFG_UPFIFO_ALMOSTEMPTY_THRESHOLD_POW;
}
static inline uint32_t read_cfg_upfifo_almostempty_threshold(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_UPFIFO_ALMOST_THRESHOLD & CFG_UPFIFO_ALMOSTEMPTY_THRESHOLD_MASK) >> CFG_UPFIFO_ALMOSTEMPTY_THRESHOLD_POW;
}
#define CFG_UPFIFO_ALMOSTFULL_THRESHOLD_POW                                            0UL
#define CFG_UPFIFO_ALMOSTFULL_THRESHOLD_MASK                                  0x0000000fUL
static inline void set_cfg_upfifo_almostfull_threshold(uint32_t cfg_upfifo_almostfull_threshold,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_UPFIFO_ALMOST_THRESHOLD &= ~CFG_UPFIFO_ALMOSTFULL_THRESHOLD_MASK;
    src->CFG_UPFIFO_ALMOST_THRESHOLD |= cfg_upfifo_almostfull_threshold<<CFG_UPFIFO_ALMOSTFULL_THRESHOLD_POW;
}
static inline uint32_t read_cfg_upfifo_almostfull_threshold(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_UPFIFO_ALMOST_THRESHOLD & CFG_UPFIFO_ALMOSTFULL_THRESHOLD_MASK) >> CFG_UPFIFO_ALMOSTFULL_THRESHOLD_POW;
}

/*CFG_DMA_UPFIFO_CTRL*/
#define CFG_DMA_REQ_BY_ALMOSTEMPTY                                            0x00000040UL
static inline void set_cfg_dma_req_by_almostempty(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_DMA_REQ_BY_ALMOSTEMPTY;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_DMA_REQ_BY_ALMOSTEMPTY;
    }
}
static inline bool read_cfg_dma_req_by_almostempty(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_DMA_REQ_BY_ALMOSTEMPTY) == CFG_DMA_REQ_BY_ALMOSTEMPTY;
}
#define CFG_DMA_REQ_BY_EMPTY                                                  0x00000020UL
static inline void set_cfg_dma_req_by_empty(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_DMA_REQ_BY_EMPTY;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_DMA_REQ_BY_EMPTY;
    }
}
static inline bool read_cfg_dma_req_by_empty(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_DMA_REQ_BY_EMPTY) == CFG_DMA_REQ_BY_EMPTY;
}
#define CFG_DMA_STOP_BY_UPFIFO_UNDERFLOW_EN                                   0x00000010UL
static inline void set_cfg_dma_stop_by_upfifo_underflow_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_DMA_STOP_BY_UPFIFO_UNDERFLOW_EN;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_DMA_STOP_BY_UPFIFO_UNDERFLOW_EN;
    }
}
static inline bool read_cfg_dma_stop_by_upfifo_underflow_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_DMA_STOP_BY_UPFIFO_UNDERFLOW_EN) == CFG_DMA_STOP_BY_UPFIFO_UNDERFLOW_EN;
}
#define CFG_DMA_STOP_BY_UPFIFO_OVERFLOW_EN                                    0x00000008UL
static inline void set_cfg_dma_stop_by_upfifo_overflow_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_DMA_STOP_BY_UPFIFO_OVERFLOW_EN;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_DMA_STOP_BY_UPFIFO_OVERFLOW_EN;
    }
}
static inline bool read_cfg_dma_stop_by_upfifo_overflow_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_DMA_STOP_BY_UPFIFO_OVERFLOW_EN) == CFG_DMA_STOP_BY_UPFIFO_OVERFLOW_EN;
}
#define CFG_DMA_ERROR_SIGNAL                                                  0x00000004UL
static inline void set_cfg_dma_error_signal(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_DMA_ERROR_SIGNAL;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_DMA_ERROR_SIGNAL;
    }
}
static inline bool read_cfg_dma_error_signal(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_DMA_ERROR_SIGNAL) == CFG_DMA_ERROR_SIGNAL;
}
#define CFG_UPFIFO_DMA_EN                                                     0x00000002UL
static inline void set_cfg_upfifo_dma_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_UPFIFO_DMA_EN;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_UPFIFO_DMA_EN;
    }
}
static inline bool read_cfg_upfifo_dma_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_UPFIFO_DMA_EN) == CFG_UPFIFO_DMA_EN;
}
#define CFG_UPFIFO_EN                                                         0x00000001UL
static inline void set_cfg_upfifo_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_DMA_UPFIFO_CTRL |= CFG_UPFIFO_EN;
    } else
    {
        src->CFG_DMA_UPFIFO_CTRL &= ~CFG_UPFIFO_EN;
    }
}
static inline bool read_cfg_upfifo_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DMA_UPFIFO_CTRL & CFG_UPFIFO_EN) == CFG_UPFIFO_EN;
}

/*CFG_CIC*/
#define CFG_CICOUT_SHRINK_POW                                                          9UL
#define CFG_CICOUT_SHRINK_MASK                                                0x00007e00UL
static inline void set_cfg_cicout_shrink(uint32_t cfg_cicout_shrink,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CIC &= ~CFG_CICOUT_SHRINK_MASK;
    src->CFG_CIC |= cfg_cicout_shrink<<CFG_CICOUT_SHRINK_POW;
}
static inline uint32_t read_cfg_cicout_shrink(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CIC & CFG_CICOUT_SHRINK_MASK) >> CFG_CICOUT_SHRINK_POW;
}
#define CFG_CIC_DIFF_DELAY                                                    0x00000100UL
static inline void set_cfg_cic_diff_delay(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_CIC |= CFG_CIC_DIFF_DELAY;
    } else
    {
        src->CFG_CIC &= ~CFG_CIC_DIFF_DELAY;
    }
}
static inline bool read_cfg_cic_diff_delay(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CIC & CFG_CIC_DIFF_DELAY) == CFG_CIC_DIFF_DELAY;
}
#define CFG_CIC_STAGES_POW                                                             4UL
#define CFG_CIC_STAGES_MASK                                                   0x000000f0UL
static inline void set_cfg_cic_stages(uint32_t cfg_cic_stages,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CIC &= ~CFG_CIC_STAGES_MASK;
    src->CFG_CIC |= cfg_cic_stages<<CFG_CIC_STAGES_POW;
}
static inline uint32_t read_cfg_cic_stages(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CIC & CFG_CIC_STAGES_MASK) >> CFG_CIC_STAGES_POW;
}
#define CFG_LOG2_CIC_DECIMATION_POW                                                    0UL
#define CFG_LOG2_CIC_DECIMATION_MASK                                          0x0000000fUL
static inline void set_cfg_log2_cic_decimation(uint32_t cfg_log2_cic_decimation,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CIC &= ~CFG_LOG2_CIC_DECIMATION_MASK;
    src->CFG_CIC |= cfg_log2_cic_decimation<<CFG_LOG2_CIC_DECIMATION_POW;
}
static inline uint32_t read_cfg_log2_cic_decimation(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CIC & CFG_LOG2_CIC_DECIMATION_MASK) >> CFG_LOG2_CIC_DECIMATION_POW;
}

/*CFG_BYPASS*/
#define CFG_HIGH_SPEED_BYPASS                                                 0x00000200UL
static inline void set_cfg_high_speed_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_HIGH_SPEED_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_HIGH_SPEED_BYPASS;
    }
}
static inline bool read_cfg_high_speed_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_HIGH_SPEED_BYPASS) == CFG_HIGH_SPEED_BYPASS;
}
#define CFG_PDM_DATA_DELAY_BYPASS                                             0x00000100UL
static inline void set_cfg_pdm_data_delay_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_PDM_DATA_DELAY_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_PDM_DATA_DELAY_BYPASS;
    }
}
static inline bool read_cfg_pdm_data_delay_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_PDM_DATA_DELAY_BYPASS) == CFG_PDM_DATA_DELAY_BYPASS;
}
#define CFG_V_S_TOF_BYPASS                                                    0x00000080UL
static inline void set_cfg_v_s_tof_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_V_S_TOF_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_V_S_TOF_BYPASS;
    }
}
static inline bool read_cfg_v_s_tof_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_V_S_TOF_BYPASS) == CFG_V_S_TOF_BYPASS;
}
#define CFG_V_H_BYPASS                                                        0x00000040UL
static inline void set_cfg_v_h_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_V_H_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_V_H_BYPASS;
    }
}
static inline bool read_cfg_v_h_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_V_H_BYPASS) == CFG_V_H_BYPASS;
}
#define CFG_IIR_GROUP_BYPASS                                                  0x00000020UL
static inline void set_cfg_iir_group_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_IIR_GROUP_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_IIR_GROUP_BYPASS;
    }
}
static inline bool read_cfg_iir_group_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_IIR_GROUP_BYPASS) == CFG_IIR_GROUP_BYPASS;
}
#define CFG_HP_IIR_BYPASS                                                     0x00000010UL
static inline void set_cfg_hp_iir_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_HP_IIR_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_HP_IIR_BYPASS;
    }
}
static inline bool read_cfg_hp_iir_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_HP_IIR_BYPASS) == CFG_HP_IIR_BYPASS;
}
#define CFG_CIC_BYPASS                                                        0x00000008UL
static inline void set_cfg_cic_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_CIC_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_CIC_BYPASS;
    }
}
static inline bool read_cfg_cic_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_CIC_BYPASS) == CFG_CIC_BYPASS;
}
#define CFG_HBF2_BYPASS                                                       0x00000004UL
static inline void set_cfg_hbf2_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_HBF2_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_HBF2_BYPASS;
    }
}
static inline bool read_cfg_hbf2_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_HBF2_BYPASS) == CFG_HBF2_BYPASS;
}
#define CFG_HBF1_BYPASS                                                       0x00000002UL
static inline void set_cfg_hbf1_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_HBF1_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_HBF1_BYPASS;
    }
}
static inline bool read_cfg_hbf1_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_HBF1_BYPASS) == CFG_HBF1_BYPASS;
}
#define CFG_HBF0_BYPASS                                                       0x00000001UL
static inline void set_cfg_hbf0_bypass(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_BYPASS |= CFG_HBF0_BYPASS;
    } else
    {
        src->CFG_BYPASS &= ~CFG_HBF0_BYPASS;
    }
}
static inline bool read_cfg_hbf0_bypass(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_BYPASS & CFG_HBF0_BYPASS) == CFG_HBF0_BYPASS;
}

/*CFG_OUT_BITWIDTH*/
#define CFG_I2S_OUT_BITWIDTH_POW                                                       3UL
#define CFG_I2S_OUT_BITWIDTH_MASK                                             0x00000038UL
static inline void set_cfg_i2s_out_bitwidth(uint32_t cfg_i2s_out_bitwidth,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_OUT_BITWIDTH &= ~CFG_I2S_OUT_BITWIDTH_MASK;
    src->CFG_OUT_BITWIDTH |= cfg_i2s_out_bitwidth<<CFG_I2S_OUT_BITWIDTH_POW;
}
static inline uint32_t read_cfg_i2s_out_bitwidth(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_OUT_BITWIDTH & CFG_I2S_OUT_BITWIDTH_MASK) >> CFG_I2S_OUT_BITWIDTH_POW;
}
#define CFG_DMA_OUT_BITWIDTH_POW                                                       0UL
#define CFG_DMA_OUT_BITWIDTH_MASK                                             0x00000007UL
static inline void set_cfg_dma_out_bitwidth(uint32_t cfg_dma_out_bitwidth,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_OUT_BITWIDTH &= ~CFG_DMA_OUT_BITWIDTH_MASK;
    src->CFG_OUT_BITWIDTH |= cfg_dma_out_bitwidth<<CFG_DMA_OUT_BITWIDTH_POW;
}
static inline uint32_t read_cfg_dma_out_bitwidth(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_OUT_BITWIDTH & CFG_DMA_OUT_BITWIDTH_MASK) >> CFG_DMA_OUT_BITWIDTH_POW;
}

/*CFG_DC_OFFSET*/
#define CFG_RIGHT_DC_OFFSET_POW                                                       12UL
#define CFG_RIGHT_DC_OFFSET_MASK                                              0x00fff000UL
static inline void set_cfg_right_dc_offset(uint32_t cfg_right_dc_offset,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_DC_OFFSET &= ~CFG_RIGHT_DC_OFFSET_MASK;
    src->CFG_DC_OFFSET |= cfg_right_dc_offset<<CFG_RIGHT_DC_OFFSET_POW;
}
static inline uint32_t read_cfg_right_dc_offset(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DC_OFFSET & CFG_RIGHT_DC_OFFSET_MASK) >> CFG_RIGHT_DC_OFFSET_POW;
}
#define CFG_LEFT_DC_OFFSET_POW                                                         0UL
#define CFG_LEFT_DC_OFFSET_MASK                                               0x00000fffUL
static inline void set_cfg_left_dc_offset(uint32_t cfg_left_dc_offset,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_DC_OFFSET &= ~CFG_LEFT_DC_OFFSET_MASK;
    src->CFG_DC_OFFSET |= cfg_left_dc_offset<<CFG_LEFT_DC_OFFSET_POW;
}
static inline uint32_t read_cfg_left_dc_offset(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_DC_OFFSET & CFG_LEFT_DC_OFFSET_MASK) >> CFG_LEFT_DC_OFFSET_POW;
}

/*CFG_WHO_TO_OUT*/
#define CFG_WHO_TO_SFT_MUX_POW                                                        10UL
#define CFG_WHO_TO_SFT_MUX_MASK                                               0x00007c00UL
static inline void set_cfg_who_to_sft_mux(uint32_t cfg_who_to_sft_mux,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_WHO_TO_OUT &= ~CFG_WHO_TO_SFT_MUX_MASK;
    src->CFG_WHO_TO_OUT |= cfg_who_to_sft_mux<<CFG_WHO_TO_SFT_MUX_POW;
}
static inline uint32_t read_cfg_who_to_sft_mux(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_WHO_TO_OUT & CFG_WHO_TO_SFT_MUX_MASK) >> CFG_WHO_TO_SFT_MUX_POW;
}
#define CFG_WHO_TO_I2S_MUX_POW                                                         5UL
#define CFG_WHO_TO_I2S_MUX_MASK                                               0x000003e0UL
static inline void set_cfg_who_to_i2s_mux(uint32_t cfg_who_to_i2s_mux,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_WHO_TO_OUT &= ~CFG_WHO_TO_I2S_MUX_MASK;
    src->CFG_WHO_TO_OUT |= cfg_who_to_i2s_mux<<CFG_WHO_TO_I2S_MUX_POW;
}
static inline uint32_t read_cfg_who_to_i2s_mux(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_WHO_TO_OUT & CFG_WHO_TO_I2S_MUX_MASK) >> CFG_WHO_TO_I2S_MUX_POW;
}
#define CFG_WHO_TO_DMA_MUX_POW                                                         0UL
#define CFG_WHO_TO_DMA_MUX_MASK                                               0x0000001fUL
static inline void set_cfg_who_to_dma_mux(uint32_t cfg_who_to_dma_mux,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_WHO_TO_OUT &= ~CFG_WHO_TO_DMA_MUX_MASK;
    src->CFG_WHO_TO_OUT |= cfg_who_to_dma_mux<<CFG_WHO_TO_DMA_MUX_POW;
}
static inline uint32_t read_cfg_who_to_dma_mux(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_WHO_TO_OUT & CFG_WHO_TO_DMA_MUX_MASK) >> CFG_WHO_TO_DMA_MUX_POW;
}

/*CFG_ZERO_CROSSING_TIMEOUT_VALUE*/
#define CFG_ZERO_CROSSING_TIMEOUT_VALUE_POW                                            0UL
#define CFG_ZERO_CROSSING_TIMEOUT_VALUE_MASK                                  0x0000ffffUL
static inline void set_cfg_zero_crossing_timeout_value(uint32_t cfg_zero_crossing_timeout_value,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_ZERO_CROSSING_TIMEOUT_VALUE &= ~CFG_ZERO_CROSSING_TIMEOUT_VALUE_MASK;
    src->CFG_ZERO_CROSSING_TIMEOUT_VALUE |= cfg_zero_crossing_timeout_value<<CFG_ZERO_CROSSING_TIMEOUT_VALUE_POW;
}
static inline uint32_t read_cfg_zero_crossing_timeout_value(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_ZERO_CROSSING_TIMEOUT_VALUE & CFG_ZERO_CROSSING_TIMEOUT_VALUE_MASK) >> CFG_ZERO_CROSSING_TIMEOUT_VALUE_POW;
}

/*CFG_VOLUME_SLIDE_CTRL*/
#define CFG_V_S_TOF_DECLINE_START_POINT_POW                                           17UL
#define CFG_V_S_TOF_DECLINE_START_POINT_MASK                                  0x007e0000UL
static inline void set_cfg_v_s_tof_decline_start_point(uint32_t cfg_v_s_tof_decline_start_point,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_VOLUME_SLIDE_CTRL &= ~CFG_V_S_TOF_DECLINE_START_POINT_MASK;
    src->CFG_VOLUME_SLIDE_CTRL |= cfg_v_s_tof_decline_start_point<<CFG_V_S_TOF_DECLINE_START_POINT_POW;
}
static inline uint32_t read_cfg_v_s_tof_decline_start_point(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_VOLUME_SLIDE_CTRL & CFG_V_S_TOF_DECLINE_START_POINT_MASK) >> CFG_V_S_TOF_DECLINE_START_POINT_POW;
}
#define CFG_V_S_TOF_CLIMB_START_POINT_POW                                             11UL
#define CFG_V_S_TOF_CLIMB_START_POINT_MASK                                    0x0001f800UL
static inline void set_cfg_v_s_tof_climb_start_point(uint32_t cfg_v_s_tof_climb_start_point,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_VOLUME_SLIDE_CTRL &= ~CFG_V_S_TOF_CLIMB_START_POINT_MASK;
    src->CFG_VOLUME_SLIDE_CTRL |= cfg_v_s_tof_climb_start_point<<CFG_V_S_TOF_CLIMB_START_POINT_POW;
}
static inline uint32_t read_cfg_v_s_tof_climb_start_point(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_VOLUME_SLIDE_CTRL & CFG_V_S_TOF_CLIMB_START_POINT_MASK) >> CFG_V_S_TOF_CLIMB_START_POINT_POW;
}
#define CFG_V_S_TOF_INTERVAL_POW                                                       3UL
#define CFG_V_S_TOF_INTERVAL_MASK                                             0x000007f8UL
static inline void set_cfg_v_s_tof_interval(uint32_t cfg_v_s_tof_interval,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_VOLUME_SLIDE_CTRL &= ~CFG_V_S_TOF_INTERVAL_MASK;
    src->CFG_VOLUME_SLIDE_CTRL |= cfg_v_s_tof_interval<<CFG_V_S_TOF_INTERVAL_POW;
}
static inline uint32_t read_cfg_v_s_tof_interval(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_VOLUME_SLIDE_CTRL & CFG_V_S_TOF_INTERVAL_MASK) >> CFG_V_S_TOF_INTERVAL_POW;
}
#define CFG_V_S_TOF_ZERO_CROSSING_EN                                          0x00000004UL
static inline void set_cfg_v_s_tof_zero_crossing_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_VOLUME_SLIDE_CTRL |= CFG_V_S_TOF_ZERO_CROSSING_EN;
    } else
    {
        src->CFG_VOLUME_SLIDE_CTRL &= ~CFG_V_S_TOF_ZERO_CROSSING_EN;
    }
}
static inline bool read_cfg_v_s_tof_zero_crossing_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_VOLUME_SLIDE_CTRL & CFG_V_S_TOF_ZERO_CROSSING_EN) == CFG_V_S_TOF_ZERO_CROSSING_EN;
}
#define CFG_V_S_TOF_RCH_EN                                                    0x00000002UL
static inline void set_cfg_v_s_tof_rch_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_VOLUME_SLIDE_CTRL |= CFG_V_S_TOF_RCH_EN;
    } else
    {
        src->CFG_VOLUME_SLIDE_CTRL &= ~CFG_V_S_TOF_RCH_EN;
    }
}
static inline bool read_cfg_v_s_tof_rch_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_VOLUME_SLIDE_CTRL & CFG_V_S_TOF_RCH_EN) == CFG_V_S_TOF_RCH_EN;
}
#define CFG_V_S_TOF_LCH_EN                                                    0x00000001UL
static inline void set_cfg_v_s_tof_lch_en(volatile COBRA_SRC_TypeDef   *src,bool en)
{
    if(en)
    {
        src->CFG_VOLUME_SLIDE_CTRL |= CFG_V_S_TOF_LCH_EN;
    } else
    {
        src->CFG_VOLUME_SLIDE_CTRL &= ~CFG_V_S_TOF_LCH_EN;
    }
}
static inline bool read_cfg_v_s_tof_lch_en(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_VOLUME_SLIDE_CTRL & CFG_V_S_TOF_LCH_EN) == CFG_V_S_TOF_LCH_EN;
}

/*CFG_HP_IIR_NUM*/
#define CFG_HP_IIR_B1_POW                                                             16UL
#define CFG_HP_IIR_B1_MASK                                                    0xffff0000UL
static inline void set_cfg_hp_iir_b1(uint32_t cfg_hp_iir_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_HP_IIR_NUM &= ~CFG_HP_IIR_B1_MASK;
    src->CFG_HP_IIR_NUM |= cfg_hp_iir_b1<<CFG_HP_IIR_B1_POW;
}
static inline uint32_t read_cfg_hp_iir_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_HP_IIR_NUM & CFG_HP_IIR_B1_MASK) >> CFG_HP_IIR_B1_POW;
}
#define CFG_HP_IIR_B0_POW                                                              0UL
#define CFG_HP_IIR_B0_MASK                                                    0x0000ffffUL
static inline void set_cfg_hp_iir_b0(uint32_t cfg_hp_iir_b0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_HP_IIR_NUM &= ~CFG_HP_IIR_B0_MASK;
    src->CFG_HP_IIR_NUM |= cfg_hp_iir_b0<<CFG_HP_IIR_B0_POW;
}
static inline uint32_t read_cfg_hp_iir_b0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_HP_IIR_NUM & CFG_HP_IIR_B0_MASK) >> CFG_HP_IIR_B0_POW;
}

/*CFG_HP_IIR_DEN*/
#define CFG_HP_IIR_A1_POW                                                              0UL
#define CFG_HP_IIR_A1_MASK                                                    0x0000ffffUL
static inline void set_cfg_hp_iir_a1(uint32_t cfg_hp_iir_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_HP_IIR_DEN &= ~CFG_HP_IIR_A1_MASK;
    src->CFG_HP_IIR_DEN |= cfg_hp_iir_a1<<CFG_HP_IIR_A1_POW;
}
static inline uint32_t read_cfg_hp_iir_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_HP_IIR_DEN & CFG_HP_IIR_A1_MASK) >> CFG_HP_IIR_A1_POW;
}

/*CFG_CLK_SPEED*/
#define CLK_SFT_SPEED_POW                                                             24UL
#define CLK_SFT_SPEED_MASK                                                    0x07000000UL
static inline void set_clk_sft_speed(uint32_t clk_sft_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CLK_SFT_SPEED_MASK;
    src->CFG_CLK_SPEED |= clk_sft_speed<<CLK_SFT_SPEED_POW;
}
static inline uint32_t read_clk_sft_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CLK_SFT_SPEED_MASK) >> CLK_SFT_SPEED_POW;
}
#define CFG_IIR_GROUP1_CLK_SPEED_POW                                                  21UL
#define CFG_IIR_GROUP1_CLK_SPEED_MASK                                         0x00e00000UL
static inline void set_cfg_iir_group1_clk_speed(uint32_t cfg_iir_group1_clk_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CFG_IIR_GROUP1_CLK_SPEED_MASK;
    src->CFG_CLK_SPEED |= cfg_iir_group1_clk_speed<<CFG_IIR_GROUP1_CLK_SPEED_POW;
}
static inline uint32_t read_cfg_iir_group1_clk_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CFG_IIR_GROUP1_CLK_SPEED_MASK) >> CFG_IIR_GROUP1_CLK_SPEED_POW;
}
#define CFG_IIR_GROUP_CLK_SPEED_POW                                                   18UL
#define CFG_IIR_GROUP_CLK_SPEED_MASK                                          0x001c0000UL
static inline void set_cfg_iir_group_clk_speed(uint32_t cfg_iir_group_clk_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CFG_IIR_GROUP_CLK_SPEED_MASK;
    src->CFG_CLK_SPEED |= cfg_iir_group_clk_speed<<CFG_IIR_GROUP_CLK_SPEED_POW;
}
static inline uint32_t read_cfg_iir_group_clk_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CFG_IIR_GROUP_CLK_SPEED_MASK) >> CFG_IIR_GROUP_CLK_SPEED_POW;
}
#define CFG_TX_FS_CLK_SPEED_POW                                                       15UL
#define CFG_TX_FS_CLK_SPEED_MASK                                              0x00038000UL
static inline void set_cfg_tx_fs_clk_speed(uint32_t cfg_tx_fs_clk_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CFG_TX_FS_CLK_SPEED_MASK;
    src->CFG_CLK_SPEED |= cfg_tx_fs_clk_speed<<CFG_TX_FS_CLK_SPEED_POW;
}
static inline uint32_t read_cfg_tx_fs_clk_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CFG_TX_FS_CLK_SPEED_MASK) >> CFG_TX_FS_CLK_SPEED_POW;
}
#define CFG_HIGH_SPEED_FIR_CLK_SPEED_POW                                              12UL
#define CFG_HIGH_SPEED_FIR_CLK_SPEED_MASK                                     0x00007000UL
static inline void set_cfg_high_speed_fir_clk_speed(uint32_t cfg_high_speed_fir_clk_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CFG_HIGH_SPEED_FIR_CLK_SPEED_MASK;
    src->CFG_CLK_SPEED |= cfg_high_speed_fir_clk_speed<<CFG_HIGH_SPEED_FIR_CLK_SPEED_POW;
}
static inline uint32_t read_cfg_high_speed_fir_clk_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CFG_HIGH_SPEED_FIR_CLK_SPEED_MASK) >> CFG_HIGH_SPEED_FIR_CLK_SPEED_POW;
}
#define CFG_CIC_CLKOUT_SPEED_POW                                                       9UL
#define CFG_CIC_CLKOUT_SPEED_MASK                                             0x00000e00UL
static inline void set_cfg_cic_clkout_speed(uint32_t cfg_cic_clkout_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CFG_CIC_CLKOUT_SPEED_MASK;
    src->CFG_CLK_SPEED |= cfg_cic_clkout_speed<<CFG_CIC_CLKOUT_SPEED_POW;
}
static inline uint32_t read_cfg_cic_clkout_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CFG_CIC_CLKOUT_SPEED_MASK) >> CFG_CIC_CLKOUT_SPEED_POW;
}
#define CLK_HBF0_SPEED_POW                                                             6UL
#define CLK_HBF0_SPEED_MASK                                                   0x000001c0UL
static inline void set_clk_hbf0_speed(uint32_t clk_hbf0_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CLK_HBF0_SPEED_MASK;
    src->CFG_CLK_SPEED |= clk_hbf0_speed<<CLK_HBF0_SPEED_POW;
}
static inline uint32_t read_clk_hbf0_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CLK_HBF0_SPEED_MASK) >> CLK_HBF0_SPEED_POW;
}
#define CLK_HBF1_SPEED_POW                                                             3UL
#define CLK_HBF1_SPEED_MASK                                                   0x00000038UL
static inline void set_clk_hbf1_speed(uint32_t clk_hbf1_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CLK_HBF1_SPEED_MASK;
    src->CFG_CLK_SPEED |= clk_hbf1_speed<<CLK_HBF1_SPEED_POW;
}
static inline uint32_t read_clk_hbf1_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CLK_HBF1_SPEED_MASK) >> CLK_HBF1_SPEED_POW;
}
#define CLK_HBF2_SPEED_POW                                                             0UL
#define CLK_HBF2_SPEED_MASK                                                   0x00000007UL
static inline void set_clk_hbf2_speed(uint32_t clk_hbf2_speed,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_CLK_SPEED &= ~CLK_HBF2_SPEED_MASK;
    src->CFG_CLK_SPEED |= clk_hbf2_speed<<CLK_HBF2_SPEED_POW;
}
static inline uint32_t read_clk_hbf2_speed(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_CLK_SPEED & CLK_HBF2_SPEED_MASK) >> CLK_HBF2_SPEED_POW;
}

/*CFG_HIGH_SPEED_FIR*/
#define CFG_HIGH_SPEED_FIR_ORDER_POW                                                  16UL
#define CFG_HIGH_SPEED_FIR_ORDER_MASK                                         0x003f0000UL
static inline void set_cfg_high_speed_fir_order(uint32_t cfg_high_speed_fir_order,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_HIGH_SPEED_FIR &= ~CFG_HIGH_SPEED_FIR_ORDER_MASK;
    src->CFG_HIGH_SPEED_FIR |= cfg_high_speed_fir_order<<CFG_HIGH_SPEED_FIR_ORDER_POW;
}
static inline uint32_t read_cfg_high_speed_fir_order(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_HIGH_SPEED_FIR & CFG_HIGH_SPEED_FIR_ORDER_MASK) >> CFG_HIGH_SPEED_FIR_ORDER_POW;
}
#define CFG_FAST_CLK_RATIO_POW                                                         0UL
#define CFG_FAST_CLK_RATIO_MASK                                               0x0000ffffUL
static inline void set_cfg_fast_clk_ratio(uint32_t cfg_fast_clk_ratio,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_HIGH_SPEED_FIR &= ~CFG_FAST_CLK_RATIO_MASK;
    src->CFG_HIGH_SPEED_FIR |= cfg_fast_clk_ratio<<CFG_FAST_CLK_RATIO_POW;
}
static inline uint32_t read_cfg_fast_clk_ratio(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_HIGH_SPEED_FIR & CFG_FAST_CLK_RATIO_MASK) >> CFG_FAST_CLK_RATIO_POW;
}

/*CFG_IIR0_NUM*/
#define CFG_IIR0_B2_POW                                                               16UL
#define CFG_IIR0_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir0_b2(uint32_t cfg_iir0_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR0_NUM &= ~CFG_IIR0_B2_MASK;
    src->CFG_IIR0_NUM |= cfg_iir0_b2<<CFG_IIR0_B2_POW;
}
static inline uint32_t read_cfg_iir0_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR0_NUM & CFG_IIR0_B2_MASK) >> CFG_IIR0_B2_POW;
}
#define CFG_IIR0_B1_POW                                                                0UL
#define CFG_IIR0_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir0_b1(uint32_t cfg_iir0_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR0_NUM &= ~CFG_IIR0_B1_MASK;
    src->CFG_IIR0_NUM |= cfg_iir0_b1<<CFG_IIR0_B1_POW;
}
static inline uint32_t read_cfg_iir0_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR0_NUM & CFG_IIR0_B1_MASK) >> CFG_IIR0_B1_POW;
}

/*CFG_IIR0_DEN*/
#define CFG_IIR0_A2_POW                                                               16UL
#define CFG_IIR0_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir0_a2(uint32_t cfg_iir0_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR0_DEN &= ~CFG_IIR0_A2_MASK;
    src->CFG_IIR0_DEN |= cfg_iir0_a2<<CFG_IIR0_A2_POW;
}
static inline uint32_t read_cfg_iir0_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR0_DEN & CFG_IIR0_A2_MASK) >> CFG_IIR0_A2_POW;
}
#define CFG_IIR0_A1_POW                                                                0UL
#define CFG_IIR0_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir0_a1(uint32_t cfg_iir0_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR0_DEN &= ~CFG_IIR0_A1_MASK;
    src->CFG_IIR0_DEN |= cfg_iir0_a1<<CFG_IIR0_A1_POW;
}
static inline uint32_t read_cfg_iir0_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR0_DEN & CFG_IIR0_A1_MASK) >> CFG_IIR0_A1_POW;
}

/*CFG_IIR1_NUM*/
#define CFG_IIR1_B2_POW                                                               16UL
#define CFG_IIR1_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir1_b2(uint32_t cfg_iir1_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR1_NUM &= ~CFG_IIR1_B2_MASK;
    src->CFG_IIR1_NUM |= cfg_iir1_b2<<CFG_IIR1_B2_POW;
}
static inline uint32_t read_cfg_iir1_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR1_NUM & CFG_IIR1_B2_MASK) >> CFG_IIR1_B2_POW;
}
#define CFG_IIR1_B1_POW                                                                0UL
#define CFG_IIR1_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir1_b1(uint32_t cfg_iir1_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR1_NUM &= ~CFG_IIR1_B1_MASK;
    src->CFG_IIR1_NUM |= cfg_iir1_b1<<CFG_IIR1_B1_POW;
}
static inline uint32_t read_cfg_iir1_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR1_NUM & CFG_IIR1_B1_MASK) >> CFG_IIR1_B1_POW;
}

/*CFG_IIR1_DEN*/
#define CFG_IIR1_A2_POW                                                               16UL
#define CFG_IIR1_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir1_a2(uint32_t cfg_iir1_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR1_DEN &= ~CFG_IIR1_A2_MASK;
    src->CFG_IIR1_DEN |= cfg_iir1_a2<<CFG_IIR1_A2_POW;
}
static inline uint32_t read_cfg_iir1_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR1_DEN & CFG_IIR1_A2_MASK) >> CFG_IIR1_A2_POW;
}
#define CFG_IIR1_A1_POW                                                                0UL
#define CFG_IIR1_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir1_a1(uint32_t cfg_iir1_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR1_DEN &= ~CFG_IIR1_A1_MASK;
    src->CFG_IIR1_DEN |= cfg_iir1_a1<<CFG_IIR1_A1_POW;
}
static inline uint32_t read_cfg_iir1_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR1_DEN & CFG_IIR1_A1_MASK) >> CFG_IIR1_A1_POW;
}

/*CFG_IIR2_NUM*/
#define CFG_IIR2_B2_POW                                                               16UL
#define CFG_IIR2_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir2_b2(uint32_t cfg_iir2_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR2_NUM &= ~CFG_IIR2_B2_MASK;
    src->CFG_IIR2_NUM |= cfg_iir2_b2<<CFG_IIR2_B2_POW;
}
static inline uint32_t read_cfg_iir2_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR2_NUM & CFG_IIR2_B2_MASK) >> CFG_IIR2_B2_POW;
}
#define CFG_IIR2_B1_POW                                                                0UL
#define CFG_IIR2_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir2_b1(uint32_t cfg_iir2_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR2_NUM &= ~CFG_IIR2_B1_MASK;
    src->CFG_IIR2_NUM |= cfg_iir2_b1<<CFG_IIR2_B1_POW;
}
static inline uint32_t read_cfg_iir2_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR2_NUM & CFG_IIR2_B1_MASK) >> CFG_IIR2_B1_POW;
}

/*CFG_IIR2_DEN*/
#define CFG_IIR2_A2_POW                                                               16UL
#define CFG_IIR2_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir2_a2(uint32_t cfg_iir2_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR2_DEN &= ~CFG_IIR2_A2_MASK;
    src->CFG_IIR2_DEN |= cfg_iir2_a2<<CFG_IIR2_A2_POW;
}
static inline uint32_t read_cfg_iir2_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR2_DEN & CFG_IIR2_A2_MASK) >> CFG_IIR2_A2_POW;
}
#define CFG_IIR2_A1_POW                                                                0UL
#define CFG_IIR2_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir2_a1(uint32_t cfg_iir2_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR2_DEN &= ~CFG_IIR2_A1_MASK;
    src->CFG_IIR2_DEN |= cfg_iir2_a1<<CFG_IIR2_A1_POW;
}
static inline uint32_t read_cfg_iir2_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR2_DEN & CFG_IIR2_A1_MASK) >> CFG_IIR2_A1_POW;
}

/*CFG_IIR3_NUM*/
#define CFG_IIR3_B2_POW                                                               16UL
#define CFG_IIR3_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir3_b2(uint32_t cfg_iir3_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR3_NUM &= ~CFG_IIR3_B2_MASK;
    src->CFG_IIR3_NUM |= cfg_iir3_b2<<CFG_IIR3_B2_POW;
}
static inline uint32_t read_cfg_iir3_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR3_NUM & CFG_IIR3_B2_MASK) >> CFG_IIR3_B2_POW;
}
#define CFG_IIR3_B1_POW                                                                0UL
#define CFG_IIR3_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir3_b1(uint32_t cfg_iir3_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR3_NUM &= ~CFG_IIR3_B1_MASK;
    src->CFG_IIR3_NUM |= cfg_iir3_b1<<CFG_IIR3_B1_POW;
}
static inline uint32_t read_cfg_iir3_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR3_NUM & CFG_IIR3_B1_MASK) >> CFG_IIR3_B1_POW;
}

/*CFG_IIR3_DEN*/
#define CFG_IIR3_A2_POW                                                               16UL
#define CFG_IIR3_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir3_a2(uint32_t cfg_iir3_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR3_DEN &= ~CFG_IIR3_A2_MASK;
    src->CFG_IIR3_DEN |= cfg_iir3_a2<<CFG_IIR3_A2_POW;
}
static inline uint32_t read_cfg_iir3_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR3_DEN & CFG_IIR3_A2_MASK) >> CFG_IIR3_A2_POW;
}
#define CFG_IIR3_A1_POW                                                                0UL
#define CFG_IIR3_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir3_a1(uint32_t cfg_iir3_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR3_DEN &= ~CFG_IIR3_A1_MASK;
    src->CFG_IIR3_DEN |= cfg_iir3_a1<<CFG_IIR3_A1_POW;
}
static inline uint32_t read_cfg_iir3_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR3_DEN & CFG_IIR3_A1_MASK) >> CFG_IIR3_A1_POW;
}

/*CFG_IIR4_NUM*/
#define CFG_IIR4_B2_POW                                                               16UL
#define CFG_IIR4_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir4_b2(uint32_t cfg_iir4_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR4_NUM &= ~CFG_IIR4_B2_MASK;
    src->CFG_IIR4_NUM |= cfg_iir4_b2<<CFG_IIR4_B2_POW;
}
static inline uint32_t read_cfg_iir4_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR4_NUM & CFG_IIR4_B2_MASK) >> CFG_IIR4_B2_POW;
}
#define CFG_IIR4_B1_POW                                                                0UL
#define CFG_IIR4_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir4_b1(uint32_t cfg_iir4_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR4_NUM &= ~CFG_IIR4_B1_MASK;
    src->CFG_IIR4_NUM |= cfg_iir4_b1<<CFG_IIR4_B1_POW;
}
static inline uint32_t read_cfg_iir4_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR4_NUM & CFG_IIR4_B1_MASK) >> CFG_IIR4_B1_POW;
}

/*CFG_IIR4_DEN*/
#define CFG_IIR4_A2_POW                                                               16UL
#define CFG_IIR4_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir4_a2(uint32_t cfg_iir4_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR4_DEN &= ~CFG_IIR4_A2_MASK;
    src->CFG_IIR4_DEN |= cfg_iir4_a2<<CFG_IIR4_A2_POW;
}
static inline uint32_t read_cfg_iir4_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR4_DEN & CFG_IIR4_A2_MASK) >> CFG_IIR4_A2_POW;
}
#define CFG_IIR4_A1_POW                                                                0UL
#define CFG_IIR4_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir4_a1(uint32_t cfg_iir4_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR4_DEN &= ~CFG_IIR4_A1_MASK;
    src->CFG_IIR4_DEN |= cfg_iir4_a1<<CFG_IIR4_A1_POW;
}
static inline uint32_t read_cfg_iir4_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR4_DEN & CFG_IIR4_A1_MASK) >> CFG_IIR4_A1_POW;
}

/*CFG_IIR5_NUM*/
#define CFG_IIR5_B2_POW                                                               16UL
#define CFG_IIR5_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir5_b2(uint32_t cfg_iir5_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR5_NUM &= ~CFG_IIR5_B2_MASK;
    src->CFG_IIR5_NUM |= cfg_iir5_b2<<CFG_IIR5_B2_POW;
}
static inline uint32_t read_cfg_iir5_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR5_NUM & CFG_IIR5_B2_MASK) >> CFG_IIR5_B2_POW;
}
#define CFG_IIR5_B1_POW                                                                0UL
#define CFG_IIR5_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir5_b1(uint32_t cfg_iir5_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR5_NUM &= ~CFG_IIR5_B1_MASK;
    src->CFG_IIR5_NUM |= cfg_iir5_b1<<CFG_IIR5_B1_POW;
}
static inline uint32_t read_cfg_iir5_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR5_NUM & CFG_IIR5_B1_MASK) >> CFG_IIR5_B1_POW;
}

/*CFG_IIR5_DEN*/
#define CFG_IIR5_A2_POW                                                               16UL
#define CFG_IIR5_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir5_a2(uint32_t cfg_iir5_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR5_DEN &= ~CFG_IIR5_A2_MASK;
    src->CFG_IIR5_DEN |= cfg_iir5_a2<<CFG_IIR5_A2_POW;
}
static inline uint32_t read_cfg_iir5_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR5_DEN & CFG_IIR5_A2_MASK) >> CFG_IIR5_A2_POW;
}
#define CFG_IIR5_A1_POW                                                                0UL
#define CFG_IIR5_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir5_a1(uint32_t cfg_iir5_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR5_DEN &= ~CFG_IIR5_A1_MASK;
    src->CFG_IIR5_DEN |= cfg_iir5_a1<<CFG_IIR5_A1_POW;
}
static inline uint32_t read_cfg_iir5_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR5_DEN & CFG_IIR5_A1_MASK) >> CFG_IIR5_A1_POW;
}

/*CFG_IIR6_NUM*/
#define CFG_IIR6_B2_POW                                                               16UL
#define CFG_IIR6_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir6_b2(uint32_t cfg_iir6_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR6_NUM &= ~CFG_IIR6_B2_MASK;
    src->CFG_IIR6_NUM |= cfg_iir6_b2<<CFG_IIR6_B2_POW;
}
static inline uint32_t read_cfg_iir6_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR6_NUM & CFG_IIR6_B2_MASK) >> CFG_IIR6_B2_POW;
}
#define CFG_IIR6_B1_POW                                                                0UL
#define CFG_IIR6_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir6_b1(uint32_t cfg_iir6_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR6_NUM &= ~CFG_IIR6_B1_MASK;
    src->CFG_IIR6_NUM |= cfg_iir6_b1<<CFG_IIR6_B1_POW;
}
static inline uint32_t read_cfg_iir6_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR6_NUM & CFG_IIR6_B1_MASK) >> CFG_IIR6_B1_POW;
}

/*CFG_IIR6_DEN*/
#define CFG_IIR6_A2_POW                                                               16UL
#define CFG_IIR6_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir6_a2(uint32_t cfg_iir6_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR6_DEN &= ~CFG_IIR6_A2_MASK;
    src->CFG_IIR6_DEN |= cfg_iir6_a2<<CFG_IIR6_A2_POW;
}
static inline uint32_t read_cfg_iir6_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR6_DEN & CFG_IIR6_A2_MASK) >> CFG_IIR6_A2_POW;
}
#define CFG_IIR6_A1_POW                                                                0UL
#define CFG_IIR6_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir6_a1(uint32_t cfg_iir6_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR6_DEN &= ~CFG_IIR6_A1_MASK;
    src->CFG_IIR6_DEN |= cfg_iir6_a1<<CFG_IIR6_A1_POW;
}
static inline uint32_t read_cfg_iir6_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR6_DEN & CFG_IIR6_A1_MASK) >> CFG_IIR6_A1_POW;
}

/*CFG_IIR7_NUM*/
#define CFG_IIR7_B2_POW                                                               16UL
#define CFG_IIR7_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir7_b2(uint32_t cfg_iir7_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR7_NUM &= ~CFG_IIR7_B2_MASK;
    src->CFG_IIR7_NUM |= cfg_iir7_b2<<CFG_IIR7_B2_POW;
}
static inline uint32_t read_cfg_iir7_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR7_NUM & CFG_IIR7_B2_MASK) >> CFG_IIR7_B2_POW;
}
#define CFG_IIR7_B1_POW                                                                0UL
#define CFG_IIR7_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir7_b1(uint32_t cfg_iir7_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR7_NUM &= ~CFG_IIR7_B1_MASK;
    src->CFG_IIR7_NUM |= cfg_iir7_b1<<CFG_IIR7_B1_POW;
}
static inline uint32_t read_cfg_iir7_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR7_NUM & CFG_IIR7_B1_MASK) >> CFG_IIR7_B1_POW;
}

/*CFG_IIR7_DEN*/
#define CFG_IIR7_A2_POW                                                               16UL
#define CFG_IIR7_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir7_a2(uint32_t cfg_iir7_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR7_DEN &= ~CFG_IIR7_A2_MASK;
    src->CFG_IIR7_DEN |= cfg_iir7_a2<<CFG_IIR7_A2_POW;
}
static inline uint32_t read_cfg_iir7_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR7_DEN & CFG_IIR7_A2_MASK) >> CFG_IIR7_A2_POW;
}
#define CFG_IIR7_A1_POW                                                                0UL
#define CFG_IIR7_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir7_a1(uint32_t cfg_iir7_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR7_DEN &= ~CFG_IIR7_A1_MASK;
    src->CFG_IIR7_DEN |= cfg_iir7_a1<<CFG_IIR7_A1_POW;
}
static inline uint32_t read_cfg_iir7_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR7_DEN & CFG_IIR7_A1_MASK) >> CFG_IIR7_A1_POW;
}

/*CFG_IIR8_NUM*/
#define CFG_IIR8_B2_POW                                                               16UL
#define CFG_IIR8_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir8_b2(uint32_t cfg_iir8_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR8_NUM &= ~CFG_IIR8_B2_MASK;
    src->CFG_IIR8_NUM |= cfg_iir8_b2<<CFG_IIR8_B2_POW;
}
static inline uint32_t read_cfg_iir8_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR8_NUM & CFG_IIR8_B2_MASK) >> CFG_IIR8_B2_POW;
}
#define CFG_IIR8_B1_POW                                                                0UL
#define CFG_IIR8_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir8_b1(uint32_t cfg_iir8_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR8_NUM &= ~CFG_IIR8_B1_MASK;
    src->CFG_IIR8_NUM |= cfg_iir8_b1<<CFG_IIR8_B1_POW;
}
static inline uint32_t read_cfg_iir8_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR8_NUM & CFG_IIR8_B1_MASK) >> CFG_IIR8_B1_POW;
}

/*CFG_IIR8_DEN*/
#define CFG_IIR8_A2_POW                                                               16UL
#define CFG_IIR8_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir8_a2(uint32_t cfg_iir8_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR8_DEN &= ~CFG_IIR8_A2_MASK;
    src->CFG_IIR8_DEN |= cfg_iir8_a2<<CFG_IIR8_A2_POW;
}
static inline uint32_t read_cfg_iir8_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR8_DEN & CFG_IIR8_A2_MASK) >> CFG_IIR8_A2_POW;
}
#define CFG_IIR8_A1_POW                                                                0UL
#define CFG_IIR8_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir8_a1(uint32_t cfg_iir8_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR8_DEN &= ~CFG_IIR8_A1_MASK;
    src->CFG_IIR8_DEN |= cfg_iir8_a1<<CFG_IIR8_A1_POW;
}
static inline uint32_t read_cfg_iir8_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR8_DEN & CFG_IIR8_A1_MASK) >> CFG_IIR8_A1_POW;
}

/*CFG_IIR9_NUM*/
#define CFG_IIR9_B2_POW                                                               16UL
#define CFG_IIR9_B2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir9_b2(uint32_t cfg_iir9_b2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR9_NUM &= ~CFG_IIR9_B2_MASK;
    src->CFG_IIR9_NUM |= cfg_iir9_b2<<CFG_IIR9_B2_POW;
}
static inline uint32_t read_cfg_iir9_b2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR9_NUM & CFG_IIR9_B2_MASK) >> CFG_IIR9_B2_POW;
}
#define CFG_IIR9_B1_POW                                                                0UL
#define CFG_IIR9_B1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir9_b1(uint32_t cfg_iir9_b1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR9_NUM &= ~CFG_IIR9_B1_MASK;
    src->CFG_IIR9_NUM |= cfg_iir9_b1<<CFG_IIR9_B1_POW;
}
static inline uint32_t read_cfg_iir9_b1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR9_NUM & CFG_IIR9_B1_MASK) >> CFG_IIR9_B1_POW;
}

/*CFG_IIR9_DEN*/
#define CFG_IIR9_A2_POW                                                               16UL
#define CFG_IIR9_A2_MASK                                                      0xffff0000UL
static inline void set_cfg_iir9_a2(uint32_t cfg_iir9_a2,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR9_DEN &= ~CFG_IIR9_A2_MASK;
    src->CFG_IIR9_DEN |= cfg_iir9_a2<<CFG_IIR9_A2_POW;
}
static inline uint32_t read_cfg_iir9_a2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR9_DEN & CFG_IIR9_A2_MASK) >> CFG_IIR9_A2_POW;
}
#define CFG_IIR9_A1_POW                                                                0UL
#define CFG_IIR9_A1_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir9_a1(uint32_t cfg_iir9_a1,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR9_DEN &= ~CFG_IIR9_A1_MASK;
    src->CFG_IIR9_DEN |= cfg_iir9_a1<<CFG_IIR9_A1_POW;
}
static inline uint32_t read_cfg_iir9_a1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR9_DEN & CFG_IIR9_A1_MASK) >> CFG_IIR9_A1_POW;
}

/*CFG_IIR0_G0*/
#define CFG_IIR0_G0_POW                                                                0UL
#define CFG_IIR0_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir0_g0(uint32_t cfg_iir0_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR0_G0 &= ~CFG_IIR0_G0_MASK;
    src->CFG_IIR0_G0 |= cfg_iir0_g0<<CFG_IIR0_G0_POW;
}
static inline uint32_t read_cfg_iir0_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR0_G0 & CFG_IIR0_G0_MASK) >> CFG_IIR0_G0_POW;
}

/*CFG_IIR1_G0*/
#define CFG_IIR1_G0_POW                                                                0UL
#define CFG_IIR1_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir1_g0(uint32_t cfg_iir1_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR1_G0 &= ~CFG_IIR1_G0_MASK;
    src->CFG_IIR1_G0 |= cfg_iir1_g0<<CFG_IIR1_G0_POW;
}
static inline uint32_t read_cfg_iir1_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR1_G0 & CFG_IIR1_G0_MASK) >> CFG_IIR1_G0_POW;
}

/*CFG_IIR2_G0*/
#define CFG_IIR2_G0_POW                                                                0UL
#define CFG_IIR2_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir2_g0(uint32_t cfg_iir2_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR2_G0 &= ~CFG_IIR2_G0_MASK;
    src->CFG_IIR2_G0 |= cfg_iir2_g0<<CFG_IIR2_G0_POW;
}
static inline uint32_t read_cfg_iir2_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR2_G0 & CFG_IIR2_G0_MASK) >> CFG_IIR2_G0_POW;
}

/*CFG_IIR3_G0*/
#define CFG_IIR3_G0_POW                                                                0UL
#define CFG_IIR3_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir3_g0(uint32_t cfg_iir3_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR3_G0 &= ~CFG_IIR3_G0_MASK;
    src->CFG_IIR3_G0 |= cfg_iir3_g0<<CFG_IIR3_G0_POW;
}
static inline uint32_t read_cfg_iir3_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR3_G0 & CFG_IIR3_G0_MASK) >> CFG_IIR3_G0_POW;
}

/*CFG_IIR4_G0*/
#define CFG_IIR4_G0_POW                                                                0UL
#define CFG_IIR4_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir4_g0(uint32_t cfg_iir4_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR4_G0 &= ~CFG_IIR4_G0_MASK;
    src->CFG_IIR4_G0 |= cfg_iir4_g0<<CFG_IIR4_G0_POW;
}
static inline uint32_t read_cfg_iir4_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR4_G0 & CFG_IIR4_G0_MASK) >> CFG_IIR4_G0_POW;
}

/*CFG_IIR5_G0*/
#define CFG_IIR5_G0_POW                                                                0UL
#define CFG_IIR5_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir5_g0(uint32_t cfg_iir5_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR5_G0 &= ~CFG_IIR5_G0_MASK;
    src->CFG_IIR5_G0 |= cfg_iir5_g0<<CFG_IIR5_G0_POW;
}
static inline uint32_t read_cfg_iir5_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR5_G0 & CFG_IIR5_G0_MASK) >> CFG_IIR5_G0_POW;
}

/*CFG_IIR6_G0*/
#define CFG_IIR6_G0_POW                                                                0UL
#define CFG_IIR6_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir6_g0(uint32_t cfg_iir6_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR6_G0 &= ~CFG_IIR6_G0_MASK;
    src->CFG_IIR6_G0 |= cfg_iir6_g0<<CFG_IIR6_G0_POW;
}
static inline uint32_t read_cfg_iir6_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR6_G0 & CFG_IIR6_G0_MASK) >> CFG_IIR6_G0_POW;
}

/*CFG_IIR7_G0*/
#define CFG_IIR7_G0_POW                                                                0UL
#define CFG_IIR7_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir7_g0(uint32_t cfg_iir7_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR7_G0 &= ~CFG_IIR7_G0_MASK;
    src->CFG_IIR7_G0 |= cfg_iir7_g0<<CFG_IIR7_G0_POW;
}
static inline uint32_t read_cfg_iir7_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR7_G0 & CFG_IIR7_G0_MASK) >> CFG_IIR7_G0_POW;
}

/*CFG_IIR8_G0*/
#define CFG_IIR8_G0_POW                                                                0UL
#define CFG_IIR8_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir8_g0(uint32_t cfg_iir8_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR8_G0 &= ~CFG_IIR8_G0_MASK;
    src->CFG_IIR8_G0 |= cfg_iir8_g0<<CFG_IIR8_G0_POW;
}
static inline uint32_t read_cfg_iir8_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR8_G0 & CFG_IIR8_G0_MASK) >> CFG_IIR8_G0_POW;
}

/*CFG_IIR9_G0*/
#define CFG_IIR9_G0_POW                                                                0UL
#define CFG_IIR9_G0_MASK                                                      0x0000ffffUL
static inline void set_cfg_iir9_g0(uint32_t cfg_iir9_g0,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_IIR9_G0 &= ~CFG_IIR9_G0_MASK;
    src->CFG_IIR9_G0 |= cfg_iir9_g0<<CFG_IIR9_G0_POW;
}
static inline uint32_t read_cfg_iir9_g0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_IIR9_G0 & CFG_IIR9_G0_MASK) >> CFG_IIR9_G0_POW;
}

/*CFG_MANUAL_GAIN_COARSE*/
#define CFG_MANUAL_GAIN_COARSE_SHIFT_R_POW                                             7UL
#define CFG_MANUAL_GAIN_COARSE_SHIFT_R_MASK                                   0x00003f80UL
static inline void set_cfg_manual_gain_coarse_shift_r(uint32_t cfg_manual_gain_coarse_shift_r,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MANUAL_GAIN_COARSE &= ~CFG_MANUAL_GAIN_COARSE_SHIFT_R_MASK;
    src->CFG_MANUAL_GAIN_COARSE |= cfg_manual_gain_coarse_shift_r<<CFG_MANUAL_GAIN_COARSE_SHIFT_R_POW;
}
static inline uint32_t read_cfg_manual_gain_coarse_shift_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MANUAL_GAIN_COARSE & CFG_MANUAL_GAIN_COARSE_SHIFT_R_MASK) >> CFG_MANUAL_GAIN_COARSE_SHIFT_R_POW;
}
#define CFG_MANUAL_GAIN_COARSE_SHIFT_L_POW                                             0UL
#define CFG_MANUAL_GAIN_COARSE_SHIFT_L_MASK                                   0x0000007fUL
static inline void set_cfg_manual_gain_coarse_shift_l(uint32_t cfg_manual_gain_coarse_shift_l,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MANUAL_GAIN_COARSE &= ~CFG_MANUAL_GAIN_COARSE_SHIFT_L_MASK;
    src->CFG_MANUAL_GAIN_COARSE |= cfg_manual_gain_coarse_shift_l<<CFG_MANUAL_GAIN_COARSE_SHIFT_L_POW;
}
static inline uint32_t read_cfg_manual_gain_coarse_shift_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MANUAL_GAIN_COARSE & CFG_MANUAL_GAIN_COARSE_SHIFT_L_MASK) >> CFG_MANUAL_GAIN_COARSE_SHIFT_L_POW;
}

/*CFG_MANUAL_GAIN_FINE*/
#define CFG_MANUAL_GAIN_FINE_MUL_R_POW                                                12UL
#define CFG_MANUAL_GAIN_FINE_MUL_R_MASK                                       0x00fff000UL
static inline void set_cfg_manual_gain_fine_mul_r(uint32_t cfg_manual_gain_fine_mul_r,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MANUAL_GAIN_FINE &= ~CFG_MANUAL_GAIN_FINE_MUL_R_MASK;
    src->CFG_MANUAL_GAIN_FINE |= cfg_manual_gain_fine_mul_r<<CFG_MANUAL_GAIN_FINE_MUL_R_POW;
}
static inline uint32_t read_cfg_manual_gain_fine_mul_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MANUAL_GAIN_FINE & CFG_MANUAL_GAIN_FINE_MUL_R_MASK) >> CFG_MANUAL_GAIN_FINE_MUL_R_POW;
}
#define CFG_MANUAL_GAIN_FINE_MUL_L_POW                                                 0UL
#define CFG_MANUAL_GAIN_FINE_MUL_L_MASK                                       0x00000fffUL
static inline void set_cfg_manual_gain_fine_mul_l(uint32_t cfg_manual_gain_fine_mul_l,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MANUAL_GAIN_FINE &= ~CFG_MANUAL_GAIN_FINE_MUL_L_MASK;
    src->CFG_MANUAL_GAIN_FINE |= cfg_manual_gain_fine_mul_l<<CFG_MANUAL_GAIN_FINE_MUL_L_POW;
}
static inline uint32_t read_cfg_manual_gain_fine_mul_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MANUAL_GAIN_FINE & CFG_MANUAL_GAIN_FINE_MUL_L_MASK) >> CFG_MANUAL_GAIN_FINE_MUL_L_POW;
}

/*CFG_MANUAL_GAIN_MODE*/
#define CFG_MANUAL_GAIN_MODE_POW                                                       0UL
#define CFG_MANUAL_GAIN_MODE_MASK                                             0x00000003UL
static inline void set_cfg_manual_gain_mode(uint32_t cfg_manual_gain_mode,volatile COBRA_SRC_TypeDef   *src)
{
    src->CFG_MANUAL_GAIN_MODE &= ~CFG_MANUAL_GAIN_MODE_MASK;
    src->CFG_MANUAL_GAIN_MODE |= cfg_manual_gain_mode<<CFG_MANUAL_GAIN_MODE_POW;
}
static inline uint32_t read_cfg_manual_gain_mode(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->CFG_MANUAL_GAIN_MODE & CFG_MANUAL_GAIN_MODE_MASK) >> CFG_MANUAL_GAIN_MODE_POW;
}

/*UPFIFO_RDAT0*/
#define UPFIFO_RDAT0_POW                                                               0UL
#define UPFIFO_RDAT0_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat0(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT0 & UPFIFO_RDAT0_MASK) >> UPFIFO_RDAT0_POW;
}

/*UPFIFO_RDAT1*/
#define UPFIFO_RDAT1_POW                                                               0UL
#define UPFIFO_RDAT1_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat1(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT1 & UPFIFO_RDAT1_MASK) >> UPFIFO_RDAT1_POW;
}

/*UPFIFO_RDAT2*/
#define UPFIFO_RDAT2_POW                                                               0UL
#define UPFIFO_RDAT2_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat2(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT2 & UPFIFO_RDAT2_MASK) >> UPFIFO_RDAT2_POW;
}

/*UPFIFO_RDAT3*/
#define UPFIFO_RDAT3_POW                                                               0UL
#define UPFIFO_RDAT3_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat3(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT3 & UPFIFO_RDAT3_MASK) >> UPFIFO_RDAT3_POW;
}

/*UPFIFO_RDAT4*/
#define UPFIFO_RDAT4_POW                                                               0UL
#define UPFIFO_RDAT4_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat4(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT4 & UPFIFO_RDAT4_MASK) >> UPFIFO_RDAT4_POW;
}

/*UPFIFO_RDAT5*/
#define UPFIFO_RDAT5_POW                                                               0UL
#define UPFIFO_RDAT5_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat5(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT5 & UPFIFO_RDAT5_MASK) >> UPFIFO_RDAT5_POW;
}

/*UPFIFO_RDAT6*/
#define UPFIFO_RDAT6_POW                                                               0UL
#define UPFIFO_RDAT6_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat6(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT6 & UPFIFO_RDAT6_MASK) >> UPFIFO_RDAT6_POW;
}

/*UPFIFO_RDAT7*/
#define UPFIFO_RDAT7_POW                                                               0UL
#define UPFIFO_RDAT7_MASK                                                     0xffffffffUL
static inline uint32_t read_upfifo_rdat7(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_RDAT7 & UPFIFO_RDAT7_MASK) >> UPFIFO_RDAT7_POW;
}

/*INT_STATUS*/
#define INTERRUPT                                                             0x00000080UL
static inline bool read_interrupt(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & INTERRUPT) == INTERRUPT;
}
#define STICKY_INT                                                            0x00000040UL
static inline bool read_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & STICKY_INT) == STICKY_INT;
}
#define UPFIFO_ALMOSTFULL_STICKY_INT                                          0x00000020UL
static inline bool read_upfifo_almostfull_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & UPFIFO_ALMOSTFULL_STICKY_INT) == UPFIFO_ALMOSTFULL_STICKY_INT;
}
#define UPFIFO_ALMOSTEMPTY_STICKY_INT                                         0x00000010UL
static inline bool read_upfifo_almostempty_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & UPFIFO_ALMOSTEMPTY_STICKY_INT) == UPFIFO_ALMOSTEMPTY_STICKY_INT;
}
#define UPFIFO_EMPTY_STICKY_INT                                               0x00000008UL
static inline bool read_upfifo_empty_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & UPFIFO_EMPTY_STICKY_INT) == UPFIFO_EMPTY_STICKY_INT;
}
#define UPFIFO_OVERFLOW_STICKY_INT                                            0x00000004UL
static inline bool read_upfifo_overflow_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & UPFIFO_OVERFLOW_STICKY_INT) == UPFIFO_OVERFLOW_STICKY_INT;
}
#define UPFIFO_UNDERFLOW_STICKY_INT                                           0x00000002UL
static inline bool read_upfifo_underflow_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & UPFIFO_UNDERFLOW_STICKY_INT) == UPFIFO_UNDERFLOW_STICKY_INT;
}
#define UPFIFO_FULL_STICKY_INT                                                0x00000001UL
static inline bool read_upfifo_full_sticky_int(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->INT_STATUS & UPFIFO_FULL_STICKY_INT) == UPFIFO_FULL_STICKY_INT;
}

/*VOLUME_SLIDE_STATUS*/
#define V_S_TF_DECLINE_DONE_R                                                 0x00000008UL
static inline bool read_v_s_tf_decline_done_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->VOLUME_SLIDE_STATUS & V_S_TF_DECLINE_DONE_R) == V_S_TF_DECLINE_DONE_R;
}
#define V_S_TF_CLIME_DONE_R                                                   0x00000004UL
static inline bool read_v_s_tf_clime_done_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->VOLUME_SLIDE_STATUS & V_S_TF_CLIME_DONE_R) == V_S_TF_CLIME_DONE_R;
}
#define V_S_TF_DECLINE_DONE_L                                                 0x00000002UL
static inline bool read_v_s_tf_decline_done_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->VOLUME_SLIDE_STATUS & V_S_TF_DECLINE_DONE_L) == V_S_TF_DECLINE_DONE_L;
}
#define V_S_TF_CLIME_DONE_L                                                   0x00000001UL
static inline bool read_v_s_tf_clime_done_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->VOLUME_SLIDE_STATUS & V_S_TF_CLIME_DONE_L) == V_S_TF_CLIME_DONE_L;
}

/*UPFIFO_STATUS*/
#define UPFIFO_FILLED_NUM_POW                                                          8UL
#define UPFIFO_FILLED_NUM_MASK                                                0x00ffff00UL
static inline uint32_t read_upfifo_filled_num(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_FILLED_NUM_MASK) >> UPFIFO_FILLED_NUM_POW;
}
#define UPFIFO_ALMOSTEMPTY                                                    0x00000020UL
static inline bool read_upfifo_almostempty(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_ALMOSTEMPTY) == UPFIFO_ALMOSTEMPTY;
}
#define UPFIFO_EMPTY                                                          0x00000010UL
static inline bool read_upfifo_empty(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_EMPTY) == UPFIFO_EMPTY;
}
#define UPFIFO_ALMOSTFULL                                                     0x00000008UL
static inline bool read_upfifo_almostfull(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_ALMOSTFULL) == UPFIFO_ALMOSTFULL;
}
#define UPFIFO_FULL                                                           0x00000004UL
static inline bool read_upfifo_full(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_FULL) == UPFIFO_FULL;
}
#define UPFIFO_UNDERFLOW                                                      0x00000002UL
static inline bool read_upfifo_underflow(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_UNDERFLOW) == UPFIFO_UNDERFLOW;
}
#define UPFIFO_OVERFLOW                                                       0x00000001UL
static inline bool read_upfifo_overflow(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->UPFIFO_STATUS & UPFIFO_OVERFLOW) == UPFIFO_OVERFLOW;
}

/*AUDIOADC_LEFT_CHANNEL_FIFO_STATUS*/
#define AUDIOADC_FIFO_RFILLED_NUM_L_POW                                               12UL
#define AUDIOADC_FIFO_RFILLED_NUM_L_MASK                                      0x0000f000UL
static inline uint32_t read_audioadc_fifo_rfilled_num_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_RFILLED_NUM_L_MASK) >> AUDIOADC_FIFO_RFILLED_NUM_L_POW;
}
#define AUDIOADC_FIFO_WFILLED_NUM_L_POW                                                8UL
#define AUDIOADC_FIFO_WFILLED_NUM_L_MASK                                      0x00000f00UL
static inline uint32_t read_audioadc_fifo_wfilled_num_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_WFILLED_NUM_L_MASK) >> AUDIOADC_FIFO_WFILLED_NUM_L_POW;
}
#define AUDIOADC_FIFO_ALMOSTEMPTY_L                                           0x00000020UL
static inline bool read_audioadc_fifo_almostempty_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_ALMOSTEMPTY_L) == AUDIOADC_FIFO_ALMOSTEMPTY_L;
}
#define AUDIOADC_FIFO_EMPTY_L                                                 0x00000010UL
static inline bool read_audioadc_fifo_empty_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_EMPTY_L) == AUDIOADC_FIFO_EMPTY_L;
}
#define AUDIOADC_FIFO_ALMOSTFULL_L                                            0x00000008UL
static inline bool read_audioadc_fifo_almostfull_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_ALMOSTFULL_L) == AUDIOADC_FIFO_ALMOSTFULL_L;
}
#define AUDIOADC_FIFO_FULL_L                                                  0x00000004UL
static inline bool read_audioadc_fifo_full_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_FULL_L) == AUDIOADC_FIFO_FULL_L;
}
#define AUDIOADC_FIFO_UNDERFLOW_L                                             0x00000002UL
static inline bool read_audioadc_fifo_underflow_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_UNDERFLOW_L) == AUDIOADC_FIFO_UNDERFLOW_L;
}
#define AUDIOADC_FIFO_OVERFLOW_L                                              0x00000001UL
static inline bool read_audioadc_fifo_overflow_l(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_LEFT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_OVERFLOW_L) == AUDIOADC_FIFO_OVERFLOW_L;
}

/*AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS*/
#define AUDIOADC_FIFO_RFILLED_NUM_R_POW                                               12UL
#define AUDIOADC_FIFO_RFILLED_NUM_R_MASK                                      0x0000f000UL
static inline uint32_t read_audioadc_fifo_rfilled_num_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_RFILLED_NUM_R_MASK) >> AUDIOADC_FIFO_RFILLED_NUM_R_POW;
}
#define AUDIOADC_FIFO_WFILLED_NUM_R_POW                                                8UL
#define AUDIOADC_FIFO_WFILLED_NUM_R_MASK                                      0x00000f00UL
static inline uint32_t read_audioadc_fifo_wfilled_num_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_WFILLED_NUM_R_MASK) >> AUDIOADC_FIFO_WFILLED_NUM_R_POW;
}
#define AUDIOADC_FIFO_ALMOSTEMPTY_R                                           0x00000020UL
static inline bool read_audioadc_fifo_almostempty_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_ALMOSTEMPTY_L) == AUDIOADC_FIFO_ALMOSTEMPTY_L;
}
#define AUDIOADC_FIFO_EMPTY_R                                                 0x00000010UL
static inline bool read_audioadc_fifo_empty_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_EMPTY_L) == AUDIOADC_FIFO_EMPTY_L;
}
#define AUDIOADC_FIFO_ALMOSTFULL_R                                            0x00000008UL
static inline bool read_audioadc_fifo_almostfull_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_ALMOSTFULL_L) == AUDIOADC_FIFO_ALMOSTFULL_L;
}
#define AUDIOADC_FIFO_FULL_R                                                  0x00000004UL
static inline bool read_audioadc_fifo_full_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_FULL_L) == AUDIOADC_FIFO_FULL_L;
}
#define AUDIOADC_FIFO_UNDERFLOW_R                                             0x00000002UL
static inline bool read_audioadc_fifo_underflow_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_UNDERFLOW_L) == AUDIOADC_FIFO_UNDERFLOW_L;
}
#define AUDIOADC_FIFO_OVERFLOW_R                                              0x00000001UL
static inline bool read_audioadc_fifo_overflow_r(volatile COBRA_SRC_TypeDef   *src)
{
    return (src->AUDIOADC_RIGHT_CHANNEL_FIFO_STATUS & AUDIOADC_FIFO_OVERFLOW_L) == AUDIOADC_FIFO_OVERFLOW_L;
}

#define PDM_OSR_128 128
void pdm_test(uint8_t clocksource);
void pdm_disable(void);
void pdm_enable(void);
void src_pdm_init(uint8_t clocksource,double _1fs_clk_kHz,uint8_t out_bitwidth,uint16_t osr,uint8_t dma_en,double pclk_kHz,uint8_t bypass_hbnum,uint8_t to_i2s);
unsigned int * cobra4_src_read_upfifo_address(void);
void cobra_src_init(void);
#endif //#if COBRA_SRC_EN
#endif //#ifndef _SRC_H
