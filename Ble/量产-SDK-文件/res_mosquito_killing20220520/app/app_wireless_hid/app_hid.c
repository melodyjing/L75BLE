/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_hid.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	  2020/8/31

	Description :   HID …Ë±∏
	Changed Log	:

		Liujian  2020/8/31		- Creation
*****************************************************************************/
#include "rwip_config.h"            // SW configuration
#include <stdio.h>
#include <string.h>
#include "app.h"
#include "app_hid.h"
#include "app_task.h"               // Application task definitions
#include "app_hid.h"                // HID Application Module Definitions
#include "hogpd_task.h"             // HID Over GATT Profile Device Role Functions
#include "prf_types.h"              // Profile common types Definition
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "ke_timer.h"
#include "co_utils.h"
#include "app_button.h"
#include "rdw_gpio.h"
#include "cobra_pinmux.h"
#include "hid_key.h"

#if (BLE_APP_HID_ENABLE)

#define INPUT_REP_REF_ID                    0                                          /**< Id of reference to Keyboard Input Report. */
#define OUTPUT_REP_REF_ID                   0                                          /**< Id of reference to Keyboard Output Report. */
// Report Type values
#define BLE_HIDS_REP_TYPE_INPUT                 1
#define BLE_HIDS_REP_TYPE_OUTPUT                2
#define BLE_HIDS_REP_TYPE_FEATURE               3


#define APP_HID_KEY_REPORT_LEN         (8)
#define MODIFIER_KEY_POS                0                                          /**< Position of the modifier byte in the Input Report. */
#define SCAN_CODE_POS                   2                                          /**< The start position of the key scan code in a HID Report. */
#define SHIFT_KEY_CODE                  0x02                                       /**< Key code indicating the press of the Shift Key. */

// Length of the HID Mouse Report
#define APP_HID_MOUSE_REPORT_LEN       (6)
// Length of the Report Descriptor for an HID Mouse
#define APP_HID_MOUSE_REPORT_MAP_LEN   (sizeof(app_hid_mouse_report_map))


#define APP_HID_KEY_REPORT_MAP_LEN   (sizeof(app_hid_key_report_map))
	
// Duration before connection update procedure if no report received (mouse is silent) - 20s
#define APP_HID_SILENCE_DURATION_1     (2000)
// Duration before disconnection if no report is received after connection update - 60s
#define APP_HID_SILENCE_DURATION_2     (6000)

// Number of reports that can be sent
#define APP_HID_NB_SEND_REPORT         (10)

void app_hid_button_init(void);

// States of the Application HID Module
enum app_hid_states
{
    // Module is disabled (Service not added in DB)
    APP_HID_DISABLED,
    // Module is idle (Service added but profile not enabled)
    APP_HID_IDLE,
    // Module is enabled (Device is connected and the profile is enabled)
    APP_HID_ENABLED,
    // The application can send reports
    APP_HID_READY,
    // Waiting for a report
    APP_HID_WAIT_REP,

    APP_HID_STATE_MAX,
};

// HID Application Module Environment Structure
static struct app_hid_env_tag app_hid_env;

