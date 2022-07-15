//liujian add 
//2018/7/2
//用于配置 RDW MCU 的工作参数

#ifndef __RDW_MCU_H__
#define __RDW_MCU_H__
#include "arch.h"
#include "rwip.h"
#include "cobra_config.h"


#define RDW_SDK_VERSION   "\x53\x44\x4B\x5F\x56\x31\x2E\x31\x2E\x31\x00"       //SDK_V1.1.0


#define KERNEL_TIME_HZ		10

#define KERNEL_SYSTICK_HZ   1000

//Liujian add
//用M0和M3 共有的一块区域来做NVDS,
// 地址是 0x20030400 ,大小 2KByte



#if ( BLE_APP_MESH == 1)

#define   M4_EFLASH_MESH_ADDRESS  (0x50000 - 0x3000)

#if (BLE_APP_MESH_TULIPE_LIGHT == 1)

#define   M4_EFLASH_COUNT_ADDRESS           (M4_EFLASH_MESH_ADDRESS + 4)

#define   M4_EFLASH_COUNT_TIMEOUT_ADDRESS   (M4_EFLASH_MESH_ADDRESS + 8)

#if (APP_FLASH_LIGHT_NUMBERS)
#define   M4_EFLASH_FLASH_NUMBERS_ADDRESS   (M4_EFLASH_MESH_ADDRESS + 16)
#endif

#define   M4_EFLASH_OTA_ADDRESS		          (M4_EFLASH_MESH_ADDRESS + 80)

#define   M4_EFLASH_SCREEN_ADDRESS          (M4_EFLASH_MESH_ADDRESS + 100)

#endif

#endif

#if ((BLE_APP_MESH_TULIPE_LIGHT == 1) && (BLE_APP_MESH == 1))

#define   M4_EFLASH_NVDS_ADDRESS  (M4_EFLASH_MESH_ADDRESS + 0x200)

#else

#define   M4_EFLASH_NVDS_ADDRESS	(0x50000 - 0x2000)

#endif

extern int Image$$RW_IRAM1$$ZI$$Limit;

//#if(COBRA_IRDA_EN == 1)

#if (MCU_POWER_DOWN)
#define 	M0_M3_SHARE_ADDRESS     0x2000b000
#else
#define 	M0_M3_SHARE_ADDRESS     0x2000b800
#endif

#define 	M0_M3_NVDS_SIZE			    0x700
#define   COBRA_SRAM_SIZE         0xb800

//#else
////如果红外没有使用, 使用红外的那 2K Memory
//#define 	M0_M3_SHARE_ADDRESS     0x400ab800
//#define 	M0_M3_NVDS_SIZE			    0x400
//#define   COBRA_SRAM_SIZE         0xC000
//#endif

#ifdef COBRA_OTA_ENABLE
#undef    COBRA_SRAM_SIZE
#define   COBRA_SRAM_SIZE          0x3000
#endif

#define   COBRA_SRAM_END          (0x20000000 + COBRA_SRAM_SIZE )


#define   M4_EFLASH_PARAM_ADDRESS       (0x50000 - 0x4000)     //0x4C000

#define   M4_MESH_DATA_ADDR  		        (0x50000 - 0x5000)

#define   APP_MESH_TULIPX_SCENS_DATA 	(0x50000 - 0x6000)

#define TIME_TICK	 1                       //10ms  systick 的tick


//这里定义所有的TASK_ID 号,这样可以避免每次CEVA 更改版本,我们需要改动他的代码

#define  TASK_ID_VOICE  	TASK_ID_CGMC + 1
#define  TASK_ID_OTA			TASK_ID_VOICE + 1
#define  TASK_ID_UART			TASK_ID_OTA + 1

#define  TASK_ID_DEVICE1_UART							TASK_ID_UART + 1   //2019/11/27  add iris
#define  TASK_ID_DEVICE1_COLLECTOR_UART		TASK_ID_DEVICE1_UART + 1
#define  TASK_ID_DEVICE2_COLLECTOR_UART   TASK_ID_DEVICE1_COLLECTOR_UART + 1
#define  TASK_ID_DEVICE3_COLLECTOR_UART   TASK_ID_DEVICE2_COLLECTOR_UART + 1

#define  TASK_ID_OTA_MASTER			          TASK_ID_DEVICE3_COLLECTOR_UART + 1

#define  TASK_ID_LED_BUTTON			          TASK_ID_OTA_MASTER + 1

#define  TASK_ID_VOICE_MASTER             TASK_ID_LED_BUTTON + 1

