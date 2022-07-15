/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	cobra_udma.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2019/4/3

	Description :   


	Changed Log	:

		Liujian  2019/4/3			- Creation



*****************************************************************************/

#include <stdio.h>
#include <cobra_config.h>

#include "cobra_udma.h"


#if COBRA_UDMA_EN
#if (COBRA_POWER_DOWN_ENABLE)
static volatile COBRA_UDMA_TypeDef *cobra_udma __attribute__((section("retention_area"),zero_init));
#else
static volatile COBRA_UDMA_TypeDef * cobra_udma;
#endif
void cobra_udma_init(void)
{
		cobra_udma = (COBRA_UDMA_TypeDef *)COBRA_UDMA_BASE;
}

//*****************************************************************************
//
//! Enables the uDMA controller for use.
//!
//! This function enables the uDMA controller.  The uDMA controller must be
//! enabled before it can be configured and used.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_enable(void)
{
		//
    // Set the master enable bit in the config register.
    //
		cobra_udma->dma_cfg = 0x01;
}

//*****************************************************************************
//
//! Disables the uDMA controller for use.
//!
//! This function disables the uDMA controller.  Once disabled, the uDMA
//! controller will not operate until re-enabled with uDMAEnable().
//!
//! \return None.
//
//*****************************************************************************
void	cobra_udma_disable(void)
{
    //
    // Clear the master enable bit in the config register.
    //
    cobra_udma->dma_cfg = 0x00;
}

//*****************************************************************************
//
//! Gets the uDMA error status.
//!
//! This function returns the uDMA error status.  It should be called from
//! within the uDMA error interrupt handler to determine if a uDMA error
//! occurred.
//!
//! \return Returns non-zero if a uDMA error is pending.
//
//*****************************************************************************
unsigned int cobra_udma_get_error_status(void)
{
    //
    // Return the uDMA error status.
    //
    return cobra_udma->err_clr ;
}


//*****************************************************************************
//
//! Clears the uDMA error interrupt.
//!
//! This function clears a pending uDMA error interrupt.  It should be called
//! from within the uDMA error interrupt handler to clear the interrupt.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_clear_error_status(void)
{
    //
    // Clear the uDMA error interrupt.
    //
    cobra_udma->err_clr = 1;
}

//*****************************************************************************
//
//! Enables a uDMA channel for operation.
//!
//! \param ulChannel is the channel number to enable.
//!
//! This function enables a specific uDMA channel for use.  This function must
//! be used to enable a channel before it can be used to perform a uDMA
//! transfer.
//!
//! When a uDMA transfer is completed, the channel will be automatically
//! disabled by the uDMA controller.  Therefore, this function should be called
//! prior to starting up any new transfer.
//! \return None.
//
//*****************************************************************************
void cobra_udma_enable_channel(unsigned int ulChannel)
{
    if (ulChannel >= 32)
        return;

    //
    // Set the bit for this channel in the enable set register.
    //
    cobra_udma->chnl_enable_set |= 1 << ulChannel;
}


void cobra_chnl_enable_all(void)
{
    //
    // Set all channel in the enable set register.
    //
    cobra_udma->chnl_enable_set |= 0xFFFFFFFF;
}

void cobra_dma_chnl_reqmask_all(void)
{
    //
    // Set all channel in the reqmask set register.
    //
    cobra_udma->chnl_req_mask_set |= 0xFFFFFFFF;
}



void cobra_udma_enable_interrupt(unsigned int ulChannel)
{
		cobra_udma->rdw_dma_enable |=  (1 << ulChannel);
}

void cobra_udma_disable_interrupt(unsigned int ulChannel)
{
		cobra_udma->rdw_dma_enable &=  ~(1 << ulChannel);
}

void cobra_udma_clear_interrupt(unsigned int ulChannel)
{
    cobra_udma->rdw_dma_done |= (1 << ulChannel);
}

unsigned int cobra_udma_get_channel_status(unsigned int ulChannel)
{
    return (cobra_udma->rdw_dma_done & (1 << ulChannel));
}

void cobra_udma_clear_channel_status(unsigned int ulChannel)
{
    cobra_udma->rdw_dma_done |= (1 << ulChannel);
}

