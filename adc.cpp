/*
 * adc.cpp
 *
 *  Created on: 3 Sep 2012
 *      Author: baz
 */

#include "OpenServoCommon.h"
#include "setup.h"
#include "FreeRTOSConfig.h"
#include "os_adc.h"


void adc_init(void)
{
    // Initialize flags and values.
    adc_power_ready = 0;
    adc_power_value = 0;
    adc_position_ready = 0;
    adc_position_value = 0;
#if BACKEMF_ENABLED
    adc_backemf_ready = 0;
    adc_backemf_value = 0;
#endif
#if TEMPERATURE_ENABLED
    adc_temperature_ready = 0;
    adc_temperature_value = 0;
#endif

    // Setup the sensor pin as an analog input
    pinMode(ADC_CHANNEL_POSITION, INPUT_ANALOG);
}


uint16 adc_read_channels(void)
{
    /* Read the sensor */
    uint16 msb = analogRead(ADC_CHANNEL_POSITION);

    return msb;
}
