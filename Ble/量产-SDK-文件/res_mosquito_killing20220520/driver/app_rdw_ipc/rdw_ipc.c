//liujian add
//2018/4/8
//ipc host cortex-m3 application driver

//2019/4/8  add

#include "cobra_pcrm.h"
#include "cobra_umcu.h"
#include "cobra_ipc.h"
#include "rdw_ipc.h"
#include "cobra_m0_sysctrl.h"
#include "rdw_mcu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
      -----------------------------------------------------------|
			address 		|														|  size            |
			-----------------------------------------------------------|	
			0x20030000  |  M0 read and write area  	| 0x200 (512Byte)	 |
									|														|                  |
									|														|									 |		
     ------------------------------------------------------------|
		  0x20030200  |  M3 read and write area   | 0x200 (512byte)  | 
									|														|									 |	
									|														|									 |	
			-----------------------------------------------------------|
*/


#if COBRA_IPC_HOST_EN
struct m3_ipc_share_mem   m3_ipc_share_env  __attribute__ ((section("ipc_shared_data_m3")));

static struct m0_ipc_share_mem  *  p_m0_ipc_share_env;
#endif

#if COBRA_IPC_EMB_EN
struct m0_ipc_share_mem    m0_ipc_share_env __attribute__ ((section("ipc_shared_data_m0")));

struct m3_ipc_share_mem * p_m3_ipc_share_env;
#endif


struct ipc_txrxchannel
{
    /// call back function pointer
    void (*callback) (void*, uint8_t);
    /// Dummy data pointer returned to callback when operation is over.
    void* dummy;
};

/// IPC EMB environment structure
struct ipc_env_tag
{
     /// tx channel
    struct ipc_txrxchannel tx;
    /// rx channel
    struct ipc_txrxchannel rx;
  

		
		uint32_t   tx_index;
		uint32_t   tx_total;
		uint8_t  	 *p_tx_buffer;
		
//---------------------------------------	
		
	
	  uint32_t  	total;
		uint8_t  		*p_buffer;
		uint32_t  	receive_index;	
		uint32_t  	read_index;
	
		uint8_t    tx_busy;
		uint8_t    rx_busy;	
	
	
}; 
#if COBRA_IPC_HOST_EN
static struct ipc_env_tag  ipc_host_env;
#endif

#if COBRA_IPC_EMB_EN
static struct ipc_env_tag  ipc_emb_env;
#endif

#if COBRA_IPC_HOST_EN
void  app_m3_ipc_host_init(void)
{
	//ָ��  �������� M0 �ķ��͵�ַ 0x20030000  ,��СΪ 0x200, 512 �ֽ�
	
	p_m0_ipc_share_env = (struct m0_ipc_share_mem  *)IPC_M0_SHARE_MEMORY_BEGIN;

	
	cobra_ipc_m3_emb2app_unmask_set(IPC_IRQ_M3_TO_M0);
	
	cobra_ipc_m3_emb2app_unmask_set(IPC_M3_TX_DATA);
	
	cobra_ipc_m3_app2emb_trigger_set(IPC_M3_RESET_M0);
	
	NVIC_SetPriority(IPC_HOST_IRQn, 0);
	
	NVIC_ClearPendingIRQ(IPC_HOST_IRQn);
	
	NVIC_EnableIRQ(IPC_HOST_IRQn);
	
	
}

static void cobra_m3_ipc_wakeup_m0(void)
{
			app_cobra_umcu_wakeup_m0_run();		
			cobra_m0_sysctrl_wakeup_the_bt(1);
}


static void cobra_m3_ipc_wakeup_m0_ack(void)
{
//			app_cobra_umcu_wakeup_m0_run();		
//			cobra_m0_sysctrl_wakeup_the_bt(1);
}

