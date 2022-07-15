/******************************************************************************
	File Name	:	app_autoconnect.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/2/29

	Description :   

	Changed Log	:
		Liujian  2020/2/29			- Creation

*****************************************************************************/
#include "app_autoconnect.h"


#if DIANBIAO_AUTO_ENABLE 	

#define PPPINITFCS16 	0xffff /* Initial FCS value */
#define PPPGOODFCS16 	0xf0b8 /* Good final FCS value */

#define MAX_DUANLUQI_NUMBER						10

// Ѱ�Ҷ�·���Ƿ�ɹ���
void iris_dianbiao_autoconnect_report_find_duanluqi_state(unsigned char error_number, unsigned char * p_mac_address);

unsigned int app_iris_autoconnect_get_sample_data(void);
int app_iris_autoconnect_scan_address(void);
void gudian_uart_module_ask_sample_data_cmd(void);
//�жϹ㲥�����Ƿ�Ϊ�ض���
unsigned int app_gudoan_autoconnect_advdata_paramet_process(unsigned char * buffer,  unsigned char length, unsigned short breakerValue);

void iris_dianbiao_autoconnect_report_start_state(unsigned char error_number);

static uint32_t timer_scan_timeout_index;

static uint32_t timer_scan_adv_record_index;

static unsigned int auto_scan_enable = 0; 
static unsigned int duanluqi_number = 0;
static unsigned int try_duanluqi_index = 0;
static struct gap_bdaddr duanluqi_array[MAX_DUANLUQI_NUMBER];  //��¼Ѱ�Ҷ�·���б�, ����������������Լ������������ݵ������ʹ��

static ADV_RESULT_INFORMATION adv_result_list[MAX_DUANLUQI_NUMBER];   //����������������Լ������������ݵ������ʹ��
static unsigned int adv_record_number = 0;

static unsigned int auto_connect_state = AUTO_CONNECT_STOP_STATE;

static int auto_connect_channel;   //  ���ڳ�������ʱ�õ��� channel
void app_iris_scan_timeout_function(unsigned int param);
void app_iris_send_modified_character_value(unsigned short value);

static unsigned short modified_value;


//�ȴ��������ݵ� timer_index
static uint32_t timer_sample_timeout_index;
static uint32_t time_sample_number;

unsigned short app_auutoconnect_698_produce_head_checksum(unsigned char * p_buffer, int len);

unsigned short app_iris_autoconnect_get_current_character(unsigned int length, unsigned char *ucDataBuf, \
							unsigned short * outDataP);

void app_iris_sample_data_timeout_function(unsigned int param);
//���Ķ�·�����������ķ�������
static const unsigned char iris_respond_modified_current[] = {
		0x68, 0x19, 0x00, 0xC3, 0x05, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
		0x00, 0x97, 0xD7, 0x86, 0x01, 0x00, 0xF2, 0x0B, 0x80, 0x00, 0x00,
		0x00, 0x00, 0x9D, 0x60, 0x16
};
//�µĸ���������2�������
static const unsigned char iris_repond_new_modified_current[] = {
     0x68, 0x1a, 0x00, 0xc3, 0x05, 
	 0xff, 0xff, 0xff, 0xff, 0xff, 
	 0xff, 0x00, 0xff, 0x70, 0x87, 
	 0x01, 0x00, 0x48, 0x05, 0x81,
	 0x00, 0x00, 0x00, 0x00, 0x00, 
	 0x56, 0x68, 0x16,
};

unsigned short duanluqi_character;

/*
��������ɨ�� ��Χ�Ķ�·��
ɨ��10s�Ӻ�, ����Χ�Ĺ㲥�����Ƿ����ض�����,
Ȼ���¼��һ���б���, ���10��

*/
void app_iris_scan_start(void)
{
		
		duanluqi_number = 0;
		memset(duanluqi_array, 0, MAX_DUANLUQI_NUMBER*sizeof(struct gap_bdaddr));
	  auto_scan_enable = 1;
//������ʱ��
		app_device1_stop_scan();
	
		timer_scan_timeout_index = cobra_soft_timer_create(app_iris_scan_timeout_function);
	
		cobra_soft_timer_start(timer_scan_timeout_index, 100*10, 0);   //20��
	
		auto_connect_state = AUTO_CONNECT_START_STATE;
	
		app_device1_autofind_duanluqi_start_scan();
	
		printf("app_iris_scan_start \n");
}

/*
����ɨ���, ���б����Ƿ��з��������Ķ�·��,
�о�ѡ���������е�һ��

*/
void app_iris_scan_timeout_function(unsigned int param)
{
		auto_scan_enable = 0;
		app_device1_stop_scan();
		cobra_soft_timer_stop(timer_scan_timeout_index);
		cobra_soft_timer_destroy(timer_scan_timeout_index);
		
		printf("duanluqi_number = %d ..... \n", duanluqi_number);
		app_device1_start_scan();
	
		if(duanluqi_number == 0)   //û�����赽�κζ�·��
		{
			 iris_dianbiao_autoconnect_report_find_duanluqi_state(0x04, NULL);  //����Ѱ��ʧ��, ʧ�ܴ���Ϊ 0x04
			 auto_connect_state = AUTO_CONNECT_STOP_STATE;
		}
		else
		{
			//��ʼ�����赽�ķ��������Ķ�·����ѡ��һ����ʼ����
			 try_duanluqi_index = 0;
			 auto_connect_state = AUTO_CONNECT_TRY_STATE;
			 auto_connect_channel = app_iris_autoconnect_scan_address();
			 if(auto_connect_channel < 0)
			 {
					iris_dianbiao_autoconnect_report_find_duanluqi_state(0x04, NULL);
					auto_connect_state = AUTO_CONNECT_STOP_STATE;
				 
			 }
		}
}