#define  TASK_ID_AT_CMD_SLAVE             TASK_ID_VOICE_MASTER + 1

#define  TASK_ID_TUYA_BLE                 (TASK_ID_AT_CMD_SLAVE + 1)

#define  TASK_ID_WECHAT                    (TASK_ID_TUYA_BLE + 1)

#if (APP_OTA_MATSER_ENABLE == 1)
#define  TASK_OTA_SCAN    TASK_RESERVE_00
#define  TASK_OTA_DEVICE  TASK_RESERVE_01
#endif

#if ((APP_METER_AT_COMMAND == 1) && (APP_METER_AS_SLAVE_ENABLE == 0))
#define  TASK_MASTER_SCAN    TASK_RESERVE_00
#define  TASK_ADV_DEVICE1    TASK_RESERVE_01

#define  TASK_CONNECTION_DEVICE1   TASK_RESERVE_02
#define  TASK_CONNECTION_DEVICE2   TASK_RESERVE_03
#define  TASK_CONNECTION_DEVICE3   TASK_RESERVE_04
#endif


#if (NVDS_SUPPORT)

#define    IRIS_MAC_SETFLAG				       NVDS_TAG_PEER_IRK+1
#define    IRIS_MAC_SETFLAG_LENGTH		     1

#define    IRIS_CALCULATE_SETFLAG				 IRIS_MAC_SETFLAG+1
#define    IRIS_CALCULATE_LENGTH		       1

#define    MESH_APPKEY_OK_FLAG   					   IRIS_CALCULATE_SETFLAG + 1
#define    MESH_APPKEY_OK_LENGTH             1

#define    MESH_DEVICE_UUID_FLAG             MESH_APPKEY_OK_FLAG+1
#define    MESH_DEVICE_UUID_LENGTH           16


#if(DIANBIAO_MODULE_ENABLE == 1)

#define    CALIBRATION_PLUSE_FLAG           MESH_DEVICE_UUID_FLAG+1
#define    CALIBRATION_PLUSE_LENGTH           1

#define    CALIBRATION_METER_PARAM_FLAG      CALIBRATION_PLUSE_FLAG+1
#define    CALIBRATION_METER_PARAM_LENGTH    sizeof(ble_meter_struct)


#define    ADV2_DEVICE_BOND_FLAG            CALIBRATION_METER_PARAM_FLAG+1
#define    ADV2_DEVICE_BOND_LENGTH          1

#define    ADV2_NVDS_TAG_LTK                ADV2_DEVICE_BOND_FLAG+1       
#define    ADV2_NVDS_LEN_LTK                28


#define    NVDS_TAG_BD_ADDRESS2           IRIS_CALCULATE_SETFLAG+1
#define    NVDS_LEN_BD_ADDRESS2           6 


#define 	 NVDS_TAG_LOC_IRK2              NVDS_TAG_BD_ADDRESS2+1
#define    NVDS_LEN_LOC_IRK2               KEY_LEN


#define    METER_REBOOT_FLAG					  NVDS_TAG_LOC_IRK2+1		// 系统是否复位了
#define    METER_REBOOT_FLAG_LEN			  4

#define    BREAKER_ADDR1_FLAG				    METER_REBOOT_FLAG+1
#define    BREAKER_ADDR1_LEN				    6

#define    BREAKER_ADDR2_FLAG				    BREAKER_ADDR1_FLAG+1
#define    BREAKER_ADDR2_LEN				    6

#define    BREAKER_ADDR3_FLAG				    BREAKER_ADDR2_FLAG+1
#define    BREAKER_ADDR3_LEN				    6

#define    BREAKER1_ENABLE_FLAG		     BREAKER_ADDR3_FLAG+1
#define    BREAKER1_ENABLE_LEN	       1

#define    BREAKER2_ENABLE_FLAG		     BREAKER1_ENABLE_FLAG+1
#define    BREAKER2_ENABLE_LEN	       1

#define    BREAKER3_ENABLE_FLAG		    BREAKER2_ENABLE_FLAG+1
#define    BREAKER3_ENABLE_LEN	      1

#define    CALIBRATION_MODE_FREQUENCY_FLAG     BREAKER3_ENABLE_FLAG+1
#define    CALIBRATION_MODE_FREQUENCY_LENGTH   2

#define    CALIBRATION_MODE_ADDRESS_FLAG       CALIBRATION_MODE_FREQUENCY_FLAG+1
#define    CALIBRATION_MODE_ADDRESS_LENGTH     4

#if DIANBIAO_PROTOCOL_REALTEK_ENABLE


