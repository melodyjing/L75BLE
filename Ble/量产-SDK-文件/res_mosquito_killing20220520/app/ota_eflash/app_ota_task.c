//liujian
//2018/4/17
//ver1.0


#include "rwip_config.h"     // SW configuration

#if (BLE_APP_OTA)

#include <stdio.h>                  


#include "app_ota_task.h"                  
#include "app.h"                     // Application Definitions
#include "app_task.h"                // application task definitions
#include "rdw_ota_task.h"             
#include "rdw_ota.h"             
#include "co_bt.h"
#include "prf_types.h"
#include "prf_utils.h"
#include "arch.h"                    // Platform Definitions
#include "prf.h"
#include "ke_mem.h"                    // Platform Definitions
#include "cobra_cache_ctrl.h"
#include "cobra_system.h"
#include "co_utils.h"
#include "co_math.h"
#include "ke_timer.h"
#include "cobra4C_eflash_ctrl.h"

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */
app_suota_state suota_state 									__attribute__((section("retention_area"),zero_init));
uint8_t suota_all_pd[SUOTA_OVERALL_PD_SIZE] 	__attribute__((section("retention_area"),zero_init));

#define  OTA_FLASH_SIZE		      0x50000     //EFLASH SIZE  ???
#define  OTA_APP_SIZE			      0x10000     //EFLASH APP  ??? ? 64K
#define  OTA_APP_RUN_ADDRESS	  (0x0030000 - 0x6000)  //??????????? 

#define FLASH_PAGE_SIZE    							4096
#define M0_ADDRESS_PROGRAM_BASE					OTA_APP_RUN_ADDRESS          // 写到 EFLASH 地址 0x30000  地址处, 最大64K
#define SPI_ADDRESS_PROGRAM_BASE				OTA_APP_RUN_ADDRESS
#define EFLASH_ADDRESS_PROGRAM_BASE			OTA_APP_RUN_ADDRESS


static void rdw_flush_write(void);

void rdw_ota_free_memory(void);

typedef void (*profile_callback) (void*, uint8_t);

void app_ota_enable_prf(uint8_t conidx);

void suotar_send_status_update_req( uint8_t status );

void app_suotar_img_hdlr(void);

int app_read_image_headers(uint8_t image_bank, uint8_t* data, uint32_t data_len);


static unsigned char  * p_mem_buffer;
/*
 * GLOBAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

void app_suotar_start(void)
{

    if( SUOTAR_IS_FOR_IMAGE(suota_state.mem_dev))
    {
        suotar_send_status_update_req((uint8_t) SUOTAR_IMG_STARTED);
    }
    else
    {
        suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
    }
		
		if(suota_state.mem_dev == SUOTAR_IMG_I2C_EEPROM)
		{
			  //write to M0
				suota_state.rdw_write_base_address = M0_ADDRESS_PROGRAM_BASE;
		}
		
		else if(suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH)
		{
			 //write to M3
			 suota_state.rdw_write_base_address = SPI_ADDRESS_PROGRAM_BASE;
		}
		else if(suota_state.mem_dev == SUOTAR_IMG_EFLASH)
		{
			 //write to M3
			 suota_state.rdw_write_base_address = EFLASH_ADDRESS_PROGRAM_BASE;
		}

		p_mem_buffer = ke_malloc(FLASH_PAGE_SIZE, KE_MEM_KE_MSG);
		
		
		if(p_mem_buffer)
		{
	
		printf("malloc ok ..... \n");
		
		}
		else
		{
	
			printf("malloc fault ..... \n");
	
		}
		
    suota_state.suota_pd_idx = 0;
    suota_state.suota_block_idx = 0;
    suota_state.suota_img_idx = 0;
    suota_state.new_patch_len = 0;
    suota_state.crc_calc = 0;
		
	  suota_state.rdw_total_size = 0;
		
		suota_state.rdw_write_index = 0;
		suota_state.rdw_write_addr = 0;

		cobra_cache_normal_mode_enable(0);
		
}

void app_suotar_stop(void)
{

    if( suota_state.mem_dev == SUOTAR_MEM_I2C_EEPROM ){

    }


    if( suota_state.mem_dev == SUOTAR_MEM_SPI_FLASH ) {

    }

		cobra_cache_invalidate_the_cache();
    // Set memory device to invalid type so that service will not
    // start until the memory device is explicitly set upon service start
    suota_state.mem_dev = SUOTAR_MEM_INVAL_DEV;
		
}

void app_suotar_reset(void)
{
		
    suota_state.suota_pd_idx = 0;
    suota_state.suota_block_idx = 0;
    suota_state.new_patch_len = 0;
	  suota_state.rdw_total_size = 0;
		
//		suota_state.rdw_remain_size = 0;
		suota_state.rdw_write_index = 0;
		suota_state.rdw_write_addr = 0;

}

/**
 ****************************************************************************************
 * @brief This function is called when the entire image has been received successfully
 *        to set the valid flag field on the image header and make the image valid for
 *        the bootloader.
 *
 * @param[in]   void
 *
 * @return      SUOTA Status values
 *
 ****************************************************************************************
 */