/*
��ʼ�Զ��������赽�����з��������Ķ�·����ַ
*/
int app_iris_autoconnect_scan_address(void)
{
		if(try_duanluqi_index < duanluqi_number)
		{
				if(!app_device_continue_scan_duanluqi_address1())   //�����·��û�б�����, ��Ҫ�쿴�Ƿ���Ҫ����
				{												
							app_device_enable_scan_duanluqi1_address(1);							
							printf("\n �������� 1\n");						
							app_device1_connection_start(duanluqi_array[try_duanluqi_index]);		
							try_duanluqi_index++;
							return CONNECT_CHANNEL1;
				}
				else if(!app_device_continue_scan_duanluqi_address2())   //�����·��û�б�����, ��Ҫ�쿴�Ƿ���Ҫ����
				{												
							app_device_enable_scan_duanluqi2_address(1);							
							printf("\n �������� 2\n");						
							app_device2_connection_start(duanluqi_array[try_duanluqi_index]);		
							try_duanluqi_index++;
							return CONNECT_CHANNEL2;
				}
				else if(!app_device_continue_scan_duanluqi_address3())   //�����·��û�б�����, ��Ҫ�쿴�Ƿ���Ҫ����
				{												
							app_device_enable_scan_duanluqi3_address(1);							
							printf("\n �������� 3\n");						
							app_device3_connection_start(duanluqi_array[try_duanluqi_index]);	
							try_duanluqi_index++;
							return CONNECT_CHANNEL3;
				}
				else
				{
							//˵��û�а취��������, ��Ϊ�������Ӷ��Ѿ�����������
							return -1;
				}
		}
		else
		{
						printf("�����������ж�·����ַ �� \n");
						cobra_soft_timer_stop(timer_sample_timeout_index);
						cobra_soft_timer_destroy(timer_sample_timeout_index);
						return -1;
		}
}

//����channel ����״̬
//index �����Լ�����һ��ͨ��, һ��ֻ������ͨ��
//����һ���ص�����, ������һ�����Ӻ�, ��������Զ�Ѱ�Ҷ�·���Ĺ�����, ���͸�����������������·��(Ҳ���ǵ�)
void app_iris_attconnect_report_try_state(unsigned int index, unsigned int state)
{
		if(auto_connect_state == AUTO_CONNECT_TRY_STATE)
		{
				if(state == CONNECT_FAIL)
				{
						 auto_connect_channel = app_iris_autoconnect_scan_address();
						 if(auto_connect_channel < 0)
						 {
								iris_dianbiao_autoconnect_report_find_duanluqi_state(0x04, NULL);//
								auto_connect_state = AUTO_CONNECT_STOP_STATE;		 
						 }
				}
				else
				{
					    //���ӳɹ���, ׼�����͸�����������ֵָ����Է�
					    printf("\n\n\n���ӳɹ���, ׼�����͸�����������ֵָ����Է� \n\n\n");
						modified_value =rand();
						app_iris_send_modified_character_value(modified_value);
				}
		}
	
}

void app_iris_sample_data_timeout_function(unsigned int param)
{
		printf("8s ��û�н��յ�����͹��������� \n");
	  //���·���
		time_sample_number++;
		if(time_sample_number < 3)
		{
				//��ʱ, ���·��ʹ��� ���Ͳ���ָ�������о
				gudian_uart_module_ask_sample_data_cmd();
				cobra_soft_timer_start(timer_sample_timeout_index, 100*40, 0);
		}
		else
		{
			  //���淢��Ѱ��ʧ��
			  // ���Ѱ�Ҷ�û��, Ҳ���Ƕ�γ�ʱ
				iris_dianbiao_autoconnect_report_find_duanluqi_state(0x02, NULL);
				auto_connect_state = AUTO_CONNECT_STOP_STATE;
				cobra_soft_timer_stop(timer_sample_timeout_index);
				cobra_soft_timer_destroy(timer_sample_timeout_index);
			
				if(auto_connect_channel == CONNECT_CHANNEL1)
				{
						app_device1_disconnect();
				}
				else if(auto_connect_channel == CONNECT_CHANNEL2)
				{
						app_device2_disconnect();
				}
				else if(auto_connect_channel == CONNECT_CHANNEL3)
				{
						app_device3_disconnect();
				}
						
		}
}

/*
2020/03/10
liujian ����, �յ���·���Ļظ���, �ȴ������ٷ��� 04 00 00 02
*/
static uint32_t timer_ask_delay_index;

void app_iris_ask_delay_timer_function(unsigned int param)
{
			cobra_soft_timer_stop(timer_ask_delay_index);
			
			cobra_soft_timer_destroy(timer_ask_delay_index);
	
			timer_sample_timeout_index = cobra_soft_timer_create(app_iris_sample_data_timeout_function);
			
			cobra_soft_timer_start(timer_sample_timeout_index, 100*40, 0);   //8��

			time_sample_number = 0;
			printf("timer_sample_timeout_index .... \n");
			gudian_uart_module_ask_sample_data_cmd();
	
}
/*
Ҫ����·�����͹����������Ƿ��� ������������ֵ������ķ���
*/
unsigned int  app_iris_is_modified_value(uint8_t length,  uint8_t *p_buffer)
{
	//iris_respond_modified_current
	if(auto_connect_state == AUTO_CONNECT_TRY_STATE)
	{
		  if(length < ((sizeof(iris_respond_modified_current)/sizeof(unsigned char)) + 4))
				return 0;
			if(!memcmp(p_buffer+4, iris_respond_modified_current, sizeof(iris_respond_modified_current)/sizeof(unsigned char)))
			{
				//˵���Ƿ��ظ��Ķ�·������������
				//���ʹ������²���ָ��
				//������ʱ

			//������ʱ��, �ȴ������ٷ��� 04 00 00 02 ����	//�ȴ�����
				timer_ask_delay_index = cobra_soft_timer_create(app_iris_ask_delay_timer_function);				
				cobra_soft_timer_start(timer_ask_delay_index, 100*3, 0);  								
				return 1;
			}
			else if(!memcmp(p_buffer+4, iris_repond_new_modified_current, sizeof(iris_repond_new_modified_current)/sizeof(unsigned char)))
			{
				timer_ask_delay_index = cobra_soft_timer_create(app_iris_ask_delay_timer_function);				
				cobra_soft_timer_start(timer_ask_delay_index, 100*3, 0);  
				return 1;
			}	
	}
	return 0;

	
}

