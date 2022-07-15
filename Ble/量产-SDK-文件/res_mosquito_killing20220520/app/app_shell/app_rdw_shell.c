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

#include "app_rdw_shell.h"
#include "cobra_uart.h"
#include "cobra_system.h"

#if (APP_RDW_SHELL_ENABLE == 1)

static Shell 					rdw_shell;
static char  rdw_shell_buffer[512];


/**
 * @brief 用户shell写
 * 
 * @param data 数据
 */
void app_rdw_shell_write(char data)
{
			cobra_uart_write(M3_UART0, data);
}

void app_rdw_shell_handler(unsigned char data)
{
		  shellHandler(&rdw_shell, data);
}

void app_rdw_shell_init(void)
{
		  //rdw_shell.read = app_rdw_shell_read;
	    cobra4_uart_set_receive_callback(app_rdw_shell_handler);
	    rdw_shell.write = app_rdw_shell_write;
		  shellInit(&rdw_shell, rdw_shell_buffer, 512);
}


void app_rdw_shell_printf(char * str)
{
	  shellWriteString(&rdw_shell, str);
}



void app_rdw_helloworld(void)
{
		shellWriteString(&rdw_shell, "Hello World\n");
}


void app_rdw_save_nvds(void)
{
	  if(cobra_system_nvds_is_modified())
		{
			  cobra_system_nvds_save_to_eflash();
		}
}

#if (SFUD_DRIVER_ENABLE == 1)
int app_rdw_sfd(int argc, char *argv[])
{
	   unsigned char temp_buf[256];
	   int number, i;
	   const sfud_flash * p_flash = sfud_get_device_table() + 0;
	   if(argc < 2) return 0;
	   number = atoi(argv[1]);
	   sfud_read(p_flash, number*512, 256, temp_buf);
	   for(i = 0; i < 256; i++)
	   {
		     printf("0x%02x ", temp_buf[i]);
	   }
		 printf("\n");
		 return 1;	   
}

void app_rdw_sfud_chip_erase(void)
{

	 const sfud_flash * p_flash = sfud_get_device_table() + 0;

	 sfud_chip_erase(p_flash);
	
}

#endif

#if (FATFS_ENABLE == 1)
void app_rdw_mount_fs(void)
{
	  app_fatfs_mount_init();
}

//void app_rdw_get_disk_information(void)
//{
//	  app_fats_test_free();  
//}

void app_rdw_format_spi_flash(void)
{
	  app_fatfs_format_init();
}



void app_rdw_list_file(void)
{
	  DIR dir;
    FILINFO file_info;
	  FRESULT result;
	  TCHAR buffer[100];
	  unsigned short year;
	  unsigned short month;
	  unsigned short day;
	  unsigned short hour;
	  unsigned short minuter;
	  unsigned short second;
	  unsigned int total_size, free_size;
    result = f_getcwd(buffer, 100);
	  if(result == FR_OK)
		{
			 result = f_findfirst(&dir, &file_info, buffer, "*");
			 while(result == FR_OK)
			 {
				  second = file_info.ftime&0x1f;
				  minuter = (file_info.ftime >> 5)&0x3f;
				  hour = (file_info.ftime >> 11)&0x1f;
				 
				  day = (file_info.fdate)&0x1f;
				  month = (file_info.fdate >> 5)&0x0f;
				  year = (((file_info.fdate >> 9)&0x7f) + 1980);
				  if(file_info.fattrib & AM_ARC)
					{
				      printf("%04d/%02d/%02d  %02d:%02d:%02d %6s %8d %s \n", year, month, day, hour, minuter, second, \
                               				 "", file_info.fsize, file_info.altname);
					}
					else if(file_info.fattrib & AM_DIR)
					{
						  printf("%04d/%02d/%02d  %02d:%02d:%02d %6s %8s %s \n", year, month, day, hour, minuter, second, \
                               				 "<DIR>", "", file_info.altname);
					}
				  result = f_findnext(&dir, &file_info);
				  if(!file_info.fname[0]) break;
			 }
			 
			 if(app_fats_get_memory_size(&total_size, &free_size))
		   {
			        printf("    total memory size = %4luKB  \n", total_size);
			        printf("    free memory size  = %4luKB  \n", free_size);
		   }

		 
		 }
		 else
		 {
			  printf("failed ...... \n");
		 }
		 
	
}

int app_rdw_create_dir(int argc, char *argv[])
{
	    FRESULT res_spi;
	
	    if(argc < 2)
			{
				  printf("usage: %s dir_name \n", argv[0]);
				  return 1;
			}
			
	   res_spi = f_mkdir(argv[1]);
		 if(res_spi == FR_OK)
		 {
			  printf("create dir success \n");
		 }
		 else
		 {
			  printf("create dir failed \n");
		 }
		 return 0;
}

