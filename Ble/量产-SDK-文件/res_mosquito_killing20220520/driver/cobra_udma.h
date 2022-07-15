/******************************************************************************

	Cobra Drivers

	File Name	:	cobra_udma.h
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/3

	Description :   modified from cobra fpga version and coney project
	
	Changed Log	:

		Liujian  2019/4/3		- Creation



*****************************************************************************/

#ifndef __COBRA_UDMA_H__
#define __COBRA_UDMA_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <cobra_config.h>

#if COBRA_UDMA_EN
typedef struct cobra_udma_type
{
	__attribute__ ((aligned (4))) unsigned int   dma_status;
  __attribute__ ((aligned (4))) unsigned int   dma_cfg;
  __attribute__ ((aligned (4))) unsigned int   ctrl_base_ptr;
  __attribute__ ((aligned (4))) unsigned int   alt_ctrl_base_ptr;
  __attribute__ ((aligned (4))) unsigned int   dma_waitonreq_status;
  __attribute__ ((aligned (4))) unsigned int   chnl_sw_request;
  __attribute__ ((aligned (4))) unsigned int   chnl_useburst_set;
  __attribute__ ((aligned (4))) unsigned int   chnl_useburst_clr;
  __attribute__ ((aligned (4))) unsigned int   chnl_req_mask_set;
  __attribute__ ((aligned (4))) unsigned int   chnl_req_mask_clr;
  __attribute__ ((aligned (4))) unsigned int   chnl_enable_set;
  __attribute__ ((aligned (4))) unsigned int   chnl_enable_clr;
  __attribute__ ((aligned (4))) unsigned int   chnl_pri_alt_set;
  __attribute__ ((aligned (4))) unsigned int   chnl_pri_alt_clr;
  __attribute__ ((aligned (4))) unsigned int   chnl_priority_set;
  __attribute__ ((aligned (4))) unsigned int   chnl_priority_clr;
  __attribute__ ((aligned (4))) unsigned int   Reserved1[3];
  __attribute__ ((aligned (4))) unsigned int   err_clr;
  __attribute__ ((aligned (4))) unsigned int   test_reg[640-20];

	__attribute__ ((aligned (4))) unsigned int   rdw_dma_enable;  //add by rdw
	__attribute__ ((aligned (4))) unsigned int   rdw_dma_done;    //add by rdw

}COBRA_UDMA_TypeDef; 


//*****************************************************************************
//
// A structure that defines an entry in the channel control table.  These
// fields are used by the uDMA controller and normally it is not necessary for
// software to directly read or write fields in the table.
//
//*****************************************************************************
typedef struct
{
    //
    // The ending source address of the data transfer.
    //
    volatile void *pvSrcEndAddr;

    //
    // The ending destination address of the data transfer.
    //
    volatile void *pvDstEndAddr;

    //
    // The channel control mode.
    //
    volatile unsigned long ulControl;

    //
    // An unused location.
    //
    volatile unsigned long ulSpare;
	
} DMA_CONTROL_TABLE;

//*****************************************************************************
//
// Flags that can be passed to uDMAChannelAttributeEnable(),
// uDMAChannelAttributeDisable(), and returned from uDMAChannelAttributeGet().
//
//*****************************************************************************
#define UDMA_ATTR_USEBURST      0x00000001
#define UDMA_ATTR_ALTSELECT     0x00000002
#define UDMA_ATTR_HIGH_PRIORITY 0x00000004
#define UDMA_ATTR_REQMASK       0x00000008
#define UDMA_ATTR_ALL           0x0000000F

//*****************************************************************************
//
// DMA control modes that can be passed to uDMAModeSet() and returned
// uDMAModeGet().
//
//*****************************************************************************
#define UDMA_MODE_STOP          0x00000000
#define UDMA_MODE_BASIC         0x00000001
#define UDMA_MODE_AUTO          0x00000002
#define UDMA_MODE_PINGPONG      0x00000003
#define UDMA_MODE_MEM_SCATTER_GATHER \
                                0x00000004