#define TRUE      1
#define FALSE     0
//ʶ�� �������ݵõ���·������ֵ
//����   length Ϊbuffer �ĳ���,
//       p_buffer Ϊ����о���͹����Ĳ�������buffer
//����ֵ, Ϊ 0 ��ʾ���ܵõ���ȷ������ֵ
//        Ϊ 1 ��ʾ�õ�����ȷ������ֵ
//        return_value Ϊ�õ�������ֵ

static unsigned int uiDataBuf[256];

unsigned short app_iris_autoconnect_get_current_character(unsigned int length, unsigned char *ucDataBuf, \
							unsigned short * outDataP)
{
    unsigned int i, j, startPoint, endPoint, levelLength, levelBitNumber, bitNumber,bitNumberH,iStartBit,iHead;
    unsigned char sub,status = 0; 
    bool edge = 0,retValue = FALSE;
    unsigned int outBuf, parity;
    
    unsigned int minValue = 0xFFFF, maxValue = 0;
    unsigned int riseEdgeLimit, dorpEdgeLimit;
    
    iHead = 0;
	
    if(length == 2)
    {
        retValue = TRUE;
        *outDataP = ucDataBuf[0] + (ucDataBuf[1] << 8);
    }
    for(i=0;i<length/2;i++)
    {
        uiDataBuf[i] = (ucDataBuf[2*i]<<8) + ucDataBuf[2*i+1];
        if(minValue > uiDataBuf[i])                             //Ѱ����Сֵ
        {
            minValue = uiDataBuf[i];
        }
        
        if(maxValue < uiDataBuf[i])                             //Ѱ�����ֵ
        {
            maxValue = uiDataBuf[i];
        }
    }
    
    for(i=0;i<length/2;i++)                                     //��ȥ����ƫ����
    {
        uiDataBuf[i] = uiDataBuf[i] - minValue;
    }
    riseEdgeLimit = (maxValue - minValue) * 3 / 5;
    dorpEdgeLimit = (maxValue - minValue) / 3;
    
    //��ʼ������
    startPoint = 0;                 //��ƽ��ʼ��        
    endPoint = 0;                   //��ƽ������
    levelLength = 0;                //��ƽ����ʱ��
    i=0;                            //�����������

    while(i < (length / 2))                       
    {
        if(retValue == TRUE)                //����ֵΪ�棬��ʶ����������ݡ�����ѭ��
        {
            break;
        }
        
        if(edge == 0)                   //��ǰ��ƽΪ��
        {
            if((uiDataBuf[i] >= riseEdgeLimit) || (i == length/2 - 1))
            {
                edge = 1;
                endPoint = i;
                levelLength = endPoint - startPoint;
                startPoint = endPoint;
            }
        }
        else                            //��ǰ��ƽΪ��
        {
            if((uiDataBuf[i] < dorpEdgeLimit) || (i == length/2 - 1))
            {
                edge = 0;
                endPoint = i;
                levelLength = endPoint - startPoint;
                startPoint = endPoint;
            }
        }

        if(levelLength)                                 //������س���
        {
            switch(status)
            {
                case 0:                                 //Ѱ�ҿ���ʱ��
                {
                    if(levelLength>=32)                 //6λ����ʱ��
                        
                    {
                        status = 1;                     //�����ȡ����λ״̬
                        bitNumber = 0;                  //�Ѷ�ȡ����Ϊ����
                        outBuf = 0;                     //�����������
                        iStartBit = i;                      //��ʼλ��ʼ��
                        iHead = i - levelLength;       //����ͷ��ʼ��
                    }
                    break;
                }
                case 1:                                 //��ȡ����
                {
                    levelBitNumber = levelLength / 6;   //��ǰ��ƽ�����е�����λ
                    if(levelLength % 6 > 3)             //�в�����ȱʧ
                    {
                        levelBitNumber += 1;            //��ǰ��ƽ����λ��1
                    }
                    
                    for(j=0;j<levelBitNumber;j++)       //��ȡ����λ���������
                    {
                        if(edge == 1)
                        {
                            outBuf |= 0x0001 << (j + bitNumber);
                        }
                    }
                    bitNumber += levelBitNumber;        //��������ȡ����λ
                    
                    if(bitNumber >= 23)                 //�ѽ���������ȫ
                    {
                        status = 2;
                    }
                    else
                    {
                        break;
                    }
                }
                case 2:
                {
                    outBuf >>= 1;                           //�Ƴ���ʼλ
                    for(j=0;j<4;j++)                        //����λ���飬����һλ�෴
                    {
                        if(((outBuf>>(j*5+3)) & 0x00000001) == ((outBuf>>(j*5+4)) & 0x00000001))
                        {
                            break;
                        }
                    }
                    if(j<4)                                 //����λ��֤
                    {
                        status = 0;
                        i = iStartBit;                      //��֤���󣬼���Ѱ�����ݿ���ʱ��
                        break;
                    }
                    //outBuf = ~outBuf;
                    parity = 0;                             //У��λ��֤
                    for(j=0;j<21;j++)
                    {
                        if(outBuf & (0x0001<<j))
                        {
                            parity++;
                        }
                    }
                
                    if((parity % 2))                        //У�飬��������1
                    {
                        *outDataP = outBuf & 0x000F;        //���������ȡ��4λ
                        outBuf = outBuf>>1;
                        *outDataP |= outBuf & 0x00F0;       //���������ȡ��4λ
                        outBuf = outBuf>>1;
                        *outDataP |= outBuf & 0x0F00;       //���������ȡ��4λ
                        outBuf = outBuf>>1;
                        *outDataP |= outBuf & 0xF000;       //���������ȡ��4λ
                        retValue = TRUE;  
                    }
                    else
                    {
                        i = iStartBit;                      //У����󣬼���Ѱ�����ݿ���ʱ��
                    }
                    status = 0;
                    break;
                }
                default:
                    break;
            }
            
            levelLength = 0;
        }
        i++;                                                    //��һ��������
    }
    
    if(retValue == FALSE)                //����ֵΪ�棬��ʶ����������ݡ�����ѭ��
    {
        i = iHead;
        startPoint = i;
        bitNumberH = 0;
    }
    while(i>0)
    {
        
        if(edge == 0)                   //��ǰ��ƽΪ��
        {
            if((uiDataBuf[i] > riseEdgeLimit) || (i == 1))
            {
                edge = 1;
                endPoint = i;
                levelLength = startPoint - endPoint;
                startPoint = endPoint;
            }
        }
        else                            //��ǰ��ƽΪ��
        {
            if((uiDataBuf[i] < dorpEdgeLimit) || (i == 1))
            {
                edge = 0;
                endPoint = i;
                levelLength = startPoint - endPoint;
                startPoint = endPoint;
            }
        }
        
        if(levelLength)                                 //������س���
        {
            switch(status)
            {
                case 0:                                 //Ѱ�ҿ���ʱ��
                {
                    if(levelLength>=32)                 //6λ����ʱ��
                    {
                        status = 1;                     //�����ȡ����λ״̬
                        bitNumber = 0;                  //�Ѷ�ȡ����Ϊ����
                        outBuf = 0;                     //�����������
                        iStartBit = i;                      //��¼��ǰλ��
                    }
                    break;
                }
                case 1:                                 //��ȡ����
                {
                    levelBitNumber = levelLength / 6;   //��ǰ��ƽ�����е�����λ
                    if(levelLength % 6 > 3)             //�в�����ȱʧ
                    {
                        levelBitNumber += 1;            //��ǰ��ƽ����λ��1
                    }
                    
                    for(j=0;j<levelBitNumber;j++)       //��ȡ����λ���������
                    {
                        if(edge == 1)
                        {
                            outBuf |= 0x400000 >> (bitNumberH + j);
                        }
                    }
                    bitNumberH += levelBitNumber;
                    bitNumber += levelBitNumber;        //��������ȡ����λ
                    
                    if(bitNumber >= 23)                 //�ѽ���������ȫ
                    {
                        status = 2;
                    }
                    else
                    {
                        break;
                    }
                }
                case 2:
                {
                    outBuf >>= 1;                           //�Ƴ���ʼλ
                    for(j=0;j<4;j++)                        //����λ���飬����һλ�෴
                    {
                        if(((outBuf>>(j*5+3)) & 0x00000001) == ((outBuf>>(j*5+4)) & 0x00000001))
                        {
                            break;
                        }
                    }
                    if(j<4)                                 //����λ��֤
                    {
                        status = 0;
                        i = iStartBit;                          //��֤���󣬼���Ѱ�����ݿ���ʱ��
                        break;
                    }
                    //outBuf = ~outBuf;
                    parity = 0;                             //У��λ��֤
                    for(j=0;j<21;j++)
                    {
                        if(outBuf & (0x0001<<j))
                        {
                            parity++;
                        }
                    }
                
                    if((parity % 2))                        //У�飬��������1
                    {
                        *outDataP = outBuf & 0x000F;        //���������ȡ��4λ
                        outBuf = outBuf>>1;
                        *outDataP |= outBuf & 0x00F0;       //���������ȡ��4λ
                        outBuf = outBuf>>1;
                        *outDataP |= outBuf & 0x0F00;       //���������ȡ��4λ
                        outBuf = outBuf>>1;
                        *outDataP |= outBuf & 0xF000;       //���������ȡ��4λ
                        retValue = TRUE;                    //����ֵ����ʶ�����������
                    }
                    else
                    {
                        i = iStartBit;                          //У����󣬼���Ѱ�����ݿ���ʱ��
                    }
                    status = 0;
                    break;
                }
                default:
                    break;
            }
            
            levelLength = 0;
        }
        i--;
    }
    printf("Out Value:%x , retValue = %d \n",*outDataP, retValue);
    return retValue;
}