uint8_t app_set_image_valid_flag(void)
{
    int8_t ret = SUOTAR_EXT_MEM_WRITE_ERR;

    if( suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH )
    {
			printf("app_set_image_valid_flag  SUOTAR_IMG_SPI_FLASH\n");
    }
    else
    {
			printf("app_set_image_valid_flag  SUOTAR_IMG_I2C_FLASH\n");
    }
		
	  ret = SUOTAR_CMP_OK;
		
    return ret;
}


/**
 ****************************************************************************************
 * @brief Reads memory device and writes memory info.
 *
 * @param[in]   mem_dev: MSbyte holds the Memory device type, rest is the base address.
 * @param[in]   mem_info: 16MSbits hold number of patches, 16LSbits hold overall mem len.
 *
 * @return      void
 ****************************************************************************************
 */
void app_suotar_read_mem(uint32_t mem_dev, uint32_t* mem_info)
{

    uint8_t mem_dev_cmd = (uint8_t) ((mem_dev & SUOTAR_READ_MEM_DEV_TYPE) >> 24); //存储设备的类型

    // If valid memory device, store memory device type and base address
    if( mem_dev_cmd < SUOTAR_MEM_INVAL_DEV )
    {
        suota_state.mem_dev = (uint8_t) ((mem_dev & SUOTAR_READ_MEM_DEV_TYPE) >> 24);
        suota_state.mem_base_add = mem_dev & SUOTAR_READ_MEM_BASE_ADD;

        // Read memory device and set memory info only for internal RAM deviced. External devices need
        // GPIO configuration first before the memory info can be returned
        if( (suota_state.mem_dev == SUOTAR_MEM_INT_SYSRAM) ||
            (suota_state.mem_dev == SUOTAR_MEM_INT_RETRAM) )
        {
            // SYSRAM patch base address is the addres of the last index of the suota_all_pd array
            suota_state.mem_base_add = (uint32_t)(suota_all_pd + (SUOTA_OVERALL_PD_SIZE - sizeof(uint32_t)));
        }

        // Store requested image bank
        if( (suota_state.mem_dev == SUOTAR_IMG_I2C_EEPROM) ||
            (suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH) || (suota_state.mem_dev == SUOTAR_IMG_EFLASH) )
        {
            if(suota_state.mem_base_add <= 3)
            {
                suota_state.suota_image_bank = suota_state.mem_base_add;
            }
            else
            {
                // Invalid image bank
                suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_IMG_BANK);
                return;
            }
        }
    }

    switch( mem_dev_cmd )
    {
        case SUOTAR_IMG_I2C_EEPROM:
        case SUOTAR_IMG_SPI_FLASH:
        case SUOTAR_IMG_EFLASH:
            // Initialise SUOTA Receiver app
            app_suotar_start();
            break;

        case SUOTAR_IMG_INT_SYSRAM:
        case SUOTAR_IMG_INT_RETRAM:
            // No support for SUOTA for SYSRAM/RETRAM
            suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
            break;

        case SUOTAR_MEM_SERVICE_EXIT:
            app_suotar_stop();
            app_suotar_reset();
            // Initiator requested to exit service. Send notification to initiator
            suotar_send_status_update_req((uint8_t) SUOTAR_SRV_EXIT);
            break;

        case SUOTAR_IMG_END:
        {
            uint8_t ret;
            // Initiator requested to exit service. Calculate CRC if succesfull, send notification to initiator
					 // rdw_flush_write();
            if(suota_state.crc_calc != 0 )
            {
                suotar_send_status_update_req((uint8_t) SUOTAR_CRC_ERR);
            }
            else
            {
                ret = app_set_image_valid_flag();
							  /*
							    把0x30000 开始的地址的数据做和校验, 放到 0x40000 - 4k 的位置, 也就是 256K byte 的最后地址
							  */
							  cobra_cache_invalidate_the_cache();
							  {
										unsigned int i;
									  unsigned int checksum = 0;
									  unsigned char *p_byte = (unsigned char *)OTA_APP_RUN_ADDRESS;  //应用程序的开始位置为 0x30000
									  for(i = 0; i < suota_state.suota_image_len; i++)
									  {
												checksum += *p_byte++; 
									  }
										cobra_eflash_page_erase(OTA_FLASH_SIZE - FLASH_PAGE_SIZE);
										cobra_flash_write(OTA_FLASH_SIZE - FLASH_PAGE_SIZE, 4,  (unsigned char *)(&suota_state.suota_image_len));
										cobra_flash_write(OTA_FLASH_SIZE - FLASH_PAGE_SIZE + 4, 4,  (unsigned char *)(&checksum));	
								}
                suotar_send_status_update_req((uint8_t) ret);
            }
            app_suotar_stop();
            app_suotar_reset();
            break;
        }

        case SUOTAR_REBOOT:
        {
						//GLOBAL_INT_DISABLE();
					  cobra_cache_normal_mode_enable(1);
					
						if(app_env.conidx == GAP_INVALID_CONIDX)
						{
							 GLOBAL_INT_DISABLE();
							 ota_platform_reset(RESET_NO_ERROR);
						}
						else
						{
							 appm_disconnect();
							 suota_state.reboot_requested = 1;
						}

						
					
//            ef_src_addr = (uint32_t *)0x810040;
//            for(int i = 0;i< 0x10000/FLASH_PAGE_SIZE;i++)
//            {
//								for(int j = 0 ;j< 4096;)
//                {
//                    buf = *ef_src_addr;
//										p_mem_buffer[j++]= buf;
//										p_mem_buffer[j++]= buf>>8;
//										p_mem_buffer[j++]= buf>>16;
//										p_mem_buffer[j++]= buf>>24;
//                    ef_src_addr++;
//                }

//                cobra_eflash_page_erase(ef_dest_addr);
//								
//                cobra_flash_write(ef_dest_addr,FLASH_PAGE_SIZE,(uint8_t *)p_mem_buffer);
//								
//                ef_dest_addr +=FLASH_PAGE_SIZE;
//            }

//						platform_reset_inline(RESET_NO_ERROR);
			

            // It's up to user's code do a reset and boot the new image when the initiator
            // disconnects. User's code in the 'user_app_on_disconnect' callback can 
            // test the 'suota_state.reboot_requested' flag and act accordingly.
            break;
        }

        default:
            suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
            suota_state.mem_dev = SUOTAR_MEM_INVAL_DEV;
            *mem_info = 0;
            break;
    }
}


