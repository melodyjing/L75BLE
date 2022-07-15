#ifndef _AGC_H
#define _AGC_H
/*RDW AGC module,reference cobra4_agc.csv,time 2019 08 14 09:35:48*/
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <cobra_config.h>
#if COBRA_AGC_EN
typedef struct RDW_AGC {
    __attribute__ ((aligned (4))) volatile unsigned int CFG_AGC_CTRL;                           /*0x000*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SLIDEMEAN_NUM;                      /*0x004*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DETECTOR_CTRL;                      /*0x008*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DETECTOR_BETA_FILTER0;              /*0x00c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DETECTOR_BETA_FILTER1;              /*0x010*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_NOISE_THRESHOLD;                    /*0x014*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_VAD_THRESHOLD;                      /*0x018*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_NOISE_THRESHOLD_HYSTERESIS;         /*0x01c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DGT_GAIN0;                          /*0x020*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_INIT_PGA_GAIN;                      /*0x024*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DB_ALLOCATION_TABLE;                /*0x028*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_FRAME_LENGTH;                       /*0x02c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTGAIN_LIMIT;                     /*0x030*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_STEP;                       /*0x034*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_TARGET_LOW;                 /*0x038*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_TARGET_HIGH;                /*0x03c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_AWAKE_STANDARD;             /*0x040*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_MUTE_STANDARD;              /*0x044*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_TIME;                       /*0x048*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_HOLDTIME;                   /*0x04c*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_SOFTAGC_HOLDTIME1;                  /*0x050*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_PGA;                                /*0x054*/
    __attribute__ ((aligned (4))) volatile unsigned int CFG_DGT_GAIN1;                          /*0x058*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED0[17];                          /*0x05c-0x09c*/
    __attribute__ ((aligned (4))) volatile unsigned int REG0;                                   /*0x0a0*/
    __attribute__ ((aligned (4))) volatile unsigned int REG1;                                   /*0x0a4*/
    __attribute__ ((aligned (4))) volatile unsigned int REG2;                                   /*0x0a8*/
    __attribute__ ((aligned (4))) volatile unsigned int REG3;                                   /*0x0ac*/
    __attribute__ ((aligned (4))) volatile unsigned int REG4;                                   /*0x0b0*/
    __attribute__ ((aligned (4))) volatile unsigned int REG5;                                   /*0x0b4*/
    __attribute__ ((aligned (4))) volatile unsigned int REG6;                                   /*0x0b8*/
    __attribute__ ((aligned (4))) volatile unsigned int REG7;                                   /*0x0bc*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED1[52];                          /*0x0c0-0x18c*/
    __attribute__ ((aligned (4))) volatile unsigned int READ0;                                  /*0x190*/
    __attribute__ ((aligned (4))) volatile unsigned int READ1;                                  /*0x194*/
    __attribute__ ((aligned (4))) volatile unsigned int READ2;                                  /*0x198*/
    __attribute__ ((aligned (4))) volatile unsigned int READ3;                                  /*0x19c*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED2[6];                           /*0x1a0-0x1b4*/
    __attribute__ ((aligned (4))) volatile unsigned int AGC_IN_L;                               /*0x1b8*/
    __attribute__ ((aligned (4))) volatile unsigned int AGC_IN_R;                               /*0x1bc*/
    __attribute__ ((aligned (4))) volatile unsigned int ENVELOPE_OUT_L;                         /*0x1c0*/
    __attribute__ ((aligned (4))) volatile unsigned int ENVELOPE_OUT_R;                         /*0x1c4*/
    __attribute__ ((aligned (4))) volatile unsigned int DETECTOR_READ_L;                        /*0x1c8*/
    __attribute__ ((aligned (4))) volatile unsigned int DETECTOR_READ_R;                        /*0x1cc*/
    __attribute__ ((aligned (4))) volatile unsigned int NOISE_L;                                /*0x1d0*/
    __attribute__ ((aligned (4))) volatile unsigned int NOISE_R;                                /*0x1d4*/
    __attribute__ ((aligned (4))) volatile unsigned int AGC_OUT_L;                              /*0x1d8*/
    __attribute__ ((aligned (4))) volatile unsigned int AGC_OUT_R;                              /*0x1dc*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER0_EMIN_OF_LONGTERM_L;        /*0x1e0*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER1_EMIN_OF_LONGTERM_L;        /*0x1e4*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER0_OUT_L;                     /*0x1e8*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER1_OUT_L;                     /*0x1ec*/
    __attribute__ ((aligned (4))) volatile unsigned int RESERVED3;                              /*0x1f0*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L;    /*0x1f4*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER0_EMIN_OF_LONGTERM_R;        /*0x1f8*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER1_EMIN_OF_LONGTERM_R;        /*0x1fc*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER0_OUT_R;                     /*0x200*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER1_OUT_R;                     /*0x204*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R;    /*0x208*/
    __attribute__ ((aligned (4))) volatile unsigned int BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R;    /*0x20c*/
    __attribute__ ((aligned (4))) volatile unsigned int VOICE_AWAKE_INFO;                       /*0x210*/
    __attribute__ ((aligned (4))) volatile unsigned int LEFT_GAIN_STATUS;                       /*0x214*/
    __attribute__ ((aligned (4))) volatile unsigned int RIGHT_GAIN_STATUS;                      /*0x218*/
} COBRA_AGC_TypeDef;
#define AGC0_BASE        (0x400cc400)
//#define AGC0             ((volatile COBRA_AGC_TypeDef   *) AGC0_BASE )
extern volatile COBRA_AGC_TypeDef *  cobra_agc0;