//�������ɨ���·���й㲥���ݷ����ض������Ĺ㲥��
static unsigned int report_adv_data_flag = 0;

unsigned int app_iris_autoconnect_get_report_flag(void)
{
		return report_adv_data_flag;
}

static void app_iris_adv_report_mac_and_current(void)
{
				unsigned char checksum, i;
			  unsigned short length;
				length = 8 * adv_record_number + 4;
	
		    cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
		
				cobra_uart_write(M3_UART0, 0x68);
				checksum = 0x68;
				cobra_uart_write(M3_UART0, length);
				cobra_uart_write(M3_UART0, (length >> 8)&0xff);
				checksum += length&0xff;
				checksum += (length >> 8)&0xff;
			
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x00);
				checksum += 0x00;
				checksum += 0x00;
				
				cobra_uart_write(M3_UART0, 0x68);
				checksum += 0x68;
				cobra_uart_write(M3_UART0, 0x84);
				checksum += 0x84;
					
				cobra_uart_write(M3_UART0, 0x04);
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x08);
				
				checksum += 0x04;
				checksum += 0x00;
				checksum += 0x00;
				checksum += 0x08;
				
				for(i = 0; i < adv_record_number; i++)
				{
						cobra_uart_write(M3_UART0, adv_result_list[i].mac_adress.addr.addr[0]);
						cobra_uart_write(M3_UART0, adv_result_list[i].mac_adress.addr.addr[1]);
					  cobra_uart_write(M3_UART0, adv_result_list[i].mac_adress.addr.addr[2]);
					  cobra_uart_write(M3_UART0, adv_result_list[i].mac_adress.addr.addr[3]);
						cobra_uart_write(M3_UART0, adv_result_list[i].mac_adress.addr.addr[4]);
					  cobra_uart_write(M3_UART0, adv_result_list[i].mac_adress.addr.addr[5]);
					
						cobra_uart_write(M3_UART0, (adv_result_list[i].character_code&0xff));
						cobra_uart_write(M3_UART0, (adv_result_list[i].character_code>>8)&0xff);
					
						checksum += adv_result_list[i].mac_adress.addr.addr[0];
					  checksum += adv_result_list[i].mac_adress.addr.addr[1];
					  checksum += adv_result_list[i].mac_adress.addr.addr[2];
					  checksum += adv_result_list[i].mac_adress.addr.addr[3];
					  checksum += adv_result_list[i].mac_adress.addr.addr[4];
					  checksum += adv_result_list[i].mac_adress.addr.addr[5];
						checksum += adv_result_list[i].character_code&0xff;
						checksum += (adv_result_list[i].character_code>>8)&0xff;
					
				}
				cobra_uart_write(M3_UART0, checksum&0xFF);
				cobra_uart_write(M3_UART0, 0x16);
	
}

