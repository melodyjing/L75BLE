/******************************************************************************
	Confidential and copyright 2014-2021 Radiawave Ltd.
	
	Author:Liujian
	
	Cobra Drivers

	File Name	:	app_rdw_shell.c
	Version		:	0.1
	Created	By	:	LiuJian
	Date		:	2020/4/30

	Description :   Test shell Driver from github
	Changed Log	:

		Liujian  2020/4/30		- Creation
*****************************************************************************/

#ifndef __APP_RDW_SHELL_H__
#define __APP_RDW_SHELL_H__

#if (APP_RDW_SHELL_ENABLE == 1)

#include "shell.h"

void app_rdw_shell_handler(unsigned char data);


void app_rdw_shell_init(void);

void app_rdw_shell_printf(char * str);

void app_rdw_save_nvds(void);

void app_rdw_sfud_chip_erase(void);
int app_rdw_sfd(int argc, char *argv[]);
void app_rdw_mount_fs(void);
void app_rdw_get_disk_information(void);
int app_rdw_create_file(int argc, char *argv[]);
void app_rdw_format_spi_flash(void);
void app_rdw_list_file(void);
int app_rdw_create_dir(int argc, char *argv[]);
int app_rdw_changed_dir(int argc, char *argv[]);
void app_rdw_show_current_dir(void);
int app_rdw_delete_file_or_dir(int argc, char *argv[]);
int app_rdw_show_file_content_hex(int argc, char *argv[]);
//echo
int app_rdw_echo_file_content(int argc, char *argv[]);
//cat
int app_rdw_show_file_content_ascii(int argc, char *argv[]);
#endif
#endif