// HID Mouse Report Descriptor
static const uint8_t app_hid_mouse_report_map[] =
{
    /**
     *  --------------------------------------------------------------------------
     *  Bit      |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
     *  --------------------------------------------------------------------------
     *  Byte 0   |               Not Used                | Middle| Right | Left  |
     *  --------------------------------------------------------------------------
     *  Byte 1   |                     X Axis Relative Movement                  |
     *  --------------------------------------------------------------------------
     *  Byte 2   |                     Y Axis Relative Movement                  |
     *  --------------------------------------------------------------------------
     *  Byte 3   |                     Wheel Relative Movement                   |
     *  --------------------------------------------------------------------------
     */

    0x05, 0x01,     // USAGE PAGE (Generic Desktop)
    0x09, 0x02,     // USAGE (Mouse)
    0xA1, 0x01,     // COLLECTION (Application)
    0x85, 0x01,     // REPORT ID (1) - MANDATORY
    0x09, 0x01,     //     USAGE (Pointer)
    0xA1, 0x00,     //     COLLECTION (Physical)

    /**
     * ----------------------------------------------------------------------------
     * BUTTONS
     * ----------------------------------------------------------------------------
     */
    0x05, 0x09,     //         USAGE PAGE (Buttons)
    0x19, 0x01,     //         USAGE MINIMUM (1)
    0x29, 0x08,     //         USAGE MAXIMUM (8)
    0x15, 0x00,     //         LOGICAL MINIMUM (0)
    0x25, 0x01,     //         LOGICAL MAXIMUM (1)
    0x75, 0x01,     //         REPORT SIZE (1)
    0x95, 0x08,     //         REPORT COUNT (8)
    0x81, 0x02,     //         INPUT (Data, Variable, Absolute)

    /**
     * ----------------------------------------------------------------------------
     * MOVEMENT DATA
     * ----------------------------------------------------------------------------
     */
    0x05, 0x01,     //         USAGE PAGE (Generic Desktop)
    0x16, 0x08, 0xFF, //       LOGICAL MINIMUM (-255)
    0x26, 0xFF, 0x00, //       LOGICAL MAXIMUM (255)
    0x75, 0x10,     //         REPORT SIZE (16)
    0x95, 0x02,     //         REPORT COUNT (2)
    0x09, 0x30,     //         USAGE (X)
    0x09, 0x31,     //         USAGE (Y)
    0x81, 0x06,     //         INPUT (Data, Variable, Relative)

    0x15, 0x81,     //         LOGICAL MINIMUM (-127)
    0x25, 0x7F,     //         LOGICAL MAXIMUM (127)
    0x75, 0x08,     //         REPORT SIZE (8)
    0x95, 0x01,     //         REPORT COUNT (1)
    0x09, 0x38,     //         USAGE (Wheel)
    0x81, 0x06,     //         INPUT (Data, Variable, Relative)

    0xC0,           //     END COLLECTION (Physical)
    0xC0            // END COLLECTION (Application)
};

// HID Key Report Descriptor
static const uint8_t app_hid_key_report_map[] =
{
        0x05, 0x01,       // Usage Page (Generic Desktop)
        0x09, 0x06,       // Usage (Keyboard)
        0xA1, 0x01,       // Collection (Application)
	
	
        0x05, 0x07,       // Usage Page (Key Codes)
        0x19, 0xe0,       // Usage Minimum (224)
        0x29, 0xe7,       // Usage Maximum (231)
        0x15, 0x00,       // Logical Minimum (0)
        0x25, 0x01,       // Logical Maximum (1)
	
        0x75, 0x01,       // Report Size (1)
        0x95, 0x08,       // Report Count (8)
        0x81, 0x02,       // Input (Data, Variable, Absolute)

        0x95, 0x01,       // Report Count (1)
        0x75, 0x08,       // Report Size (8)
        0x81, 0x01,       // Input (Constant) reserved byte(1)

        0x95, 0x05,       // Report Count (5)
        0x75, 0x01,       // Report Size (1)
        0x05, 0x08,       // Usage Page (Page# for LEDs)
        0x19, 0x01,       // Usage Minimum (1)
        0x29, 0x05,       // Usage Maximum (5)
        0x91, 0x02,       // Output (Data, Variable, Absolute), Led report
				
        0x95, 0x01,       // Report Count (1)
        0x75, 0x03,       // Report Size (3)
        0x91, 0x01,       // Output (Data, Variable, Absolute), Led report padding

        0x95, 0x06,       // Report Count (6)
        0x75, 0x08,       // Report Size (8)
        0x15, 0x00,       // Logical Minimum (0)
        0x25, 0x65,       // Logical Maximum (101)
        0x05, 0x07,       // Usage Page (Key codes)
				
        0x19, 0x00,       // Usage Minimum (0)
        0x29, 0x65,       // Usage Maximum (101)
        0x81, 0x00,       // Input (Data, Array) Key array(6 bytes)

        0x09, 0x05,       // Usage (Vendor Defined)
        0x15, 0x00,       // Logical Minimum (0)
        0x26, 0xFF, 0x00, // Logical Maximum (255)
        0x75, 0x08,       // Report Count (2)
        0x95, 0x02,       // Report Size (8 bit)
        0xB1, 0x02,       // Feature (Data, Variable, Absolute)

        0xC0              // End Collection (Application)
};

