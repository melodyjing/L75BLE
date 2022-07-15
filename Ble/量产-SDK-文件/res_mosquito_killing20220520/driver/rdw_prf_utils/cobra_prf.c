/**@file			cobra_prf.c
* @brief            
* @details          
* @author      			
* @date				2020-08-13
* @version			V1.0
* @copyright		Copyright (c)	2014-2021 Radiawave Ltd.
**********************************************************************************
* @attention
* 硬件平台:			rdw2150 \n
* SDK 版本:			cobra4a-ble-lib-v1.0.0
* @par 修改日志:
        为了支持外设为  Master, 连接外部设备, 蓝牙作为 HOST 而
				补充的功能代码
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2019/05/05  <td>1.0      <td>liujian   <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "cobra_prf.h"
#include <stdint.h>
#include <stdbool.h>
#include "ke_task.h"
#include "attm.h"
#include "gattc_task.h"
#include "prf_utils.h"
#include "prf.h"
#include "gap.h"
#include "gapc.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "co_error.h"

void prf_disc_svc_send_uuid128(prf_env_t *prf_env, uint8_t conidx, uint8_t *uuid)
{
    //send GATT discover primary services by UUID request
    struct gattc_sdp_svc_disc_cmd * svc_req = KE_MSG_ALLOC_DYN(GATTC_SDP_SVC_DISC_CMD,
            KE_BUILD_ID(TASK_GATTC, conidx), prf_src_task_get(prf_env, conidx),
            gattc_sdp_svc_disc_cmd, ATT_UUID_128_LEN);

    //gatt request type: by UUID
    svc_req->operation         = GATTC_SDP_DISC_SVC;
    //start handle;
    svc_req->start_hdl        = ATT_1ST_REQ_START_HDL;
    //end handle
    svc_req->end_hdl          = ATT_1ST_REQ_END_HDL;

    // UUID search
    svc_req->uuid_len = ATT_UUID_128_LEN;

    //set the first two bytes to the value array, LSB to MSB:Health Thermometer Service UUID first
    //co_write16p(&(svc_req->uuid[0]), uuid);
		memcpy(&(svc_req->uuid[0]), uuid, ATT_UUID_128_LEN);

    //send the message to GATT, which will send back the response when it gets it
    ke_msg_send(svc_req);
}

uint8_t prf_check_svc_char_validity128(uint8_t nb_chars,
                                    const struct prf_char_inf* chars,
                                    const struct prf_char_def_uuid128* chars_req)
{
    uint8_t status = GAP_ERR_NO_ERROR;
    uint8_t i;

    for(i = 0; ((i < nb_chars) && (status == GAP_ERR_NO_ERROR)); i++)
    {
        if (chars[i].char_hdl == ATT_INVALID_HANDLE)
        {
            //If Characteristic is not present, check requirements
            if (chars_req[i].req_flag == ATT_MANDATORY)
            {
                status = PRF_ERR_STOP_DISC_CHAR_MISSING;
            }
        }
        else
        {
            //If Characteristic is present, check properties
            if((chars[i].prop & chars_req[i].prop_mand) != chars_req[i].prop_mand)
            {
							 
                status = PRF_ERR_STOP_DISC_WRONG_CHAR_PROP;
            }
        }
    }

    return (status);
}

void prf_extract_svc_info128(const struct gattc_sdp_svc_ind* param,
        uint8_t nb_chars, const struct prf_char_def_uuid128* chars_req, struct prf_char_inf* chars,
        uint8_t nb_descs, const struct prf_char_desc_def* descs_req, struct prf_char_desc_inf* descs)
{
    //Counters
    uint8_t svc_char;
    uint8_t svc_desc;
    uint8_t fnd_att;

		uint8_t i;
	
    for (fnd_att=0; fnd_att< (param->end_hdl - param->start_hdl); fnd_att++)
    {
        if(param->info[fnd_att].att_type == GATTC_SDP_ATT_CHAR)
        {
            uint16_t char_hdl = param->start_hdl+ 1 + fnd_att;
            uint16_t val_hdl  = param->info[fnd_att].att_char.handle;
            uint8_t  val_prop = param->info[fnd_att].att_char.prop;
            uint8_t  char_idx = fnd_att;

            // check that value handle is in a valid range
            if((val_hdl <= param->end_hdl) && (val_hdl > (param->start_hdl + fnd_att)))
            {
                // retrieve value index
                uint8_t val_idx = (val_hdl - param->start_hdl - 1);

                //Look over requested characteristics
                for (svc_char=0; svc_char<nb_chars ; svc_char++)
                {
                    // check if attribute is valid
                    if((chars[svc_char].char_hdl == ATT_INVALID_HDL)
                            && attm_uuid_comp((uint8_t*)param->info[val_idx].att.uuid,
                                                param->info[val_idx].att.uuid_len, chars_req[svc_char].uuid, ATT_UUID_128_LEN))									 
                    {
											
                        //Save properties and handles
                        chars[svc_char].char_hdl       = char_hdl;
                        chars[svc_char].val_hdl        = val_hdl;
                        chars[svc_char].prop           = val_prop;

                        // find end of characteristic handle and discover descriptors
                        do
                        {
                            fnd_att++;

                            // found a descriptor
                            if(param->info[fnd_att].att_type == GATTC_SDP_ATT_DESC)
                            {
                                //Retrieve characteristic descriptor handle using UUID
                                for(svc_desc = 0; svc_desc<nb_descs; svc_desc++)
                                {
																		
                                    // check if it's expected descriptor
                                    if ((descs[svc_desc].desc_hdl == ATT_INVALID_HANDLE)
                                        && (descs_req[svc_desc].char_code == svc_char)
                                        && (attm_uuid16_comp((uint8_t*)param->info[fnd_att].att.uuid,
                                                param->info[fnd_att].att.uuid_len, descs_req[svc_desc].uuid)))
                                    {
																				
                                        descs[svc_desc].desc_hdl = param->start_hdl + 1 + fnd_att;
																
                                        break;
                                    }
                                }
                            }
                        } while(((param->start_hdl+ 1 + fnd_att) <= param->end_hdl)
                                && (param->info[fnd_att].att_type != GATTC_SDP_ATT_CHAR)
                                && (param->info[fnd_att].att_type != GATTC_SDP_INC_SVC));

                        // return to previous valid value
                        fnd_att--;
                        // previous handle was end of the characteristic
                        chars[svc_char].char_ehdl_off    = fnd_att - char_idx;

                        // search next characteristic
                        break;
                    }
                }
            }
        }
    }
	
}

/*
  2020/6/23 Liujian
  Profile ????
*/

#if (BLE_BATT_SERVER)
void prf_pack_char_pres_fmt(uint8_t *packed_val, const struct prf_char_pres_fmt* char_pres_fmt)
{
    *packed_val =               char_pres_fmt->format;
    *(packed_val + 1) =         char_pres_fmt->exponent;
    co_write16p(packed_val + 2, char_pres_fmt->unit);
    *(packed_val + 4) =         char_pres_fmt->name_space;
    co_write16p(packed_val + 5, char_pres_fmt->description);
}
#endif // (BLE_BATT_SERVER)