#define    NVDS_TAG_PAIR_PARAM_FLAG         CALIBRATION_MODE_ADDRESS_FLAG + 1
#define    NVDS_TAG_PAIR_PARAM_LENGTH       8*4


#define    NVDS_TAG_BLE_PARAM_FLAG          NVDS_TAG_PAIR_PARAM_FLAG + 1
#define    NVDS_TAG_BLE_PARAM_LENGTH        5

#endif  

#endif  //#if(DIANBIAO_MODULE_ENABLE == 1)


#endif  //#if (NVDS_SUPPORT)

//#if (TULIPX_MESH)
//#define 	 NVDS_TAG_XTAL_TRIM_VALUE             0x54
//#define 	 NVDS_LEN_XTAL_TRIM_VALUE             1
//#define 	 NVDS_TAG_RESET_FACTORY               0x55
//#define 	 NVDS_LEN_RESET_FACTORY 	            1
//#endif


#define METER_REBOOT_MAGIC		0x4C4F5645

//#define IRIS_UUID		"\x03\x03\xF0\xFF"
//#define IRIS_UUID_LEN	4

#define IRIS_UUID		"\x02\x01\x06"
#define IRIS_UUID_LEN	(3)


//广播数据标志
#define APP_ADV1					"\x02\x01\x06"
#define APP_ADV1_LEN			(3)
//广播发射功率
#define APP_ADV2				 "\x02\x0A\x01"
#define APP_ADV2_LEN			(3)
//从机连接间隔
#define APP_ADV3				 "\x05\x12\x18\x00\x50\x00"
#define APP_ADV3_LEN			(6)

/**
 * Advertising Parameters
 */

/// Advertising channel map - 37, 38, 39
#define APP_ADV_CHMAP           (0x07)

#if (DIANBIAO_MODULE_ENABLE == 1)
/// Advertising minimum interval - 40ms (64*0.625ms)
#define APP_ADV_INT_MIN         (64*5)
/// Advertising maximum interval - 40ms (64*0.625ms)
#define APP_ADV_INT_MAX         (64*5)
#else

#if (APP_SLEEP_MODE_ENABLE == 1)
/// Advertising minimum interval - 1s
#define APP_ADV_INT_MIN         (64)
/// Advertising maximum interval - 1s
#define APP_ADV_INT_MAX         (64)
#else
/// Advertising minimum interval - 40ms (64*0.625ms)

#if COBRA_POWER_DOWN_ENABLE
#define APP_ADV_INT_MIN         (160)//160
/// Advertising maximum interval - 40ms (64*0.625ms)
#define APP_ADV_INT_MAX         (160)

#else

#define APP_ADV_INT_MIN         (64)
/// Advertising maximum interval - 40ms (64*0.625ms)
#define APP_ADV_INT_MAX         (64)
#endif
#endif
#endif


#if COBRA_POWER_DOWN_ENABLE
/// Fast advertising interval
#define APP_ADV_FAST_INT        (80)//32
#else
/// Fast advertising interval
#define APP_ADV_FAST_INT        (32)
#endif

/**
 * Default Scan response data
 * --------------------------------------------------------------------------------------
 * x09                             - Length
 * xFF                             - Vendor specific advertising type
 * x00\x60\x52\x57\x2D\x42\x4C\x45 - "RW-BLE"
 * --------------------------------------------------------------------------------------
 */
 #if(DIANBIAO_MODULE_ENABLE == 1)
#define APP_SCNRSP_DATA					  "\x11\x07\x79\x41\xDC\x24\x0E\xE5\xA9\xE0\x93\xF3\xA3\xB5\x01\x00\x40\x6E"
#define APP_SCNRSP_DATA_LEN     (18)
#else
#define APP_SCNRSP_DATA           "\x09\xFF\x00\x60\x52\x57\x2D\x42\x4C\x45"
#define APP_SCNRSP_DATA_LEN      (10)
#endif

#define APP_DFLT_DEVICE_NAME            ("FFFFFF")
#define APP_DFLT_DEVICE_NAME_LEN        (sizeof(APP_DFLT_DEVICE_NAME))


#define DEVICE_NAME        "RW DEVICE"

#define DEVICE_NAME_SIZE    sizeof(DEVICE_NAME)
	

#define COBRA4_MTU				512
#define COBRA4_MPS				512


#if(TUYA_BLE_SDK_ENABLE == 1)
#define TUYA_BLE_DEMO_VERSION_NUM      0x010200
#define TUYA_BLE_DEMO_VERSION_STR	    "1.2.0" 	
#endif