void app_hid_mouse_init(void)
{
    // Length of the mouse timeout value
    uint8_t length = NVDS_LEN_MOUSE_TIMEOUT;

    // Reset the environment
    memset(&app_hid_env, 0, sizeof(app_hid_env));

    app_hid_env.nb_report = APP_HID_NB_SEND_REPORT;

    /*
     * Get the timeout value from the NVDS - This value is used each time a report is received
     * from the PS2 driver, store it in the environment.
     */
    if (nvds_get(NVDS_TAG_MOUSE_TIMEOUT, &length, (uint8_t *)&app_hid_env.timeout) != NVDS_OK)
    {
        app_hid_env.timeout = APP_HID_SILENCE_DURATION_1;
    }
}

/*
 ****************************************************************************************
 * @brief Function called when get GAP manager command complete events.
 *
 ****************************************************************************************
 */


uint32_t app_hid_key_state_is_ready(void)
{
	  if(app_hid_env.state == APP_HID_READY)
			return 1;
		else
			return 0;
}

void app_hid_mouse_add_hids(void)
{
    struct hogpd_db_cfg *db_cfg;
    // Prepare the HOGPD_CREATE_DB_REQ message
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                   TASK_GAPM, TASK_APP,
                                                   gapm_profile_task_add_cmd, sizeof(struct hogpd_db_cfg));

    // Fill message
    req->operation   = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl     = PERM(SVC_AUTH, UNAUTH);  //AUTH  NO_AUTH  UNAUTH
    req->prf_task_id = TASK_ID_HOGPD;
    req->app_task    = TASK_APP;
    req->start_hdl   = 0;

    // Set parameters
    db_cfg = (struct hogpd_db_cfg* ) req->param;

    // Only one HIDS instance is useful
    db_cfg->hids_nb = 1;

    // The device is a keyboard
    db_cfg->cfg[0].svc_features = HOGPD_CFG_KEYBOARD; //|HOGPD_CFG_PROTO_MODE|HOGPD_CFG_BOOT_KB_WR;

    // Only one Report Characteristic is requested
    db_cfg->cfg[0].report_nb    = 1;

    db_cfg->cfg[0].report_id[0] = 0;

    // The report is an input report
    db_cfg->cfg[0].report_char_cfg[0] = HOGPD_CFG_REPORT_IN; //HOGPD_CFG_REPORT_IN;

    // HID Information
    db_cfg->cfg[0].hid_info.bcdHID       = 0x0111;         // HID Version 1.11
    db_cfg->cfg[0].hid_info.bCountryCode = 0x00;
    db_cfg->cfg[0].hid_info.flags        = HIDS_REMOTE_WAKE_CAPABLE | HIDS_NORM_CONNECTABLE;

    // Send the message
    ke_msg_send(req);
}


/*
 ****************************************************************************************
 * @brief Function called when get connection complete event from the GAP
 *
 ****************************************************************************************
 */
void app_hid_mouse_enable_prf(uint8_t conidx)
{
    // Requested connection parameters
    struct gapc_conn_param conn_param;

    uint16_t ntf_cfg;

    // Store the connection handle
    app_hid_env.conidx = conidx;

    // Allocate the message
    struct hogpd_enable_req * req = KE_MSG_ALLOC(HOGPD_ENABLE_REQ,
                                                 prf_get_task_from_id(TASK_ID_HOGPD),
                                                 TASK_APP,
                                                 hogpd_enable_req);

    // Fill in the parameter structure
    req->conidx     = conidx;
    // Notifications are disabled
    ntf_cfg         = 0;

    // Go to Enabled state
    app_hid_env.state = APP_HID_ENABLED;

    #if (NVDS_SUPPORT)
    // If first connection with the peer device
    if (app_sec_get_bond_status())
    {
        // Length of the value read in NVDS
        uint8_t length   = NVDS_LEN_MOUSE_NTF_CFG;
        // Notification configuration

        if (nvds_get(NVDS_TAG_MOUSE_NTF_CFG, &length, (uint8_t *)&ntf_cfg) != NVDS_OK)
        {
            // If we are bonded this information should be present in the NVDS
            ASSERT_ERR(0);
        }

//        // CCC enable notification
//        if ((ntf_cfg & HOGPD_CFG_REPORT_NTF_EN ) != 0)
//        {
//            // The device is ready to send reports to the peer device
//            app_hid_env.state = APP_HID_READY;
//            app_hid_env.nb_report = APP_HID_NB_SEND_REPORT;

//            // Restart the mouse timeout timer if needed
//            if (app_hid_env.timeout != 0)
//            {
//                ke_timer_set(APP_HID_MOUSE_TIMEOUT_TIMER, TASK_APP, (uint16_t)(app_hid_env.timeout));
//                app_hid_env.timer_enabled = true;
//            }
//        }
    }
    #endif //(NVDS_SUPPORT)

    req->ntf_cfg[conidx] = ntf_cfg;

    /*
     * Requested connection interval: 10ms
     * Latency: 25
     * Supervision Timeout: 2s
     */
    conn_param.intv_min = 64;
    conn_param.intv_max = 64;
    conn_param.latency  = 5;
    conn_param.time_out = 200;

    host_appm_update_param(&conn_param, TASK_APP);

    // Send the message
    ke_msg_send(req);
		
		app_hid_button_init();
		
}