/*CFG_AGC_CTRL*/
#define CFG_GAIN_DGT_ALIGNED_WITH_PGA_EN                                      0x00040000UL
static inline void set_cfg_gain_dgt_aligned_with_pga_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_GAIN_DGT_ALIGNED_WITH_PGA_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_GAIN_DGT_ALIGNED_WITH_PGA_EN;
    }
}
static inline bool read_cfg_gain_dgt_aligned_with_pga_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_GAIN_DGT_ALIGNED_WITH_PGA_EN) == CFG_GAIN_DGT_ALIGNED_WITH_PGA_EN;
}
#define CFG_DGT_SOFT_AGC_ENABLE                                               0x00020000UL
static inline void set_cfg_dgt_soft_agc_enable(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_DGT_SOFT_AGC_ENABLE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_DGT_SOFT_AGC_ENABLE;
    }
}
static inline bool read_cfg_dgt_soft_agc_enable(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_DGT_SOFT_AGC_ENABLE) == CFG_DGT_SOFT_AGC_ENABLE;
}
#define CFG_AGC_POSITION_POW                                                          14UL
#define CFG_AGC_POSITION_MASK                                                 0x0001c000UL
static inline void set_cfg_agc_position(uint32_t cfg_agc_position,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_AGC_CTRL &= ~CFG_AGC_POSITION_MASK;
    agc->CFG_AGC_CTRL |= cfg_agc_position<<CFG_AGC_POSITION_POW;
}
static inline uint32_t read_cfg_agc_position(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_AGC_POSITION_MASK) >> CFG_AGC_POSITION_POW;
}
#define CFG_ENVELOPE_R_EN                                                     0x00002000UL
static inline void set_cfg_envelope_r_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_ENVELOPE_R_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_ENVELOPE_R_EN;
    }
}
static inline bool read_cfg_envelope_r_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_ENVELOPE_R_EN) == CFG_ENVELOPE_R_EN;
}
#define CFG_ENVELOPE_L_EN                                                     0x00001000UL
static inline void set_cfg_envelope_l_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_ENVELOPE_L_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_ENVELOPE_L_EN;
    }
}
static inline bool read_cfg_envelope_l_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_ENVELOPE_L_EN) == CFG_ENVELOPE_L_EN;
}
#define CFG_SLIDEMEAN_EN                                                      0x00000800UL
static inline void set_cfg_slidemean_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_SLIDEMEAN_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_SLIDEMEAN_EN;
    }
}
static inline bool read_cfg_slidemean_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_SLIDEMEAN_EN) == CFG_SLIDEMEAN_EN;
}
#define CFG_HAND_ADJUST_DGT_GAIN_EN                                           0x00000400UL
static inline void set_cfg_hand_adjust_dgt_gain_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_HAND_ADJUST_DGT_GAIN_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_HAND_ADJUST_DGT_GAIN_EN;
    }
}
static inline bool read_cfg_hand_adjust_dgt_gain_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_HAND_ADJUST_DGT_GAIN_EN) == CFG_HAND_ADJUST_DGT_GAIN_EN;
}
#define CFG_BEGINNING_HOLD_ENABLE                                             0x00000200UL
static inline void set_cfg_beginning_hold_enable(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_BEGINNING_HOLD_ENABLE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_BEGINNING_HOLD_ENABLE;
    }
}
static inline bool read_cfg_beginning_hold_enable(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_BEGINNING_HOLD_ENABLE) == CFG_BEGINNING_HOLD_ENABLE;
}
#define CFG_NORMAL_TO_SILENCE_HOLD_ENABLE                                     0x00000100UL
static inline void set_cfg_normal_to_silence_hold_enable(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_NORMAL_TO_SILENCE_HOLD_ENABLE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_NORMAL_TO_SILENCE_HOLD_ENABLE;
    }
}
static inline bool read_cfg_normal_to_silence_hold_enable(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_NORMAL_TO_SILENCE_HOLD_ENABLE) == CFG_NORMAL_TO_SILENCE_HOLD_ENABLE;
}
#define CFG_SILENCE_TO_NORMAL_HOLD_ENABLE                                     0x00000080UL
static inline void set_cfg_silence_to_normal_hold_enable(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_SILENCE_TO_NORMAL_HOLD_ENABLE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_SILENCE_TO_NORMAL_HOLD_ENABLE;
    }
}
static inline bool read_cfg_silence_to_normal_hold_enable(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_SILENCE_TO_NORMAL_HOLD_ENABLE) == CFG_SILENCE_TO_NORMAL_HOLD_ENABLE;
}
#define CFG_AGC_MUTE_FLAG_EN                                                  0x00000040UL
static inline void set_cfg_agc_mute_flag_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_AGC_MUTE_FLAG_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_AGC_MUTE_FLAG_EN;
    }
}
static inline bool read_cfg_agc_mute_flag_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_AGC_MUTE_FLAG_EN) == CFG_AGC_MUTE_FLAG_EN;
}
#define CFG_AGC_TARGET_LOW_EN                                                 0x00000020UL
static inline void set_cfg_agc_target_low_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_AGC_TARGET_LOW_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_AGC_TARGET_LOW_EN;
    }
}
static inline bool read_cfg_agc_target_low_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_AGC_TARGET_LOW_EN) == CFG_AGC_TARGET_LOW_EN;
}
#define CFG_AGC_LUT1_ATTACK0                                                  0x00000010UL
static inline void set_cfg_agc_lut1_attack0(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_AGC_LUT1_ATTACK0;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_AGC_LUT1_ATTACK0;
    }
}
static inline bool read_cfg_agc_lut1_attack0(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_AGC_LUT1_ATTACK0) == CFG_AGC_LUT1_ATTACK0;
}
#define CFG_AGC_DGT_EN                                                        0x00000008UL
static inline void set_cfg_agc_dgt_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_AGC_DGT_EN;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_AGC_DGT_EN;
    }
}
static inline bool read_cfg_agc_dgt_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_AGC_DGT_EN) == CFG_AGC_DGT_EN;
}
#define CFG_AGC_MUTE_TYPE                                                     0x00000004UL
static inline void set_cfg_agc_mute_type(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_AGC_MUTE_TYPE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_AGC_MUTE_TYPE;
    }
}
static inline bool read_cfg_agc_mute_type(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_AGC_MUTE_TYPE) == CFG_AGC_MUTE_TYPE;
}
#define CFG_RIGHT_AGC_ENABLE                                                  0x00000002UL
static inline void set_cfg_right_agc_enable(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_RIGHT_AGC_ENABLE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_RIGHT_AGC_ENABLE;
    }
}
static inline bool read_cfg_right_agc_enable(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_RIGHT_AGC_ENABLE) == CFG_RIGHT_AGC_ENABLE;
}
#define CFG_LEFT_AGC_ENABLE                                                   0x00000001UL
static inline void set_cfg_left_agc_enable(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_AGC_CTRL |= CFG_LEFT_AGC_ENABLE;
    }else
    {
        agc->CFG_AGC_CTRL &= ~CFG_LEFT_AGC_ENABLE;
    }
}
static inline bool read_cfg_left_agc_enable(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_AGC_CTRL & CFG_LEFT_AGC_ENABLE) == CFG_LEFT_AGC_ENABLE;
}

/*CFG_SLIDEMEAN_NUM*/
#define CFG_SLIDEMEAN_NUM_POW                                                          0UL
#define CFG_SLIDEMEAN_NUM_MASK                                                0x00000003UL
static inline void set_cfg_slidemean_num(uint32_t cfg_slidemean_num,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SLIDEMEAN_NUM &= ~CFG_SLIDEMEAN_NUM_MASK;
    agc->CFG_SLIDEMEAN_NUM |= cfg_slidemean_num<<CFG_SLIDEMEAN_NUM_POW;
}
static inline uint32_t read_cfg_slidemean_num(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SLIDEMEAN_NUM & CFG_SLIDEMEAN_NUM_MASK) >> CFG_SLIDEMEAN_NUM_POW;
}

/*CFG_DETECTOR_CTRL*/
#define CFG_DETECTOR_START_POSITION_POW                                               16UL
#define CFG_DETECTOR_START_POSITION_MASK                                      0xffff0000UL
static inline void set_cfg_detector_start_position(uint32_t cfg_detector_start_position,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_CTRL &= ~CFG_DETECTOR_START_POSITION_MASK;
    agc->CFG_DETECTOR_CTRL |= cfg_detector_start_position<<CFG_DETECTOR_START_POSITION_POW;
}
static inline uint32_t read_cfg_detector_start_position(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_CTRL & CFG_DETECTOR_START_POSITION_MASK) >> CFG_DETECTOR_START_POSITION_POW;
}
#define CFG_DETECTOR_OUT_MODE_POW                                                      4UL
#define CFG_DETECTOR_OUT_MODE_MASK                                            0x00000030UL
static inline void set_cfg_detector_out_mode(uint32_t cfg_detector_out_mode,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_CTRL &= ~CFG_DETECTOR_OUT_MODE_MASK;
    agc->CFG_DETECTOR_CTRL |= cfg_detector_out_mode<<CFG_DETECTOR_OUT_MODE_POW;
}
static inline uint32_t read_cfg_detector_out_mode(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_CTRL & CFG_DETECTOR_OUT_MODE_MASK) >> CFG_DETECTOR_OUT_MODE_POW;
}
#define CFG_NOISE_RENEW_SEL_POW                                                        2UL
#define CFG_NOISE_RENEW_SEL_MASK                                              0x0000000cUL
static inline void set_cfg_noise_renew_sel(uint32_t cfg_noise_renew_sel,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_CTRL &= ~CFG_NOISE_RENEW_SEL_MASK;
    agc->CFG_DETECTOR_CTRL |= cfg_noise_renew_sel<<CFG_NOISE_RENEW_SEL_POW;
}
static inline uint32_t read_cfg_noise_renew_sel(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_CTRL & CFG_NOISE_RENEW_SEL_MASK) >> CFG_NOISE_RENEW_SEL_POW;
}
#define CFG_NOISE_MODE_POW                                                             0UL
#define CFG_NOISE_MODE_MASK                                                   0x00000003UL
static inline void set_cfg_noise_mode(uint32_t cfg_noise_mode,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_CTRL &= ~CFG_NOISE_MODE_MASK;
    agc->CFG_DETECTOR_CTRL |= cfg_noise_mode<<CFG_NOISE_MODE_POW;
}
static inline uint32_t read_cfg_noise_mode(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_CTRL & CFG_NOISE_MODE_MASK) >> CFG_NOISE_MODE_POW;
}