unsigned int cobra_udma_get_status(void)
{
    return cobra_udma->rdw_dma_done;
}


//*****************************************************************************
//
//! Disables a uDMA channel for operation.
//!
//! \param ulChannel is the channel number to disable.
//!
//! This function disables a specific uDMA channel.  Once disabled, a channel
//! will not respond to uDMA transfer requests until re-enabled via
//! uDMAChannelEnable().
//! \return None.
//
//*****************************************************************************
void cobra_udma_disable_channel(unsigned int ulChannel)
{
    //
    // Check the arguments.
    //
    if (ulChannel >= 32)
        return;

    //
    // Set the bit for this channel in the enable clear register.
    //
    cobra_udma->chnl_enable_clr |= 1 << ulChannel;
}

//*****************************************************************************
//
//! Checks if a uDMA channel is enabled for operation.
//!
//! \param ulChannel is the channel number to check.
//!
//! This function checks to see if a specific uDMA channel is enabled.  This
//! can be used to check the status of a transfer, since the channel will
//! be automatically disabled at the end of a transfer.
//!
//
//*****************************************************************************
unsigned int cobra_udma_channel_is_enable(unsigned int ulChannel)
{
    if(cobra_udma->chnl_enable_set & (1 << ulChannel))
			return 1;
		else
			return 0;
}

//*****************************************************************************
//
//! Sets the base address for the channel control table.
//!
//! \param pControlTable is a pointer to the 1024 byte aligned base address
//! of the uDMA channel control table.
//!
//! This function sets the base address of the channel control table.  This
//! table resides in system memory and holds control information for each uDMA
//! channel.  The table must be aligned on a 1024 byte boundary.  The base
//! address must be set before any of the channel functions can be used.
//!
//! The size of the channel control table depends on the number of uDMA
//! channels, and which transfer modes are used.  Refer to the introductory
//! text and the microcontroller datasheet for more information about the
//! channel control table.
//!
//! \return None.
//
//*****************************************************************************
void cobra_umda_set_control_base(void *pControlTable)
{
    //
    // Check the arguments.
    //
		if(((unsigned int)pControlTable & ~0x3FF) != (unsigned int)pControlTable)
				return;
		
    //
    // Program the base address into the register.
    //
    cobra_udma->ctrl_base_ptr = (unsigned int)pControlTable;
}

//*****************************************************************************
//
//! Gets the base address for the channel control table.
//!
//! This function gets the base address of the channel control table.  This
//! table resides in system memory and holds control information for each uDMA
//! channel.
//!
//! \return Returns a pointer to the base address of the channel control table.
//
//*****************************************************************************
void * cobra_udma_get_control_base(void)
{
    //
    // Read the current value of the control base register, and return it to
    // the caller.
    //
    return((void *)cobra_udma->ctrl_base_ptr);
}


//*****************************************************************************
//
//! Requests a uDMA channel to start a transfer.
//!
//! \param ulChannel is the channel number on which to request a uDMA transfer.
//!
//! This function allows software to request a uDMA channel to begin a
//! transfer.  This could be used for performing a memory to memory transfer,
//! or if for some reason a transfer needs to be initiated by software instead
//! of the peripheral associated with that channel.

//! \note If the channel is \b UDMA_CHANNEL_SW and interrupts are used, then
//! the completion will be signaled on the uDMA dedicated interrupt.  If a
//! peripheral channel is used, then the completion will be signaled on the
//! peripheral's interrupt.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_channel_request(unsigned int ulChannel)
{
    //
    // Check the arguments.
    //
    if (ulChannel >= 32)
        return;

    //
    // Set the bit for this channel in the software uDMA request register.
    //
    cobra_udma->chnl_sw_request = 1 << ulChannel;
}

//*****************************************************************************
//
//! Enables attributes of a uDMA channel.
//!
//! \param ulChannel is the channel to configure.
//! \param ulAttr is a combination of attributes for the channel.
//!
//! The \e ulChannel parameter must be one of the following:

