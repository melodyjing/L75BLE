
#include <stdint.h>         // Standard Integer

#if (COBRA_APP_EFLASH_TEST)
#include "cobra4C_eflash_ctrl.h"
#define   M4_EFLASH_TEST_ADDRESS  (0x50000 - 0x3000)
void cobra_test_nvds_save_to_eflash(void)
{
    unsigned char  temp_buffer[4];
		temp_buffer[0] = 0x55;
		temp_buffer[1] = 0x45;
		temp_buffer[2] = 0x35;
		temp_buffer[3] = 0x25;

		cobra_flash_erase(M4_EFLASH_TEST_ADDRESS, 4096);	

		cobra_flash_write(M4_EFLASH_TEST_ADDRESS, sizeof(temp_buffer), temp_buffer);	
}
void cobra_test_nvds_read_from_eflash(void)
{
	unsigned int chipid[4];
//  cobra_information_flash_read(M4_EFLASH_TEST_ADDRESS, 16, (uint8_t *)chipid);
	uint8_t *  p_count = (uint8_t * )M4_EFLASH_TEST_ADDRESS;
	printf("eflash_test %x %x %x %x\n",*p_count,*(p_count+1),*(p_count+2),*(p_count+3));
}
void cobra_test_flash(void)
{
	cobra_test_nvds_save_to_eflash();
	cobra_test_nvds_read_from_eflash();
}
#endif