/*CFG_DETECTOR_BETA_FILTER0*/
#define CFG_BETA_FILTER0_IN_MODE_POW                                                  18UL
#define CFG_BETA_FILTER0_IN_MODE_MASK                                         0x001c0000UL
static inline void set_cfg_beta_filter0_in_mode(uint32_t cfg_beta_filter0_in_mode,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_BETA_FILTER0 &= ~CFG_BETA_FILTER0_IN_MODE_MASK;
    agc->CFG_DETECTOR_BETA_FILTER0 |= cfg_beta_filter0_in_mode<<CFG_BETA_FILTER0_IN_MODE_POW;
}
static inline uint32_t read_cfg_beta_filter0_in_mode(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER0 & CFG_BETA_FILTER0_IN_MODE_MASK) >> CFG_BETA_FILTER0_IN_MODE_POW;
}
#define CFG_BETA_FILTER0_EMIN_OF_LONGTERM_CLR                                 0x00020000UL
static inline void set_cfg_beta_filter0_emin_of_longterm_clr(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_DETECTOR_BETA_FILTER0 |= CFG_BETA_FILTER0_EMIN_OF_LONGTERM_CLR;
    }else
    {
        agc->CFG_DETECTOR_BETA_FILTER0 &= ~CFG_BETA_FILTER0_EMIN_OF_LONGTERM_CLR;
    }
}
static inline bool read_cfg_beta_filter0_emin_of_longterm_clr(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER0 & CFG_BETA_FILTER0_EMIN_OF_LONGTERM_CLR) == CFG_BETA_FILTER0_EMIN_OF_LONGTERM_CLR;
}
#define CFG_BETA_FILTER0_EN                                                   0x00010000UL
static inline void set_cfg_beta_filter0_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_DETECTOR_BETA_FILTER0 |= CFG_BETA_FILTER0_EN;
    }else
    {
        agc->CFG_DETECTOR_BETA_FILTER0 &= ~CFG_BETA_FILTER0_EN;
    }
}
static inline bool read_cfg_beta_filter0_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER0 & CFG_BETA_FILTER0_EN) == CFG_BETA_FILTER0_EN;
}
#define CFG_BETA_FILTER0_BETA_POW                                                      0UL
#define CFG_BETA_FILTER0_BETA_MASK                                            0x0000ffffUL
static inline void set_cfg_beta_filter0_beta(uint32_t cfg_beta_filter0_beta,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_BETA_FILTER0 &= ~CFG_BETA_FILTER0_BETA_MASK;
    agc->CFG_DETECTOR_BETA_FILTER0 |= cfg_beta_filter0_beta<<CFG_BETA_FILTER0_BETA_POW;
}
static inline uint32_t read_cfg_beta_filter0_beta(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER0 & CFG_BETA_FILTER0_BETA_MASK) >> CFG_BETA_FILTER0_BETA_POW;
}

/*CFG_DETECTOR_BETA_FILTER1*/
#define CFG_BETA_FILTER1_IN_MODE_POW                                                  18UL
#define CFG_BETA_FILTER1_IN_MODE_MASK                                         0x001c0000UL
static inline void set_cfg_beta_filter1_in_mode(uint32_t cfg_beta_filter1_in_mode,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_BETA_FILTER1 &= ~CFG_BETA_FILTER1_IN_MODE_MASK;
    agc->CFG_DETECTOR_BETA_FILTER1 |= cfg_beta_filter1_in_mode<<CFG_BETA_FILTER1_IN_MODE_POW;
}
static inline uint32_t read_cfg_beta_filter1_in_mode(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER1 & CFG_BETA_FILTER1_IN_MODE_MASK) >> CFG_BETA_FILTER1_IN_MODE_POW;
}
#define CFG_BETA_FILTER1_EMIN_OF_LONGTERM_CLR                                 0x00020000UL
static inline void set_cfg_beta_filter1_emin_of_longterm_clr(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_DETECTOR_BETA_FILTER1 |= CFG_BETA_FILTER1_EMIN_OF_LONGTERM_CLR;
    }else
    {
        agc->CFG_DETECTOR_BETA_FILTER1 &= ~CFG_BETA_FILTER1_EMIN_OF_LONGTERM_CLR;
    }
}
static inline bool read_cfg_beta_filter1_emin_of_longterm_clr(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER1 & CFG_BETA_FILTER1_EMIN_OF_LONGTERM_CLR) == CFG_BETA_FILTER1_EMIN_OF_LONGTERM_CLR;
}
#define CFG_BETA_FILTER1_EN                                                   0x00010000UL
static inline void set_cfg_beta_filter1_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_DETECTOR_BETA_FILTER1 |= CFG_BETA_FILTER1_EN;
    }else
    {
        agc->CFG_DETECTOR_BETA_FILTER1 &= ~CFG_BETA_FILTER1_EN;
    }
}
static inline bool read_cfg_beta_filter1_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER1 & CFG_BETA_FILTER1_EN) == CFG_BETA_FILTER1_EN;
}
#define CFG_BETA_FILTER1_BETA_POW                                                      0UL
#define CFG_BETA_FILTER1_BETA_MASK                                            0x0000ffffUL
static inline void set_cfg_beta_filter1_beta(uint32_t cfg_beta_filter1_beta,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DETECTOR_BETA_FILTER1 &= ~CFG_BETA_FILTER1_BETA_MASK;
    agc->CFG_DETECTOR_BETA_FILTER1 |= cfg_beta_filter1_beta<<CFG_BETA_FILTER1_BETA_POW;
}
static inline uint32_t read_cfg_beta_filter1_beta(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DETECTOR_BETA_FILTER1 & CFG_BETA_FILTER1_BETA_MASK) >> CFG_BETA_FILTER1_BETA_POW;
}

/*CFG_NOISE_THRESHOLD*/
#define CFG_NOISE_THRESHOLD_POW                                                        0UL
#define CFG_NOISE_THRESHOLD_MASK                                              0x00000fffUL
static inline void set_cfg_noise_threshold(uint32_t cfg_noise_threshold,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_NOISE_THRESHOLD &= ~CFG_NOISE_THRESHOLD_MASK;
    agc->CFG_NOISE_THRESHOLD |= cfg_noise_threshold<<CFG_NOISE_THRESHOLD_POW;
}
static inline uint32_t read_cfg_noise_threshold(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_NOISE_THRESHOLD & CFG_NOISE_THRESHOLD_MASK) >> CFG_NOISE_THRESHOLD_POW;
}

/*CFG_VAD_THRESHOLD*/
#define CFG_VAD_THRESHOLD_POW                                                          0UL
#define CFG_VAD_THRESHOLD_MASK                                                0x00000fffUL
static inline void set_cfg_vad_threshold(uint32_t cfg_vad_threshold,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_VAD_THRESHOLD &= ~CFG_VAD_THRESHOLD_MASK;
    agc->CFG_VAD_THRESHOLD |= cfg_vad_threshold<<CFG_VAD_THRESHOLD_POW;
}
static inline uint32_t read_cfg_vad_threshold(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_VAD_THRESHOLD & CFG_VAD_THRESHOLD_MASK) >> CFG_VAD_THRESHOLD_POW;
}

/*CFG_NOISE_THRESHOLD_HYSTERESIS*/
#define CFG_NOISE_THRESHOLD_HYSTERESIS_POW                                             0UL
#define CFG_NOISE_THRESHOLD_HYSTERESIS_MASK                                   0x00000fffUL
static inline void set_cfg_noise_threshold_hysteresis(uint32_t cfg_noise_threshold_hysteresis,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_NOISE_THRESHOLD_HYSTERESIS &= ~CFG_NOISE_THRESHOLD_HYSTERESIS_MASK;
    agc->CFG_NOISE_THRESHOLD_HYSTERESIS |= cfg_noise_threshold_hysteresis<<CFG_NOISE_THRESHOLD_HYSTERESIS_POW;
}
static inline uint32_t read_cfg_noise_threshold_hysteresis(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_NOISE_THRESHOLD_HYSTERESIS & CFG_NOISE_THRESHOLD_HYSTERESIS_MASK) >> CFG_NOISE_THRESHOLD_HYSTERESIS_POW;
}