//! The \e ulAttr parameter is the logical OR of any of the following:
//!
//! - \b UDMA_ATTR_USEBURST is used to restrict transfers to use only a burst
//!   mode.
//! - \b UDMA_ATTR_ALTSELECT is used to select the alternate control structure
//!   for this channel.
//! - \b UDMA_ATTR_HIGH_PRIORITY is used to set this channel to high priority.
//! - \b UDMA_ATTR_REQMASK is used to mask the hardware request signal from the
//!   peripheral for this channel.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_channel_attribute_enable(unsigned int ulChannel, unsigned int ulAttr)
{
    //
    // Check the arguments.
    //
    if (ulChannel >= 32)
        return;

    if ((ulAttr & ~(UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK)) != 0)
        return;

    //
    // Set the useburst bit for this channel if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_USEBURST)
    {
        cobra_udma->chnl_useburst_set = 1 << ulChannel;
    }

    //
    // Set the alternate control select bit for this channel,
    // if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_ALTSELECT)
    {
        cobra_udma->chnl_pri_alt_set = 1 << ulChannel;
    }

    //
    // Set the high priority bit for this channel, if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_HIGH_PRIORITY)
    {
        cobra_udma->chnl_priority_set = 1 << ulChannel;
    }

    //
    // Set the request mask bit for this channel, if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_REQMASK)
    {
        cobra_udma->chnl_req_mask_set = 1 << ulChannel;
    }
}

//*****************************************************************************
//
//! Disables attributes of a uDMA channel.
//!
//! \param ulChannel is the channel to configure.
//! \param ulAttr is a combination of attributes for the channel.
//!
//! This function is used to disable attributes of a uDMA channel.
//!
//! The \e ulAttr parameter is the logical OR of any of the following:
//!
//! - \b UDMA_ATTR_USEBURST is used to restrict transfers to use only a burst
//!   mode.
//! - \b UDMA_ATTR_ALTSELECT is used to select the alternate control structure
//!   for this channel.
//! - \b UDMA_ATTR_HIGH_PRIORITY is used to set this channel to high priority.
//! - \b UDMA_ATTR_REQMASK is used to mask the hardware request signal from the
//!   peripheral for this channel.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_channel_attribute_disable(unsigned long ulChannel, unsigned long ulAttr)
{
    //
    // Check the arguments.
    //
    if (ulChannel >= 32)
        return;

    if ((ulAttr & ~(UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK)) != 0)
        return;

    //
    // Clear the useburst bit for this channel if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_USEBURST)
    {
        cobra_udma->chnl_useburst_clr = 1 << ulChannel;
    }

    //
    // Clear the alternate control select bit for this channel, if set in
    // ulConfig.
    //
    if(ulAttr & UDMA_ATTR_ALTSELECT)
    {
        cobra_udma->chnl_pri_alt_clr = 1 << ulChannel;
    }

    //
    // Clear the high priority bit for this channel, if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_HIGH_PRIORITY)
    {
        cobra_udma->chnl_priority_clr = 1 << ulChannel;
    }

    //
    // Clear the request mask bit for this channel, if set in ulConfig.
    //
    if(ulAttr & UDMA_ATTR_REQMASK)
    {
        cobra_udma->chnl_req_mask_clr = 1 << ulChannel;
    }
}

//*****************************************************************************
//
//! Gets the enabled attributes of a uDMA channel.
//!
//! \param ulChannel is the channel to configure.
//!
//! This function returns a combination of flags representing the attributes of
//! the uDMA channel.
//!
//! The \e ulChannel parameter must be one of the following:
//!
//! \return Returns the logical OR of the attributes of the uDMA channel, which
//! can be any of the following:
//! - \b UDMA_ATTR_USEBURST is used to restrict transfers to use only a burst
//!   mode.
//! - \b UDMA_ATTR_ALTSELECT is used to select the alternate control structure
//!   for this channel.
//! - \b UDMA_ATTR_HIGH_PRIORITY is used to set this channel to high priority.
//! - \b UDMA_ATTR_REQMASK is used to mask the hardware request signal from the
//!   peripheral for this channel.
//
//*****************************************************************************
unsigned int cobra_udma_get_channel_attribute(unsigned int ulChannel)
{
    unsigned int ulAttr = 0;

    //
    // Check the arguments.
    //
    if (ulChannel >= 32)
        return 0;

    //
    // Check to see if useburst bit is set for this channel.
    //
    if(cobra_udma->chnl_useburst_set & (1 << ulChannel))
    {
        ulAttr |= UDMA_ATTR_USEBURST;
    }

    //
    // Check to see if the alternate control bit is set for this channel.
    //
    if(cobra_udma->chnl_pri_alt_set & (1 << ulChannel))
    {
        ulAttr |= UDMA_ATTR_ALTSELECT;
    }

    //
    // Check to see if the high priority bit is set for this channel.
    //
    if(cobra_udma->chnl_priority_set & (1 << ulChannel))
    {
        ulAttr |= UDMA_ATTR_HIGH_PRIORITY;
    }

    //
    // Check to see if the request mask bit is set for this channel.
    //
    if(cobra_udma->chnl_req_mask_set & (1 << ulChannel))
    {
        ulAttr |= UDMA_ATTR_REQMASK;
    }

    //
    // Return the configuration flags.
    //
    return(ulAttr);
}