void cobra_m3_ipc_interrupt(void)
{
		uint32_t status = cobra_ipc_m3_emb2app_status_get();
	
		if (status & IPC_IRQ_M0_TO_M3)
		{
				cobra_ipc_m3_emb2app_ack_clear(IPC_IRQ_M0_TO_M3);
						
				if(ipc_host_env.rx_busy) 
				{
						if(p_m0_ipc_share_env->m0_send_length >= ipc_host_env.total)
						{
								memcpy(&ipc_host_env.p_buffer[ipc_host_env.receive_index], (void *)(p_m0_ipc_share_env->m0_tx), ipc_host_env.total);
								ipc_host_env.read_index += ipc_host_env.total;
								ipc_host_env.receive_index += ipc_host_env.total;								
						}
						else
						{
//								printf("Error ...... \n");
								memcpy(&ipc_host_env.p_buffer[ipc_host_env.receive_index], (void *)(p_m0_ipc_share_env->m0_tx), p_m0_ipc_share_env->m0_send_length);
								ipc_host_env.receive_index += p_m0_ipc_share_env->m0_send_length;
								ipc_host_env.read_index += p_m0_ipc_share_env->m0_send_length;
						}
						
						if(ipc_host_env.read_index == p_m0_ipc_share_env->m0_send_length)
						{
								cobra_m3_ipc_wakeup_m0_ack();
								ipc_host_env.rx_busy = 0;
								cobra_ipc_m3_app2emb_trigger_set(IPC_IRQ_M3_TO_M0);    //ֻ��ȫ���������������,��֪ͨ M0  ����ȫ���յ���
								//printf("M0 ACK0 \n");
						}
						
						if(ipc_host_env.receive_index == ipc_host_env.total) 
						{
								void (*callback) (void*, uint8_t) = NULL;
								void* data =NULL;
							
								//ipc_env.rx_busy = 0;
								callback = ipc_host_env.rx.callback;
								data     = ipc_host_env.rx.dummy;

								if(callback != NULL)
								{
										// Clear callback pointer
										ipc_host_env.rx.callback = NULL;
										ipc_host_env.rx.dummy    = NULL;

										// Call handler
									
										callback(data, RWIP_EIF_STATUS_OK);
								}
								
						}
										
			}
				
		}
		
		if (status & IPC_M0_TX_ACK)   //�����M0 ���� M3, ���Ѿ���ȷ�յ��� M3  ������
		{
				cobra_ipc_m3_emb2app_ack_clear(IPC_M0_TX_ACK);
				cobra_m0_sysctrl_wakeup_the_bt(0);
		//	  printf("M0 Receive OK : index= %d , total = %d \n", ipc_env.tx_index, ipc_env.tx_total);
		
				void (*callback) (void*, uint8_t) = NULL;
				void* data =NULL;
				if(ipc_host_env.tx_busy) 
				{
						
							ipc_host_env.tx_busy = 0;
							callback = ipc_host_env.tx.callback;
							data     = ipc_host_env.tx.dummy;

							if(callback != NULL)
							{
									// Clear callback pointer
									ipc_host_env.tx.callback = NULL;
									ipc_host_env.tx.dummy    = NULL;
									// Call handler
									callback(data, RWIP_EIF_STATUS_OK);
								
							}
				}
				
		}
	
}




//�������ݵ� Device ��
//M3 ----> M0
void app_rdw_ipc_host_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
    if(bufptr == NULL) return;
    if(size == 0) return;
    if(callback == NULL) return;
	
    ipc_host_env.tx.callback = callback;
    ipc_host_env.tx.dummy    = dummy;

		if(ipc_host_env.tx_busy){
			printf("error ------------------- \n");
			return;
		}
			ipc_host_env.p_tx_buffer = bufptr;
			ipc_host_env.tx_total = size;
			ipc_host_env.tx_index = 0;
			ipc_host_env.tx_busy = 1;

	
			m3_ipc_share_env.m3_send_length = size;  //M3 �������ݷ���receive_length ��������, ���ֲ���,�����Ǵ��� M3 ���͵� M0 �����ݳ���
	
			memcpy((void *)(m3_ipc_share_env.m3_tx), ipc_host_env.p_tx_buffer, m3_ipc_share_env.m3_send_length);
			
		  // Cortex-M3 want the M0 wakeup ....
		  cobra_m3_ipc_wakeup_m0();
		
			//printf("M3 %d \n", size);
			//����� ֪ͨ M0 Ҫ����������
			//SEV();
		//Ҫ�ӻ���
			//cobra_ipc_m3_app2emb_trigger_set(IPC_M3_TX_DATA);
		
		  
			
		//	ipc_env.tx_index++;
}