void app_ota_init(void)
{

}

void rdw_ota_free_memory(void)
{
		if(p_mem_buffer)
		{
			ke_free(p_mem_buffer);
			p_mem_buffer = NULL;
		}
	
}

void app_ota_add_otas(void)
{
    struct ota_db_cfg* db_cfg;
 
    struct gapm_profile_task_add_cmd *req = KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                                                  TASK_GAPM, TASK_APP,
                                                  gapm_profile_task_add_cmd, sizeof(struct ota_db_cfg));
    // Fill message
    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = PERM(SVC_AUTH, NO_AUTH)|PERM(SVC_UUID_LEN, UUID_16);  //NO_AUTH);    UNAUTH
    req->prf_task_id = TASK_ID_OTA;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    // Set parameters
    db_cfg = (struct ota_db_cfg* ) req->param;
    // All features are supported
    db_cfg->features = 0x00;     //no use

    // Send the message
    ke_msg_send(req);
		
		printf("app_ota_add_otas ..... \n");
		

}



void app_ota_enable_prf(uint8_t conidx)
{
    // Allocate the message
    struct ota_enable_req * req = KE_MSG_ALLOC(OTA_ENABLE_REQ,
                                                prf_get_task_from_id(TASK_ID_OTA),
                                                TASK_APP,
                                                ota_enable_req);
    // Fill in the parameter structure
    req->conidx        = conidx;
    // NTF/IND initial status - Disabled
    req->ntf_ind_cfg   = PRF_CLI_STOP_NTFIND;

    // Send the message
    ke_msg_send(req);

}