int app_rdw_create_file(int argc, char *argv[])
{
	    unsigned char buffer[100];
	    FRESULT res_spi;
	    size_t fnumber;
	    FIL fil;
	    if(argc < 2)
			{
				  printf("usage: %s filename \n", argv[0]);
				  return 1;
			}
	   /* Open file to write */
     if(f_open(&fil, argv[1], FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
		 {
			   printf("open file error \n");
         return 1;
     }
		  memset(buffer, 0x66, sizeof(buffer));

	
	    res_spi = f_write(&fil, buffer, sizeof(buffer), &fnumber);
	
	    if(res_spi == FR_OK)
 	    {
		     printf("wite  ok  %d \n",  fnumber);
	    }
	
	
      /* Close file */
      if(f_close(&fil) != FR_OK)
			{
				  printf("close file error \n");
          return 1;
	    }
			
			return 0;
}

int app_rdw_changed_dir(int argc, char *argv[])
{
	    FRESULT res_spi;
	
	    if(argc < 2)
			{
				  printf("usage: %s dir_name \n", argv[0]);
				  return 1;
			}
			
	    res_spi = f_chdir(argv[1]);
			
		  if(res_spi != FR_OK)
 	    {
		     printf("no dir %s\n",  argv[1]);
				 return 1;
	    }
			return 0;
			
	
}

void app_rdw_show_current_dir(void)
{
	  TCHAR buffer[100];
	  FRESULT result;
	  result = f_getcwd(buffer, 100);
	  if(result == FR_OK)
		{
			printf("%s \n", buffer);
		}
}

int app_rdw_delete_file_or_dir(int argc, char *argv[])
{
	    FRESULT res_spi;
	
	    if(argc < 2)
			{
				  printf("usage: %s dir or file name \n", argv[0]);
				  return 1;
			}
			
			res_spi = f_unlink(argv[1]);
			
		  if(res_spi != FR_OK)
 	    {
		     printf("can not delete %s\n",  argv[1]);
				 return 1;
	    }
			return 0;
			
}


static void hexdump(unsigned char *buf, int len)
{
    int i = 0;

    for(i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if( (i+1) % 16 == 0) {
            printf("\n");
        }
    }
    if(i%16 != 0) {
        printf("\n");
    }
}

int app_rdw_show_file_content_hex(int argc, char *argv[])
{
	    unsigned char buffer[64];
	    FRESULT res_spi;
	    size_t fnumber;
	    FIL fil;
	    if(argc < 2)
			{
				  printf("usage: %s filename \n", argv[0]);
				  return 1;
			}
	   /* Open file to read */
     if(f_open(&fil, argv[1],  FA_READ) != FR_OK)
		 {
			   printf("can not open  %s \n", argv[1]);
         return 1;
     }
		 while(1)
		 {
			  res_spi = f_read(&fil, buffer, 64, &fnumber);
			  if(res_spi == FR_OK)
				{
			     if(fnumber != 64)
				   {
					    hexdump(buffer, fnumber);
						  break;
				   }
					 else
					 {
						  hexdump(buffer, 64);
					 }
			  }
				else
				{
					 break;
				}
		 }
		 f_close(&fil);
		 return 0;	
}

//echo
int app_rdw_echo_file_content(int argc, char *argv[])
{
	    unsigned char buffer[100];
	    FRESULT res_spi;
	    size_t fnumber;
	    FIL fil;
	    FSIZE_t offset;
	    FILINFO file_info;
	    if(argc < 2)
			{
				  printf("usage: %s \n", argv[0]);
				  return 1;
			}			
			else if(argc < 3)
			{
				  printf("usage: %s \n", argv[1]);
				  return 1;
			}
			else if(argc < 4)
			{
				 if(!strncmp(argv[2], ">", 1))
				 {
					   printf("error ...... \n");
					   return 1;
				 }
				 else
				 {
					  printf("usage: %s  %s\n", argv[1], argv[2]);
					  return 1;
				 }
			}
			
			if(f_stat(argv[3], &file_info) == FR_OK)
			{
				   offset = file_info.fsize;
			}
			else
			{
				  offset = 0;
			}
	   /* Open file to write */
     if(f_open(&fil, argv[3], FA_OPEN_ALWAYS | FA_WRITE) != FR_OK)
		 {
			   printf("open file error \n");
         return 1;
     }
		 
		 f_lseek(&fil, offset);
		 
     res_spi = f_puts(argv[1], &fil);
		 
		 f_puts("\n", &fil);
		 
		 f_close(&fil);
		 return 0;	
		 
}


static void text_dump(unsigned char *buf, int len)
{
    int i = 0;

    for(i = 0; i < len; i++) {
        printf("%c", buf[i]);
    }
}
//cat show text file
int app_rdw_show_file_content_ascii(int argc, char *argv[])
{
	    unsigned char buffer[64];
	    FRESULT res_spi;
	    size_t fnumber;
	    FIL fil;
	    if(argc < 2)
			{
				  printf("usage: %s filename \n", argv[0]);
				  return 1;
			}
	   /* Open file to read */
     if(f_open(&fil, argv[1],  FA_READ) != FR_OK)
		 {
			   printf("can not open  %s \n", argv[1]);
         return 1;
     }
		 while(1)
		 {
			  res_spi = f_read(&fil, buffer, 64, &fnumber);
			  if(res_spi == FR_OK)
				{
			     if(fnumber != 64)
				   {
					    text_dump(buffer, fnumber);
						  break;
				   }
					 else
					 {
						  text_dump(buffer, 64);
					 }
			  }
				else
				{
					 break;
				}
		 }
		 f_close(&fil);
		 return 0;	
}
#endif

#endif