//void app_iris_scan_timeout_function(unsigned int param)
static void app_iris_adv_record_timeout_function(unsigned int param)
{
		report_adv_data_flag = 0;
		app_device1_stop_scan();
		cobra_soft_timer_stop(timer_scan_adv_record_index);
		cobra_soft_timer_destroy(timer_scan_adv_record_index);
		app_device1_start_scan();
//	  printf("adv_record_number = %d \n", adv_record_number);
	  app_iris_adv_report_mac_and_current();
		
}


void app_iris_autoconnect_get_adv_record(void)
{		
	  if(report_adv_data_flag) return;
		report_adv_data_flag = 1;
		adv_record_number = 0;
		memset(adv_result_list, 0, MAX_DUANLUQI_NUMBER*sizeof(ADV_RESULT_INFORMATION));
//������ʱ��
		app_device1_stop_scan();
	
		timer_scan_adv_record_index = cobra_soft_timer_create(app_iris_adv_record_timeout_function);
	
		cobra_soft_timer_start(timer_scan_adv_record_index, 100*10, 0);   //10��
	
		app_device1_autofind_duanluqi_start_scan();	
}



//���ڽ��յ�����������ָ�����ݲɼ�
void app_iris_autoconnect_process_sample_data(unsigned short length, unsigned char *p_buffer)
{
		
		unsigned short current_character = 0;  
		unsigned short ret;

	//���������Ѱ�Ҷ�·��ʱ�õ������� ��ô�����κδ���
		if(auto_connect_state != AUTO_CONNECT_TRY_STATE)
				return;
		
#if 0	
		unsigned char i;
	  for(i = 0; i < length; i++)
		{
				printf("0x%02x ", p_buffer[i]);
		}
#endif
		
		//Ҫʶ��������� ???  ����Ҫ�������
		//���ʶ�����ȷ��������
		printf("length = %d \n", length);
		ret = app_iris_autoconnect_get_current_character(length-4, p_buffer + 4, &current_character); 

		if(ret ==  0)  //���û�еõ���ȷ��������
		{
				//���ڷ������²���ָ��
				gudian_uart_module_ask_sample_data_cmd();
			
		}
		else
		{
				cobra_soft_timer_stop(timer_sample_timeout_index);
				cobra_soft_timer_destroy(timer_sample_timeout_index);
				if(current_character == modified_value)
				{
					//����Ѱ�ҳɹ� 
						iris_dianbiao_autoconnect_report_find_duanluqi_state(0x00, duanluqi_array[try_duanluqi_index-1].addr.addr);
						auto_connect_state = AUTO_CONNECT_STOP_STATE;
					//���úö�·�����ӵĵ�ַ
						if(auto_connect_channel == CONNECT_CHANNEL1)
						{
								app_set_new_duanluqi1_address(duanluqi_array[try_duanluqi_index-1].addr.addr);
						}
						else if(auto_connect_channel == CONNECT_CHANNEL2)
						{
								app_set_new_duanluqi2_address(duanluqi_array[try_duanluqi_index-1].addr.addr);
						}
						else if(auto_connect_channel == CONNECT_CHANNEL3)
						{
								app_set_new_duanluqi3_address(duanluqi_array[try_duanluqi_index-1].addr.addr);
						}
				}
				else
				{
					  //��Ҫ�Ͽ���������
						if(auto_connect_channel == CONNECT_CHANNEL1)
						{
								app_device1_disconnect();
						}
						else if(auto_connect_channel == CONNECT_CHANNEL2)
						{
								app_device2_disconnect();
						}
						else if(auto_connect_channel == CONNECT_CHANNEL3)
						{
								app_device3_disconnect();
						}
				}
		}
}