/**
 * Appearance part of ADV Data
 * --------------------------------------------------------------------------------------
 * x03 - Length
 * x19 - Appearance
 * x03\x00 - Generic Thermometer
 *   or
 * xC2\x04 - HID Mouse
 * --------------------------------------------------------------------------------------
 */

#define APP_HT_ADV_DATA_APPEARANCE    "\x03\x19\x00\x03"


#define APP_ADV_DATA_APPEARANCE_LEN  (4)


//OTA 版本信息
#define OTA_PD_CHAR_SIZE      244  
#define OTA_VERSION_1_0       10
#define OTA_VERSION_1_3       13

#ifndef OTA_VERSION
#define OTA_VERSION       OTA_VERSION_1_3
#endif


//memory mangement
#define MMU_ALIGN_SIZE 4


/**
 * @ingroup BasicDef
 *
 * @def COBRA_NULL
 * Similar as the \c NULL in C library.
 */
#define COBRA_NULL                         (0)

/**
 * @ingroup BasicDef
 *
 * @def COBRA_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. COBRA_ALIGN(13, 4)
 * would return 16.
 */
#define COBRA_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def COBRA_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. COBRA_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define COBRA_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))


extern void rdw_umcu_init(void);

//中断

#define AON_EVENT0_IRQn				 			0   //aon_event0	
#define PAD_TRIGGER_SINGLE_IRQ			1		//pad_trigger_single
	
#define	INT_RTMT0_WAKEUP_IRQ				2 					// int_rmt0_wakeup
	
#define	BUS_EVENT1_IRQn					 3				//bus_event1
	
#define	SPIF_INT_IRQn						4	       //spif_int_n
	
#define	SHA_INTERRUPT_IRQn		 5				//sha_interrupt_n
	
#define	I2SPCM0_IRQn						 6 				//i2spcm0_int

#define	I2SPCM1_IRQn						 7				//i2spcm1_int
	
#define	DMA_ERR_IRQn						 8				//dma_err
	
#define	PDM0_INT_IRQn						 9				//pdm0_int
	
#define	COBRA4_UART0_IRQn							 10				//int_uart0
	
#define	RMT0_RXD_SYNC_IRQ				 11				//rmt0_rxd_sync 
	
#define	MPWM_IRQn								 12				//int_mpwm
	
#define	SPWM_IRQn								 13				//int_spwm
	
#define	SPI0_IRQn								 14 			//int_spi0
	
#define	EFC_IRQn								 15				//efc_int   EFlash interrupt
	
#define	VA_VOICE_AWKAE_IRQ			 16				//va_voice_awake           
	
#define	AON_EVENT3_IRQn					 17				//aon_event3               
	
#define	AON_EVENT4_IRQn					 18				//aon_event4               
	
#define	I2C0_IRQn								 19				//int_i2c0
	
#define	I2C1_IRQn								 20				//int_i2c1
	
#define	RMT0_IRQ							 21				//int_rmt0                 
	
#define	AES0_SM4_IRQn								 22				//int_aes0
	
#define	TRNG_IRQn								 23				//int_trng
	
#define	COBRA4_TIMER0_IRQn							 24				//int_timer0
	
#define	COBRA4_TIMER1_IRQn							 25				//int_timer1
	
#define	DUALTIMER2_IRQn					 26				//int_dualtimer2
	
#define	AON_EVENT5_IRQn					 27				//aon_event5
	
#define	SPI1_IRQn								28				//int_spi1
	
#define	ECCMICRO0_IRQn				 29				//int_eccmicro0
	
#define	QDEC0_IRQn						 30				//qdec0_int
	
#define	COBRA4_BT_IRQn								 31				//Bluethooth interrupt, bt_sirq
	
#define	COBRA4_UART1_IRQn						 32				//int_uart1                
	
#define	DMADONE_IRQn					 33			//dmadone_int
	
#define	VIMS_CACHE_IRQn				 34				//Cache interrupt
	
#define	CSEM_IRQ_TXFIFO				 35				//icytrx_irq_4
	
#define	CSEM_IRQ_RXFIFO				 36				//icytrx_irq_5
	
#define	CSEM_IRQ_TX						 37				//icytrx_irq_0
	
#define	CSEM_IRQ_RXSTOP				 38				//icytrx_irq_1
	
#define	CSEM_IRQ_RECEIVED			 39				//icytrx_irq_2
	
#define	CSEM_IRQ_SYNC					 40				//icytrx_irq_3

#endif