/*CFG_DGT_GAIN0*/
#define CFG_INIT_DGT_GAIN_POW                                                         12UL
#define CFG_INIT_DGT_GAIN_MASK                                                0x00fff000UL
static inline void set_cfg_init_dgt_gain(uint32_t cfg_init_dgt_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DGT_GAIN0 &= ~CFG_INIT_DGT_GAIN_MASK;
    agc->CFG_DGT_GAIN0 |= cfg_init_dgt_gain<<CFG_INIT_DGT_GAIN_POW;
}
static inline uint32_t read_cfg_init_dgt_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DGT_GAIN0 & CFG_INIT_DGT_GAIN_MASK) >> CFG_INIT_DGT_GAIN_POW;
}
#define CFG_HAND_DGT_GAIN_POW                                                          0UL
#define CFG_HAND_DGT_GAIN_MASK                                                0x00000fffUL
static inline void set_cfg_hand_dgt_gain(uint32_t cfg_hand_dgt_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DGT_GAIN0 &= ~CFG_HAND_DGT_GAIN_MASK;
    agc->CFG_DGT_GAIN0 |= cfg_hand_dgt_gain<<CFG_HAND_DGT_GAIN_POW;
}
static inline uint32_t read_cfg_hand_dgt_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DGT_GAIN0 & CFG_HAND_DGT_GAIN_MASK) >> CFG_HAND_DGT_GAIN_POW;
}

/*CFG_INIT_PGA_GAIN*/
#define CFG_INIT_R_PGA_GAIN_POW                                                        8UL
#define CFG_INIT_R_PGA_GAIN_MASK                                              0x0000ff00UL
static inline void set_cfg_init_r_pga_gain(uint32_t cfg_init_r_pga_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_INIT_PGA_GAIN &= ~CFG_INIT_R_PGA_GAIN_MASK;
    agc->CFG_INIT_PGA_GAIN |= cfg_init_r_pga_gain<<CFG_INIT_R_PGA_GAIN_POW;
}
static inline uint32_t read_cfg_init_r_pga_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_INIT_PGA_GAIN & CFG_INIT_R_PGA_GAIN_MASK) >> CFG_INIT_R_PGA_GAIN_POW;
}
#define CFG_INIT_L_PGA_GAIN_POW                                                        0UL
#define CFG_INIT_L_PGA_GAIN_MASK                                              0x000000ffUL
static inline void set_cfg_init_l_pga_gain(uint32_t cfg_init_l_pga_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_INIT_PGA_GAIN &= ~CFG_INIT_L_PGA_GAIN_MASK;
    agc->CFG_INIT_PGA_GAIN |= cfg_init_l_pga_gain<<CFG_INIT_L_PGA_GAIN_POW;
}
static inline uint32_t read_cfg_init_l_pga_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_INIT_PGA_GAIN & CFG_INIT_L_PGA_GAIN_MASK) >> CFG_INIT_L_PGA_GAIN_POW;
}

/*CFG_DB_ALLOCATION_TABLE*/
#define CFG_DB_ALLOCAION_TABLE_CLEAR                                          0x00000002UL
static inline void set_cfg_db_allocaion_table_clear(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_DB_ALLOCATION_TABLE |= CFG_DB_ALLOCAION_TABLE_CLEAR;
    }else
    {
        agc->CFG_DB_ALLOCATION_TABLE &= ~CFG_DB_ALLOCAION_TABLE_CLEAR;
    }
}
static inline bool read_cfg_db_allocaion_table_clear(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DB_ALLOCATION_TABLE & CFG_DB_ALLOCAION_TABLE_CLEAR) == CFG_DB_ALLOCAION_TABLE_CLEAR;
}
#define CFG_DB_ALLOCAION_TABLE_EN                                             0x00000001UL
static inline void set_cfg_db_allocaion_table_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_DB_ALLOCATION_TABLE |= CFG_DB_ALLOCAION_TABLE_EN;
    }else
    {
        agc->CFG_DB_ALLOCATION_TABLE &= ~CFG_DB_ALLOCAION_TABLE_EN;
    }
}
static inline bool read_cfg_db_allocaion_table_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DB_ALLOCATION_TABLE & CFG_DB_ALLOCAION_TABLE_EN) == CFG_DB_ALLOCAION_TABLE_EN;
}

/*CFG_FRAME_LENGTH*/
#define CFG_FRAME_STAT_LENGTH_POW                                                      16UL
#define CFG_FRAME_STAT_LENGTH_MASK                                            0xffff0000UL
static inline void set_cfg_frame_stat_length(uint32_t cfg_frame_stat_length,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_FRAME_LENGTH &= ~CFG_FRAME_STAT_LENGTH_MASK;
    agc->CFG_FRAME_LENGTH |= cfg_frame_stat_length<<CFG_FRAME_STAT_LENGTH_POW;
}
static inline uint32_t read_cfg_frame_stat_length(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_FRAME_LENGTH & CFG_FRAME_STAT_LENGTH_MASK) >> CFG_FRAME_STAT_LENGTH_POW;
}
#define CFG_FRAME_LENGTH_POW                                                           0UL
#define CFG_FRAME_LENGTH_MASK                                                 0x0000ffffUL
static inline void set_cfg_frame_length(uint32_t cfg_frame_length,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_FRAME_LENGTH &= ~CFG_FRAME_LENGTH_MASK;
    agc->CFG_FRAME_LENGTH |= cfg_frame_length<<CFG_FRAME_LENGTH_POW;
}
static inline uint32_t read_cfg_frame_length(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_FRAME_LENGTH & CFG_FRAME_LENGTH_MASK) >> CFG_FRAME_LENGTH_POW;
}

/*CFG_SOFTGAIN_LIMIT*/
#define CFG_MIN_GAIN_POW                                                               8UL
#define CFG_MIN_GAIN_MASK                                                     0x0000ff00UL
static inline void set_cfg_min_gain(uint32_t cfg_min_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTGAIN_LIMIT &= ~CFG_MIN_GAIN_MASK;
    agc->CFG_SOFTGAIN_LIMIT |= cfg_min_gain<<CFG_MIN_GAIN_POW;
}
static inline uint32_t read_cfg_min_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTGAIN_LIMIT & CFG_MIN_GAIN_MASK) >> CFG_MIN_GAIN_POW;
}
#define CFG_MAX_GAIN_POW                                                               0UL
#define CFG_MAX_GAIN_MASK                                                     0x000000ffUL
static inline void set_cfg_max_gain(uint32_t cfg_max_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTGAIN_LIMIT &= ~CFG_MAX_GAIN_MASK;
    agc->CFG_SOFTGAIN_LIMIT |= cfg_max_gain<<CFG_MAX_GAIN_POW;
}
static inline uint32_t read_cfg_max_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTGAIN_LIMIT & CFG_MAX_GAIN_MASK) >> CFG_MAX_GAIN_POW;
}

/*CFG_SOFTAGC_STEP*/
#define CFG_DECAY_STEP_LENGTH_POW                                                      8UL
#define CFG_DECAY_STEP_LENGTH_MASK                                            0x0000ff00UL
static inline void set_cfg_decay_step_length(uint32_t cfg_decay_step_length,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_STEP &= ~CFG_DECAY_STEP_LENGTH_MASK;
    agc->CFG_SOFTAGC_STEP |= cfg_decay_step_length<<CFG_DECAY_STEP_LENGTH_POW;
}
static inline uint32_t read_cfg_decay_step_length(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_STEP & CFG_DECAY_STEP_LENGTH_MASK) >> CFG_DECAY_STEP_LENGTH_POW;
}
#define CFG_ATTACK_STEP_LENGTH_POW                                                     0UL
#define CFG_ATTACK_STEP_LENGTH_MASK                                           0x000000ffUL
static inline void set_cfg_attack_step_length(uint32_t cfg_attack_step_length,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_STEP &= ~CFG_ATTACK_STEP_LENGTH_MASK;
    agc->CFG_SOFTAGC_STEP |= cfg_attack_step_length<<CFG_ATTACK_STEP_LENGTH_POW;
}
static inline uint32_t read_cfg_attack_step_length(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_STEP & CFG_ATTACK_STEP_LENGTH_MASK) >> CFG_ATTACK_STEP_LENGTH_POW;
}

