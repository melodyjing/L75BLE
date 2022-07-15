/**@file               rdw_convert.h
* @brief            
* @details          
* @author      			Liujian
* @date                 2021-10-28
* @version			    V1.0
* @copyright		    Copyright (c)	2019-2025 Radiawave Ltd.
**********************************************************************************/
#ifndef __RDW_CONVERT_H__
#define __RDW_CONVERT_H__

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
/*
字符串转换为 16 进制
*/
void rdw_str_to_hex(char *pbDest, char *pbSrc, int nLen);


/*
 十六进制转换为字符串
*/
void rdw_hex_to_str(char *pszDest, char *pbSrc, int nLen);

/*
   带符号的字符转换为 数字
*/
int rdw_atoi(const char *str);

/*
  十进制转换为字符
*/
unsigned short rdw_dec_to_str (uint8_t *pSrc, uint16_t SrcLen, uint8_t *pObj);

/*
将32为数转换为 u8 数组
*/
void rdw_u32_to_u8_array(uint8_t *buf, uint32_t u32Value);


/*
将32为数转换为 u8 数组
*/
void rdw_u8_array_to_u32(uint8_t *buf, uint32_t *u32Value);

/* 包换转为float          */
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值

int rdw_str_to_num(unsigned char *buf, unsigned char *dx);

#endif
