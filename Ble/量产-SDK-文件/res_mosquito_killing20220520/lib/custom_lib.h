/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	cobra Drivers

	File Name	:	custom_lib.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/5/22

	Description :   
       for OTA library
	Changed Log	:

		Liujian  2020/5/22			- Creation



*****************************************************************************/

#ifndef __CUSTOM_LIB_H__
#define __CUSTOM_LIB_H__
#include "cobra_system.h"
#include "ke_task.h"
#include "gapm_task.h"
#include "attm.h"
#include "prf.h"
#include "llm.h"
#include "prf_utils.h"
#include "aes.h"
#include "sch_alarm.h"

typedef void (*p_callback)(void);
typedef void (*p_callback_param1)(uint32_t error);
typedef const struct prf_task_cbs* (*p_callback_profile)(uint32_t task_id);
typedef const struct rwip_eif_api* (*p_rwip_eif_get)(uint8_t type);

typedef struct {
	//ATTM
	bool (*attm_uuid_comp)(uint8_t *uuid_a, uint8_t uuid_a_len, uint8_t *uuid_b, uint8_t uuid_b_len);
	
	bool (*attm_uuid16_comp)(uint8_t *uuid_a, uint8_t uuid_a_len, uint16_t uuid_b);
	
  void (*attm_convert_to128)(uint8_t *uuid128, uint8_t *uuid, uint8_t uuid_len);
	
	bool (*attm_is_bt16_uuid)(uint8_t *uuid);
	
	bool (*attm_is_bt32_uuid)(uint8_t *uuid);
	
	uint8_t (*attm_svc_create_db)(uint16_t *shdl, uint16_t uuid, uint8_t *cfg_flag, uint8_t max_nb_att,
                           uint8_t *att_tbl, ke_task_id_t const dest_id,
                           const struct attm_desc *att_db, uint8_t svc_perm);
													 
	uint8_t (*attm_svc_create_db_128)(uint16_t *shdl, const uint8_t* uuid, uint8_t *cfg_flag, uint8_t max_nb_att,
                               uint8_t *att_tbl, ke_task_id_t const dest_id,
                               const struct attm_desc_128 *att_db, uint8_t svc_perm);
															 
	uint8_t (*attmdb_svc_visibility_set)(uint16_t handle, bool hide);
	
	uint8_t (*attm_reserve_handle_range)(uint16_t* start_hdl, uint8_t nb_att);
															 
	uint8_t (*attm_att_set_value)(uint16_t handle, att_size_t length, att_size_t offset, uint8_t* value);
	
	uint8_t  (*attm_get_value)(uint16_t handle, att_size_t* length, uint8_t** value);

  uint8_t  (*attm_att_set_permission)(uint16_t handle, uint16_t perm, uint16_t ext_perm);
	
	uint8_t  (*attm_att_update_perm)(uint16_t handle, uint16_t access_mask, uint16_t perm);
	
	uint8_t  (*attm_svc_set_permission)(uint16_t handle, uint8_t perm);

	uint8_t  (*attm_svc_get_permission)(uint16_t handle, uint8_t* perm);
													 
  void     (*attmdb_destroy)(void);
															 
  void     (*attm_init)(uint8_t init_type);

//PRF
  void        (*prf_init)(uint8_t reset);
	
  uint8_t     (*prf_add_profile)(struct gapm_profile_task_add_cmd * params, ke_task_id_t *prf_task);

  void         (*prf_create)(uint8_t conidx);
	
  void         (*prf_cleanup)(uint8_t conidx, uint8_t reason);
	
	prf_env_t*   (*prf_env_get)(uint16_t prf_id);
	
	ke_task_id_t (*prf_src_task_get)(prf_env_t* env, uint8_t conidx);
	
	ke_task_id_t (*prf_dst_task_get)(prf_env_t* env, uint8_t conidx);
	
	ke_task_id_t  (*prf_get_id_from_task)(ke_msg_id_t task);
	
	ke_task_id_t  (*prf_get_task_from_id)(ke_msg_id_t id);
	
	uint8_t       (*prf_pack_date_time)(uint8_t *packed_date, const struct prf_date_time* date_time);
	
	uint8_t       (*prf_unpack_date_time)(uint8_t *packed_date, struct prf_date_time* date_time);
	
//nvds	
	uint8_t       (*nvds_init)(uint8_t *base, uint32_t len);
	
	uint8_t (*nvds_get)(uint8_t tag, nvds_tag_len_t * lengthPtr, uint8_t *buf);
	
	uint8_t (*nvds_del)(uint8_t tag);
	
	uint8_t (*nvds_lock)(uint8_t tag);
	
	uint8_t (*nvds_put)(uint8_t tag, nvds_tag_len_t length, uint8_t *buf);
	
	void (*iris_set_bdaddress)(struct bd_addr blue_address);
//aes
  void (*aes_k1)(const uint8_t* salt, const uint8_t* n, uint8_t n_len, const uint8_t* p, uint8_t p_len,
            aes_func_result_cb res_cb, uint32_t src_info);
	
	void  (*aes_k2)(const uint8_t* n, const uint8_t* p, uint8_t p_len, aes_func_result_cb res_cb, uint32_t src_info);
	
	void  (*aes_k3)(const uint8_t* n, aes_func_result_cb res_cb, uint32_t src_info);
	
	void  (*aes_k4)(const uint8_t* n, aes_func_result_cb res_cb, uint32_t src_info);
	
	void  (*aes_s1)(const uint8_t* message, uint8_t message_len, aes_func_result_cb res_cb, uint32_t src_info);
	
	void  (*aes_ccm)(const uint8_t* key, const uint8_t* nonce, const uint8_t* in_message,
             uint8_t* out_message, uint16_t message_len, uint8_t mic_len, bool cipher,
             const uint8_t* add_auth_data, uint8_t add_auth_data_len, aes_ccm_func_result_cb res_cb, uint32_t src_info);
						 
	void (*aes_cmac)(const uint8_t* key, const uint8_t* message, uint16_t message_len,
              aes_func_result_cb res_cb, uint32_t src_info);
	
//sch alarm
   uint8_t (*sch_alarm_clear)(struct sch_alarm_tag* elt);
	
//custom library	
	void  (*custom_appm_init)(p_callback p_func);
	
	void  (*custom_platform_reset)(p_callback_param1 p_func);
	
	void  (*custom_rdw_custom_define_profile)(p_callback_profile p_func);
	
	void  (*custom_rdw_rwip_eif_get)(p_rwip_eif_get p_func);
	
}ble_basic_function;

extern p_callback         p_appm_init;
extern p_callback_param1  p_platform_reset;
extern p_callback_profile p_custom_defined_profile;
extern p_rwip_eif_get     p_custom_rwip_eif_get;

void custom_appm_init(p_callback p_func);
void custom_platform_reset(p_callback_param1 p_func);
void custom_rdw_custom_define_profile(p_callback_profile p_func);
void custom_rdw_rwip_eif_get(p_rwip_eif_get p_func);
ble_basic_function * custom_lib_init(void);

void cobra_application_is_ok(void);
#endif