void cobra_ipc_test_send_flag(void)
{
		cobra_ipc_m3_app2emb_trigger_set(IPC_M3_TX_DATA);
}
//------------------------------------------------------------------------



//��ȡ M0 ���͹���������
//M3 ���� M0 �������ݹ���
void app_rdw_ipc_host_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
		uint32_t len;
    if(bufptr == NULL) return;
    if(size == 0) return;
    if(callback == NULL) return;
	
    ipc_host_env.rx.callback = callback;
    ipc_host_env.rx.dummy    = dummy;



//	//��ʾ׼������ M0 ���͹���������
 // printf("M3 Need Data size is  %d \n", size);
	
			if(ipc_host_env.rx_busy)   //˵�����յ�buffer ���滹������
			{
						len = p_m0_ipc_share_env->m0_send_length - ipc_host_env.read_index;  //Buf ���滹ʣ�µ����ݸ���
						if(len >= size)
						{
								memcpy(bufptr, &p_m0_ipc_share_env->m0_tx[ipc_host_env.read_index], size);
								ipc_host_env.read_index += size;
								ipc_host_env.receive_index += size;	
							
								if(ipc_host_env.read_index == p_m0_ipc_share_env->m0_send_length )
								{
										cobra_m3_ipc_wakeup_m0_ack();
										ipc_host_env.rx_busy = 0;
										cobra_ipc_m3_app2emb_trigger_set(IPC_IRQ_M3_TO_M0);   //ֻ��ȫ���������������,��֪ͨ M3  ����ȫ���յ���
									  //printf("M0 ACK1 \n");
								}
						
						
								{
										void (*callback) (void*, uint8_t) = NULL;
										void* data =NULL;
									
										
										callback = ipc_host_env.rx.callback;
										data     = ipc_host_env.rx.dummy;

										if(callback != NULL)
										{
												// Clear callback pointer
												ipc_host_env.rx.callback = NULL;
												ipc_host_env.rx.dummy    = NULL;

												// Call handler
												callback(data, RWIP_EIF_STATUS_OK);
										}
								}
								
						}
						else   
						{
								// ˵��Buffer ��������ݲ�����һ�δ���
							  printf("Maybe have error .............. \n");
								memcpy(bufptr, &p_m0_ipc_share_env->m0_tx[ipc_host_env.read_index], len);
								ipc_host_env.receive_index += len;
								ipc_host_env.read_index += len;
							
								if(ipc_host_env.read_index == p_m0_ipc_share_env->m0_send_length)
								{
										cobra_m3_ipc_wakeup_m0_ack();
										ipc_host_env.rx_busy = 0;
										cobra_ipc_m3_app2emb_trigger_set(IPC_IRQ_M3_TO_M0);  //ֻ��ȫ���������������,��֪ͨ M3  ����ȫ���յ���
										//printf("M0 ACK2 \n");
								}
								
								ipc_host_env.p_buffer = bufptr+len;
								ipc_host_env.total = size - len;
							//	ipc_env.receive_index = 0;
								ipc_host_env.rx_busy = 1;
								ipc_host_env.read_index = 0;	
						}
						
		}
		else
		{
				ipc_host_env.p_buffer = bufptr;
				ipc_host_env.total = size;
				ipc_host_env.receive_index = 0;
				ipc_host_env.rx_busy = 1;
				ipc_host_env.read_index = 0;
		}
	
}