//*****************************************************************************
//
//! Sets the control parameters for a uDMA channel.
//!
//! \param ulChannel is the logical OR of the uDMA channel number with
//! \b UDMA_PRI_SELECT or \b UDMA_ALT_SELECT.
//! \param ulControl is logical OR of several control values to set the control
//! parameters for the channel.
//!
//! This function is used to set control parameters for a uDMA transfer.  These
//! are typically parameters that are not changed often.
//!
//! The \e ulChannel parameter is one of the choices documented in the
//! uDMAChannelEnable() function.  It should be the logical OR of the channel
//! with one of \b UDMA_PRI_SELECT or \b UDMA_ALT_SELECT to choose whether
//! the primary or alternate data structure is used.
//!
//! The \e ulControl parameter is the logical OR of five values: the data size,
//! the source address increment, the destination address increment, the
//! arbitration size, and the use burst flag.  The choices available for each
//! of these values is described below.
//!
//! Choose the data size from one of \b UDMA_SIZE_8, \b UDMA_SIZE_16, or
//! \b UDMA_SIZE_32 to select a data size of 8, 16, or 32 bits.
//!
//! Choose the source address increment from one of \b UDMA_SRC_INC_8,
//! \b UDMA_SRC_INC_16, \b UDMA_SRC_INC_32, or \b UDMA_SRC_INC_NONE to select
//! an address increment of 8-bit bytes, 16-bit halfwords, 32-bit words, or
//! to select non-incrementing.
//!
//! Choose the destination address increment from one of \b UDMA_DST_INC_8,
//! \b UDMA_DST_INC_16, \b UDMA_DST_INC_32, or \b UDMA_DST_INC_NONE to select
//! an address increment of 8-bit bytes, 16-bit halfwords, 32-bit words, or
//! to select non-incrementing.
//!
//! The arbitration size determines how many items are transferred before
//! the uDMA controller re-arbitrates for the bus.  Choose the arbitration size
//! from one of \b UDMA_ARB_1, \b UDMA_ARB_2, \b UDMA_ARB_4, \b UDMA_ARB_8,
//! through \b UDMA_ARB_1024 to select the arbitration size from 1 to 1024
//! items, in powers of 2.
//!
//! The value \b UDMA_NEXT_USEBURST is used to force the channel to only
//! respond to burst requests at the tail end of a scatter-gather transfer.
//!
//! \note The address increment cannot be smaller than the data size.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_set_channel_control(unsigned long ulChannel, unsigned long ulControl)
{
    DMA_CONTROL_TABLE *pCtl;

    //
    // Check the arguments.
    //
    if (ulChannel >= 64)
        return;

    if (cobra_udma->ctrl_base_ptr == 0)
        return;

    //
    // Get the base address of the control table.
    //
    pCtl = (DMA_CONTROL_TABLE *)cobra_udma->ctrl_base_ptr;

    //
    // Get the current control word value and mask off the fields to be
    // changed, then OR in the new settings.
    //
    pCtl[ulChannel].ulControl = ((pCtl[ulChannel].ulControl &
                                  ~(UDMA_CHCTL_DSTINC_M |
                                    UDMA_CHCTL_DSTSIZE_M |
                                    UDMA_CHCTL_SRCINC_M |
                                    UDMA_CHCTL_SRCSIZE_M |
                                    UDMA_CHCTL_ARBSIZE_M |
                                    UDMA_CHCTL_NXTUSEBURST)) |
                                 ulControl);
}