void app_hid_report_key(uint8_t *p_key_pattern, uint16_t pattern_len, uint16_t  pattern_offset)
{
	   uint16_t offset;
     uint16_t data_len;
	
	   uint8_t data[APP_HID_KEY_REPORT_LEN];
	
	   offset   = pattern_offset;
     data_len = pattern_len;
	
	   do
		 {
			  
	      memset(data, 0, sizeof(data));
			  // Copy the scan code.
        memcpy(data + SCAN_CODE_POS + offset, p_key_pattern + offset, data_len - offset);
			 
			  struct hogpd_report_upd_req * req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_UPD_REQ,
                                                                  prf_get_task_from_id(TASK_ID_HOGPD),
                                                                  TASK_APP,
                                                                  hogpd_report_upd_req,
                                                                  APP_HID_KEY_REPORT_LEN);

         req->conidx  = app_hid_env.conidx;
                //now fill report
         req->report.hid_idx  = app_hid_env.conidx;
         req->report.type     = HOGPD_REPORT;         //HOGPD_BOOT_MOUSE_INPUT_REPORT;//
         req->report.idx      = 0;                    //0 for boot reports and report map
         req->report.length   = APP_HID_KEY_REPORT_LEN;
         memcpy(&req->report.value[0], &data[0], APP_HID_KEY_REPORT_LEN);

         ke_msg_send(req);
			 
			   offset++;
				
	   }while (offset <= data_len);
	   
	
}



static int hogpd_ctnl_pt_ind_handler(ke_msg_id_t const msgid,
                                     struct hogpd_ctnl_pt_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{

    printf("1111111111111111111111 \n");
    if (param->conidx == app_hid_env.conidx)
    {
        //make use of param->hid_ctnl_pt
        struct hogpd_report_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_CFM,
                                                        prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                        TASK_APP,
                                                        hogpd_report_cfm,
                                                        0);

        req->conidx = param->conidx; //app_hid_env.conidx; //???
        // Operation requested (read/write @see enum hogpd_op)
        req->operation = HOGPD_OP_REPORT_WRITE;
        // Status of the request
        req->status = GAP_ERR_NO_ERROR;  //???
        // Report Info
        //req->report;
        // HIDS Instance
        req->report.hid_idx = app_hid_env.conidx; //???
        // type of report (@see enum hogpd_report_type)
        req->report.type = -1;//outside 
        // Report Length (uint8_t)
        req->report.length = 0;
        // Report Instance - 0 for boot reports and report map
        req->report.idx = 0;
        // Report data
        
        printf("11111111111111 \n");
        // Send the message
        ke_msg_send(req);
    }
    return (KE_MSG_CONSUMED);
}




static int hogpd_ntf_cfg_ind_handler(ke_msg_id_t const msgid,
                                     struct hogpd_ntf_cfg_ind const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
	  printf("hogpd_ntf_cfg_ind_handler ...... \n");
    if (app_hid_env.conidx == param->conidx)
    {
        if ((param->ntf_cfg[param->conidx] & HOGPD_CFG_REPORT_NTF_EN ) != 0)
        {
            // The device is ready to send reports to the peer device
            app_hid_env.state = APP_HID_READY;
        }
        else
        {
            // Come back to the Enabled state
            if (app_hid_env.state == APP_HID_READY)
            {
                app_hid_env.state = APP_HID_ENABLED;
            }
        }

        // Store the notification configuration in the database
        if (nvds_put(NVDS_TAG_MOUSE_NTF_CFG, NVDS_LEN_MOUSE_NTF_CFG,
                     (uint8_t *)&param->ntf_cfg[param->conidx]) != NVDS_OK)
        {
            // Should not happen
            ASSERT_ERR(0);
        }
    }

    return (KE_MSG_CONSUMED);
}

