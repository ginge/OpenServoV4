/*
    Copyright (c) 2006 Michael P. Thompson <mpthompson@gmail.com>
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

    $Id: pid.h,v 1.0 2012/08/16 00:20:48 ginge Exp $
*/

#ifndef _OS_PID_H_
#define _OS_PID_H_ 1

// Initialize the PID algorithm module.
void pid_init();

// Initialize the PID related register values.
void pid_registers_defaults(void);

// Take the 10-bit position as input and output a signed PWM to be
// applied to the servo motors.
int32_t pid_position_to_pwm(int16_t position);

#endif // _OS_PID_H_