//*****************************************************************************
//
//! Sets the transfer parameters for a uDMA channel.
//!
//! \param ulChannel is the logical or of the uDMA channel number with either
//! \b UDMA_PRI_SELECT or \b UDMA_ALT_SELECT.
//! \param ulMode is the type of uDMA transfer.
//! \param pvSrcAddr is the source address for the transfer.
//! \param pvDstAddr is the destination address for the transfer.
//! \param ulTransferSize is the number of data items to transfer.
//!
//! This function is used to set the parameters for a uDMA transfer.  These are
//! typically parameters that are changed often.  The function
//! uDMAChannelControlSet() MUST be called at least once for this channel prior
//! to calling this function.
//!
//! The \e ulChannel parameter is one of the choices documented in the
//! uDMAChannelEnable() function.  It should be the logical OR of the channel
//! with either \b UDMA_PRI_SELECT or \b UDMA_ALT_SELECT to choose whether the
//! primary or alternate data structure is used.
//!
//! The \e ulMode parameter should be one of the following values:
//!
//! - \b UDMA_MODE_STOP stops the uDMA transfer.  The controller sets the mode
//!   to this value at the end of a transfer.
//! - \b UDMA_MODE_BASIC to perform a basic transfer based on request.
//! - \b UDMA_MODE_AUTO to perform a transfer that will always complete once
//!   started even if request is removed.
//! - \b UDMA_MODE_PINGPONG to set up a transfer that switches between the
//!   primary and alternate control structures for the channel.  This allows
//!   use of ping-pong buffering for uDMA transfers.
//! - \b UDMA_MODE_MEM_SCATTER_GATHER to set up a memory scatter-gather
//!   transfer.
//! - \b UDMA_MODE_PER_SCATTER_GATHER to set up a peripheral scatter-gather
//!   transfer.
//!
//! The \e pvSrcAddr and \e pvDstAddr parameters are pointers to the first
//! location of the data to be transferred.  These addresses should be aligned
//! according to the item size.  The compiler will take care of this if the
//! pointers are pointing to storage of the appropriate data type.
//!
//! The \e ulTransferSize parameter is the number of data items, not the number
//! of bytes.
//!
//! The two scatter/gather modes, memory and peripheral, are actually different
//! depending on whether the primary or alternate control structure is
//! selected.  This function will look for the \b UDMA_PRI_SELECT and
//! \b UDMA_ALT_SELECT flag along with the channel number and will set the
//! scatter/gather mode as appropriate for the primary or alternate control
//! structure.
//!
//! The channel must also be enabled using uDMAChannelEnable() after calling
//! this function.  The transfer will not begin until the channel has been set
//! up and enabled.  Note that the channel is automatically disabled after the
//! transfer is completed, meaning that uDMAChannelEnable() must be called
//! again after setting up the next transfer.
//!
//! \note Great care must be taken to not modify a channel control structure
//! that is in use or else the results will be unpredictable, including the
//! possibility of undesired data transfers to or from memory or peripherals.
//! For BASIC and AUTO modes, it is safe to make changes when the channel is
//! disabled, or the uDMAChannelModeGet() returns \b UDMA_MODE_STOP.  For
//! PINGPONG or one of the SCATTER_GATHER modes, it is safe to modify the
//! primary or alternate control structure only when the other is being used.
//! The uDMAChannelModeGet() function will return \b UDMA_MODE_STOP when a
//! channel control structure is inactive and safe to modify.
//!
//! \return None.
//
//*****************************************************************************
void cobra_udma_set_channel_transfer(unsigned int ulChannel, unsigned int ulMode,
                       void *pvSrcAddr, void *pvDstAddr,
                       unsigned int ulTransferSize)
{
		DMA_CONTROL_TABLE *pControlTable;
    unsigned int ulControl;
    unsigned int ulSize;
    unsigned int ulInc;

    unsigned int  temp_size;

    //
    // Check the arguments.
    //
    if (ulChannel >= 64)
        return;
    if (cobra_udma->ctrl_base_ptr == 0)
        return;
    if (ulMode > UDMA_MODE_PER_SCATTER_GATHER)
        return;
    //  ASSERT((unsigned long)pvSrcAddr >= 0x20000000);
    //  ASSERT((unsigned long)pvDstAddr >= 0x20000000);
    if ((ulTransferSize == 0) || (ulTransferSize > 1024))
        return;

    //
    // Get the base address of the control table.
    //
    pControlTable = (DMA_CONTROL_TABLE *)cobra_udma->ctrl_base_ptr;

    //
    // Get the current control word value and mask off the mode and size
    // fields.
    //
    ulControl = (pControlTable[ulChannel].ulControl &
                 ~(UDMA_CHCTL_XFERSIZE_M | UDMA_CHCTL_XFERMODE_M));

    //
    // Adjust the mode if the alt control structure is selected.
    //
    if(ulChannel & UDMA_ALT_SELECT)
    {
        if((ulMode == UDMA_MODE_MEM_SCATTER_GATHER) ||
           (ulMode == UDMA_MODE_PER_SCATTER_GATHER))
        {
            ulMode |= UDMA_MODE_ALT_SELECT;
        }
    }

    //
    // Set the transfer size and mode in the control word (but dont write the
    // control word yet as it could kick off a transfer).
    //
    ulControl |= ulMode | ((ulTransferSize - 1) << 4);

    //
    // Get the data item size from the control word (set previously).
    //
    ulSize = (ulControl & UDMA_CHCTL_DSTSIZE_M) >> 28;

    //
    // Convert the transfer size to be in units of bytes.  Shift (multiply) to
    // get the value in bytes, based on the data item size.
    //
//    ulTransferSize = ulTransferSize << ulSize;

    temp_size = (ulTransferSize - 1) << ulSize;  //2019/1/21 liujian modified
    //
    // Get the address increment value for the source, from the control word.
    //
    ulInc = (ulControl & UDMA_CHCTL_SRCINC_M);

    //
    // Compute the ending source address of the transfer.  If the source
    // increment is set to none, then the ending address is the same as the
    // beginning.
    //
    if(ulInc != UDMA_SRC_INC_NONE)
    {
     //   pvSrcAddr = (void *)((unsigned long)pvSrcAddr + ulTransferSize - 1);

        pvSrcAddr = (void *)((unsigned int)pvSrcAddr + temp_size); //2019/1/21 liujian modified
    }

    //
    // Load the source ending address into the control block.
    //
    pControlTable[ulChannel].pvSrcEndAddr = pvSrcAddr;

    //
    // Get the address increment value for the destination, from the control
    // word.
    //
    ulInc = (ulControl & UDMA_CHCTL_DSTINC_M);

    //
    // Compute the ending destination address of the transfer.  If the
    // destination increment is set to none, then the ending address is the
    // same as the beginning.
    //
    if(ulInc != UDMA_DST_INC_NONE)
    {
  //      pvDstAddr = (void *)((unsigned long)pvDstAddr + ulTransferSize - 1);

        pvDstAddr = (void *)((unsigned int)pvDstAddr + temp_size);  //2019/1/21 liujian modified
    }

    //
    // Load the destination ending address into the control block.
    //
    pControlTable[ulChannel].pvDstEndAddr = pvDstAddr;

    //
    // Write the new control word value.
    //
    pControlTable[ulChannel].ulControl = ulControl;
}

