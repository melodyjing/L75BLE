/******************************************************************************
	File Name	:	  app_sec2.c
	Version		:	  0.1
	Created	By	:	LiuJian
	Date		:	    2020/12/07

	Description :
	          为了支持第二个从设备也可以是由等级的安全连接  SMP

	Changed Log	:
		Liujian     2020/12/07			- Creation

*****************************************************************************/

#include "rwip_config.h"

#if((DIANBIAO_SPM_ENABLE == 1)&&(APP_HOST2_ENABLE == 1))

#include <string.h>
#include "co_utils.h"
#include "co_math.h"
#include "gapc_task.h"      // GAP Controller Task API Definition
#include "gap.h"            // GAP Definition
#include "gapc.h"           // GAPC Definition
#include "prf_types.h"
#include "app_sec2.h"
#include "nvds.h"
#include "rdw_mcu.h"
#include <stdio.h>
#include "host_advertiser1.h"
#include "app.h"
#include "device1_scan.h"
#include "app_iris_console.h"

#if (COBRA_POWER_DOWN_ENABLE)
static struct app_sec_env2_tag  app_sec_env2		__attribute__((section("retention_area"),zero_init));
#else
static struct app_sec_env2_tag  app_sec_env2;
#endif

extern struct adv_env_tag  adv_device1_env;
void app_sec2_init()
{

    uint8_t length = ADV2_DEVICE_BOND_LENGTH;

    // Get bond status from NVDS
    if (nvds_get(ADV2_DEVICE_BOND_FLAG, &length, (uint8_t *)&app_sec_env2.bonded) != NVDS_OK)
    {
        // If read value is invalid, set status to not bonded
        app_sec_env2.bonded = false;
    }

    if ((app_sec_env2.bonded != true) && (app_sec_env2.bonded != false))
    {
        app_sec_env2.bonded = false;
    }

}

bool app_sec2_get_bond_status(void)
{
    return app_sec_env2.bonded;
}

void app_sec2_remove_bond(void)
{
    // Check if we are well bonded
    if (app_sec_env2.bonded == true)
    {
        // Update the environment variable
        app_sec_env2.bonded = false;

        if (nvds_put(ADV2_DEVICE_BOND_FLAG, ADV2_DEVICE_BOND_LENGTH,
                     (uint8_t *)&app_sec_env2.bonded) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }
    }
}

void app_sec2_send_security_req(uint8_t conidx)
{
    // Send security request
    struct gapc_security_cmd *cmd = KE_MSG_ALLOC(GAPC_SECURITY_CMD,
                                    KE_BUILD_ID(TASK_GAPC, conidx), TASK_ADV_DEVICE1,
                                    gapc_security_cmd);
    cmd->operation = GAPC_SECURITY_REQ;

//		cmd->auth      = GAP_AUTH_REQ_MITM_BOND;


    if(0)
    {
        cmd->auth      = GAP_AUTH_REQ_MITM_BOND;
    }
    else
    {
        cmd->auth      = GAP_AUTH_REQ_MITM_NO_BOND;
    }

    // Send the message
    ke_msg_send(cmd);
}