void app_rdw_ipc_host_flow_on(void)
{
    // Configure modem (HW flow control enable)
    //uart_force_rts_setf(0);
		GLOBAL_INT_DISABLE();
	  ipc_host_env.rx_busy = 0;
		ipc_host_env.tx_busy = 0;
		ipc_host_env.rx.callback = NULL;
    ipc_host_env.tx.callback = NULL;
	
		GLOBAL_INT_RESTORE();
	
		
}


bool app_rdw_ipc_host_flow_off(void)
{
    bool flow_off = true;
	
		GLOBAL_INT_DISABLE();
	  if(ipc_host_env.rx_busy || ipc_host_env.tx_busy)
			 flow_off = false; 
		GLOBAL_INT_RESTORE();

    return flow_off;
}

#endif

#if COBRA_IPC_EMB_EN

/**
 ****************************************************************************************
 */
void app_m0_ipc_init(void)
{
		m0_ipc_share_env.m0_send_length = 0;
		p_m3_ipc_share_env = (struct m3_ipc_share_mem  *)IPC_M3_SHARE_MEMORY_BEGIN;
	
	// configure the IPC line selection
    cobra_ipc_m0_app2emb_line_sel_low_set(0);    //10 01 00
    cobra_ipc_m0_app2emb_line_sel_high_set(0);

    cobra_ipc_m0_set_line_and_trigger_interrupt(0, IPC_IRQ0); // 
	
    cobra_ipc_m0_set_line_and_trigger_interrupt(1, IPC_IRQ1); //
	
		cobra_ipc_m0_set_line_and_trigger_interrupt(2, IPC_IRQ2); // IPC reset
	
	//enable interrupt
		cobra_ipc_m0_app2emb_unmask_set(IPC_IRQ_M3_TO_M0);  //����� M3 ���͵�ACK
	
		cobra_ipc_m0_app2emb_unmask_set(IPC_M3_TX_DATA);   //�����M3 ���͵�����
	
		cobra_ipc_m0_app2emb_unmask_set(IPC_M3_RESET_M0);   //�����M3 ���͵ĸ�λ�ź�
	
	
	
}

//interrupt function

void rdw_m3_reset_signal(void)
{
		if(cobra_ipc_m0_app2emb_status_get() & IPC_M3_RESET_M0)
    {
				cobra_ipc_m0_app2emb_ack_clear(IPC_M3_RESET_M0);
			//	ipc_env.tx_busy = 0;
			//	ipc_env.rx_busy = 0;
		//	hci_cmd_received(HCI_RESET_CMD_OPCODE, 0, NULL);
			
		//	h4tl_init(0, rwip_eif_get(2));
		}
}

/*
rdw_m3_to_m0_data �� ipc_emb_write ���������������������� M0 �������ݵ� M3, �� HCI �ӿ�
����.
1. ipc_emb_write , M0 ��������������������ݵ� M3. 
2. M0 �����ݷŵ� share Memory ��, ͨ�� IPC ���� IPC_IRQ_M0_TO_M3.  ��λ���bit
3. M3 �����ж�, �� IPC_IRQ_M3_TO_M0 ��λ, M0 �����ж�rdw_m3_to_m0_data
4. M0 ��rdw_m3_to_m0_data ,�жϳ����� M3 �Ѿ���ȫ���յ�������.
5. M0 �ٵ��ûص�����



*/
void rdw_m3_to_m0_ack(void)
{
		if (cobra_ipc_m0_app2emb_status_get() & IPC_IRQ_M3_TO_M0)
    {
			//������� M3 �Ѿ���ȷ�յ�  M0 ���͹���������, Ȼ�� M3  ��λһ�� APP2EMB ֪ͨ  M0
			// M0 �����ж�,�жϳ��� ���ݷ������ 

      //  ipc_app2emb_unmask_clear(IPC_IRQ_M3_TO_M0);

        cobra_ipc_m0_app2emb_ack_clear(IPC_IRQ_M3_TO_M0);
			  printf("*\n");
			
						
						
				void (*callback) (void*, uint8_t) = NULL;
				void* data =NULL;
				if(ipc_emb_env.tx_busy) 
				{
						
							ipc_emb_env.tx_busy = 0;
							callback = ipc_emb_env.tx.callback;
							data     = ipc_emb_env.tx.dummy;

							if(callback != NULL)
							{
									// Clear callback pointer
									ipc_emb_env.tx.callback = NULL;
									ipc_emb_env.tx.dummy    = NULL;
									// Call handler
									
									callback(data, RWIP_EIF_STATUS_OK);
							}
				}

		
			//	ipc_app2emb_unmask_set(IPC_IRQ_M3_TO_M0);
				
    }
}


