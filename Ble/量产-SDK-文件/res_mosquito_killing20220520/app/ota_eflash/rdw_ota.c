// 参考 Dialog 的代码
//liujian
//2018/4/17
//ver1.0



#include "rwip_config.h"

#if (BLE_OTA_PROFILE)
#include "attm.h"
#include "rdw_ota.h"
#include "rdw_ota_task.h"
#include "co_utils.h"
#include "prf_utils.h"

#include "ke_mem.h"
#include "co_utils.h"
#include "gattc_task.h"
#include "gattm_task.h"


//static void  ota_set_dle_params_in_db(uint16_t suota_shdl);

// SIG has assigned a 16bit UUID for SUOTAR service (0xFEF5). However we use proprietary UUIDs for the characteristics.
#define ATT_SVC_SUOTAR                   {0xF5, 0xFE}
#define SUOTA_MEM_DEV_UUID               {0x34, 0xcc, 0x54, 0xb9, 0xf9, 0x56, 0xc6, 0x91, 0x21, 0x40, 0xa6, 0x41, 0xa8, 0xca, 0x82, 0x80}
#define SUOTA_GPIO_MAP_UUID              {0x51, 0x86, 0xf0, 0x5a, 0x34, 0x42, 0x04, 0x88, 0x5f, 0x4b, 0xc3, 0x5e, 0xf0, 0x49, 0x42, 0x72}
#define SUOTA_MEM_INFO_UUID              {0xd4, 0x4f, 0x33, 0xfb, 0x92, 0x7c, 0x22, 0xa0, 0xfe, 0x45, 0xa1, 0x47, 0x25, 0xdb, 0x53, 0x6c}
#define SUOTA_PATCH_LEN_UUID             {0x31, 0xda, 0x3f, 0x67, 0x5b, 0x85, 0x83, 0x91, 0xd8, 0x49, 0x0c, 0x00, 0xa3, 0xb9, 0x84, 0x9d}
#define SUOTA_PATCH_DATA_UUID            {0xb2, 0x9c, 0x7b, 0xb1, 0xd0, 0x57, 0x16, 0x91, 0xa1, 0x4c, 0x16, 0xd5, 0xe8, 0x71, 0x78, 0x45}
#define SUOTA_SERV_STATUS_UUID           {0x88, 0x5c, 0x06, 0x6a, 0xeb, 0xb3, 0x0a, 0x99, 0xf5, 0x46, 0x8c, 0x79, 0x94, 0xdf, 0x78, 0x5f}


#define SUOTA_VERSION_UUID               {0x3a, 0x91, 0x3b, 0xdb, 0xc8, 0xac, 0x1d, 0xa2, 0x1b, 0x40, 0xe5, 0x0d, 0xb5, 0xe8, 0xb4, 0x64}
#define SUOTA_PATCH_DATA_CHAR_SIZE_UUID  {0x3b, 0xfb, 0x67, 0x52, 0x87, 0x8f, 0x54, 0x84, 0x9c, 0x4d, 0xbe, 0x77, 0xdd, 0xdf, 0xc3, 0x42}
#define SUOTA_MTU_UUID                   {0x3c, 0xe2, 0xfc, 0x3d, 0x90, 0xc4, 0xaf, 0xa3, 0xbb, 0x43, 0x3d, 0x82, 0xea, 0x1e, 0xde, 0xb7}


//  //[]         =   {00, PERM(RD, ENABLE), PERM(RI, ENABLE), HTPT_MEAS_INTV_MAX_LEN},
//	
///*
// ****************************************************************************************
// */
//const struct attm_desc_128  ota_att_db[SUOTA_IDX_NB] =
//{