/*
ͨ������
���յ��Զ���������ָ��

*/
void app_iris_autoconnect_process(unsigned short length, unsigned char * p_buffer)
{
		//unsigned short duanluqi_character;
		unsigned short ret;
	
#if 0	
		unsigned char i;
	  for(i = 0; i < length; i++)
		{
				printf("0x%02x ", p_buffer[i]);
		}
#endif
		
		if(app_iris_auto_connect_state())
		{
			 //˵��Ŀǰ�Ѿ�����Ѱ�Ҷ�·��״̬�� ��ôֹͣ��������
				return ;
		}
		
		printf("length1 = %d \n", length);
		
		ret = app_iris_autoconnect_get_current_character(length, p_buffer, &duanluqi_character);
		if(ret)
		{
				//�ظ������ɹ�
				iris_dianbiao_autoconnect_report_start_state(0x00);
				app_iris_scan_start();
		}	
		else
		{
			   //�ظ�����ʧ��
					iris_dianbiao_autoconnect_report_start_state(0x02);
					return;
		}
}

//��¼ɨ�赽�Ķ�·�����б�, �����ص�Ķ�·��
void app_iris_recode_adv_data(struct gapm_ext_adv_report_ind* p_param)
{
	    unsigned char * p = p_param->data;
		unsigned char adv_length;
		unsigned char adv_character;
	    unsigned char temp_length = 0;
	    unsigned char length = p_param->length;
		unsigned char find_flag = 0;
		unsigned char find_ok = 0;
	    unsigned int i;
	    unsigned short temp_character;  //��ʱ��������
		while((p != NULL)&&(temp_length < length))
		{
				adv_length = *p++;
				adv_character = *p;
				if(adv_character != 0xFF)
				{
					p += adv_length;
				}
				else
				{
					 if(*(p+3) == 0xC4)
					 {
						  find_ok = 1;
						  temp_character = p[5];
						  temp_character <<= 8;
						  temp_character += p[4];
						  break;
					 }
				}
				temp_length ++;
				temp_length += adv_length;
		}
		if(find_ok)
		{
			find_flag = 0;
			for(i = 0; i < adv_record_number; i++)
			{
					if(!memcmp((void *)&adv_result_list[i].mac_adress, (void *)&(p_param->trans_addr), sizeof(struct gap_bdaddr)))
					{
							find_flag = 1;   //�б����Ѿ����ڷ��������Ķ�·���ĵ�ַ��
						    break;
					}
			}
			if(!find_flag)
			{
					if(adv_record_number < MAX_DUANLUQI_NUMBER)
					{
							memcpy((void *)&adv_result_list[adv_record_number].mac_adress, (void *)&(p_param->trans_addr), sizeof(struct gap_bdaddr));
							adv_result_list[adv_record_number].character_code = temp_character;
							adv_record_number++;
					}
			}
		}
}


unsigned int app_iris_get_scan_mode(void)
{
		return auto_scan_enable;
}

//��������Զ�Ѱ�Ҷ�·����ʱ��,Ѱ�ҷ����ض�������Ķ�·��  duanluqi_character
void app_iris_process_adv_data(struct gapm_ext_adv_report_ind* p_param)
{
		unsigned int i;
		unsigned int find_flag;
		if(auto_scan_enable)
		{
			  if(app_gudoan_autoconnect_advdata_paramet_process(p_param->data, p_param->length, duanluqi_character))
				{
						printf("app_gudoan_autoconnect_advdata_paramet_process ..... \n");
						find_flag = 0;
						for(i = 0; i < duanluqi_number; i++)
						{
								if(!memcmp((void *)&duanluqi_array[i], (void *)&(p_param->trans_addr), sizeof(struct gap_bdaddr)))
								{
										find_flag = 1;   //�б����Ѿ����ڷ��������Ķ�·���ĵ�ַ��
									  break;
								}
						}
						if(!find_flag)
						{
								if(duanluqi_number < MAX_DUANLUQI_NUMBER)
								{
										memcpy((void *)&duanluqi_array[duanluqi_number], (void *)&(p_param->trans_addr), sizeof(struct gap_bdaddr));
										duanluqi_number++;
								}
						}
				}
		}
	
}

//�жϵ���Ƿ����Զ���������ģ��״̬
unsigned int app_iris_auto_connect_state(void)
{
		if(auto_connect_state != AUTO_CONNECT_STOP_STATE)
		{			
				return 1;
		}
		return 0;
			
}


//�жϹ㲥�����Ƿ�Ϊ�ض���
unsigned int app_gudoan_autoconnect_advdata_paramet_process(unsigned char * buffer,  unsigned char length, unsigned short breakerValue)
{
		unsigned char * p = buffer;
		unsigned char adv_length;
		unsigned char adv_character;
	  unsigned char temp_length = 0;
		while((p != NULL)&&(temp_length < length))
		{
				adv_length = *p++;
				adv_character = *p;
				if(adv_character != 0xFF)
				{
						p += adv_length;
				}
				else
				{
/*
	 | ����   |  ����  |  ���̴���   |  ��·���豸����� |   ��·���������ֵ |
   | 0x09   | 0xFF   |  0xFF 0xFF  |   0xC4            |   0xE2 0x12        |
*/					
					 if((*(p+3) == 0xC4)&&(breakerValue == p[4] + (p[5]<<8)))
					 {
						   return 1;
					 }
				}
				temp_length ++;
				temp_length += adv_length;
		}
		return 0;
	
}


