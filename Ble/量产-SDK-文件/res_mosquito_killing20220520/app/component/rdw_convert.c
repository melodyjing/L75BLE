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
字符串转换为 16 进制
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
 
        s1 = toupper(h1) - 0x30;  //toupper 转换为大写字母
        if (s1 > 9)
            s1 -= 7;
        s2 = toupper(h2) - 0x30;
        if (s2 > 9)
            s2 -= 7;
 
        pbDest[i] = s1*16 + s2;
    }
}

/*
 十六进制转换为字符串
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
   带符号的字符转换为 数字
*/
int rdw_atoi(const char *str)
{
    int value = 0;
    int flag = 1; //判断符号
    while (*str == ' ')  //跳过字符串前面的空格
    {
        str++;
    }
    if (*str == '-')  //第一个字符若是‘-’，说明可能是负数
    {
        flag = 0;
        str++;
    }
    else if (*str == '+') //第一个字符若是‘+’，说明可能是正数
    {
        flag = 1;
        str++;
    }//第一个字符若不是‘+’‘-’也不是数字字符，直接返回0
    else if (*str >= '9' || *str <= '0') 
    {
        return 0;    
    }
    //当遇到非数字字符或遇到‘\0’时，结束转化
    while (*str != '\0' && *str <= '9' && *str >= '0')
    {
        value = value * 10 + *str - '0'; //将数字字符转为对应的整形数
        str++;
    }
    if (flag == 0) //负数的情况
    {
        value = -value;
    }
    return value;
}

//m^n函数
//返回值:m^n次方.
unsigned int  rdw_nmea_pow(unsigned char m, unsigned char n)
{
    unsigned int result = 1;    
    while(n--)
			result *= m;
    
    return result;
}

/* 包换转为float          */
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值

int rdw_str_to_num(unsigned char *buf, unsigned char *dx)
{
    unsigned char *p = buf;
	
    unsigned int  ires=0, fres=0;
	
    unsigned char ilen=0,flen=0,i;
  
    unsigned char mask=0;
  
    int res;
    
	  while(1) //得到整数和小数的长度
    {
        if(*p=='-')
        {
				    mask|=0X02;
				    p++;
				}//是负数
        
				if(*p==',' || (*p=='*')) //遇到结束了
					break;
        
        if(*p=='.') //遇到小数点了
        {
				    mask|=0X01;
				    p++;
				}
        else if((*p>'9') || (*p<'0'))   //有非法字符
        {   
            ilen=0;
            flen=0;
            break;
        }   
        if(mask&0X01)flen++;
        else ilen++;
        p++;
    }
		
    if(mask&0X02)buf++; //去掉负号
    for(i=0;i<ilen;i++) //得到整数部分数据
    {  
        ires += rdw_nmea_pow(10,ilen-1-i)*(buf[i]-'0');
    }
		
    if(flen>5) flen=5;   //最多取5位小数
		
    *dx=flen;           //小数点位数
		
    for(i=0;i<flen;i++) //得到小数部分数据
    {  
        fres+=rdw_nmea_pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
    } 
		
    res = ires*rdw_nmea_pow(10,flen)+fres;
		
    if(mask&0X02) res = -res;  
		
    return res;
} 

/*
  十进制转换为字符
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
将32为数转换为 u8 数组
*/
void rdw_u32_to_u8_array(uint8_t *buf, uint32_t u32Value)
{
    buf[0] = ((u32Value >> 24) & 0xFF);
    buf[1] = ((u32Value >> 16) & 0xFF);
    buf[2] = ((u32Value >> 8) & 0xFF);
    buf[3] = (u32Value & 0xFF);
}

/*
   将 u8 数组 转为 32 位数
*/
void rdw_u8_array_to_u32(uint8_t *buf, uint32_t *u32Value)
{
    *u32Value = (buf[0] <<24) + (buf[1] <<16) + (buf[2] <<8) + (buf[3] <<0);
}