/*CFG_SOFTAGC_TARGET_LOW*/
#define CFG_AGC_TARGET_LOW_POW                                                         0UL
#define CFG_AGC_TARGET_LOW_MASK                                               0x00000fffUL
static inline void set_cfg_agc_target_low(uint32_t cfg_agc_target_low,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_TARGET_LOW &= ~CFG_AGC_TARGET_LOW_MASK;
    agc->CFG_SOFTAGC_TARGET_LOW |= cfg_agc_target_low<<CFG_AGC_TARGET_LOW_POW;
}
static inline uint32_t read_cfg_agc_target_low(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_TARGET_LOW & CFG_AGC_TARGET_LOW_MASK) >> CFG_AGC_TARGET_LOW_POW;
}

/*CFG_SOFTAGC_TARGET_HIGH*/
#define CFG_AGC_TARGET_HIGH_POW                                                        0UL
#define CFG_AGC_TARGET_HIGH_MASK                                              0x00000fffUL
static inline void set_cfg_agc_target_high(uint32_t cfg_agc_target_high,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_TARGET_HIGH &= ~CFG_AGC_TARGET_HIGH_MASK;
    agc->CFG_SOFTAGC_TARGET_HIGH |= cfg_agc_target_high<<CFG_AGC_TARGET_HIGH_POW;
}
static inline uint32_t read_cfg_agc_target_high(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_TARGET_HIGH & CFG_AGC_TARGET_HIGH_MASK) >> CFG_AGC_TARGET_HIGH_POW;
}

/*CFG_SOFTAGC_AWAKE_STANDARD*/
#define CFG_AGC_AWAKE_STANDARD_POW                                                     0UL
#define CFG_AGC_AWAKE_STANDARD_MASK                                           0x00000fffUL
static inline void set_cfg_agc_awake_standard(uint32_t cfg_agc_awake_standard,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_AWAKE_STANDARD &= ~CFG_AGC_AWAKE_STANDARD_MASK;
    agc->CFG_SOFTAGC_AWAKE_STANDARD |= cfg_agc_awake_standard<<CFG_AGC_AWAKE_STANDARD_POW;
}
static inline uint32_t read_cfg_agc_awake_standard(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_AWAKE_STANDARD & CFG_AGC_AWAKE_STANDARD_MASK) >> CFG_AGC_AWAKE_STANDARD_POW;
}

/*CFG_SOFTAGC_MUTE_STANDARD*/
#define CFG_AGC_MUTE_STANDARD_POW                                                      0UL
#define CFG_AGC_MUTE_STANDARD_MASK                                            0x00000fffUL
static inline void set_cfg_agc_mute_standard(uint32_t cfg_agc_mute_standard,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_MUTE_STANDARD &= ~CFG_AGC_MUTE_STANDARD_MASK;
    agc->CFG_SOFTAGC_MUTE_STANDARD |= cfg_agc_mute_standard<<CFG_AGC_MUTE_STANDARD_POW;
}
static inline uint32_t read_cfg_agc_mute_standard(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_MUTE_STANDARD & CFG_AGC_MUTE_STANDARD_MASK) >> CFG_AGC_MUTE_STANDARD_POW;
}

/*CFG_SOFTAGC_TIME*/
#define CFG_DECAY_TIME_POW                                                            16UL
#define CFG_DECAY_TIME_MASK                                                   0xffff0000UL
static inline void set_cfg_decay_time(uint32_t cfg_decay_time,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_TIME &= ~CFG_DECAY_TIME_MASK;
    agc->CFG_SOFTAGC_TIME |= cfg_decay_time<<CFG_DECAY_TIME_POW;
}
static inline uint32_t read_cfg_decay_time(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_TIME & CFG_DECAY_TIME_MASK) >> CFG_DECAY_TIME_POW;
}
#define CFG_ATTACK_TIME_POW                                                            0UL
#define CFG_ATTACK_TIME_MASK                                                  0x0000ffffUL
static inline void set_cfg_attack_time(uint32_t cfg_attack_time,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_TIME &= ~CFG_ATTACK_TIME_MASK;
    agc->CFG_SOFTAGC_TIME |= cfg_attack_time<<CFG_ATTACK_TIME_POW;
}
static inline uint32_t read_cfg_attack_time(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_TIME & CFG_ATTACK_TIME_MASK) >> CFG_ATTACK_TIME_POW;
}

/*CFG_SOFTAGC_HOLDTIME*/
#define CFG_SILENCE_TO_NORMAL_HOLD_TIME_POW                                           16UL
#define CFG_SILENCE_TO_NORMAL_HOLD_TIME_MASK                                  0xffff0000UL
static inline void set_cfg_silence_to_normal_hold_time(uint32_t cfg_silence_to_normal_hold_time,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_HOLDTIME &= ~CFG_SILENCE_TO_NORMAL_HOLD_TIME_MASK;
    agc->CFG_SOFTAGC_HOLDTIME |= cfg_silence_to_normal_hold_time<<CFG_SILENCE_TO_NORMAL_HOLD_TIME_POW;
}
static inline uint32_t read_cfg_silence_to_normal_hold_time(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_HOLDTIME & CFG_SILENCE_TO_NORMAL_HOLD_TIME_MASK) >> CFG_SILENCE_TO_NORMAL_HOLD_TIME_POW;
}
#define CFG_NORMAL_TO_SILENCE_HOLD_TIME_POW                                            0UL
#define CFG_NORMAL_TO_SILENCE_HOLD_TIME_MASK                                  0x0000ffffUL
static inline void set_cfg_normal_to_silence_hold_time(uint32_t cfg_normal_to_silence_hold_time,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_HOLDTIME &= ~CFG_NORMAL_TO_SILENCE_HOLD_TIME_MASK;
    agc->CFG_SOFTAGC_HOLDTIME |= cfg_normal_to_silence_hold_time<<CFG_NORMAL_TO_SILENCE_HOLD_TIME_POW;
}
static inline uint32_t read_cfg_normal_to_silence_hold_time(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_HOLDTIME & CFG_NORMAL_TO_SILENCE_HOLD_TIME_MASK) >> CFG_NORMAL_TO_SILENCE_HOLD_TIME_POW;
}

/*CFG_SOFTAGC_HOLDTIME1*/
#define CFG_BEGINNING_HOLD_TIME_POW                                                    0UL
#define CFG_BEGINNING_HOLD_TIME_MASK                                          0x0000ffffUL
static inline void set_cfg_beginning_hold_time(uint32_t cfg_beginning_hold_time,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_SOFTAGC_HOLDTIME1 &= ~CFG_BEGINNING_HOLD_TIME_MASK;
    agc->CFG_SOFTAGC_HOLDTIME1 |= cfg_beginning_hold_time<<CFG_BEGINNING_HOLD_TIME_POW;
}
static inline uint32_t read_cfg_beginning_hold_time(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_SOFTAGC_HOLDTIME1 & CFG_BEGINNING_HOLD_TIME_MASK) >> CFG_BEGINNING_HOLD_TIME_POW;
}

