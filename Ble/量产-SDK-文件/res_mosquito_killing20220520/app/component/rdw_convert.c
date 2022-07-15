/**@file               rdw_convert.h
* @brief            
* @details          
* @author      			Liujian
* @date                 2021-10-28
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.
**********************************************************************************/
#include "rdw_convert.h"

/*
�ַ���ת��Ϊ 16 ����
*/
void rdw_str_to_hex(char *pbDest, char *pbSrc, int nLen)
{
  char h1,h2;
  char s1,s2;
  int i;
 
    for (i=0; i<nLen/2; i++)
    {
        h1 = pbSrc[2*i];
        h2 = pbSrc[2*i+1];
 
        s1 = toupper(h1) - 0x30;  //toupper ת��Ϊ��д��ĸ
        if (s1 > 9)
            s1 -= 7;
        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
            s2 -= 7;
 
        pbDest[i] = s1*16 + s2;
    }
}

/*
 ʮ������ת��Ϊ�ַ���
*/
void rdw_hex_to_str(char *pszDest, char *pbSrc, int nLen)
{
    char    ddl, ddh;
    for (int i = 0; i < nLen; i++)
    {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pszDest[i * 2] = ddh;
        pszDest[i * 2 + 1] = ddl;
    }
    pszDest[nLen * 2] = '\0';
}

/*
   �����ŵ��ַ�ת��Ϊ ����
*/
int rdw_atoi(const char *str)
{
    int value = 0;
    int flag = 1; //�жϷ���
    while (*str == ' ')  //�����ַ���ǰ��Ŀո�
    {
        str++;
    }
    if (*str == '-')  //��һ���ַ����ǡ�-����˵�������Ǹ���
    {
        flag = 0;
        str++;
    }
    else if (*str == '+') //��һ���ַ����ǡ�+����˵������������
    {
        flag = 1;
        str++;
    }//��һ���ַ������ǡ�+����-��Ҳ���������ַ���ֱ�ӷ���0
    else if (*str >= '9' || *str <= '0') 
    {
        return 0;    
    }
    //�������������ַ���������\0��ʱ������ת��
    while (*str != '\0' && *str <= '9' && *str >= '0')
    {
        value = value * 10 + *str - '0'; //�������ַ�תΪ��Ӧ��������
        str++;
    }
    if (flag == 0) //���������
    {
        value = -value;
    }
    return value;
}

//m^n����
//����ֵ:m^n�η�.
unsigned int  rdw_nmea_pow(unsigned char m, unsigned char n)
{
    unsigned int result = 1;    
    while(n--)
			result *= m;
    
    return result;
}

/* ����תΪfloat          */
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ

int rdw_str_to_num(unsigned char *buf, unsigned char *dx)
{
    unsigned char *p = buf;
	
    unsigned int  ires=0, fres=0;
	
    unsigned char ilen=0,flen=0,i;
  
    unsigned char mask=0;
  
    int res;
    
	  while(1) //�õ�������С���ĳ���
    {
        if(*p=='-')
        {
				    mask|=0X02;
				    p++;
				}//�Ǹ���
        
				if(*p==',' || (*p=='*')) //����������
					break;
        
        if(*p=='.') //����С������
        {
				    mask|=0X01;
				    p++;
				}
        else if((*p>'9') || (*p<'0'))   //�зǷ��ַ�
        {   
            ilen=0;
            flen=0;
            break;
        }   
        if(mask&0X01)flen++;
        else ilen++;
        p++;
    }
		
    if(mask&0X02)buf++; //ȥ������
    for(i=0;i<ilen;i++) //�õ�������������
    {  
        ires += rdw_nmea_pow(10,ilen-1-i)*(buf[i]-'0');
    }
		
    if(flen>5) flen=5;   //���ȡ5λС��
		
    *dx=flen;           //С����λ��
		
    for(i=0;i<flen;i++) //�õ�С����������
    {  
        fres+=rdw_nmea_pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
    } 
		
    res = ires*rdw_nmea_pow(10,flen)+fres;
		
    if(mask&0X02) res = -res;  
		
    return res;
} 

/*
  ʮ����ת��Ϊ�ַ�
*/
unsigned short rdw_dec_to_str (uint8_t *pSrc, uint16_t SrcLen, uint8_t *pObj)
{
    uint16_t i = 0;
 
    for(i=0; i<SrcLen; i++)
    {
        sprintf((char *)(pObj + i * 2), "%02d", *(pSrc + i));
    }
		
    *(pObj + i * 2) = '\0';
		
    return  (i * 2);
}
 
/*
��32Ϊ��ת��Ϊ u8 ����
*/
void rdw_u32_to_u8_array(uint8_t *buf, uint32_t u32Value)
{
    buf[0] = ((u32Value >> 24) & 0xFF);
    buf[1] = ((u32Value >> 16) & 0xFF);
    buf[2] = ((u32Value >> 8) & 0xFF);
    buf[3] = (u32Value & 0xFF);
}

/*
   �� u8 ���� תΪ 32 λ��
*/
void rdw_u8_array_to_u32(uint8_t *buf, uint32_t *u32Value)
{
    *u32Value = (buf[0] <<24) + (buf[1] <<16) + (buf[2] <<8) + (buf[3] <<0);
}




