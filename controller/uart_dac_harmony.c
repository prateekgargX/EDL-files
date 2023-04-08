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
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#define F_CPU 2000000

#include <asf.h>
#include <usart_driver.h>
//#include "avr_compiler.h"
#include <avr/io.h>
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
#define NR_OF_SAMPLES 16

#define USART USARTE0

void usart_config(void){
	PORTE.DIRSET = PIN3_bm;
	PORTE.DIRCLR = PIN2_bm;
	
	USART_Format_Set(&USART, USART_CHSIZE_8BIT_gc, USART_PMODE_DISABLED_gc, false);
	USART_Baudrate_Set(&USART, 12 , 0);
	USART_Rx_Enable(&USART);
	USART_Tx_Enable(&USART);
}

void sendChar(char c)
{
	
	while(!USART_IsTXDataRegisterEmpty(&USART)); //Wait until DATA buffer is empty
	
	USART_PutChar(&USART, c);
	
}

void sendString(char *text)
{
	while(*text)
	{
		sendChar(*text++);
	}
}


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

int main(void)
{
	board_init();
	
	usart_config();
	evsys_init();
	lcd_init();
	tc_init();
	
	dac_init();
	adc_init();
//	usart_config();
//  sysclk_init();
// 	usart_config();


	lcd_cmd(0xC0);
	lcd_write_string("hi");
	uint16_t i =0;

	while(1)
	{
		_delay_ms(3000);
		sendString("awesome\r\n");
		dac_wait_for_channel_ready(&MAIN_DAC, MAIN_DAC_CHANNEL);
		dac_set_channel_value(&MAIN_DAC, MAIN_DAC_CHANNEL, ramp[i]);
		i =(i+1)%10;

	}
}