#define UDMA_MODE_PER_SCATTER_GATHER \
                                0x00000006
#define UDMA_MODE_ALT_SELECT    0x00000001


//*****************************************************************************
//
// Flags to be OR'd with the channel ID to indicate if the primary or alternate
// control structure should be used.
//
//*****************************************************************************
#define UDMA_PRI_SELECT         0x00000000
#define UDMA_ALT_SELECT         0x00000020


//*****************************************************************************
//
// The following are defines for the bit fields in the UDMA_O_CHCTL register.
//
//*****************************************************************************
#define UDMA_CHCTL_DSTINC_M     0xC0000000  // Destination Address Increment.
#define UDMA_CHCTL_DSTINC_8     0x00000000  // Byte
#define UDMA_CHCTL_DSTINC_16    0x40000000  // Half-word
#define UDMA_CHCTL_DSTINC_32    0x80000000  // Word
#define UDMA_CHCTL_DSTINC_NONE  0xC0000000  // No increment
#define UDMA_CHCTL_DSTSIZE_M    0x30000000  // Destination Data Size.
#define UDMA_CHCTL_DSTSIZE_8    0x00000000  // Byte
#define UDMA_CHCTL_DSTSIZE_16   0x10000000  // Half-word
#define UDMA_CHCTL_DSTSIZE_32   0x20000000  // Word
#define UDMA_CHCTL_SRCINC_M     0x0C000000  // Source Address Increment.
#define UDMA_CHCTL_SRCINC_8     0x00000000  // Byte
#define UDMA_CHCTL_SRCINC_16    0x04000000  // Half-word
#define UDMA_CHCTL_SRCINC_32    0x08000000  // Word
#define UDMA_CHCTL_SRCINC_NONE  0x0C000000  // No increment
#define UDMA_CHCTL_SRCSIZE_M    0x03000000  // Source Data Size.
#define UDMA_CHCTL_SRCSIZE_8    0x00000000  // Byte
#define UDMA_CHCTL_SRCSIZE_16   0x01000000  // Half-word
#define UDMA_CHCTL_SRCSIZE_32   0x02000000  // Word
#define UDMA_CHCTL_ARBSIZE_M    0x0003C000  // Arbitration Size.
#define UDMA_CHCTL_ARBSIZE_1    0x00000000  // 1 Transfer
#define UDMA_CHCTL_ARBSIZE_2    0x00004000  // 2 Transfers
#define UDMA_CHCTL_ARBSIZE_4    0x00008000  // 4 Transfers
#define UDMA_CHCTL_ARBSIZE_8    0x0000C000  // 8 Transfers
#define UDMA_CHCTL_ARBSIZE_16   0x00010000  // 16 Transfers
#define UDMA_CHCTL_ARBSIZE_32   0x00014000  // 32 Transfers
#define UDMA_CHCTL_ARBSIZE_64   0x00018000  // 64 Transfers
#define UDMA_CHCTL_ARBSIZE_128  0x0001C000  // 128 Transfers
#define UDMA_CHCTL_ARBSIZE_256  0x00020000  // 256 Transfers
#define UDMA_CHCTL_ARBSIZE_512  0x00024000  // 512 Transfers
#define UDMA_CHCTL_ARBSIZE_1024 0x00028000  // 1024 Transfers
#define UDMA_CHCTL_XFERSIZE_M   0x00003FF0  // Transfer Size (minus 1).
#define UDMA_CHCTL_NXTUSEBURST  0x00000008  // Next Useburst.
#define UDMA_CHCTL_XFERMODE_M   0x00000007  // DMA Transfer Mode.
#define UDMA_CHCTL_XFERMODE_STOP \
                                0x00000000  // Stop
#define UDMA_CHCTL_XFERMODE_BASIC \
                                0x00000001  // Basic
#define UDMA_CHCTL_XFERMODE_AUTO \
                                0x00000002  // Auto-Request
#define UDMA_CHCTL_XFERMODE_PINGPONG \
                                0x00000003  // Ping-Pong