static const unsigned char new_change_current_cmd[] = 
{
//    0x68, 0x1b, 0x00, 0xc3, 0x05, 
//	0x89, 0x89, 0x89, 0x89, 0x89, 
//	0x89, 0x00, 0xb5, 0x7c, 0x06, 
//	0x01, 0x00, 0xf2, 0x0b, 0x80,
//	0x00, 0x09, 0x02, 0x01, 0x12,
//	0x00, 0x4b, 0xff, 0x16,
	
	0x68, 0x1b, 0x00, 0x43, 0x05, 
	0xff, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0x00, 0xb9, 0xe1, 0x07, 
	0x01, 0x00, 0x48, 0x05, 0x81,
	0x00, 0x09, 0x02, 0x01, 0x12, 
	0x00, 0x8c, 0xfd, 0x16,
};

void app_iris_send_modified_character_value(unsigned short value)
{
		unsigned short frame_hcs;
		unsigned char temp_data[29];
	
	   memcpy(temp_data, new_change_current_cmd, 29);
/*		
		temp_data[0] = 0x68;
		temp_data[1] = 0x1B;
		temp_data[2] = 0x00;
		temp_data[3] = 0xC3;
		temp_data[4] = 0x05;
	
  	    temp_data[5] = 0x89;
		temp_data[6] = 0x89;
		temp_data[7] = 0x89;
		temp_data[8] = 0x89;
		temp_data[9] = 0x89;
		temp_data[10] = 0x89;
	
		temp_data[11] = 0x00;
		temp_data[12] = 0xB5;
		temp_data[13] = 0x7C;
		temp_data[14] = 0x06;
		temp_data[15] = 0x01;
		temp_data[16] = 0x00;
		temp_data[17] = 0xF2;
		temp_data[18] = 0x0B;
		temp_data[19] = 0x80;
		temp_data[20] = 0x00;
		temp_data[21] = 0x09;
		temp_data[22] = 0x02;
*/		
    printf("value =0x%04x \n", value);
		temp_data[23] = value&0xff;
		temp_data[24] = (value >> 8)&0xff;
		
//		temp_data[25] = 0x00;
		
		frame_hcs = app_auutoconnect_698_produce_head_checksum(&temp_data[1], 0x1B - 2);
		
		temp_data[26] = frame_hcs&0xff;
		temp_data[27] = (frame_hcs >> 8)&0xFF;
		
//		temp_data[26] = 0x4B;
//		temp_data[27] = 0xFF;
		temp_data[28] = 0x16;
		
		if(auto_connect_channel == CONNECT_CHANNEL1)
		{
				app_device1_send_data_to_uart_server(sizeof(temp_data), temp_data);
		}
		else if(auto_connect_channel == CONNECT_CHANNEL2)
		{
				app_device2_send_data_to_uart_server(sizeof(temp_data), temp_data);
		}
		else if(auto_connect_channel == CONNECT_CHANNEL3)
		{
				app_device3_send_data_to_uart_server(sizeof(temp_data), temp_data);
		}
		
}

// Ѱ�Ҷ�·���Ƿ�ɹ���
void iris_dianbiao_autoconnect_report_find_duanluqi_state(unsigned char error_number, unsigned char * p_mac_address)
{
		unsigned short checksum = 0;
	
		if(error_number != 0x00)  
		{
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
		
				cobra_uart_write(M3_UART0, 0x68);
				checksum = 0x68;
				cobra_uart_write(M3_UART0, 0x05);
				cobra_uart_write(M3_UART0, 0x00);
				checksum += 0x05;
				checksum += 0x00;
			
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x00);
				checksum += 0x00;
				checksum += 0x00;
				
				cobra_uart_write(M3_UART0, 0x68);
				checksum += 0x68;
				cobra_uart_write(M3_UART0, 0x84);
				checksum += 0x84;
					
				cobra_uart_write(M3_UART0, 0x04);
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x03);
				
				checksum += 0x04;
				checksum += 0x00;
				checksum += 0x00;
				checksum += 0x03;
				
				cobra_uart_write(M3_UART0, error_number);
				checksum += error_number;
				
				cobra_uart_write(M3_UART0, checksum&0xFF);
				cobra_uart_write(M3_UART0, 0x16);
		}
		else
		{
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
				cobra_uart_write(M3_UART0, 0xFE);
		
				cobra_uart_write(M3_UART0, 0x68);
				checksum = 0x68;
				cobra_uart_write(M3_UART0, 0x0B);
				cobra_uart_write(M3_UART0, 0x00);
				checksum += 0x0B;
				checksum += 0x00;
			
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				cobra_uart_write(M3_UART0, 0x89);
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				checksum += 0x89;
				
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x00);
				checksum += 0x00;
				checksum += 0x00;
				
				cobra_uart_write(M3_UART0, 0x68);
				checksum += 0x68;
				cobra_uart_write(M3_UART0, 0x84);
				checksum += 0x84;
					
				cobra_uart_write(M3_UART0, 0x04);
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x00);
				cobra_uart_write(M3_UART0, 0x03);
				
				checksum += 0x04;
				checksum += 0x00;
				checksum += 0x00;
				checksum += 0x03;
				
				cobra_uart_write(M3_UART0, 0x00);
				checksum += 0x00;
				
				cobra_uart_write(M3_UART0, *p_mac_address);
				cobra_uart_write(M3_UART0, *(p_mac_address + 1));
				cobra_uart_write(M3_UART0, *(p_mac_address + 2));
				cobra_uart_write(M3_UART0, *(p_mac_address + 3));
				cobra_uart_write(M3_UART0, *(p_mac_address + 4));
				cobra_uart_write(M3_UART0, *(p_mac_address + 5));
				
				checksum += *p_mac_address;
				checksum += *(p_mac_address + 1);
				checksum += *(p_mac_address + 2);
				checksum += *(p_mac_address + 3);
				checksum += *(p_mac_address + 4);
				checksum += *(p_mac_address + 5);
				
				cobra_uart_write(M3_UART0, checksum&0xFF);
				cobra_uart_write(M3_UART0, 0x16);
		}
}

