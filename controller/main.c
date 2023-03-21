/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>

#define F_CPU 2000000UL
#include <util/delay.h>
#include "lcd.h"

#include <stdio.h>
#include <dac.h>
#include <tc.h>
#include <sysclk.h>

#define MY_ADC ADCA // Select ADC(A) from A and B
#define MY_ADC_CH0 ADC_CH1 // Select CH(0) from 0-7 
#define MY_ADC_CH1 ADC_CH2 // Select CH(0) from 0-7
#define MASK 0x000000ff


#define MAIN_DAC DACB
#define MAIN_DAC_CHANNEL DAC_CH0
#define RATE_OF_CONVERSION 100
#define NR_OF_SAMPLES 10

static const uint16_t ramp[] = {
	0x1000, 0x2000, 0x3000, 0x4000 ,0x5000, 0x6000, 0x7000, 0x8000, 0x9000, 0xa000, 0xb000, 0xc000, 0xd000, 0xe000, 0xf000,0xffff
};
static void evsys_init(void)
{
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_EVSYS);
	EVSYS.CH3MUX = EVSYS_CHMUX_TCC0_OVF_gc;
}

void tc_init(void)
{
	tc_enable(&TCC0);
	tc_set_wgm(&TCC0, TC_WG_NORMAL);
	tc_write_period(&TCC0, (sysclk_get_per_hz() / RATE_OF_CONVERSION) - 1);
	tc_write_clock_source(&TCC0, TC_CLKSEL_DIV1_gc);
}

void dac_init(void)
{
	struct dac_config conf;
	dac_read_configuration(&MAIN_DAC, &conf);
	dac_set_conversion_parameters(&conf, DAC_REF_AVCC, DAC_ADJ_LEFT);
	dac_set_active_channel(&conf, MAIN_DAC_CHANNEL, 0);
	dac_set_conversion_trigger(&conf, MAIN_DAC_CHANNEL, 3);
	#ifdef XMEGA_DAC_VERSION_1
	dac_set_conversion_interval(&conf, 1);
	#endif
	dac_write_configuration(&MAIN_DAC, &conf);
	dac_enable(&MAIN_DAC);
}

static void adc_init(void)
{
	struct adc_config adc_conf;
	struct adc_channel_config adcch_conf0;
	struct adc_channel_config adcch_conf1;
	adc_read_configuration(&MY_ADC, &adc_conf);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH0, &adcch_conf0);
	adcch_read_configuration(&MY_ADC, MY_ADC_CH1, &adcch_conf1);
	adc_set_conversion_parameters(&adc_conf, ADC_SIGN_OFF, ADC_RES_12,
	ADC_REF_AREFA);
	adc_set_conversion_trigger(&adc_conf, ADC_TRIG_MANUAL, 1, 0);
	adc_set_clock_rate(&adc_conf, 200000UL);
	adcch_set_input(&adcch_conf0, ADCCH_POS_PIN1, ADCCH_NEG_NONE, 1);
	adcch_set_input(&adcch_conf1, ADCCH_POS_PIN2, ADCCH_NEG_NONE, 1);
	adc_write_configuration(&MY_ADC, &adc_conf);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH0, &adcch_conf0);
	adcch_write_configuration(&MY_ADC, MY_ADC_CH1, &adcch_conf1);
		
}

unsigned char ch0_ascii[] = {0,0,0,0,'\0'};
unsigned char ch1_ascii[] = {0,0,0,0,'\0'};
	
void int_to_string(unsigned int val,unsigned char *temp_str_data)
{
	// char str_data[4]=0;
	//temp_str_data[0]=48+(val/10000);
	temp_str_data[0]=48+(val%10000/1000);
	temp_str_data[1]=48+((val%1000)/100);
	temp_str_data[2]=48+((val%100)/10);
	temp_str_data[3]=48+(val%10);
	// return str_data;
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	board_init();
	sysclk_init();
	evsys_init();
	lcd_init();
	tc_init();
	dac_init();
	adc_init();
	//ioport_set_port_dir(PORTE, IOPORT_DIR_INPUT);

//	ioport_set_port_dir(IOPORT_PORTF,MASK , IOPORT_DIR_OUTPUT);


	/* Insert application code here, after the board has been initialized. */
	
	uint16_t result_ch0,result_ch1,DAC_data;
	
	adc_enable(&MY_ADC);
	uint8_t i = 8;
	

	while (1)
	{

		dac_wait_for_channel_ready(&MAIN_DAC, MAIN_DAC_CHANNEL);
		dac_set_channel_value(&MAIN_DAC, MAIN_DAC_CHANNEL, ramp[i]);
		int_to_string((unsigned int) (((3.3*ramp[i])/4096/16)*1000),DAC_data);

		lcd_cmd(0x8b);
		lcd_write_string(DAC_data);
		//_delay_ms(2000);
		//i = (i+1)%16;
		adc_start_conversion(&MY_ADC, MY_ADC_CH0);
		adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH0);
		result_ch0 = adc_get_result(&MY_ADC, MY_ADC_CH0);
						
		adc_start_conversion(&MY_ADC, MY_ADC_CH1);
		adc_wait_for_interrupt_flag(&MY_ADC, MY_ADC_CH1);
		result_ch1 = adc_get_result(&MY_ADC, MY_ADC_CH1);
		
		int_to_string((unsigned int) (((3.3*result_ch0)/4096)*1000),ch0_ascii);
		int_to_string((unsigned int) (((3.3*result_ch1)/4096)*1000),ch1_ascii);
		
		lcd_cmd(0x80);
		lcd_write_string("CH0 : ");		
		lcd_write_string(ch0_ascii);

		lcd_cmd(0xC0);
		lcd_write_string("CH1 : ");
		lcd_write_string(ch1_ascii);
				
		
		//_delay_ms(2000);

	}
}