static int app_ota_msg_handler(ke_msg_id_t const msgid,
                              void const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id)
{	
    return (KE_MSG_CONSUMED);
}
							  
void suotar_send_mem_info_update_req(uint32_t mem_info)
{
    // Inform SUOTAR task.
    struct ota_patch_mem_info_update_req *req = KE_MSG_ALLOC(OTA_PATCH_MEM_INFO_UPDATE_REQ,
                      prf_get_task_from_id(TASK_ID_OTA), TASK_APP,
                      ota_patch_mem_info_update_req);

    req->mem_info = mem_info;

    // Send the message
    ke_msg_send(req);
			
	  printf("suotar_send_mem_info_update_req .... \n");
	
}

/**
 ****************************************************************************************
 * @brief Updates SUOTAR status characteristic.
 *
 * @param[in]   status: SUOTAR application status.
 *
 * @return      void
 ****************************************************************************************
 */

void suotar_send_status_update_req( uint8_t status )
{
    // Inform SUOTAR task.
    struct ota_status_update_req *req = KE_MSG_ALLOC(OTA_STATUS_UPDATE_REQ,
                      prf_get_task_from_id(TASK_ID_OTA), TASK_APP,
                      ota_status_update_req);

    req->status = status;

    // Send the message
    ke_msg_send(req);
			
	  printf("suotar_send_status_update_req .... \n");

}



							  
/**
 ****************************************************************************************
 * @brief Handles memory device indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ota_patch_mem_dev_ind_handler(ke_msg_id_t const msgid,
                                            struct  ota_patch_mem_dev_ind const *param,
                                            ke_task_id_t const dest_id,
                                            ke_task_id_t const src_id)
{
    uint32_t mem_info = 0;

    if (param->char_code)
    {
         app_suotar_read_mem(param->mem_dev, &mem_info);

        // write memory info to db
        // Inform SUOTAR task. Allocate the  memory info updadte indication
        suotar_send_mem_info_update_req(mem_info);
    }
				
    printf("ota_patch_mem_dev_ind_handler ....:%x \n",mem_info);

    return (KE_MSG_CONSUMED);
}

											
											
/**
 ****************************************************************************************
 * @brief Handles GPIO mapping indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int  ota_gpio_map_ind_handler(ke_msg_id_t const msgid,
                                       struct ota_gpio_map_ind const *param,
                                       ke_task_id_t const dest_id,
                                       ke_task_id_t const src_id)
{
    uint32_t mem_info;

    if (param->char_code)
    {
         suota_state.gpio_map = (uint32_t)param->gpio_map;

        // Setting GPIO map is only valid for external devices
        if( (suota_state.mem_dev == SUOTAR_MEM_I2C_EEPROM) ||
            (suota_state.mem_dev == SUOTAR_MEM_SPI_FLASH) ||(suota_state.mem_dev == SUOTAR_MEM_EFLASH))
        {
            mem_info = 0;
            suotar_send_mem_info_update_req(mem_info);
        }
        else if( (suota_state.mem_dev == SUOTAR_IMG_I2C_EEPROM)  ||
                 (suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH )   ||(suota_state.mem_dev == SUOTAR_IMG_EFLASH)   )
        {
            // return ACTIVE_IMAGE info

        }
        else
        {
            suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
        }
    }
			
		printf("ota_gpio_map_ind_handler .... \n");

    return (KE_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles Patch Length indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ota_patch_len_ind_handler(ke_msg_id_t const msgid,
                                        struct ota_patch_len_ind const *param,
                                        ke_task_id_t const dest_id,
                                        ke_task_id_t const src_id)
{
    if (param->char_code)
    {   // Patchs are send in words (usually 4 bytes). So overall length in bytes should be multiple of 4.
        if( param->len == 0 )
        {
            // Inavid PATCH_LEN char value
            suotar_send_status_update_req((uint8_t) SUOTAR_PATCH_LEN_ERR);
        }
        else
        {
            // Check if the memory device has been set correctly
            if( SUOTAR_IS_VALID_MEM_DEV (suota_state.mem_dev) )
            {
                if( SUOTAR_IS_FOR_IMAGE( suota_state.mem_dev ) )
                {
                    // SUOTAR is configured to recive image, check SYSRAM buffer size to determine if the block can be stored
                    if( param->len > SUOTA_OVERALL_PD_SIZE )
                    {
                        // Not enough memory for new patch. Update status.
                        suotar_send_status_update_req((uint8_t) SUOTAR_INT_MEM_ERR);
                    }
                    else
                    {
                        suota_state.new_patch_len = param->len;
                    }
                }
                else
                {
                    suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
                }
            }
        }
    }

 	printf("ota_patch_len_ind_handler .... \n");

   return (KE_MSG_CONSUMED);
}



static void rdw_write_flash(void)
{
		 uint32_t length;
	   printf("rdw_write_flash ............................... \n");
		if(suota_state.rdw_write_index + suota_state.new_patch_len < FLASH_PAGE_SIZE)
		{
		
        printf("rdw_write_flash:%d\n",suota_state.new_patch_len);
		
				memcpy(&p_mem_buffer[suota_state.rdw_write_index], suota_all_pd, suota_state.new_patch_len);
			
				suota_state.rdw_write_index += suota_state.new_patch_len;
			
			  if((suota_state.new_patch_len == SUOTA_PD_CHAR_SIZE && suota_state.rdw_write_addr<suota_state.suota_image_len) || (suota_state.rdw_write_addr == 0))		//240 = max_mtu
				{
					   return;
				}
				else
				{
					GLOBAL_INT_DISABLE();
				  //cobra_eflash_page_erase(suota_state.rdw_write_base_address + suota_state.rdw_write_addr);
					cobra_flash_write(suota_state.rdw_write_base_address + suota_state.rdw_write_addr, suota_state.rdw_write_index, (uint8_t *)p_mem_buffer);
					suota_state.rdw_write_addr += suota_state.rdw_write_index;
					suota_state.rdw_write_index = 0;
					GLOBAL_INT_RESTORE();
				}		
		}

		else
		{
				GLOBAL_INT_DISABLE();
				
				printf("rdw_write_flash begain 1\n");
	
				//cobra_eflash_page_erase(suota_state.rdw_write_base_address + suota_state.rdw_write_addr);
			  length = FLASH_PAGE_SIZE - suota_state.rdw_write_index ;
			  memcpy(&p_mem_buffer[suota_state.rdw_write_index], suota_all_pd, length);
		  
        printf("rdw_write_flash begain 2:%d,%x,%x\n",length,suota_state.rdw_write_addr,suota_state.rdw_write_base_address);
			  
			  cobra_flash_write(suota_state.rdw_write_base_address + suota_state.rdw_write_addr, (uint16_t)FLASH_PAGE_SIZE, (uint8_t *)p_mem_buffer);
				suota_state.rdw_write_addr += FLASH_PAGE_SIZE;
			  memcpy(p_mem_buffer, &suota_all_pd[length], suota_state.new_patch_len - length );
			  suota_state.rdw_write_index = suota_state.new_patch_len - length ;

			  GLOBAL_INT_RESTORE();
		}	
}

static void rdw_flush_write(void)
{
	 
		cobra_flash_write(suota_state.rdw_write_base_address + suota_state.rdw_write_addr, suota_state.rdw_write_index, (uint8_t *)p_mem_buffer);
	  cobra_cache_normal_mode_enable(1);
		
	  
}

/**
 ****************************************************************************************
 * @brief Handles patch data indication from SUOTA receiver profile.
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_GAP).
 * @param[in] src_id    ID of the sending task instance.
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int ota_patch_data_ind_handler(ke_msg_id_t const msgid,
                                         struct ota_patch_data_ind const *param,
                                         ke_task_id_t const dest_id,
                                         ke_task_id_t const src_id)
{
    if( suota_state.mem_dev < SUOTAR_MEM_INVAL_DEV )
    {
        if (param->char_code)
        {
            if( suota_state.new_patch_len )
            {
                if( SUOTAR_IS_FOR_IMAGE(suota_state.mem_dev))
                {
                    //---------------------------- Handle SUOTAR image payload -----------------------
                    if( (suota_state.suota_block_idx + param->len) <= SUOTA_OVERALL_PD_SIZE)
                    {
                        memcpy(&suota_all_pd[suota_state.suota_block_idx], param->pd, param->len );
                        suota_state.suota_block_idx += param->len;
												suota_state.new_patch_len = param->len;
						           // printf("11111111111111111111111111111111111111111111 %d %d \n", suota_state.new_patch_len, suota_state.suota_block_idx);
                        if( suota_state.new_patch_len == suota_state.suota_block_idx )
                        {
                            app_suotar_img_hdlr();
                        }
//                        if( suota_state.suota_block_idx > suota_state.new_patch_len  )
//                        {
//                            // Received patch len not equal to PATCH_LEN char value
//                            suotar_send_status_update_req((uint8_t) SUOTAR_PATCH_LEN_ERR);
//                        }
                    }
                    else
                    {
                        suotar_send_status_update_req((uint8_t) SUOTAR_INT_MEM_ERR);
                    }
                }
                else
                {
                    suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
                }
            }
            else
            {
                // Inavild PATCH_LEN char value
                suotar_send_status_update_req((uint8_t) SUOTAR_PATCH_LEN_ERR);
            }
        }
    }
    else
    {
        suotar_send_status_update_req((uint8_t) SUOTAR_INVAL_MEM_TYPE);
    }
		
	printf("ota_patch_data_ind_handler .... \n");
	

    return (KE_MSG_CONSUMED);
}



/**
 ****************************************************************************************
 * @brief This function is called to get the correct image id to write to the image header
 *
 * @param[in]   imgid: current latest image id
 *
 * @return      new image id
 *
 ****************************************************************************************
 */