//liujian add get the DMA buffer
void  * cobra_udma_get_channel_dst_buffer(unsigned int ulChannel, unsigned int ulTransferSize)
{
		DMA_CONTROL_TABLE *pControlTable;
    unsigned long ulControl;
    unsigned long ulSize;


		void * pvDstAddr;

    //
    // Check the arguments.
    //
    if (ulChannel >= 64)
        return NULL;

    if ((ulTransferSize == 0) || (ulTransferSize > 1024))
        return NULL;

    //
    // Get the base address of the control table.
    //
    pControlTable = (DMA_CONTROL_TABLE *)cobra_udma->ctrl_base_ptr;

    //
    // Get the current control word value and mask off the mode and size
    // fields.
    //
    ulControl = (pControlTable[ulChannel].ulControl &
                 ~(UDMA_CHCTL_XFERSIZE_M | UDMA_CHCTL_XFERMODE_M));


    //
    // Set the transfer size and mode in the control word (but dont write the
    // control word yet as it could kick off a transfer).
    //
    ulControl |= ((ulTransferSize - 1) << 4);

    //
    // Get the data item size from the control word (set previously).
    //
    ulSize = (ulControl & UDMA_CHCTL_DSTSIZE_M) >> 28;

    //
    // Convert the transfer size to be in units of bytes.  Shift (multiply) to
    // get the value in bytes, based on the data item size.
    //
    ulTransferSize = ulTransferSize << ulSize;



		pvDstAddr = (void *)((unsigned long)pControlTable[ulChannel].pvDstEndAddr - ulTransferSize + 1);

    return pvDstAddr;
}