//		[SUOTA_IDX_SVC]                  =   { {0x00, 0x28},  PERM(RD, ENABLE), 0, 0},
//		
//		[SUOTA_IDX_PATCH_MEM_DEV_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_PATCH_MEM_DEV_VAL]         =   {SUOTA_MEM_DEV_UUID, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), sizeof(uint32_t)}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//		[SUOTA_IDX_GPIO_MAP_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_GPIO_MAP_VAL]         =   {SUOTA_GPIO_MAP_UUID, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), sizeof(uint32_t)}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//		[SUOTA_IDX_PATCH_MEM_INFO_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_PATCH_MEM_INFO_VAL]         =   { SUOTA_MEM_INFO_UUID, PERM(RD, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), sizeof(uint32_t)}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//		[SUOTA_IDX_PATCH_LEN_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_PATCH_LEN_VAL]         =   { SUOTA_PATCH_LEN_UUID, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), sizeof(uint16_t)}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//		[SUOTA_IDX_PATCH_DATA_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_PATCH_DATA_VAL]         =   {SUOTA_PATCH_DATA_UUID, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE)|PERM(WRITE_COMMAND, ENABLE), PERM(UUID_LEN, UUID_128),  sizeof(uint8_t) * OTA_PD_CHAR_SIZE}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//	    [SUOTA_IDX_PATCH_STATUS_CHAR]        =   {{0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//	    [SUOTA_IDX_PATCH_STATUS_VAL]         =   { SUOTA_SERV_STATUS_UUID,  PERM(RD, ENABLE)|PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), sizeof(uint8_t)}, //PERM(WRITE_COMMAND, ENABLE), 0, MAX_RX_UART } , //PERM(UUID_LEN, UUID_128), MAX_RX_UART},
//	    [SUOTA_IDX_PATCH_STATUS_NTF_CFG]     =   {{0x02, 0x29}, PERM(RD, ENABLE)|PERM(WRITE_REQ, ENABLE), 0, 0},
//		
//		//----------------
//		[SUOTA_IDX_VERSION_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_VERSION_VAL]         =   {SUOTA_VERSION_UUID, PERM(RD, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), 0}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//		[SUOTA_IDX_PATCH_DATA_CHAR_SIZE_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_PATCH_DATA_CHAR_SIZE_VAL]         =   {SUOTA_PATCH_DATA_CHAR_SIZE_UUID, PERM(RD, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), 0}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//	
//		[SUOTA_IDX_MTU_CHAR]        =   { {0x03, 0x28},   PERM(RD, ENABLE), 0, 0},
//		[SUOTA_IDX_MTU_VAL]         =   {SUOTA_MTU_UUID,  PERM(RD, ENABLE), PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE), 0}, //    PERM(NTF, ENABLE), PERM(UUID_LEN, UUID_128), 0},
//		
//};


#if OTA_VERSION >= OTA_VERSION_1_3
static void suotar_set_dle_params_in_db(uint16_t suota_shdl)
{
    uint8_t version = OTA_VERSION;
    uint16_t pd_char_size = SUOTA_PD_CHAR_SIZE;
    //Set SUOTA version into DB
    attm_att_set_value(suota_shdl + SUOTA_IDX_VERSION_VAL,
                         sizeof(uint8_t),
                         0,
                         &version);
    //Set PATCH_DATA_CHAR_SIZE value
    attm_att_set_value(suota_shdl + SUOTA_IDX_PATCH_DATA_CHAR_SIZE_VAL,
                         sizeof(uint16_t),
                         0,
                         (uint8_t*)&pd_char_size);
}
#endif