/*CFG_PGA*/
#define CFG_PGA_GAIN_DCOFFSET_R_POW                                                   25UL
#define CFG_PGA_GAIN_DCOFFSET_R_MASK                                          0x3e000000UL
static inline void set_cfg_pga_gain_dcoffset_r(uint32_t cfg_pga_gain_dcoffset_r,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_PGA &= ~CFG_PGA_GAIN_DCOFFSET_R_MASK;
    agc->CFG_PGA |= cfg_pga_gain_dcoffset_r<<CFG_PGA_GAIN_DCOFFSET_R_POW;
}
static inline uint32_t read_cfg_pga_gain_dcoffset_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_GAIN_DCOFFSET_R_MASK) >> CFG_PGA_GAIN_DCOFFSET_R_POW;
}
#define CFG_PGA_DB_R_POW                                                              19UL
#define CFG_PGA_DB_R_MASK                                                     0x01f80000UL
static inline void set_cfg_pga_db_r(uint32_t cfg_pga_db_r,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_PGA &= ~CFG_PGA_DB_R_MASK;
    agc->CFG_PGA |= cfg_pga_db_r<<CFG_PGA_DB_R_POW;
}
static inline uint32_t read_cfg_pga_db_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_DB_R_MASK) >> CFG_PGA_DB_R_POW;
}
#define CFG_PGA_ADD_NEG6DB_EN_R                                               0x00040000UL
static inline void set_cfg_pga_add_neg6db_en_r(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_PGA |= CFG_PGA_ADD_NEG6DB_EN_R;
    }else
    {
        agc->CFG_PGA &= ~CFG_PGA_ADD_NEG6DB_EN_R;
    }
}
static inline bool read_cfg_pga_add_neg6db_en_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_ADD_NEG6DB_EN_R) == CFG_PGA_ADD_NEG6DB_EN_R;
}
#define CFG_PGA_EN_R                                                          0x00020000UL
static inline void set_cfg_pga_en_r(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_PGA |= CFG_PGA_EN_R;
    }else
    {
        agc->CFG_PGA &= ~CFG_PGA_EN_R;
    }
}
static inline bool read_cfg_pga_en_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_EN_R) == CFG_PGA_EN_R;
}
#define CFG_HAND_ADJUST_PGA_R_EN                                              0x00010000UL
static inline void set_cfg_hand_adjust_pga_r_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_PGA |= CFG_HAND_ADJUST_PGA_R_EN;
    }else
    {
        agc->CFG_PGA &= ~CFG_HAND_ADJUST_PGA_R_EN;
    }
}
static inline bool read_cfg_hand_adjust_pga_r_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_HAND_ADJUST_PGA_R_EN) == CFG_HAND_ADJUST_PGA_R_EN;
}
#define CFG_PGA_GAIN_DCOFFSET_L_POW                                                    9UL
#define CFG_PGA_GAIN_DCOFFSET_L_MASK                                          0x00003e00UL
static inline void set_cfg_pga_gain_dcoffset_l(uint32_t cfg_pga_gain_dcoffset_l,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_PGA &= ~CFG_PGA_GAIN_DCOFFSET_L_MASK;
    agc->CFG_PGA |= cfg_pga_gain_dcoffset_l<<CFG_PGA_GAIN_DCOFFSET_L_POW;
}
static inline uint32_t read_cfg_pga_gain_dcoffset_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_GAIN_DCOFFSET_L_MASK) >> CFG_PGA_GAIN_DCOFFSET_L_POW;
}
#define CFG_PGA_DB_L_POW                                                               3UL
#define CFG_PGA_DB_L_MASK                                                     0x000001f8UL
static inline void set_cfg_pga_db_l(uint32_t cfg_pga_db_l,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_PGA &= ~CFG_PGA_DB_L_MASK;
    agc->CFG_PGA |= cfg_pga_db_l<<CFG_PGA_DB_L_POW;
}
static inline uint32_t read_cfg_pga_db_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_DB_L_MASK) >> CFG_PGA_DB_L_POW;
}
#define CFG_PGA_ADD_NEG6DB_EN_L                                               0x00000004UL
static inline void set_cfg_pga_add_neg6db_en_l(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_PGA |= CFG_PGA_ADD_NEG6DB_EN_L;
    }else
    {
        agc->CFG_PGA &= ~CFG_PGA_ADD_NEG6DB_EN_L;
    }
}
static inline bool read_cfg_pga_add_neg6db_en_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_ADD_NEG6DB_EN_L) == CFG_PGA_ADD_NEG6DB_EN_L;
}
#define CFG_PGA_EN_L                                                          0x00000002UL
static inline void set_cfg_pga_en_l(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_PGA |= CFG_PGA_EN_L;
    }else
    {
        agc->CFG_PGA &= ~CFG_PGA_EN_L;
    }
}
static inline bool read_cfg_pga_en_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_PGA_EN_L) == CFG_PGA_EN_L;
}
#define CFG_HAND_ADJUST_PGA_L_EN                                              0x00000001UL
static inline void set_cfg_hand_adjust_pga_l_en(volatile COBRA_AGC_TypeDef   *agc,bool en)
{
    if(en)
    {
        agc->CFG_PGA |= CFG_HAND_ADJUST_PGA_L_EN;
    }else
    {
        agc->CFG_PGA &= ~CFG_HAND_ADJUST_PGA_L_EN;
    }
}
static inline bool read_cfg_hand_adjust_pga_l_en(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_PGA & CFG_HAND_ADJUST_PGA_L_EN) == CFG_HAND_ADJUST_PGA_L_EN;
}

/*CFG_DGT_GAIN1*/
#define CFG_DGT_GAIN_WORK_DELAY_POW                                                   28UL
#define CFG_DGT_GAIN_WORK_DELAY_MASK                                          0x70000000UL
static inline void set_cfg_dgt_gain_work_delay(uint32_t cfg_dgt_gain_work_delay,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DGT_GAIN1 &= ~CFG_DGT_GAIN_WORK_DELAY_MASK;
    agc->CFG_DGT_GAIN1 |= cfg_dgt_gain_work_delay<<CFG_DGT_GAIN_WORK_DELAY_POW;
}
static inline uint32_t read_cfg_dgt_gain_work_delay(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DGT_GAIN1 & CFG_DGT_GAIN_WORK_DELAY_MASK) >> CFG_DGT_GAIN_WORK_DELAY_POW;
}
#define CFG_DGT_INIT_GAIN_POW                                                         16UL
#define CFG_DGT_INIT_GAIN_MASK                                                0x0fff0000UL
static inline void set_cfg_dgt_init_gain(uint32_t cfg_dgt_init_gain,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DGT_GAIN1 &= ~CFG_DGT_INIT_GAIN_MASK;
    agc->CFG_DGT_GAIN1 |= cfg_dgt_init_gain<<CFG_DGT_INIT_GAIN_POW;
}
static inline uint32_t read_cfg_dgt_init_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DGT_GAIN1 & CFG_DGT_INIT_GAIN_MASK) >> CFG_DGT_INIT_GAIN_POW;
}
#define CFG_DGT_DB_0_125_MAX_POW                                                       8UL
#define CFG_DGT_DB_0_125_MAX_MASK                                             0x0000ff00UL
static inline void set_cfg_dgt_db_0_125_max(uint32_t cfg_dgt_db_0_125_max,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DGT_GAIN1 &= ~CFG_DGT_DB_0_125_MAX_MASK;
    agc->CFG_DGT_GAIN1 |= cfg_dgt_db_0_125_max<<CFG_DGT_DB_0_125_MAX_POW;
}
static inline uint32_t read_cfg_dgt_db_0_125_max(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DGT_GAIN1 & CFG_DGT_DB_0_125_MAX_MASK) >> CFG_DGT_DB_0_125_MAX_POW;
}
#define CFG_DGT_DB_0_125_MIN_POW                                                       0UL
#define CFG_DGT_DB_0_125_MIN_MASK                                             0x000000ffUL
static inline void set_cfg_dgt_db_0_125_min(uint32_t cfg_dgt_db_0_125_min,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->CFG_DGT_GAIN1 &= ~CFG_DGT_DB_0_125_MIN_MASK;
    agc->CFG_DGT_GAIN1 |= cfg_dgt_db_0_125_min<<CFG_DGT_DB_0_125_MIN_POW;
}
static inline uint32_t read_cfg_dgt_db_0_125_min(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->CFG_DGT_GAIN1 & CFG_DGT_DB_0_125_MIN_MASK) >> CFG_DGT_DB_0_125_MIN_POW;
}

/*REG0*/
#define REG0_POW                                                                       0UL
#define REG0_MASK                                                             0xffffffffUL
static inline void set_reg0(uint32_t reg0,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG0 &= ~REG0_MASK;
    agc->REG0 |= reg0<<REG0_POW;
}
static inline uint32_t read_reg0(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG0 & REG0_MASK) >> REG0_POW;
}

