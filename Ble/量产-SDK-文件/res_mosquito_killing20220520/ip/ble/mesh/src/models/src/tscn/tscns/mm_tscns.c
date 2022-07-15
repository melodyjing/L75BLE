/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	File Name	  :	mm_tscns.c
	Version		  :	0.1
	Created	By	:	LiuJian
	Date		    :	2021/2/1

	Description :   


	Changed Log	:

		Liujian  2021/2/1		- Creation

*****************************************************************************/

#include "mm_tscns.h"
#include "mm_tscns_time.h"
#include "mm_tscns_schdule.h"
#include "mm_tscns_scene.h"

uint16_t mm_tscns_init(bool reset, void *p_env, const mm_cfg_t *p_cfg)
{
    // Return size of environment
    return (0);
}

uint16_t mm_tscns_get_env_size(const mm_cfg_t *p_cfg)
{
    // Return size of environment
    return (0);
}

__STATIC uint16_t mm_tscns_register_time(uint8_t elmt_idx, bool main)
{
    // Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;

    if (mm_tb_state_get_lid(elmt_idx, MM_ID_TSCNS_TIM) == MESH_INVALID_LID)
    {
        // Model local index
        m_lid_t mdl_lid;

        // Register the Time Server model
        status = mm_tscns_time_register(elmt_idx, &mdl_lid);

        if (main && (status == MESH_ERR_NO_ERROR))
        {
            // Group local index
            m_lid_t grp_lid;

            // Create group
            mm_tb_bind_add_group(0, elmt_idx, &grp_lid, mdl_lid,
                                 mm_tscns_time_cb_grp_event, NULL);
        }
    }

    return (status);
}


__STATIC uint16_t mm_tscns_register_schdule(uint8_t elmt_idx, bool main)
{
	// Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;

    if (mm_tb_state_get_lid(elmt_idx, MM_ID_TSCNS_SCH) == MESH_INVALID_LID)
    {
        // Model local index
        m_lid_t mdl_lid;

        // Register the Schdule Server model
        status = mm_tscns_schdule_register(elmt_idx, &mdl_lid);

        if (main && (status == MESH_ERR_NO_ERROR))
        {
            // Group local index
            m_lid_t grp_lid;

            // Create group
            mm_tb_bind_add_group(0, elmt_idx, &grp_lid, mdl_lid,
                                 mm_tscns_schdule_cb_grp_event, NULL);
        }
    }

    return (status);
	
}


__STATIC uint16_t mm_tscns_register_scene(uint8_t elmt_idx, bool main)
{
	// Status
    uint16_t status = MESH_ERR_MDL_ALREADY_REGISTERED;

    if (mm_tb_state_get_lid(elmt_idx, MM_ID_TSCNS_SCN) == MESH_INVALID_LID)
    {
        // Model local index
        m_lid_t mdl_lid;

        // Register the Scene Server model
        status = mm_tscns_scene_register(elmt_idx, &mdl_lid);

        if (main && (status == MESH_ERR_NO_ERROR))
        {
            // Group local index
            m_lid_t grp_lid;

            // Create group
            mm_tb_bind_add_group(0, elmt_idx, &grp_lid, mdl_lid,
                                 mm_tscns_scene_cb_grp_event, NULL);
        }
    }

    return (status);
	
}



uint16_t mm_tscns_register(uint8_t elmt_idx, uint8_t mdl_cfg_idx, bool main)
{
    // Status
    uint16_t status = MESH_ERR_NO_ERROR;

    switch (mdl_cfg_idx)
    {
        case (MM_CFG_IDX_TSCNS_TIME):
        {
            // Add models needed for use of Time Server model
            status = mm_tscns_register_time(elmt_idx, main);
        } break;
				
				case (MM_CFG_IDX_TSCNS_SCHDULE):
				{
					  status = mm_tscns_register_schdule(elmt_idx, main);
				}break;
				
				case (MM_CFG_IDX_TSCNS_SCENCE):
				{
					  status = mm_tscns_register_scene(elmt_idx, main);
				}break;

        default:
        {
            // Configuration is unknown
            ASSERT_INFO(0, mdl_cfg_idx, 0);
            status = MESH_ERR_COMMAND_DISALLOWED;
        }
    }

    return (status);
}


