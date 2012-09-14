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

    $Id: setup.cpp,v 1.00 2012/07/17 23:43:28 ginge Exp $
*/
#include "OpenServoCommon.h"
#include "setup.h"
#include "pid.h"
#include "registers.h"
#include "banks.h"
#include "pwm_os.h"
#include "i2c_module.h"

/*
 * Configure the default pin configuration on startup of the device
 */
void setup_pin_defaults(void)
{
    // Setup the LED to steady on
    pinMode(BOARD_LED_PIN, OUTPUT);
    digitalWrite(BOARD_LED_PIN, HIGH);

    // Setup the button as input
    pinMode(BOARD_BUTTON_PIN, INPUT);
    digitalWrite(BOARD_BUTTON_PIN, HIGH);
}


void setup_modules(void)
{
    registers_init();
    banks_init();
    pwm_init();
    i2c_module_init();
}


void setup_default_OpenServo(void)
{
    pid_registers_defaults();
}

void setup_OpenServo()
{
    setup_pin_defaults();
    setup_modules();
    setup_default_OpenServo();
}
