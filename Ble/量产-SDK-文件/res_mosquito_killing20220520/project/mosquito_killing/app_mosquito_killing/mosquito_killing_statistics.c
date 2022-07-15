#include "mosquito_killing_statistics.h"

#if (APP_MOSQUITO_KILLING_ENABLE == 1)

/***************************************************************************/
/*adc*/
uint8_t gain_single_end,offset_single_end;
uint32_t app_mosquito_killing_ADC_Get(uint8_t ch)
{
    uint32_t u32Res=0;

    cobra4_gpadc_fifo_wen_enable(1);
    rdw_adc_single_adc_mode_select_channel(31 - ch);
    cobra4_gpadc_fifo_enable(0);
    cobra4_gpadc_fifo_enable(1);

    while(cobra4_gpadc_fifo_is_empty());
    u32Res = cobra4_gpadc_read_adc_data();
    while(cobra4_gpadc_fifo_is_empty());
    u32Res = cobra4_gpadc_read_adc_data();
    while(cobra4_gpadc_fifo_is_empty());
    u32Res = cobra4_gpadc_read_adc_data();

    cobra4_gpadc_fifo_wen_enable(0);

    return u32Res;
}

void app_adc_single_end_calibration(uint8_t *gain_shift,uint8_t *offset_shift)
{
    cobra4_aon_pd_top_protection_en(1);
    *gain_shift =   (uint8_t)(REG_PL_RD(0x400C6904UL)>>16)&0x00FF;
    *offset_shift = (uint8_t)(REG_PL_RD(0x400C6904UL)>>24)&0x00FF;
    cobra4_aon_pd_top_protection_en(0);

    if ((*gain_shift == 0x00) || (*gain_shift == 0xff))
    {
        *gain_shift = 0x35;
    }

    if ((*offset_shift == 0x00) || (*offset_shift == 0xff))
    {
        *offset_shift = 0x22;
    }

}
void app_mosquito_killing_ADC_Init(void)
{
    cobra4_aon_pd_top_protection_en(1);
    cobra4_aon_pd_top_d2a_anatmux_en(1);
    cobra4_aon_pd_top_d2a_anatmux_sel(10);
    cobra4_aon_pd_top_protection_en(0);

    rdw_adc_set_adc_power(PD0_SEL_BGP_LN, REF_SEL_BLP_LN);
    rdw_adc_set_adc_clk(ADC_RATE_8M, CLK_SRC_XO);

    cobra4_aon_pd_top_protection_en(1);

    rdw_adc_get_adc_data_init(ADC_MODE_NORMAL, ADC_SINGLE_END_MODE, ADC_GAIN_1X, DIFF_POS_DPIN27, DIFF_NEG_DPIN_MAX);

    rdw_adc_fifo_init();


//    cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN27);
//    cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN28);
//    cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN29);
	cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN30);
//    cobra4_gpadc_get_adc_data_init_gpio_pos(DIFF_POS_DPIN31);

    cobra4_aon_pd_top_protection_en(0);
    app_adc_single_end_calibration(&gain_single_end,&offset_single_end);
}



uint32_t app_mosquito_killing_ADC_voltage(void)
{
    uint32_t _u32Res = 0;
    uint32_t voltage = 0;

    _u32Res = app_mosquito_killing_ADC_Get(APP_MOSQUITO_KILLING_HV_ADC_DETECT_PIN_NUM);

//	__direct_printf("[%d]", _u32Res);
//	if(_u32Res>APP_MOSQUITO_KILLING_HV_ADC_DETECT_LIMIT)
//	{
//		printf("\n???\n");
//	}
//	printf(".");

//    voltage = ((_u32Res - offset_single_end + 250)*1000) / (gain_single_end + 1200);
	voltage = _u32Res;

    return voltage;
}
/***************************************************************************/


#endif

