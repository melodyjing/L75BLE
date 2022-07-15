

#include "cobra4_aon_top.h"
#include "cobra4_aon_wk.h"
#include "cobra_system.h"
#if (COBRA_APP_TRN_TEST)
int trng_test_init (void)
{
		cobra_pcrm_trng_active_mode_clock_enable(1);
}
int trng_test (void) {

    uint32_t rand_data = 0,rand[6]= {0};

		rand_data = cobra_trng_rand_read();
		printf("rand_data0 is %08x\n",rand_data);
//			  printf("rand_data1 is %x\n",rand[1]);
//			  printf("rand_data2 is %x\n",rand[2]);
//			  printf("rand_data3 is %x\n",rand[3]);
//			  printf("rand_data4 is %x\n",rand[4]);
//			  printf("rand_data5 is %x\n",rand[5]);
}
#endif