static uint8_t rwd_ota_init(struct prf_task_env* env, uint16_t* start_hdl, uint16_t app_task,
                            uint8_t sec_lvl, struct ota_db_cfg* params)
{

    struct ota_env_tag* suotar_env = NULL;
    struct gattm_svc_desc *svc_desc = NULL;
    uint8_t att_idx = 0;
    uint8_t status;

    // Allocate database descriptor
    svc_desc = (struct gattm_svc_desc *) ke_malloc(sizeof(struct gattm_svc_desc) + \
                                // No need for svc declaration, thus (SUOTA_IDX_NB - 1)
                                (SUOTA_IDX_NB - 1) * sizeof(struct gattm_att_desc), \
                                KE_MEM_NON_RETENTION);

    /*---------------------------------------------------*
     * SUOTAR Service Creation
     *---------------------------------------------------*/
    svc_desc->start_hdl = *start_hdl;
    svc_desc->task_id = env->task;
    svc_desc->perm = ((sec_lvl & PERM_MASK_SVC_AUTH) | (sec_lvl & PERM_MASK_SVC_EKS) | \
                        PERM(SVC_MI, DISABLE) | \
                        PERM_VAL(SVC_UUID_LEN, PERM_UUID_16));
    *(struct att_uuid_128 *)(&svc_desc->uuid) = (struct att_uuid_128) ATT_SVC_SUOTAR;

    // Char. decl. and value attribute for patch_mem_dev
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 ,.ext_perm =0};
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_MEM_DEV_UUID,
            .perm = PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE),
            .max_len = sizeof(uint32_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl. and value attribute for gpio_map
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_GPIO_MAP_UUID,
            .perm = PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE) ,
            .max_len = sizeof(uint32_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl. and value attribute for patch_mem info
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_MEM_INFO_UUID,
            .perm = PERM(RD, ENABLE) ,
            .max_len = sizeof(uint32_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl. and value attribute for patch_len
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_PATCH_LEN_UUID,
            .perm = PERM(RD, ENABLE) |  PERM(WRITE_REQ, ENABLE) ,
            .max_len = sizeof(uint16_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl. and value attribute for patch_data
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_PATCH_DATA_UUID,
            .perm = PERM(RD, ENABLE) |  PERM(WRITE_REQ, ENABLE) | \
                    PERM(WRITE_COMMAND, ENABLE) ,
            .max_len = sizeof(uint8_t) * SUOTA_PD_CHAR_SIZE,
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl., value attribute and CCC for patch_status
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_SERV_STATUS_UUID,
            .perm = PERM(RD, ENABLE) | PERM(NTF, ENABLE) ,
            .max_len = sizeof(uint8_t) ,
            .ext_perm =PERM(UUID_LEN, UUID_128)};
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DESC_CLIENT_CHAR_CFG),
            .perm = PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE),
            .max_len = 0 };

#if OTA_VERSION >= OTA_VERSION_1_3
    // Char. decl. and value attribute for version
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_VERSION_UUID,
            .perm = PERM(RD, ENABLE) ,
            .max_len = sizeof(uint8_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl. and value attribute for patch_data_char_size
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_PATCH_DATA_CHAR_SIZE_UUID,
            .perm = PERM(RD, ENABLE) ,
            .max_len = sizeof(uint16_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)};

    // Char. decl. and value attribute for mtu
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = ATT_UUID_16_ARR(ATT_DECL_CHARACTERISTIC), .perm = 0, .max_len = 0 };
    svc_desc->atts[att_idx++] = (struct gattm_att_desc) {
            .uuid = SUOTA_MTU_UUID,
            .perm = PERM(RD, ENABLE) ,
            .max_len =  sizeof(uint16_t),
            .ext_perm =PERM(UUID_LEN, UUID_128)|PERM(RI, ENABLE)};
#endif

    // Count all attributes and create database
    svc_desc->nb_att = att_idx;
    status = attmdb_add_service(svc_desc);

    if (status == ATT_ERR_NO_ERROR)
    {
        // Allocate SUOTAR required environment variable
        suotar_env = (struct ota_env_tag*) \
                        ke_malloc(sizeof(struct ota_env_tag), KE_MEM_ENV);
        memset(suotar_env, 0 , sizeof(struct ota_env_tag));

        suotar_env->shdl        = svc_desc->start_hdl;
        suotar_env->features          = params->features;
        memset(suotar_env->ntf_ind_cfg, 0 , sizeof(suotar_env->ntf_ind_cfg));
        suotar_env->prf_env.app_task  = app_task
            | (PERM_GET(sec_lvl, SVC_MI) ? PERM(PRF_MI, ENABLE) : PERM(PRF_MI, DISABLE));
        suotar_env->prf_env.prf_task  = env->task | PERM(PRF_MI, DISABLE);

        // Initialize environment variable
        env->id                     = TASK_ID_OTA;
        env->desc.idx_max           = OTA_IDX_MAX;
        env->desc.state             = suotar_env->state;
        env->env = (prf_env_t*) suotar_env;
        ota_task_init(&(env->desc));

        // Set start handle to first allocated service value
        *start_hdl = suotar_env->shdl;
        //printf("start_hdl:%d\n",suotar_env->shdl);
        // Service is ready, go into an Idle state
        ke_state_set(env->task, OTA_IDLE);

#if OTA_VERSION >= OTA_VERSION_1_3
        suotar_set_dle_params_in_db(suotar_env->shdl);
#endif
    }

    ke_free(svc_desc);
    return status;   
}