uint8_t app_get_image_id(uint8_t imgid)
{
	uint8_t new_imgid = IMAGE_ID_0;
	if (imgid == 0xff) return new_imgid;
    else {
 		new_imgid = imgid + 1;
		return new_imgid;
	}
}

/**
 ****************************************************************************************
 * @brief This function is called to find the older image of the two in the external memmory
 *
 * @param[in]   imgid: current ids of the two images
 *
 * @return      older image id
 *
 ****************************************************************************************
 */
uint8_t app_find_old_img(uint8_t id1, uint8_t id2)
{
	if (id1 == 0xFF && id2 == 0xFF) return IMAGE_ID_1;
	if (id1 == 0xFF && id2 == 0) return IMAGE_ID_1;
	if (id2 == 0xFF && id1 == 0) return IMAGE_ID_2;
	if (id1 > id2) return IMAGE_ID_2;
	else return 1;
}

/**
 ****************************************************************************************
 * @brief Read data from a given starting address
 *
 * @param[in] *rd_data_ptr:  Points to the position the read data will be stored
 * @param[in] address:       Starting address of data to be read
 * @param[in] size:          Size of the data to be read
 *
 * @return  Number of read bytes or error code
 ****************************************************************************************
 */
//int app_read_ext_mem( uint8_t *rd_data_ptr, uint32_t address, uint32_t size )
//{
//    int ret = -1;
//	
//	  int i;
//	
//	  uint8_t * p_temp = (uint8_t *)address;
//	
//  	cobra_cache_invalidate_the_cache();
//	
//    if( suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH ||suota_state.mem_dev == SUOTAR_IMG_EFLASH )
//    {
//#if (!SUOTAR_SPI_DISABLE)
//        //cobra_eflash_read((unsigned long)address, (uint32_t*)rd_data_ptr);
//			  for(i = 0; i < size; i++)
//			  {
//						*rd_data_ptr++ =  *p_temp++;
//				}
//        ret = size;
//#endif
//    }
//    else
//    {
//    }
//    return ret;
//}