static int hogpd_report_req_ind_handler(ke_msg_id_t const msgid,
                                    struct hogpd_report_req_ind const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{

	  printf("param->report.type = %d \n", param->report.type);
	
    if ((param->operation == HOGPD_OP_REPORT_READ) && (param->report.type == HOGPD_REPORT_MAP))
    {
        struct hogpd_report_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_CFM,
                                                        src_id, //prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                        dest_id, //TASK_APP,
                                                        hogpd_report_cfm,
                                                        APP_HID_KEY_REPORT_MAP_LEN);

        req->conidx = app_hid_env.conidx; //???
        // Operation requested (read/write @see enum hogpd_op)
        req->operation = HOGPD_OP_REPORT_READ;
        // Status of the request
        req->status = GAP_ERR_NO_ERROR;  //???
        // Report Info
        //req->report;
        // HIDS Instance
        req->report.hid_idx = param->report.hid_idx; //  app_hid_env.conidx; ???
        // type of report (@see enum hogpd_report_type)
        req->report.type = HOGPD_REPORT_MAP;
        // Report Length (uint8_t)
       req->report.length = APP_HID_KEY_REPORT_MAP_LEN;
        // Report Instance - 0 for boot reports and report map
        req->report.idx = 0;
        // Report data
         memcpy(&req->report.value[0], &app_hid_key_report_map[0], APP_HID_KEY_REPORT_MAP_LEN);

        // Send the message
        ke_msg_send(req);
				
				printf("33333333333333333333 \n");
    }
    else
    {
        if (param->report.type == HOGPD_BOOT_MOUSE_INPUT_REPORT)
        { //request of boot mouse report
            struct hogpd_report_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_CFM,
                                                            prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                            TASK_APP,
                                                            hogpd_report_cfm,
                                                            0/*param->report.length*/);

            req->conidx = param->conidx; //app_hid_env.conidx; //???
            // Operation requested (read/write @see enum hogpd_op)
            req->operation = HOGPD_OP_REPORT_READ;
            // Status of the request
            req->status = GAP_ERR_NO_ERROR;  //???
            // HIDS Instance
            req->report.hid_idx = app_hid_env.conidx; //???
            // type of report (@see enum hogpd_report_type)
            req->report.type = param->report.type;//-1;//outside 
            // Report Length (uint8_t)
            req->report.length = 0; //param->report.length;
            // Report Instance - 0 for boot reports and report map
            req->report.idx = param->report.idx; //0;
            // Report data

            // Send the message
            ke_msg_send(req);
						printf("444444444444444444 \n");
        }
        else if (param->report.type == HOGPD_REPORT)
        { //request of mouse report
            struct hogpd_report_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_CFM,
                                                            prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                            TASK_APP,
                                                            hogpd_report_cfm,
                                                            8/*param->report.length*/);

            req->conidx = param->conidx; //app_hid_env.conidx; //???
            // Operation requested (read/write @see enum hogpd_op)
            req->operation = HOGPD_OP_REPORT_READ;
            // Status of the request
            req->status = GAP_ERR_NO_ERROR;  //???
            // Report Info
            //req->report;
            // HIDS Instance
            req->report.hid_idx = app_hid_env.conidx; //???
            // type of report (@see enum hogpd_report_type)
            req->report.type = param->report.type;//-1;//outside 
            // Report Length (uint8_t)
            req->report.length = 8; //param->report.length;
            // Report Instance - 0 for boot reports and report map
            req->report.idx = param->report.idx; //0;
            // Report data
            memset(&req->report.value[0], 0, 8); //???
            req->report.value[0] = param->report.hid_idx;    // HIDS Instance
            req->report.value[1] = param->report.type;    // type of report (@see enum hogpd_report_type)
            req->report.value[2] = param->report.length;    // Report Length (uint8_t)
            req->report.value[3] = param->report.idx;    // Report Instance - 0 for boot reports and report map

            // Send the message
            ke_msg_send(req);
						printf("555555555555555555 \n");
        }
        else
        {
            struct hogpd_report_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_REPORT_CFM,
                                                            prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                            TASK_APP,
                                                            hogpd_report_cfm,
                                                            8/*param->report.length*/);

            req->conidx = param->conidx; //app_hid_env.conidx; //???
            // Operation requested (read/write @see enum hogpd_op)
            req->operation = HOGPD_OP_REPORT_READ;
            // Status of the request
            req->status = GAP_ERR_NO_ERROR;  //???
            // Report Info
            //req->report;
            // HIDS Instance
            req->report.hid_idx = app_hid_env.conidx; //???
            // type of report (@see enum hogpd_report_type)
            req->report.type = param->report.type;//-1;//outside 
            // Report Length (uint8_t)
            req->report.length = 8; //param->report.length;
            // Report Instance - 0 for boot reports and report map
            req->report.idx = param->report.idx; //0;
            // Report data
            memset(&req->report.value[0], 0, 8); //???
            req->report.value[0] = param->report.hid_idx;    // HIDS Instance
            req->report.value[1] = param->report.type;    // type of report (@see enum hogpd_report_type)
            req->report.value[2] = param->report.length;    // Report Length (uint8_t)
            req->report.value[3] = param->report.idx;    // Report Instance - 0 for boot reports and report map

            // Send the message
            ke_msg_send(req);
						printf("66666666666666666 \n");
        }
    }

    return (KE_MSG_CONSUMED);
}

