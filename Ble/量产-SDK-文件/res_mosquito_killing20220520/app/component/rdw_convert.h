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
�ַ���ת��Ϊ 16 ����
*/
void rdw_str_to_hex(char *pbDest, char *pbSrc, int nLen);


/*
 ʮ������ת��Ϊ�ַ���
*/
void rdw_hex_to_str(char *pszDest, char *pbSrc, int nLen);

/*
   �����ŵ��ַ�ת��Ϊ ����
*/
int rdw_atoi(const char *str);

/*
  ʮ����ת��Ϊ�ַ�
*/
unsigned short rdw_dec_to_str (uint8_t *pSrc, uint16_t SrcLen, uint8_t *pObj);

/*
��32Ϊ��ת��Ϊ u8 ����
*/
void rdw_u32_to_u8_array(uint8_t *buf, uint32_t u32Value);


/*
��32Ϊ��ת��Ϊ u8 ����
*/
void rdw_u8_array_to_u32(uint8_t *buf, uint32_t *u32Value);

/* ����תΪfloat          */
//strת��Ϊ����,��','����'*'����
//buf:���ִ洢��
//dx:С����λ��,���ظ����ú���
//����ֵ:ת�������ֵ

int rdw_str_to_num(unsigned char *buf, unsigned char *dx);

#endif