static int gapc_bond_req_ind_handler(ke_msg_id_t const msgid,
                                     struct gapc_bond_req_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
    // Prepare the GAPC_BOND_CFM message
    struct gapc_bond_cfm *cfm = KE_MSG_ALLOC(GAPC_BOND_CFM,
                                src_id, TASK_ADV_DEVICE1,
                                gapc_bond_cfm);

    printf("gapc_bond_req_ind_handler \n");
    switch (param->request)
    {
    case (GAPC_PAIRING_REQ):
    {
        cfm->request = GAPC_PAIRING_RSP;

        cfm->accept  = false;
        printf("GAPC_PAIRING_REQ  2222 \n");
        // Check if we are already bonded (Only one bonded connection is supported)
        // if (!app_sec_env2.bonded)
        {
            cfm->accept  = true;

            //liujian add 2019/5/17

            cfm->data.pairing_feat.auth      =  GAP_AUTH_REQ_MITM_NO_BOND; //GAP_AUTH_REQ_NO_MITM_NO_BOND;

            if(1)
            {
                cfm->data.pairing_feat.iocap     = GAP_IO_CAP_DISPLAY_ONLY;
            }
            else
            {
                cfm->data.pairing_feat.iocap     = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
            }

            cfm->data.pairing_feat.key_size  = 16;
            cfm->data.pairing_feat.oob       = GAP_OOB_AUTH_DATA_NOT_PRESENT;
            cfm->data.pairing_feat.sec_req   = GAP_NO_SEC;
            cfm->data.pairing_feat.ikey_dist = GAP_KDIST_NONE;
            cfm->data.pairing_feat.rkey_dist = GAP_KDIST_ENCKEY;

        }
    }
    break;

    case (GAPC_LTK_EXCH):
    {
        // Counter
        uint8_t counter;
        printf("GAPC_LTK_EXCH  2222 \n");
        cfm->accept  = true;
        cfm->request = GAPC_LTK_EXCH;

        // Generate all the values
        cfm->data.ltk.ediv = (uint16_t)co_rand_word();

        for (counter = 0; counter < RAND_NB_LEN; counter++)
        {
            cfm->data.ltk.ltk.key[counter]    = (uint8_t)co_rand_word();
            cfm->data.ltk.randnb.nb[counter] = (uint8_t)co_rand_word();
        }

        for (counter = RAND_NB_LEN; counter < KEY_LEN; counter++)
        {
            cfm->data.ltk.ltk.key[counter]    = (uint8_t)co_rand_word();
        }

        // Store the generated value in NVDS
        if (nvds_put(ADV2_NVDS_TAG_LTK, ADV2_NVDS_LEN_LTK, (uint8_t *)&cfm->data.ltk) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }

    }
    break;


    case (GAPC_IRK_EXCH):
    {
#if (NVDS_SUPPORT)
        uint8_t addr_len = BD_ADDR_LEN;
#endif //(NVDS_SUPPORT)
        printf("GAPC_IRK_EXCH  2222 \n");
        cfm->accept  = true;
        cfm->request = GAPC_IRK_EXCH;

        // Load IRK
        memcpy(cfm->data.irk.irk.key, adv_device1_env.loc_irk, KEY_LEN);


        if (nvds_get(NVDS_TAG_BD_ADDRESS, &addr_len, cfm->data.irk.addr.addr.addr) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }
        // load device address
        cfm->data.irk.addr.addr_type = (cfm->data.irk.addr.addr.addr[5] & 0xC0) ? ADDR_RAND : ADDR_PUBLIC;
    }
    break;


    case (GAPC_TK_EXCH):
    {

        cfm->accept  = true;
        cfm->request = GAPC_TK_EXCH;

        // Set the TK value
        memset(cfm->data.tk.key, 0, KEY_LEN);
#if(DIANBIAO_MODULE_ENABLE == 1)
        unsigned char *p_pincode = app_get_meter_master_pincode();

        uint32_t pin_code = app_get_pincode_convert_ascii_to_hex(p_pincode, 6);
#else
        uint32_t pin_code = (100000 + (co_rand_word()%900000));
#endif
        printf("pin_code 222 = %d \n", pin_code);

        cfm->data.tk.key[0] = (uint8_t)((pin_code & 0x000000FF) >>  0);
        cfm->data.tk.key[1] = (uint8_t)((pin_code & 0x0000FF00) >>  8);
        cfm->data.tk.key[2] = (uint8_t)((pin_code & 0x00FF0000) >> 16);
        cfm->data.tk.key[3] = (uint8_t)((pin_code & 0xFF000000) >> 24);
    }
    break;


    default:
    {
        ASSERT_ERR(0);
    }
    break;
    }

    // Send the message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}

static int gapc_bond_ind_handler(ke_msg_id_t const msgid,
                                 struct gapc_bond_ind const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id)
{
    printf("gapc_bond_ind_handler \n");
    switch (param->info)
    {
    case (GAPC_PAIRING_SUCCEED):
    {
        // Update the bonding status in the environment
        app_sec_env2.bonded = true;
        printf("GAPC_PAIRING_SUCCEED \n");

        if (nvds_put(ADV2_DEVICE_BOND_FLAG, ADV2_DEVICE_BOND_LENGTH,
                     (uint8_t *)&app_sec_env2.bonded) != NVDS_OK)
        {
            // An error has occurred during access to the NVDS
            ASSERT_ERR(0);
        }

        // Update the bonding status in the environment
    }
    break;

    case (GAPC_REPEATED_ATTEMPT):
    {
        printf("GAPC_REPEATED_ATTEMPT  2222 \n");
        app_collection1_disconnect();
    }
    break;

    case (GAPC_IRK_EXCH):
    {
        printf("GAPC_IRK_EXCH  2222 \n");
        // Store peer identity in NVDS
        if (nvds_put(NVDS_TAG_PEER_IRK, NVDS_LEN_PEER_IRK, (uint8_t *)&param->data.irk) != NVDS_OK)
        {
            ASSERT_ERR(0);
        }

    }
    break;

    case (GAPC_PAIRING_FAILED):
    {
        printf("pair failed \n");
        app_collection1_disconnect();        //liujian add
        //  app_sec_send_security_req(0);
    }
    break;

    // In Secure Connections we get BOND_IND with SMPC calculated LTK
    case (GAPC_LTK_EXCH) :
    {
        printf("GAPC_LTK_EXCH  2222 \n");
    }
    break;

    default:
    {
        ASSERT_ERR(0);
    }
    break;
    }

    return (KE_MSG_CONSUMED);
}

static int gapc_encrypt_req_ind_handler(ke_msg_id_t const msgid,
                                        struct gapc_encrypt_req_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{

    printf("gapc_encrypt_req_ind_handler bond = %d\n", app_sec_env2.bonded);


    // LTK value
    struct gapc_ltk ltk;
    // Length
    uint8_t length = NVDS_LEN_LTK;

    // Prepare the GAPC_ENCRYPT_CFM message
    struct gapc_encrypt_cfm *cfm = KE_MSG_ALLOC(GAPC_ENCRYPT_CFM,
                                   src_id, TASK_ADV_DEVICE1,
                                   gapc_encrypt_cfm);

    cfm->found    = false;
#if 1
    if (app_sec_env2.bonded)
    {
        // Retrieve the required informations from NVDS
        if (nvds_get(ADV2_NVDS_TAG_LTK, &length, (uint8_t *)&ltk) == NVDS_OK)
        {
            // Check if the provided EDIV and Rand Nb values match with the stored values
            if ((param->ediv == ltk.ediv) &&
                    !memcmp(&param->rand_nb.nb[0], &ltk.randnb.nb[0], sizeof(struct rand_nb)))
            {
                cfm->found    = true;
                cfm->key_size = 16;
                memcpy(&cfm->ltk, &ltk.ltk, sizeof(struct gap_sec_key));
            }
            /*
             * else we are bonded with another device, disconnect the link
             */
        }
        else
        {
            ASSERT_ERR(0);
        }

    }
    else   //liujian add
    {
#if (DIANBIAO_SPM_ENABLE == 1)
        if(app_get_ble_smp_level() != SMP_JUST_WORK)
        {
            printf("app_sec_send_security_req \n");
            // Ask for the peer device to either start encryption
            app_sec2_send_security_req(adv_device1_env.conidx);
        }
#endif
    }
#endif

    /*
     * else the peer device is not known, an error should trigger a new pairing procedure.
     */

    // Send the message
    ke_msg_send(cfm);

    return (KE_MSG_CONSUMED);
}


static int gapc_encrypt_ind_handler(ke_msg_id_t const msgid,
                                    struct gapc_encrypt_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // encryption/ re-encryption succeeded
    return (KE_MSG_CONSUMED);
}

static int app_sec_msg_dflt_handler(ke_msg_id_t const msgid,
                                    void *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Drop the message

    return (KE_MSG_CONSUMED);
}


/// Default State handlers definition
const struct ke_msg_handler app_sec2_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,  (ke_msg_func_t)app_sec_msg_dflt_handler},

    {GAPC_BOND_REQ_IND,       (ke_msg_func_t)gapc_bond_req_ind_handler},
    {GAPC_BOND_IND,           (ke_msg_func_t)gapc_bond_ind_handler},

    {GAPC_ENCRYPT_REQ_IND,    (ke_msg_func_t)gapc_encrypt_req_ind_handler},
    {GAPC_ENCRYPT_IND,        (ke_msg_func_t)gapc_encrypt_ind_handler},
};

const struct app_subtask_handlers app_sec2_handlers = {&app_sec2_msg_handler_list[0], ARRAY_LEN(app_sec2_msg_handler_list)};


#endif