/*REG1*/
#define REG1_POW                                                                       0UL
#define REG1_MASK                                                             0xffffffffUL
static inline void set_reg1(uint32_t reg1,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG1 &= ~REG1_MASK;
    agc->REG1 |= reg1<<REG1_POW;
}
static inline uint32_t read_reg1(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG1 & REG1_MASK) >> REG1_POW;
}

/*REG2*/
#define REG2_POW                                                                       0UL
#define REG2_MASK                                                             0xffffffffUL
static inline void set_reg2(uint32_t reg2,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG2 &= ~REG2_MASK;
    agc->REG2 |= reg2<<REG2_POW;
}
static inline uint32_t read_reg2(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG2 & REG2_MASK) >> REG2_POW;
}

/*REG3*/
#define REG3_POW                                                                       0UL
#define REG3_MASK                                                             0xffffffffUL
static inline void set_reg3(uint32_t reg3,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG3 &= ~REG3_MASK;
    agc->REG3 |= reg3<<REG3_POW;
}
static inline uint32_t read_reg3(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG3 & REG3_MASK) >> REG3_POW;
}

/*REG4*/
#define REG4_POW                                                                       0UL
#define REG4_MASK                                                             0xffffffffUL
static inline void set_reg4(uint32_t reg4,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG4 &= ~REG4_MASK;
    agc->REG4 |= reg4<<REG4_POW;
}
static inline uint32_t read_reg4(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG4 & REG4_MASK) >> REG4_POW;
}

/*REG5*/
#define REG5_POW                                                                       0UL
#define REG5_MASK                                                             0xffffffffUL
static inline void set_reg5(uint32_t reg5,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG5 &= ~REG5_MASK;
    agc->REG5 |= reg5<<REG5_POW;
}
static inline uint32_t read_reg5(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG5 & REG5_MASK) >> REG5_POW;
}

/*REG6*/
#define REG6_POW                                                                       0UL
#define REG6_MASK                                                             0xffffffffUL
static inline void set_reg6(uint32_t reg6,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG6 &= ~REG6_MASK;
    agc->REG6 |= reg6<<REG6_POW;
}
static inline uint32_t read_reg6(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG6 & REG6_MASK) >> REG6_POW;
}

/*REG7*/
#define REG7_POW                                                                       0UL
#define REG7_MASK                                                             0xffffffffUL
static inline void set_reg7(uint32_t reg7,volatile COBRA_AGC_TypeDef   *agc)
{
    agc->REG7 &= ~REG7_MASK;
    agc->REG7 |= reg7<<REG7_POW;
}
static inline uint32_t read_reg7(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->REG7 & REG7_MASK) >> REG7_POW;
}

/*READ0*/
#define READ0_POW                                                                      0UL
#define READ0_MASK                                                            0xffffffffUL
static inline uint32_t read_read0(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->READ0 & READ0_MASK) >> READ0_POW;
}

/*READ1*/
#define READ1_POW                                                                      0UL
#define READ1_MASK                                                            0xffffffffUL
static inline uint32_t read_read1(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->READ1 & READ1_MASK) >> READ1_POW;
}

/*READ2*/
#define READ2_POW                                                                      0UL
#define READ2_MASK                                                            0xffffffffUL
static inline uint32_t read_read2(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->READ2 & READ2_MASK) >> READ2_POW;
}

/*READ3*/
#define READ3_POW                                                                      0UL
#define READ3_MASK                                                            0xffffffffUL
static inline uint32_t read_read3(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->READ3 & READ3_MASK) >> READ3_POW;
}

/*AGC_IN_L*/
#define AGC_IN_L_POW                                                                   0UL
#define AGC_IN_L_MASK                                                         0x00000fffUL
static inline uint32_t read_agc_in_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->AGC_IN_L & AGC_IN_L_MASK) >> AGC_IN_L_POW;
}

/*AGC_IN_R*/
#define AGC_IN_R_POW                                                                   0UL
#define AGC_IN_R_MASK                                                         0x00000fffUL
static inline uint32_t read_agc_in_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->AGC_IN_R & AGC_IN_R_MASK) >> AGC_IN_R_POW;
}

/*ENVELOPE_OUT_L*/
#define ENVELOPE_OUT_L_POW                                                             0UL
#define ENVELOPE_OUT_L_MASK                                                   0x00000fffUL
static inline uint32_t read_envelope_out_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->ENVELOPE_OUT_L & ENVELOPE_OUT_L_MASK) >> ENVELOPE_OUT_L_POW;
}

/*ENVELOPE_OUT_R*/
#define ENVELOPE_OUT_R_POW                                                             0UL
#define ENVELOPE_OUT_R_MASK                                                   0x00000fffUL
static inline uint32_t read_envelope_out_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->ENVELOPE_OUT_R & ENVELOPE_OUT_R_MASK) >> ENVELOPE_OUT_R_POW;
}

/*DETECTOR_READ_L*/
#define DETECTOR_OUT_L_VLD                                                    0x80000000UL
static inline bool read_detector_out_l_vld(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->DETECTOR_READ_L & DETECTOR_OUT_L_VLD) == DETECTOR_OUT_L_VLD;
}
#define DETECTOR_OUT_L_POW                                                             0UL
#define DETECTOR_OUT_L_MASK                                                   0x00000fffUL
static inline uint32_t read_detector_out_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->DETECTOR_READ_L & DETECTOR_OUT_L_MASK) >> DETECTOR_OUT_L_POW;
}

/*DETECTOR_READ_R*/
#define DETECTOR_OUT_R_VLD                                                    0x80000000UL
static inline bool read_detector_out_r_vld(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->DETECTOR_READ_R & DETECTOR_OUT_R_VLD) == DETECTOR_OUT_R_VLD;
}
#define DETECTOR_OUT_R_POW                                                             0UL
#define DETECTOR_OUT_R_MASK                                                   0x00000fffUL
static inline uint32_t read_detector_out_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->DETECTOR_READ_R & DETECTOR_OUT_R_MASK) >> DETECTOR_OUT_R_POW;
}

/*NOISE_L*/
#define NOISE_L_POW                                                                    0UL
#define NOISE_L_MASK                                                          0x00000fffUL
static inline uint32_t read_noise_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->NOISE_L & NOISE_L_MASK) >> NOISE_L_POW;
}

/*NOISE_R*/
#define NOISE_R_POW                                                                    0UL
#define NOISE_R_MASK                                                          0x00000fffUL
static inline uint32_t read_noise_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->NOISE_R & NOISE_R_MASK) >> NOISE_R_POW;
}

/*AGC_OUT_L*/
#define AGC_OUT_L_POW                                                                  0UL
#define AGC_OUT_L_MASK                                                        0x00000fffUL
static inline uint32_t read_agc_out_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->AGC_OUT_L & AGC_OUT_L_MASK) >> AGC_OUT_L_POW;
}

/*AGC_OUT_R*/
#define AGC_OUT_R_POW                                                                  0UL
#define AGC_OUT_R_MASK                                                        0x00000fffUL
static inline uint32_t read_agc_out_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->AGC_OUT_R & AGC_OUT_R_MASK) >> AGC_OUT_R_POW;
}

/*BETA_FILTER0_EMIN_OF_LONGTERM_L*/
#define BETA_FILTER0_EMIN_OF_LONGTERM_L_POW                                            0UL
#define BETA_FILTER0_EMIN_OF_LONGTERM_L_MASK                                  0x00000fffUL
static inline uint32_t read_beta_filter0_emin_of_longterm_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER0_EMIN_OF_LONGTERM_L & BETA_FILTER0_EMIN_OF_LONGTERM_L_MASK) >> BETA_FILTER0_EMIN_OF_LONGTERM_L_POW;
}

