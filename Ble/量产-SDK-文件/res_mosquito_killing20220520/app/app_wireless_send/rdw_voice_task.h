//2018/06/12
//liujian add

#ifndef __RDW_VOICE_TASK_H__
#define __RDW_VOICE_TASK_H__



/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_task.h" // Task definitions
#include "rdw_mcu.h"
#include "rdw_voice.h"


#define   MAX_AUDIO_DATA     204 

#define 	VOICE_IND_NTF_CFG_MAX_LEN  2   //属性最大的长度
enum  voice_msg_id
{
  
    VOICE_ENABLE_REQ = TASK_FIRST_MSG(TASK_ID_VOICE),
    /// Enable confirmation
    VOICE_ENABLE_RSP,      

	VOICE_AUDIO_START_REQ,    	//profile Audio start
	VOICE_AUDIO_START_RSP,   
	
	VOICE_AUDIO_DATA_REQ,   	//发送语音数据
	VOICE_AUDIO_DATA_RSP,
	

    /// Inform APP that Indication Configuration has been changed - use to update bond data
    VOICE_CFG_INDNTF_IND,                         
};


enum  voice_features
{
    /// All Features supported
    VOICE_ALL_FEAT_SUP              = 0x7F,
};


/// Notification and indication configuration
enum  voice_ntf_ind_cfg
{
    
    VOICE_CFG_START_NTF    = (1 << 0),
	
	VOICE_CFG_DATA_NTF    =  ( 1 << 1),
  

};


/*
 * API MESSAGES STRUCTURES
 ****************************************************************************************
 */

struct  voice_db_cfg
{
    uint8_t features;
};



//-------------------------Voice Enable -------------------------------------
struct voice_enable_req
{
    /// Connection index
    uint8_t conidx;
    uint8_t  ntf_ind_cfg;
};

struct voice_enable_rsp
{
    /// Connection index
    uint8_t conidx;
    /// Status of enable request
    uint8_t status;
};


struct voice_start_req
{
		uint8_t  start_state;   
};
struct voice_start_rsp
{
    /// Status
    uint8_t status;
};



struct voice_data_req
{
		uint8_t  length;   
		uint8_t  audio_data[MAX_AUDIO_DATA];
};

struct voice_data_rsp
{
    /// Status
    uint8_t status;
};





struct voice_cfg_indntf_ind
{
    /// connection index
    uint8_t  conidx;
    /// Notification Configuration (@see enum )
    uint8_t  ntf_ind_cfg;
};




void voice_task_init(struct ke_task_desc *task_desc);

/// @} HTPTTASK
#endif // HTPT_TASK_H_
