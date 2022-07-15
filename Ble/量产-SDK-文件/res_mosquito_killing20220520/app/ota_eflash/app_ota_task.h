//liujian
//2018/4/17
//ver1.0

#ifndef APP_OTA_TASK_H__
#define APP_OTA_TASK_H__



/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW configuration

#if (BLE_APP_OTA)

#include <stdint.h>          // Standard Integer Definition
#include "ke_task.h"
#include "rdw_ota_task.h"

/*
 * TYPE DEFINITIONS
 ****************************************************************************************
 */

//liujian add 测试使用, 需要添加I2C 和 SPI 驱动
//#define SUOTAR_SPI_DISABLE  1
//#define SUOTAR_I2C_DISABLE  1


/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */

extern struct app_ota_env_tag app_ota_env;

/// Table of message handlers
extern const struct app_subtask_handlers app_ota_handlers;


// Holds the retainable variables of SUOTAR app
typedef struct
{
    uint8_t     mem_dev;
    uint32_t    mem_base_add;
    uint32_t    gpio_map;
    uint32_t    new_patch_len;
    uint8_t     suota_pd_idx;
    uint8_t     suota_image_bank;
    uint32_t    suota_block_idx;
    uint32_t    suota_img_idx;
    uint8_t     crc_calc;
    uint32_t    suota_image_len;
    void (*status_ind_func) (const uint8_t);
    uint8_t     reboot_requested;
	
	   uint32_t    rdw_total_size;
	//  uint32_t    erase_block_flag;  //4096 Byte
	//	uint32_t    rdw_remain_size;
		uint32_t		rdw_write_index;
		uint32_t    rdw_write_addr;
	
	  uint32_t    rdw_write_base_address;
}app_suota_state;


// Product header for SUOTA
typedef struct
{
    uint8_t signature[2];
    uint8_t version[2];
    uint32_t offset1; // Start address of first image header
    uint32_t offset2; // Start address of second image header
}product_header_t;

// Image header for SUOTA
typedef struct
{
    uint8_t signature[2];
    uint8_t validflag;      // Set to STATUS_VALID_IMAGE at the end of the image update
    uint8_t imageid;        // it is used to determine which image is the newest
    uint32_t code_size;     // Image size
    uint32_t CRC ;          // Image CRC
    uint8_t version[16];    // Vertion of the image
    uint32_t timestamp;
    uint8_t encryption;
    uint8_t reserved[31];
}image_header_t;


/// SUOTA Status values
enum
{
// SUOTAR/SUOTAR Physical memory device selection and commands
    // SUOTA is used to send patches
    SUOTAR_MEM_INT_SYSRAM = 0x00,
    SUOTAR_MEM_INT_RETRAM = 0x01,
    SUOTAR_MEM_I2C_EEPROM = 0x02,
    SUOTAR_MEM_SPI_FLASH  = 0x03,
		SUOTAR_MEM_EFLASH  		= 0x04,
    // SUOTA is used to send entire image
    SUOTAR_IMG_INT_SYSRAM = 0x10,
    SUOTAR_IMG_INT_RETRAM = 0x11,
    SUOTAR_IMG_I2C_EEPROM = 0x12,
    SUOTAR_IMG_SPI_FLASH  = 0x13,
    SUOTAR_IMG_EFLASH  		= 0x14,

    SUOTAR_MEM_INVAL_DEV  = 0x15, // DO NOT move. Must be before commands

    /* SUOTAR/SUOTAR commands
    */
    SUOTAR_REBOOT         = 0xFD,
    SUOTAR_IMG_END        = 0xFE,
    // When initiator selects 0xff, it wants to exit SUOTAR service.
    // This is used in case of unexplained failures. If SUOTAR process
    // finishes correctly it will exit automatically.
    SUOTAR_MEM_SERVICE_EXIT   = 0xFF,
};
	

#define SUOTAR_READ_MEM_DEV_TYPE    0xFF000000
#define SUOTAR_READ_MEM_BASE_ADD    0x0000FFFF

// SUOTAR defines for image manipulation
#define PRODUCT_HEADER_POSITION     0x20000
#define PRODUCT_HEADER_SIGNATURE1   0x70
#define PRODUCT_HEADER_SIGNATURE2   0x52
#define IMAGE_HEADER_SIGNATURE1     0x70
#define IMAGE_HEADER_SIGNATURE2     0x51
#define CODE_OFFSET                 64
#define ADDITINAL_CRC_SIZE          1
#define IMAGE_HEADER_VERSION_SIZE   16
#define STATUS_VALID_IMAGE          0xAA
#define IMAGE_HEADER_OK             0
#define IMAGE_HEADER_INVALID        1
#define IMAGE_HEADER_SAME_VERSION   2
#define ANY_IMAGE_BANK              0
#define FISRT_IMAGE_BANK            1
#define SECOND_IMAGE_BANK           2
#define IMAGE_ID_0                  0
#define IMAGE_ID_1                  1
#define IMAGE_ID_2                  2



#define SUOTAR_IS_VALID_MEM_DEV(mem_dev) ( (mem_dev) < SUOTAR_MEM_INVAL_DEV )
//#define SUOTAR_IS_VALID_MEM_DEV(mem_dev) ( (mem_dev) < SUOTAR_MEM_INVAL_DEV )


#define SUOTA_OVERALL_PD_SIZE   0x200


 // SUOTAR application types
#define SUOTAR_PATCH            0   // Software patch over the air (SUOTA)
#define SUOTAR_IMAGE            1   // Software image update over the air (SUOTA)

// Checks the application type (SUOTA or SUOTA)
#define SUOTAR_IS_FOR_PATCH(mem_dev) ( (((mem_dev) & 0x10) >> 4 ) == SUOTAR_PATCH)
#define SUOTAR_IS_FOR_IMAGE(mem_dev) ( (((mem_dev) & 0x10) >> 4 ) == SUOTAR_IMAGE)


#define STATUS_VALID_IMAGE          0xAA
#define IMAGE_HEADER_OK							0
#define ADDITINAL_CRC_SIZE					1



/*
 * DEFINES
 ****************************************************************************************
 */

/// Number of APP Task Instances
#define APP_IDX_MAX                 (1)

/*
 * ENUMERATIONS
 ****************************************************************************************
 */
extern void app_ota_init(void);
void app_ota_enable_prf(uint8_t conidx);

void app_ota_disconnection(void);
#endif //(BLE_APP_OTA)



#endif 
