/*
 * adc.h
 *
 *  Created on: 3 Sep 2012
 *      Author: baz
 */

#ifndef OS_ADC_H_
#define OS_ADC_H_

/* config section */



// Initialize ADC conversion registers.
void adc_init(void);
uint16 adc_read_channels(void);

// Declare externally so in-lines work.
#if CURRENT_ENABLED
extern volatile uint8_t adc_power_ready;
extern volatile uint16_t adc_power_value;
#endif
#if ADC_POSITION_ENABLED
extern volatile uint8_t adc_position_ready;
extern volatile uint16_t adc_position_value;
#endif
#if BACKEMF_ENABLED
extern volatile uint8_t adc_backemf_ready;
extern volatile uint16_t adc_backemf_value;
#endif
#if TEMPERATURE_ENABLED
extern volatile uint8_t adc_temperature_ready;
extern volatile uint16_t adc_temperature_value;
#endif



#endif /* ADC_H_ */