static int hogpd_proto_mode_req_ind_handler(ke_msg_id_t const msgid,
                                        struct hogpd_proto_mode_req_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{

	  printf("hogpd_proto_mode_req_ind_handler .......\n");
    if ((param->conidx == app_hid_env.conidx) && (param->operation == HOGPD_OP_PROT_UPDATE))
    {

        //make use of param->proto_mode
        struct hogpd_proto_mode_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_PROTO_MODE_CFM,
                                                        prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                        TASK_APP,
                                                        hogpd_proto_mode_cfm,
                                                        0);
        // Connection Index
        req->conidx = app_hid_env.conidx; 
        // Status of the request
        req->status = GAP_ERR_NO_ERROR;
        // HIDS Instance
        req->hid_idx = app_hid_env.conidx;
        // New Protocol Mode Characteristic Value
        req->proto_mode = param->proto_mode;
        

        // Send the message
        ke_msg_send(req);
    }
    else
    {
        struct hogpd_proto_mode_cfm *req = KE_MSG_ALLOC_DYN(HOGPD_PROTO_MODE_CFM,
                                                        prf_get_task_from_id(TASK_ID_HOGPD),/* src_id */
                                                        TASK_APP,
                                                        hogpd_proto_mode_cfm,
                                                        0);
        // Status of the request
        req->status = ATT_ERR_APP_ERROR;

        // Connection Index
        req->conidx = app_hid_env.conidx;
        // HIDS Instance
        req->hid_idx = app_hid_env.conidx;
        // New Protocol Mode Characteristic Value
        req->proto_mode = param->proto_mode;
        
        // Send the message
        ke_msg_send(req);
    }
    return (KE_MSG_CONSUMED);
}


static int hogpd_report_upd_handler(ke_msg_id_t const msgid,
                                   struct hogpd_report_upd_rsp const *param,
                                   ke_task_id_t const dest_id,
                                   ke_task_id_t const src_id)
{
	  printf("send ok .... \n");
    if (app_hid_env.conidx == param->conidx)
    {
        if (GAP_ERR_NO_ERROR == param->status)
        {
            if (app_hid_env.nb_report < APP_HID_NB_SEND_REPORT)
            {
                app_hid_env.nb_report++;
            }
        }
        else
        {
					  printf("error ...... \n");
            // we get this message if error occur while sending report
            // most likely - disconnect
            // Go back to the ready state
            app_hid_env.state = APP_HID_IDLE;
            // change mode
            // restart adv
            // Try to restart advertising if needed
            appm_update_adv_state(true);

            //report was not success - need to restart???
        }
    }
    return (KE_MSG_CONSUMED);
}