//liujian add get the source addr 2020/03/10
void *cobra_udma_get_channel_src_buffer(unsigned int ulChannel, unsigned int ulTransferSize)
{
    DMA_CONTROL_TABLE *pControlTable;
    unsigned long ulControl;
    unsigned long ulSize;

    void *pvDstAddr;

    //
    // Check the arguments.
    //
    if (ulChannel >= 64)
        return NULL;

    if ((ulTransferSize == 0) || (ulTransferSize > 1024))
        return NULL;

    //
    // Get the base address of the control table.
    //
    pControlTable = (DMA_CONTROL_TABLE *)cobra_udma->ctrl_base_ptr;

    //
    // Get the current control word value and mask off the mode and size
    // fields.
    //
    ulControl = (pControlTable[ulChannel].ulControl &
                 ~(UDMA_CHCTL_XFERSIZE_M | UDMA_CHCTL_XFERMODE_M));

    //
    // Set the transfer size and mode in the control word (but dont write the
    // control word yet as it could kick off a transfer).
    //
    ulControl |= ((ulTransferSize - 1) << 4);

    //
    // Get the data item size from the control word (set previously).
    //
    ulSize = (ulControl & UDMA_CHCTL_DSTSIZE_M) >> 28;

    //
    // Convert the transfer size to be in units of bytes.  Shift (multiply) to
    // get the value in bytes, based on the data item size.
    //
    ulTransferSize = ulTransferSize << ulSize;

    pvDstAddr = (void *)((unsigned long)pControlTable[ulChannel].pvSrcEndAddr - ulTransferSize + 1);

    return pvDstAddr;
}

unsigned long cobra_udma_get_channel_size(unsigned long ulChannel)
{
    DMA_CONTROL_TABLE *pControlTable;
    unsigned long ulControl;

    //
    // Check the arguments.
    //
    if (ulChannel >= 64)
        return 0;
    if (cobra_udma->ctrl_base_ptr == 0)
        return 0;

    //
    // Get the base address of the control table.
    //
    pControlTable = (DMA_CONTROL_TABLE *)cobra_udma->ctrl_base_ptr;

    //
    // Get the current control word value and mask off all but the size field.
    //
    ulControl = pControlTable[ulChannel].ulControl & UDMA_CHCTL_XFERSIZE_M;

    //
    // Shift the size field and add one, then return to user.
    //
    return((ulControl >> 4) + 1);
}

unsigned long cobra_udma_get_channel_mode(unsigned long ulChannel)
{
    DMA_CONTROL_TABLE *pControlTable;
    unsigned long ulControl;

    //
    // Check the arguments.
    //
    if (ulChannel >= 64)
        return 0;

    if (cobra_udma->ctrl_base_ptr == 0)
        return 0;

    //
    // Get the base address of the control table.
    //
    pControlTable = (DMA_CONTROL_TABLE *)cobra_udma->ctrl_base_ptr;

    //
    // Get the current control word value and mask off all but the mode field.
    //
    ulControl = pControlTable[ulChannel].ulControl & UDMA_CHCTL_XFERMODE_M;

    //
    // Check if scatter/gather mode, and if so, mask off the alt bit.
    //
    if(((ulControl & ~UDMA_MODE_ALT_SELECT) == UDMA_MODE_MEM_SCATTER_GATHER) ||
       ((ulControl & ~UDMA_MODE_ALT_SELECT) == UDMA_MODE_PER_SCATTER_GATHER))
    {
        ulControl &= ~UDMA_MODE_ALT_SELECT;
    }

    //
    // Return the mode to the caller.
    //
    return(ulControl);
}

#endif














