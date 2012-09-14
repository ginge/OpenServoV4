/*
    Copyright (c) 2012 Barry Carter <barry.carter@gmail.com>

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

    $Id: pid.cpp,v 1.0 2012/07/13 00:30:27 ginge Exp $
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
