//liujian add 
//2019/4/8
//IPC host M3 application driver

//IPC slave M0 application driver

/*
      -----------------------------------------------------------|
			address 		|														|  size            |
			-----------------------------------------------------------|	
			0x20030000  |  M0 send data area 				| 0x200 (512Byte)	 |
									|														|                  |
									|														|									 |		
     ------------------------------------------------------------|
		  0x20030200  |  M3 send data area   			| 0x200 (512byte)  | 
									|														|									 |	
									|														|									 |	
			-----------------------------------------------------------|
*/

#ifndef __RDW_IPC_H__
#define __RDW_IPC_H__


#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
	
#define IPC_SHARE_BUFFER_SIZE       256
#define IPC_M0_SHARE_MEMORY_BEGIN		 (0x20030000UL)
#define IPC_M3_SHARE_MEMORY_BEGIN		 (0x20030200UL)

#define IPC_IRQ_M3_TO_M0           	0x01
#define IPC_IRQ_M0_TO_M3		   		 	0x01


#define IPC_M3_TX_DATA							0x02
#define IPC_M0_TX_ACK								0x02

#define IPC_M3_RESET_M0							0x04
	
	
struct m3_ipc_share_mem
{
		uint32_t m3_send_length;
		uint8_t  m3_tx[IPC_SHARE_BUFFER_SIZE];
};


struct m0_ipc_share_mem
{
		uint32_t m0_send_length;
		uint8_t  m0_tx[IPC_SHARE_BUFFER_SIZE];
};


void app_m3_ipc_host_init(void);

void app_rdw_ipc_host_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void app_rdw_ipc_host_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void app_rdw_ipc_host_flow_on(void);
bool app_rdw_ipc_host_flow_off(void);


//////////////////////////////

void app_m0_ipc_init(void);

void app_rdw_ipc_emb_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void app_rdw_ipc_emb_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy);
void app_rdw_ipc_flow_on(void);
bool app_rdw_ipc_flow_off(void);

#ifdef __cplusplus
}
#endif

#endif 
