/******************************************************************************
	File Name	:	sound_scan.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/03/06

	Description :   

	Changed Log	:
		Liujian  2020/03/06		- Creation

*****************************************************************************/

#include "sound_scan.h"
#include "sound_scan_task.h"
#include "rdw_mcu.h"

#if BLE_APP_RECEIVE_VOICE

#define M_SOUND_SCAN_INTERVAL  5

struct sound_scan_device_env_tag  sound_scan_env;

extern const struct ke_task_desc TASK_DESC_SOUND_RECEIVER;

/*
建立一个 扫描任务
*/
void app_sound_scan_init(void)
{
	
		memset(&sound_scan_env, 0, sizeof(sound_scan_env));
	
		ke_task_create(TASK_SOUND_RECIVER, &TASK_DESC_SOUND_RECEIVER);
	
		sound_scan_env.scan_state = APP_SOUND_SCAN_STATE_IDLE;
	
		sound_scan_env.interval = M_SOUND_SCAN_INTERVAL;
}

void app_sound_scan_send_gapm_reset_cmd(void)
{
    // Reset the stack
    struct gapm_reset_cmd *p_cmd = KE_MSG_ALLOC(GAPM_RESET_CMD,
                                                TASK_GAPM, TASK_SOUND_RECIVER,
                                                gapm_reset_cmd);

    p_cmd->operation = GAPM_RESET;

    ke_msg_send(p_cmd);
}


/*
发送消息给 GAPM , 建立一个扫描 , 蓝牙扫描, 成为 MASTER 
*/
void app_sound_scan_create_activity(void)
{
			struct gapm_activity_create_cmd *p_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_CREATE_CMD, TASK_GAPM,
																																							 TASK_SOUND_RECIVER, gapm_activity_create_cmd);
			p_cmd->operation                       = GAPM_CREATE_SCAN_ACTIVITY;
			p_cmd->own_addr_type                   = GAPM_STATIC_ADDR  ; //GAPM_STATIC_ADDR;
	
			
			ke_msg_send(p_cmd);
			
			sound_scan_env.scan_state = APP_SOUND_SCAN_STATE_CREATING;
			
}


void app_sound_scan_set_device_config(void)
{
		// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KE_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
																												 TASK_GAPM, TASK_SOUND_RECIVER,
																												 gapm_set_dev_config_cmd);
			// Set the operation
			cmd->operation = GAPM_SET_DEV_CONFIG;
			// Set the device role - Peripheral
			cmd->role      = GAP_ROLE_ALL;
			
			cmd->max_mtu = COBRA4_MTU;
			cmd->max_mps = COBRA4_MPS;
			// Set Data length parameters
			cmd->sugg_max_tx_octets = LE_MAX_OCTETS; //BLE_MIN_OCTETS;
			cmd->sugg_max_tx_time   = LE_MAX_TIME;
			
			cmd->privacy_cfg = 0;
#if DIANBIAO_ENABLE_PRIV_EN				
			cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
#endif
			memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
			
			// Send message
      ke_msg_send(cmd);
	
}

void app_sound_scan_start_scan(void)
{
			struct gapm_activity_start_cmd *p_start_cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_START_CMD, TASK_GAPM, TASK_SOUND_RECIVER,
																																						 gapm_activity_start_cmd);

			//普通 1M  扫描
			p_start_cmd->operation                                  = GAPM_START_ACTIVITY;
			p_start_cmd->actv_idx                                   = sound_scan_env.act_id;
			p_start_cmd->u_param.scan_param.type                    = GAPM_SCAN_TYPE_OBSERVER;
			p_start_cmd->u_param.scan_param.prop                    = GAPM_SCAN_PROP_PHY_1M_BIT ;
			p_start_cmd->u_param.scan_param.dup_filt_pol            = GAPM_DUP_FILT_DIS;
			
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_intv = 64*sound_scan_env.interval;    //old 32
			p_start_cmd->u_param.scan_param.scan_param_1m.scan_wd   = 4*sound_scan_env.interval;
			
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_intv = 4*sound_scan_env.interval;
			p_start_cmd->u_param.scan_param.scan_param_coded.scan_wd   = sound_scan_env.interval;
			
			p_start_cmd->u_param.scan_param.duration                = 0;
			p_start_cmd->u_param.scan_param.period                  = 0;
		
			// Send message
      ke_msg_send(p_start_cmd);
}

void app_sound_scan_stop_scan(void)
{
    // Prepare the GAPM_ACTIVITY_STOP_CMD message
    struct gapm_activity_stop_cmd *cmd = KE_MSG_ALLOC(GAPM_ACTIVITY_STOP_CMD,
                                                      TASK_GAPM, TASK_SOUND_RECIVER,
                                                      gapm_activity_stop_cmd);

    // Fill the allocated kernel message
    cmd->operation = GAPM_STOP_ACTIVITY;
    cmd->actv_idx = sound_scan_env.act_id;

    // Send the message
    ke_msg_send(cmd);

}



/*********************************************************************
 * @fn      app_sound_scan_find_svc_uuid
 *
 * @brief   Find a given UUID in an advertiser's service UUID list.
 *
 * @return  TRUE if service UUID found
 */
bool app_sound_scan_find_svc_uuid(uint16_t uuid, uint8_t *pData, uint16_t dataLen)
{
  uint8_t adLen;
  uint8_t adType;
  uint8_t *pEnd;

  if (dataLen > 0)
  {
    pEnd = pData + dataLen - 1;
    // While end of data not reached
    while (pData < pEnd)
    {
      // Get length of next AD item
      adLen = *pData++;
      if (adLen > 0)
      {
        adType = *pData;

        // If AD type is for 16-bit service UUID
        if ((adType == GAP_ADTYPE_16BIT_MORE) ||
            (adType == GAP_ADTYPE_16BIT_COMPLETE))
        {
          pData++;
          adLen--;

          // For each UUID in list
          while (adLen >= 2 && pData < pEnd)
          {
            // Check for match
            if ((pData[0] == LO_UINT16(uuid)) && (pData[1] == HI_UINT16(uuid)))
            {
              // Match found
              return true;
            }
            // Go to next
            pData += 2;
            adLen -= 2;
          }
          // Handle possible erroneous extra byte in UUID list
          if (adLen == 1)
          {
            pData++;
          }
        }
        else
        {
          // Go to next item
          pData += adLen;
        }
      }
    }
  }

  // Match not found
  return false;
}
#endif