/**
 ****************************************************************************************
 * @brief This function is called when the first SUOTA block is received.
 *        Firstly, the image header is extracted from the first block, then the external memmory
 *        is checked to determin where to write this new image and finaly the header and the first
 *        image data are written to external memory.
 *
 * @param[in]   bank:      User can select bank=1 or bank=2 to force where to write the image
 *                         bypassing the check to update the oldest image in the ext memory
 *              data:      Points to the first data block received over SUOTAR protocol
 *              data_len:  Length of the data block
 *
 * @return      0 for success, otherwise error codes
 *
 ****************************************************************************************
 */
int app_read_image_headers(uint8_t image_bank, uint8_t* data, uint32_t data_len)
{

	
    image_header_t *pfwHeader;

    if( data_len < sizeof(image_header_t) )
    {
        // block size should be at least image header size
        return SUOTAR_INVAL_IMG_HDR;
    }
    else
    {
        // read header form first data block
        pfwHeader = (image_header_t*)data;
    }

    // check firmware header
    if( (pfwHeader->signature[0] != IMAGE_HEADER_SIGNATURE1) || (pfwHeader->signature[1] != IMAGE_HEADER_SIGNATURE2) )
    {
        return SUOTAR_INVAL_IMG_HDR;
    }

//		memcpy(data, data+sizeof(image_header_t), (data_len-sizeof(image_header_t)));
//		
    suota_state.suota_image_len = pfwHeader->code_size; //+sizeof(image_header_t);
		if(suota_state.suota_image_len >= (OTA_APP_SIZE - FLASH_PAGE_SIZE*2))
		{
				return IMAGE_HEADER_INVALID;
		}
		
		if((suota_state.mem_dev == SUOTAR_IMG_SPI_FLASH ) ||(suota_state.mem_dev == SUOTAR_IMG_I2C_EEPROM ))
    {
			  cobra_flash_erase(suota_state.rdw_write_base_address, suota_state.suota_image_len);
		}

		return IMAGE_HEADER_OK;
}