// ���� �Ƿ�ɹ��� ������
void iris_dianbiao_autoconnect_report_start_state(unsigned char error_number)
{
			unsigned short checksum = 0;
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
	
			cobra_uart_write(M3_UART0, 0x68);
			checksum = 0x68;
	
			cobra_uart_write(M3_UART0, 0x05);
			cobra_uart_write(M3_UART0, 0x00);
			checksum += 0x05;
			checksum += 0x00;
	
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			checksum += 0x89;
			checksum += 0x89;
			checksum += 0x89;
			checksum += 0x89;
			checksum += 0x89;
			checksum += 0x89;
	
			
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			checksum += 0x00;
			checksum += 0x00;

			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x84);
			
			cobra_uart_write(M3_UART0, 0x04);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x01);
			
			checksum += 0x68;
			checksum += 0x84;
			
			checksum += 0x04;
			checksum += 0x00;
			checksum += 0x00;
			checksum += 0x01;
			
			
			cobra_uart_write(M3_UART0, error_number);   //������ 
			
			
			checksum += error_number;
			
			cobra_uart_write(M3_UART0, checksum&0xFF);
			cobra_uart_write(M3_UART0, 0x16);
			
}

// 2020/3/2 
// ����ģ��Ҫ�����²���
// ���͸�����о

void gudian_uart_module_ask_sample_data_cmd(void)
{
			unsigned int i;
			unsigned short toatl_length;
			unsigned short checksum = 0;
	
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			cobra_uart_write(M3_UART0, 0xFE);
			
			cobra_uart_write(M3_UART0, 0x68);
	
			cobra_uart_write(M3_UART0, 0x06);	
			cobra_uart_write(M3_UART0, 0x00);
	
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
			cobra_uart_write(M3_UART0, 0x89);
	
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			
			cobra_uart_write(M3_UART0, 0x68);
			cobra_uart_write(M3_UART0, 0x84);
			
			cobra_uart_write(M3_UART0, 0x04);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x00);
			cobra_uart_write(M3_UART0, 0x02);
			
			cobra_uart_write(M3_UART0, 0xFF);
			cobra_uart_write(M3_UART0, 0xFF);
	
			cobra_uart_write(M3_UART0, 0xE6);
			cobra_uart_write(M3_UART0, 0x16);
}

static unsigned short fcstab[256] = 
{
		0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
		0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
		0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
		0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
		0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
		0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
		0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
		0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
		0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
		0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
		0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
		0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
		0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
		0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
		0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
		0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
		0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
		0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
		0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
		0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
		0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
		0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
		0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
		0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
		0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
		0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
		0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
		0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
		0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
		0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
		0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
		0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};


/*
* Calculate a new fcs given the current fcs and the new data.
*/
static unsigned short pppfcs16(unsigned short fcs, unsigned char *cp, int len)
{
		while (len--)
		fcs=(fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
		return (fcs);
}

unsigned short app_auutoconnect_698_produce_head_checksum(unsigned char * p_buffer, int len)
{
		unsigned short fcs;
		fcs = pppfcs16(PPPINITFCS16, p_buffer, len);
		fcs ^= 0xffff;
		return fcs;	
}

#if DUANLUQI_ENABLE
///////////////////��·�� /////////////////////
///////////////////��·�� /////////////////////
///////////////////��·�� /////////////////////
///////////////////��·�� /////////////////////
///////////////////��·�� /////////////////////
///////////////////��·�� /////////////////////
///////////////////��·�� /////////////////////

extern struct app_host_uart_env_tag app_host_env_1;
extern struct app_host_uart_env_tag app_host_env_2;	

//ֻҪ�ж�һ��������������ͺ���, �����
static const unsigned char iris_modified_current_cmd[] = {
	 0xFE, 0xFE, 0xFE, 0xFE, 0x68, 0x1B, 0x00, 0xC3, 0x05, 0x89, 0x89, 0x89, 0x89, 0x89, 0x89,
	 0x00, 0xB5, 0x7C, 0x06, 0x01, 0x00, 0xF2, 0x0B, 0x80, 0x00, 0x09,
	 0x02
};

unsigned int app_iris_duanluqi_compare_modified_current_command(unsigned char *p_buffer, unsigned char length)
{
	 if(length < sizeof(iris_modified_current_cmd)/sizeof(unsigned char))
			return 0;
	 if(!memcmp(p_buffer, iris_modified_current_cmd, sizeof(iris_modified_current_cmd)/sizeof(unsigned char)))
		 return 1;
	 else
		 return 0;
}


void app_iris_duanluqi_respond_modified_current_command(unsigned int connection_index)
{
	
		if(connection_index == app_host_env_1.connection_index)
		{
				app_meter_send_data_to_collection2(sizeof(iris_respond_modified_current), iris_respond_modified_current);
		}
		else if(connection_index == app_host_env_2.connection_index)
		{
				app_meter_send_data_to_collection1(sizeof(iris_respond_modified_current), iris_respond_modified_current);
		}
	
}

#endif




#endif