void rdw_m3_to_m0_data(void)
{

			if (cobra_ipc_m0_app2emb_status_get() & IPC_M3_TX_DATA)
			{
		
        cobra_ipc_m0_app2emb_ack_clear(IPC_M3_TX_DATA);
				
	//			ipc_emb2app_trigger_set(IPC_M0_TX_ACK); 
				
				if(ipc_emb_env.rx_busy) 
				{
						if(p_m3_ipc_share_env->m3_send_length >= ipc_emb_env.total)
						{
								memcpy(&ipc_emb_env.p_buffer[ipc_emb_env.receive_index], p_m3_ipc_share_env->m3_tx, ipc_emb_env.total);
								ipc_emb_env.read_index += ipc_emb_env.total;
								ipc_emb_env.receive_index += ipc_emb_env.total;								
						}
						else
						{
								memcpy(&ipc_emb_env.p_buffer[ipc_emb_env.receive_index], p_m3_ipc_share_env->m3_tx, p_m3_ipc_share_env->m3_send_length);
								ipc_emb_env.receive_index += p_m3_ipc_share_env->m3_send_length;
								ipc_emb_env.read_index += p_m3_ipc_share_env->m3_send_length;
						}
						
						if(ipc_emb_env.read_index == p_m3_ipc_share_env->m3_send_length)
						{
								ipc_emb_env.rx_busy = 0;
								cobra_ipc_m0_emb2app_trigger_set(IPC_M0_TX_ACK);    //ֻ��ȫ���������������,��֪ͨ M3  ����ȫ���յ���
						}
						
						if(ipc_emb_env.receive_index == ipc_emb_env.total) 
						{
								void (*callback) (void*, uint8_t) = NULL;
								void* data =NULL;
							
								//ipc_env.rx_busy = 0;
								callback = ipc_emb_env.rx.callback;
								data     = ipc_emb_env.rx.dummy;

								if(callback != NULL)
								{
										// Clear callback pointer
										ipc_emb_env.rx.callback = NULL;
										ipc_emb_env.rx.dummy    = NULL;

										// Call handler
									
										callback(data, RWIP_EIF_STATUS_OK);
								}
								
						}
										
			}
		
			
		}
					
				
				
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//M0
//�������ݵ� HOST ��
void app_rdw_ipc_emb_write(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
    ASSERT_ERR(bufptr != NULL);
    ASSERT_ERR(size != 0);
    ASSERT_ERR(callback != NULL);
    ipc_emb_env.tx.callback = callback;
    ipc_emb_env.tx.dummy    = dummy;

		if(ipc_emb_env.tx_busy) {
			 printf("error ---------------- \n");
			 return;
		}
		
			ipc_emb_env.p_tx_buffer = bufptr;
			ipc_emb_env.tx_total = size;
		  ipc_emb_env.tx_index = 0;
			ipc_emb_env.tx_busy = 1;

	  // printf("M0 send data ------->\n");
		
		 m0_ipc_share_env.m0_send_length = size;
		 printf("s = %d \n", size);
	
		 memcpy(m0_ipc_share_env.m0_tx, ipc_emb_env.p_tx_buffer, m0_ipc_share_env.m0_send_length);
	
	//����� ֪ͨ M3 Ҫ����������
			cobra_ipc_m0_emb2app_trigger_set(IPC_IRQ_M0_TO_M3);
			
}


//��ȡ M3 ���͹���������
void app_rdw_ipc_emb_read(uint8_t *bufptr, uint32_t size, void (*callback) (void*, uint8_t), void* dummy)
{
    // Sanity check
	  uint32_t len;
    ASSERT_ERR(bufptr != NULL);
    ASSERT_ERR(size != 0);
    ASSERT_ERR(callback != NULL);
    ipc_emb_env.rx.callback = callback;
    ipc_emb_env.rx.dummy    = dummy;
	
	
		if(ipc_emb_env.rx_busy)   //˵�����յ�buffer ���滹������
		{
						len = p_m3_ipc_share_env->m3_send_length - ipc_emb_env.read_index;  //Buf ���滹ʣ�µ����ݸ���
			
						if(len >= size)
						{
								memcpy(bufptr, &p_m3_ipc_share_env->m3_tx[ipc_emb_env.read_index], size);
								ipc_emb_env.read_index += size;
								ipc_emb_env.receive_index += size;	
							
								if(ipc_emb_env.read_index == p_m3_ipc_share_env->m3_send_length)
								{
										ipc_emb_env.rx_busy = 0;
										cobra_ipc_m0_emb2app_trigger_set(IPC_M0_TX_ACK);    //ֻ��ȫ���������������,��֪ͨ M3  ����ȫ���յ���
								}
						
						
								{
										void (*callback) (void*, uint8_t) = NULL;
										void* data =NULL;
									
										
										callback = ipc_emb_env.rx.callback;
										data     = ipc_emb_env.rx.dummy;

										if(callback != NULL)
										{
												// Clear callback pointer
												ipc_emb_env.rx.callback = NULL;
												ipc_emb_env.rx.dummy    = NULL;

												// Call handler
												callback(data, RWIP_EIF_STATUS_OK);
										}
								}
								
						}
						else   
						{
								// ˵��Buffer ��������ݲ�����һ�δ���
							  printf("Maybe have error .............. \n");
								memcpy(bufptr, &p_m3_ipc_share_env->m3_tx[ipc_emb_env.read_index], len);
								ipc_emb_env.receive_index += len;
								ipc_emb_env.read_index += len;
							
								if(ipc_emb_env.read_index == p_m3_ipc_share_env->m3_send_length)
								{
										ipc_emb_env.rx_busy = 0;
										cobra_ipc_m0_emb2app_trigger_set(IPC_M0_TX_ACK);    //ֻ��ȫ���������������,��֪ͨ M3  ����ȫ���յ���
								}
								
								ipc_emb_env.p_buffer = bufptr+len;
								ipc_emb_env.total = size - len;
							//	ipc_env.receive_index = 0;
								ipc_emb_env.rx_busy = 1;
								ipc_emb_env.read_index = 0;	
						}
						
		}
		else
		{
				ipc_emb_env.p_buffer = bufptr;
				ipc_emb_env.total = size;
				ipc_emb_env.receive_index = 0;
				ipc_emb_env.rx_busy = 1;
				ipc_emb_env.read_index = 0;
		}

	//��ʾ׼������ M3 ���͹���������
	
//	  rdw_config_uart_rx_interrupt(1);

}


void app_rdw_ipc_flow_on(void)
{
    // Configure modem (HW flow control enable)
    //uart_force_rts_setf(0);
		GLOBAL_INT_DISABLE();
	  ipc_emb_env.rx_busy = 0;
		ipc_emb_env.tx_busy = 0;
	  ipc_emb_env.tx_index = 0;
		ipc_emb_env.tx_total = 0;
		ipc_emb_env.rx.callback = NULL;
    ipc_emb_env.tx.callback = NULL;
	
		GLOBAL_INT_RESTORE();
	
		
}


bool app_rdw_ipc_flow_off(void)
{
    bool flow_off = true;
	
		GLOBAL_INT_DISABLE();
	  if(ipc_emb_env.rx_busy || ipc_emb_env.tx_busy)
			 flow_off = false; 
		GLOBAL_INT_RESTORE();

    return flow_off;
}


#endif