/**
 ****************************************************************************************
 * @brief SUOTA image block handler. Validates image block and stores it to
 *        external memory device.
 *
 * @param[in]   void
 *
 * @return      void
 *
 ****************************************************************************************
 */
void app_suotar_img_hdlr(void)
{
    uint32_t mem_info;
    uint16_t status = SUOTAR_CMP_OK;

    uint32_t    ret;
    uint32_t     i;
	
	

    // Update CRC
    for(i=0; i<suota_state.suota_block_idx; i++)
	  {
        suota_state.crc_calc ^= suota_all_pd[i];
    }
		
    // Check mem dev.
    switch (suota_state.mem_dev)
    {
        case SUOTAR_IMG_I2C_EEPROM:

//#if (!SUOTAR_I2C_DISABLE)

//            // When the first block is received, read image header first
//            if( suota_state.suota_block_idx != 0 && suota_state.suota_img_idx == 0 )
//            {
//                 suota_state.suota_img_idx += suota_state.suota_block_idx;
//            }
//            else
//            {
//								suota_state.suota_img_idx += suota_state.suota_block_idx;
//							
//            }
//						
//						status = SUOTAR_CMP_OK;   //liujian add
//            suota_state.suota_block_idx = 0;
//            mem_info = suota_state.suota_img_idx;
//            suotar_send_mem_info_update_req(mem_info);
//#else
//            status = SUOTAR_INVAL_MEM_TYPE;
//#endif // (!SUOTAR_I2C_DISABLE)
//
//            break;
        case SUOTAR_IMG_SPI_FLASH:
        case SUOTAR_IMG_EFLASH:

				
				
#if (!SUOTAR_SPI_DISABLE)

            // When the first block is received, read image header first
            if( suota_state.suota_block_idx != 0 && suota_state.suota_img_idx == 0 )
            {
                // Read image headers and determine active image.
                ret = app_read_image_headers(suota_state.suota_image_bank, suota_all_pd, suota_state.suota_block_idx );
                printf("image_headers:%d,%x\n",ret,suota_state.suota_image_len);
                if( ret != IMAGE_HEADER_OK )
                {
                    status = ret;
                }
                else
                {
                    // Update block index
                    suota_state.suota_img_idx += suota_state.suota_block_idx;
									  suota_state.new_patch_len -= sizeof(image_header_t);
									  memcpy(suota_all_pd, suota_all_pd+sizeof(image_header_t),suota_state.new_patch_len);
					
                }
            }
            else
            {
								suota_state.suota_img_idx += suota_state.suota_block_idx;
            }
						
            rdw_write_flash();

			      status = SUOTAR_CMP_OK;   //liujian add
            suota_state.suota_block_idx = 0;
            mem_info = suota_state.suota_img_idx;
            suotar_send_mem_info_update_req(mem_info);
#else
            status = SUOTAR_INVAL_MEM_TYPE;
#endif //(!SUOTAR_SPI_DISABLE)

            break;

        default:
            status = SUOTAR_INVAL_MEM_TYPE;
            break;
    }

    // SUOTA finished successfully. Send Indication to initiator
    suotar_send_status_update_req((uint8_t) status);
}		