static void  rdw_ota_destroy(struct prf_task_env* env)
{
    struct ota_env_tag* ota_env = (struct ota_env_tag*) env->env;

    // free profile environment variables
    if(ota_env->operation != NULL)
    {
        ke_free(ota_env->operation);
    }

    env->env = NULL;
    ke_free(ota_env);
}

static void  rdw_ota_create(struct prf_task_env* env, uint8_t conidx)
{
    /* Clear configuration for this connection */
    struct  ota_env_tag* ota_env = (struct ota_env_tag*) env->env;
    ota_env->ntf_ind_cfg[conidx] = 0;
}

static void  rdw_ota_cleanup(struct prf_task_env* env, uint8_t conidx, uint8_t reason)
{
    /* Clear configuration for this connection */
    struct ota_env_tag* ota_env = (struct ota_env_tag*) env->env;
	
    uint8_t suota_patch_status = SUOTAR_RESERVED;
	
    ota_env->ntf_ind_cfg[conidx] = 0;
    // Reset patch status value
    attm_att_set_value(ota_env->shdl + SUOTA_IDX_PATCH_STATUS_VAL,
                                  sizeof(uint8_t), 0, (uint8_t *)(&suota_patch_status));
}


const struct prf_task_cbs  rdw_ota_itf =
{
    (prf_init_fnct) rwd_ota_init,
    rdw_ota_destroy,
    rdw_ota_create,
    rdw_ota_cleanup,
};

const struct prf_task_cbs* ota_prf_itf_get(void)
{
   return &rdw_ota_itf;
}



//void  ota_exe_operation(void)
//{
//    struct ota_env_tag* ota_env =(struct ota_env_tag*)prf_env_get(TASK_ID_OTA);

//    ASSERT_ERR(ota_env->operation != NULL);

//    bool finished = true;
//    while(ota_env->operation->cursor < BLE_CONNECTION_MAX)
//    {

//			   if(ota_env->ntf_ind_cfg[ota_env->operation->cursor]) 
//        {
//            // trigger the event
//            struct gattc_send_evt_cmd * evt = KE_MSG_ALLOC_DYN(GATTC_SEND_EVT_CMD,
//                    KE_BUILD_ID(TASK_GATTC , ota_env->operation->cursor), prf_src_task_get(&ota_env->prf_env, 0),
//                    gattc_send_evt_cmd, ota_env->operation->length);

//            evt->operation = (ota_env->ntf_ind_cfg[ota_env->operation->cursor] == PRF_CLI_START_NTF) ? GATTC_NOTIFY : GATTC_INDICATE;
//            evt->length    = ota_env->operation->length;
//            evt->handle    = ota_env->operation->handle;
//            memcpy(evt->value, ota_env->operation->data, evt->length);

//            ke_msg_send(evt);

//            finished = false;
//            ota_env->operation->cursor++;
//            break;
//        }
//        ota_env->operation->cursor++;
//    }

//    if(finished)
//    {
//        // free operation
//        ke_free(ota_env->operation);
//        ota_env->operation = NULL;
//        // go back to idle state
//        ke_state_set(prf_src_task_get(&(ota_env->prf_env), 0), OTA_IDLE);

//    }
//}

#endif 