/*BETA_FILTER1_EMIN_OF_LONGTERM_L*/
#define BETA_FILTER1_EMIN_OF_LONGTERM_L_POW                                            0UL
#define BETA_FILTER1_EMIN_OF_LONGTERM_L_MASK                                  0x00000fffUL
static inline uint32_t read_beta_filter1_emin_of_longterm_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_EMIN_OF_LONGTERM_L & BETA_FILTER1_EMIN_OF_LONGTERM_L_MASK) >> BETA_FILTER1_EMIN_OF_LONGTERM_L_POW;
}

/*BETA_FILTER0_OUT_L*/
#define BETA_FILTER0_OUT_L_POW                                                         0UL
#define BETA_FILTER0_OUT_L_MASK                                               0x00000fffUL
static inline uint32_t read_beta_filter0_out_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER0_OUT_L & BETA_FILTER0_OUT_L_MASK) >> BETA_FILTER0_OUT_L_POW;
}

/*BETA_FILTER1_OUT_L*/
#define BETA_FILTER1_OUT_L_POW                                                         0UL
#define BETA_FILTER1_OUT_L_MASK                                               0x00000fffUL
static inline uint32_t read_beta_filter1_out_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_OUT_L & BETA_FILTER1_OUT_L_MASK) >> BETA_FILTER1_OUT_L_POW;
}
#define BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_L_POW                                  0UL
#define BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_L_MASK                        0x0000ffffUL
static inline uint32_t read_beta_filter0_zero_crossing_num_of_frame_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_OUT_L & BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_L_MASK) >> BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_L_POW;
}

/*BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L*/
#define BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L_POW                                  0UL
#define BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L_MASK                        0x0000ffffUL
static inline uint32_t read_beta_filter1_zero_crossing_num_of_frame_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L & BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L_MASK) >> BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_L_POW;
}

/*BETA_FILTER0_EMIN_OF_LONGTERM_R*/
#define BETA_FILTER0_EMIN_OF_LONGTERM_R_POW                                            0UL
#define BETA_FILTER0_EMIN_OF_LONGTERM_R_MASK                                  0x00000fffUL
static inline uint32_t read_beta_filter0_emin_of_longterm_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER0_EMIN_OF_LONGTERM_R & BETA_FILTER0_EMIN_OF_LONGTERM_R_MASK) >> BETA_FILTER0_EMIN_OF_LONGTERM_R_POW;
}

/*BETA_FILTER1_EMIN_OF_LONGTERM_R*/
#define BETA_FILTER1_EMIN_OF_LONGTERM_R_POW                                            0UL
#define BETA_FILTER1_EMIN_OF_LONGTERM_R_MASK                                  0x00000fffUL
static inline uint32_t read_beta_filter1_emin_of_longterm_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_EMIN_OF_LONGTERM_R & BETA_FILTER1_EMIN_OF_LONGTERM_R_MASK) >> BETA_FILTER1_EMIN_OF_LONGTERM_R_POW;
}

/*BETA_FILTER0_OUT_R*/
#define BETA_FILTER0_OUT_R_POW                                                         0UL
#define BETA_FILTER0_OUT_R_MASK                                               0x00000fffUL
static inline uint32_t read_beta_filter0_out_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER0_OUT_R & BETA_FILTER0_OUT_R_MASK) >> BETA_FILTER0_OUT_R_POW;
}

/*BETA_FILTER1_OUT_R*/
#define BETA_FILTER1_OUT_R_POW                                                         0UL
#define BETA_FILTER1_OUT_R_MASK                                               0x00000fffUL
static inline uint32_t read_beta_filter1_out_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_OUT_R & BETA_FILTER1_OUT_R_MASK) >> BETA_FILTER1_OUT_R_POW;
}

/*BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R*/
#define BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R_POW                                  0UL
#define BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R_MASK                        0x0000ffffUL
static inline uint32_t read_beta_filter0_zero_crossing_num_of_frame_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R & BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R_MASK) >> BETA_FILTER0_ZERO_CROSSING_NUM_OF_FRAME_R_POW;
}

/*BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R*/
#define BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R_POW                                  0UL
#define BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R_MASK                        0x0000ffffUL
static inline uint32_t read_beta_filter1_zero_crossing_num_of_frame_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R & BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R_MASK) >> BETA_FILTER1_ZERO_CROSSING_NUM_OF_FRAME_R_POW;
}

/*VOICE_AWAKE_INFO*/
#define VOICE_AWAKE_R                                                         0x00000002UL
static inline bool read_voice_awake_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->VOICE_AWAKE_INFO & VOICE_AWAKE_R) == VOICE_AWAKE_R;
}
#define VOICE_AWAKE_L                                                         0x00000001UL
static inline bool read_voice_awake_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->VOICE_AWAKE_INFO & VOICE_AWAKE_L) == VOICE_AWAKE_L;
}

/*LEFT_GAIN_STATUS*/
#define MUTE_FLAG_L                                                           0x00000001UL
static inline bool read_mute_flag_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & MUTE_FLAG_L) == MUTE_FLAG_L;
}
#define L_PGA_GAIN_RENEW_PUL                                                  0x00000001UL
static inline bool read_l_pga_gain_renew_pul(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & L_PGA_GAIN_RENEW_PUL) == L_PGA_GAIN_RENEW_PUL;
}
#define INCR_GAIN_PUL_L                                                       0x00000001UL
static inline bool read_incr_gain_pul_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & INCR_GAIN_PUL_L) == INCR_GAIN_PUL_L;
}
#define REACH_PGA_MIN_IND_L                                                   0x00000001UL
static inline bool read_reach_pga_min_ind_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & REACH_PGA_MIN_IND_L) == REACH_PGA_MIN_IND_L;
}
#define REACH_PGA_MAX_IND_L                                                   0x00000001UL
static inline bool read_reach_pga_max_ind_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & REACH_PGA_MAX_IND_L) == REACH_PGA_MAX_IND_L;
}
#define ANA_DB_RESIDUE_L                                                      0x00000001UL
static inline bool read_ana_db_residue_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & ANA_DB_RESIDUE_L) == ANA_DB_RESIDUE_L;
}
#define L_PGA_GAIN                                                            0x00000001UL
static inline bool read_l_pga_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & L_PGA_GAIN) == L_PGA_GAIN;
}
#define INCR_GAIN_L                                                           0x00000001UL
static inline bool read_incr_gain_l(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->LEFT_GAIN_STATUS & INCR_GAIN_L) == INCR_GAIN_L;
}

/*RIGHT_GAIN_STATUS*/
#define MUTE_FLAG_R                                                           0x00000001UL
static inline bool read_mute_flag_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & MUTE_FLAG_R) == MUTE_FLAG_R;
}
#define R_PGA_GAIN_RENEW_PUL                                                  0x00000001UL
static inline bool read_r_pga_gain_renew_pul(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & R_PGA_GAIN_RENEW_PUL) == R_PGA_GAIN_RENEW_PUL;
}
#define INCR_GAIN_PUL_R                                                       0x00000001UL
static inline bool read_incr_gain_pul_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & INCR_GAIN_PUL_R) == INCR_GAIN_PUL_R;
}
#define REACH_PGA_MIN_IND_R                                                   0x00000001UL
static inline bool read_reach_pga_min_ind_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & REACH_PGA_MIN_IND_R) == REACH_PGA_MIN_IND_R;
}
#define REACH_PGA_MAX_IND_R                                                   0x00000001UL
static inline bool read_reach_pga_max_ind_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & REACH_PGA_MAX_IND_R) == REACH_PGA_MAX_IND_R;
}
#define ANA_DB_RESIDUE_R                                                      0x00000001UL
static inline bool read_ana_db_residue_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & ANA_DB_RESIDUE_R) == ANA_DB_RESIDUE_R;
}
#define R_PGA_GAIN                                                            0x00000001UL
static inline bool read_r_pga_gain(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & R_PGA_GAIN) == R_PGA_GAIN;
}
#define INCR_GAIN_R                                                           0x00000001UL
static inline bool read_incr_gain_r(volatile COBRA_AGC_TypeDef   *agc)
{
    return (agc->RIGHT_GAIN_STATUS & INCR_GAIN_R) == INCR_GAIN_R;
}

void cobra_agc_init(void);

#endif //#if COBRA_AGC_EN
#endif //#ifndef _AGC_H