static int hogpd_enable_rsp_handler(ke_msg_id_t const msgid,
                                     struct hogpd_enable_rsp const *param,
                                     ke_task_id_t const dest_id,
                                     ke_task_id_t const src_id)
{
	  uint16_t ntf_cfg;
	
    #if (NVDS_SUPPORT)
    // If first connection with the peer device
    if (app_sec_get_bond_status())
    {
        // Length of the value read in NVDS
        uint8_t length   = NVDS_LEN_MOUSE_NTF_CFG;
        // Notification configuration

        if (nvds_get(NVDS_TAG_MOUSE_NTF_CFG, &length, (uint8_t *)&ntf_cfg) != NVDS_OK)
        {
            // If we are bonded this information should be present in the NVDS
            ASSERT_ERR(0);
        }

        // CCC enable notification
        if ((ntf_cfg & HOGPD_CFG_REPORT_NTF_EN ) != 0)
        {
            // The device is ready to send reports to the peer device
            app_hid_env.state = APP_HID_READY;
            app_hid_env.nb_report = APP_HID_NB_SEND_REPORT;
        }
    }
    #endif //(NVDS_SUPPORT)

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Function called when the APP_HID_MOUSE_TIMEOUT_TIMER expires.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_hid_mouse_timeout_timer_handler(ke_msg_id_t const msgid,
                                               void const *param,
                                               ke_task_id_t const dest_id,
                                               ke_task_id_t const src_id)
{
    app_hid_env.timer_enabled = false;

	  printf("app_hid_env.state = %d \n", app_hid_env.state);
    if (app_hid_env.state == APP_HID_READY)
    {
        // Requested connection parameters
        struct gapc_conn_param conn_param;
        // Length
        uint8_t length = NVDS_LEN_MOUSE_TIMEOUT;
        // Timer value
        uint16_t timer_val;

        /*
         * Request an update of the connection parameters
         * Requested connection interval: 10ms
         * Latency: 200
         * Supervision Timeout: 5s
         */
        conn_param.intv_min = 8;
        conn_param.intv_max = 8;
        conn_param.latency  = 200;
        conn_param.time_out = 500;

        host_appm_update_param(&conn_param, TASK_APP);

        // Go to the Wait for Report state
        app_hid_env.state = APP_HID_WAIT_REP;

        // Get the timer value from the NVDS
        if (nvds_get(NVDS_TAG_MOUSE_ENERGY_SAFE, &length, (uint8_t *)&timer_val) != NVDS_OK)
        {
            timer_val = APP_HID_SILENCE_DURATION_2;
        }

        // Relaunch the timer
        ke_timer_set(APP_HID_MOUSE_TIMEOUT_TIMER, TASK_APP, timer_val);
        app_hid_env.timer_enabled = true;
    }
    else if (app_hid_env.state == APP_HID_WAIT_REP)
    {
      // Disconnect the link with the device
        appm_disconnect();


        // Go back to the ready state
        app_hid_env.state = APP_HID_IDLE;
    }

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_hid_msg_dflt_handler(ke_msg_id_t const msgid,
                                    void const *param,
                                    ke_task_id_t const dest_id,
                                    ke_task_id_t const src_id)
{
    // Drop the message

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Set the value of the Report Map Characteristic in the database
 ****************************************************************************************
 */
void app_hid_set_report_map(void);

/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

// Default State handlers definition
const struct ke_msg_handler app_hid_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,        (ke_msg_func_t)app_hid_msg_dflt_handler},

    {HOGPD_ENABLE_RSP,              (ke_msg_func_t)hogpd_enable_rsp_handler},
    {HOGPD_NTF_CFG_IND,             (ke_msg_func_t)hogpd_ntf_cfg_ind_handler},
    {HOGPD_REPORT_REQ_IND,          (ke_msg_func_t)hogpd_report_req_ind_handler},
    {HOGPD_PROTO_MODE_REQ_IND,      (ke_msg_func_t)hogpd_proto_mode_req_ind_handler},
    {HOGPD_CTNL_PT_IND,             (ke_msg_func_t)hogpd_ctnl_pt_ind_handler},
    {HOGPD_REPORT_UPD_RSP,          (ke_msg_func_t)hogpd_report_upd_handler},

    {APP_HID_MOUSE_TIMEOUT_TIMER,   (ke_msg_func_t)app_hid_mouse_timeout_timer_handler},
};

const struct app_subtask_handlers app_hid_handlers = APP_HANDLERS(app_hid);

#endif