#define UDMA_CHCTL_XFERMODE_MEM_SG \
                                0x00000004  // Memory Scatter-Gather
#define UDMA_CHCTL_XFERMODE_MEM_SGA \
                                0x00000005  // Alternate Memory Scatter-Gather
#define UDMA_CHCTL_XFERMODE_PER_SG \
                                0x00000006  // Peripheral Scatter-Gather
#define UDMA_CHCTL_XFERMODE_PER_SGA \
                                0x00000007  // Alternate Peripheral
                                            // Scatter-Gather
#define UDMA_CHCTL_XFERSIZE_S   4


//*****************************************************************************
//
// Channel configuration values that can be passed to uDMAControlSet().
//
//*****************************************************************************
#define UDMA_DST_INC_8          0x00000000
#define UDMA_DST_INC_16         0x40000000
#define UDMA_DST_INC_32         0x80000000
#define UDMA_DST_INC_NONE       0xc0000000
#define UDMA_SRC_INC_8          0x00000000
#define UDMA_SRC_INC_16         0x04000000
#define UDMA_SRC_INC_32         0x08000000
#define UDMA_SRC_INC_NONE       0x0c000000
#define UDMA_SIZE_8             0x00000000
#define UDMA_SIZE_16            0x11000000
#define UDMA_SIZE_32            0x22000000
#define UDMA_ARB_1              0x00000000
#define UDMA_ARB_2              0x00004000
#define UDMA_ARB_4              0x00008000
#define UDMA_ARB_8              0x0000c000
#define UDMA_ARB_16             0x00010000
#define UDMA_ARB_32             0x00014000
#define UDMA_ARB_64             0x00018000
#define UDMA_ARB_128            0x0001c000
#define UDMA_ARB_256            0x00020000
#define UDMA_ARB_512            0x00024000
#define UDMA_ARB_1024           0x00028000
#define UDMA_NEXT_USEBURST      0x00000008



#define COBRA_UDMA_BASE				(0x400C5000UL)

void cobra_udma_init(void);

void	cobra_udma_disable(void);
void cobra_udma_enable(void);


void cobra_udma_clear_error_status(void);
unsigned int cobra_udma_get_error_status(void);

void cobra_udma_enable_interrupt(unsigned int ulChannel);
void cobra_udma_disable_interrupt(unsigned int ulChannel);
void cobra_udma_clear_interrupt(unsigned int ulChannel);

unsigned int cobra_udma_get_status(void);

void cobra_udma_enable_channel(unsigned int ulChannel);
void cobra_udma_disable_channel(unsigned int ulChannel);

void cobra_udma_channel_request(unsigned int ulChannel);


unsigned int cobra_udma_channel_is_enable(unsigned int ulChannel);
void cobra_umda_set_control_base(void *pControlTable);
void * cobra_udma_get_control_base(void);

void cobra_udma_channel_attribute_enable(unsigned int ulChannel, unsigned int ulAttr);
void cobra_udma_channel_attribute_disable(unsigned long ulChannel, unsigned long ulAttr);

unsigned int cobra_udma_get_channel_attribute(unsigned int ulChannel);
void cobra_udma_set_channel_control(unsigned long ulChannel, unsigned long ulControl);

void cobra_udma_set_channel_transfer(unsigned int ulChannel, unsigned int ulMode, void *pvSrcAddr, void *pvDstAddr, unsigned int ulTransferSize);

void * cobra_udma_get_channel_dst_buffer(unsigned int ulChannel, unsigned int ulTransferSize);
void *cobra_udma_get_channel_src_buffer(unsigned int ulChannel, unsigned int ulTransferSize);
unsigned long cobra_udma_get_channel_size(unsigned long ulChannel);
unsigned long cobra_udma_get_channel_mode(unsigned long ulChannel);

unsigned int cobra_udma_get_channel_status(unsigned int ulChannel);
void cobra_chnl_enable_all(void);
void cobra_dma_chnl_reqmask_all(void);
void cobra_udma_clear_channel_status(unsigned int ulChannel);

#endif   //COBRA_UDMA_EN



#endif