void app_ota_disconnection(void)
{
//	  if (suota_state.reboot_requested)
//    {
//			  printf("app_ota_disconnection ......... \n");
//        // Reboot request will be served
//        suota_state.reboot_requested = 0;

//        // Platform reset
//				platform_reset_inline(RESET_NO_ERROR);
//    }
}


/*
 * LOCAL VARIABLE DEFINITIONS
 ****************************************************************************************
 */

/// Default State handlers definition
const struct ke_msg_handler app_ota_msg_handler_list[] =
{
    // Note: first message is latest message checked by kernel so default is put on top.
    {KE_MSG_DEFAULT_HANDLER,             (ke_msg_func_t)app_ota_msg_handler},

		{OTA_PATCH_MEM_DEV_IND,              (ke_msg_func_t)ota_patch_mem_dev_ind_handler},
	
    {OTA_GPIO_MAP_IND,                   (ke_msg_func_t)ota_gpio_map_ind_handler},
    
    {OTA_PATCH_LEN_IND,                  (ke_msg_func_t)ota_patch_len_ind_handler},
    
    {OTA_PATCH_DATA_IND,                 (ke_msg_func_t)ota_patch_data_ind_handler},
    
};

const struct app_subtask_handlers app_ota_handlers = APP_HANDLERS(app_ota);
#endif 


