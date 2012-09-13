/*
    Copyright (c) 2006 Michael P. Thompson <mpthompson@gmail.com>

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
#include "config.h"
#include "pid.h"
#include "registers.h"
#include "banks.h"
/*
#include "motion.h"
*/


// The minimum and maximum output.
#define MAX_OUTPUT              (65535) // using a 16bit timer
#define MIN_OUTPUT              (-MAX_OUTPUT)

// Values preserved across multiple PID iterations.
static int16_t previous_seek;
static int16_t previous_position;

//
// Digital Lowpass Filter Implementation
//
// See: A Simple Software Lowpass Filter Suits Embedded-system Applications
// http://www.edn.com/article/CA6335310.html
//
// k    Bandwidth (Normalized to 1Hz)   Rise Time (samples)
// 1    0.1197                          3
// 2    0.0466                          8
// 3    0.0217                          16
// 4    0.0104                          34
// 5    0.0051                          69
// 6    0.0026                          140
// 7    0.0012                          280
// 8    0.0007                          561
//

#define FILTER_SHIFT 2

static int32_t filter_reg_pos = 0;
static int32_t filter_reg_emf = 0;

static int16_t filter_update(int16_t input, int32_t *filter_source)
{
    // Update the filter with the current input.
    *filter_source = *filter_source - (*filter_source >> FILTER_SHIFT) + input;

    // Scale output for unity gain.
    return (int16_t) (*filter_source >> FILTER_SHIFT);
}

void pid_init(void)
// Initialize the PID algorithm module.
{
    // Initialize preserved values.
    previous_seek = 0;
    previous_position = 0;
}


void pid_registers_defaults(void)
// Initialize the PID algorithm related register values.  This is done
// here to keep the PID related code in a single file.
{
    // Default deadband.
    banks_write_byte(POS_PID_BANK, REG_PID_DEADBAND, DEFAULT_PID_DEADBAND);

    // Default gain values.
    banks_write_word(POS_PID_BANK, REG_PID_PGAIN_HI, REG_PID_PGAIN_LO, DEFAULT_PID_PGAIN);
    banks_write_word(POS_PID_BANK, REG_PID_DGAIN_HI, REG_PID_DGAIN_LO, DEFAULT_PID_DGAIN);
    banks_write_word(POS_PID_BANK, REG_PID_IGAIN_HI, REG_PID_IGAIN_LO, DEFAULT_PID_IGAIN);

    // Default position limits.
    banks_write_word(POS_PID_BANK, REG_MIN_SEEK_HI, REG_MIN_SEEK_LO, DEFAULT_MIN_SEEK);
    banks_write_word(POS_PID_BANK, REG_MAX_SEEK_HI, REG_MAX_SEEK_LO, DEFAULT_MAX_SEEK);
}


int32_t pidPositionToPwm(int16_t current_position)
// This is a modified pid algorithm by which the seek position and seek
// velocity are assumed to be a moving target.  The algorithm attempts to
// output a pwm value that will achieve a predicted position and velocity.
{
    // We declare these static to keep them off the stack.
    static int16_t deadband;
    static int16_t p_component;
    static int16_t d_component;
    static int16_t seek_position;
    static int16_t seek_velocity;
    static int16_t minimum_position;
    static int16_t maximum_position;
    static int16_t current_velocity;
    static int16_t filtered_position;
    static int32_t pwm_output;
    static uint16_t d_gain;
    static uint16_t p_gain;

    // Sanity check the input for negative overflow (encoders shooting below 0 will roll over to 65535
    /*if (previous_position <= 65000 && current_position > 65000)
    {
        current_position = -(65535 - current_position);
    }*/

    // Filter the current position thru a digital low-pass filter.
    filtered_position = filter_update(current_position, &filter_reg_pos);

    // Use the filtered position to determine velocity.
    current_velocity = filtered_position - previous_position;
    previous_position = filtered_position;

#if CURVE_MOTION_ENABLED
    if (motion_enabled())
    {
      seek_position = (int16_t) banks_read_word(INFORMATION_BANK, REG_SEEK_TO_POSITION_HI, REG_SEEK_TO_POSITION_LO);
      seek_velocity = (int16_t) banks_read_word(INFORMATION_BANK, REG_SEEK_TO_VELOCITY_HI, REG_SEEK_TO_VELOCITY_LO);
    }
    else
#else
    
    {
      // Get the seek position and velocity.
      seek_position = (int16_t) registers_read_word(REG_SEEK_POSITION_HI, REG_SEEK_POSITION_LO);
      seek_velocity = (int16_t) registers_read_word(REG_SEEK_VELOCITY_HI, REG_SEEK_VELOCITY_LO);
    }
#endif

    // Get the minimum and maximum position.
    minimum_position = (int16_t) banks_read_word(POS_PID_BANK, REG_MIN_SEEK_HI, REG_MIN_SEEK_LO);
    maximum_position = (int16_t) banks_read_word(POS_PID_BANK, REG_MAX_SEEK_HI, REG_MAX_SEEK_LO);

    // Update the position and velocity registers without change.
    registers_write_word(REG_POSITION_HI, REG_POSITION_LO, (uint16_t) current_position);
    registers_write_word(REG_VELOCITY_HI, REG_VELOCITY_LO, (uint16_t) current_velocity);

    // Get the deadband.
    deadband = (int16_t) banks_read_byte(POS_PID_BANK, REG_PID_DEADBAND);

    // Use the filtered position when the seek position is not changing.
    if (seek_position == previous_seek) current_position = filtered_position;
    previous_seek = seek_position;

    // Keep the seek position bound within the minimum and maximum position.
    if (seek_position < minimum_position) seek_position = minimum_position;
    if (seek_position > maximum_position) seek_position = maximum_position;

    // The proportional component to the PID is the position error.
    p_component = seek_position - current_position;

    // Get the proportional, derivative and integral gains.
    p_gain = banks_read_word(POS_PID_BANK, REG_PID_PGAIN_HI, REG_PID_PGAIN_LO);
    d_gain = banks_read_word(POS_PID_BANK, REG_PID_DGAIN_HI, REG_PID_DGAIN_LO);

    // Start with zero PWM output.
    pwm_output = 0;

    // Apply proportional component to the PWM output if outside the deadband.
    if ((p_component > deadband) || (p_component < -deadband))
    {
        // Apply the proportional component of the PWM output.
        pwm_output = (int32_t) p_component * (int32_t) p_gain;
    }
    else
    {
        // Reset the speed portion of the motion to zero when in deadband. Stops D error increasing.
        seek_velocity = 0;
    }

    // The derivative component to the PID is the velocity.
    d_component = seek_velocity - current_velocity;

    // Apply the derivative component of the PWM output.
    pwm_output += (int32_t) d_component * (int32_t) d_gain;

    // Shift by 8 to account for the multiply by the 8:8 fixed point gain values.
    pwm_output >>= 8;

    // Check for output saturation.
    if (pwm_output > MAX_OUTPUT)
    {
        // Can't go higher than the maximum output value.
        pwm_output = MAX_OUTPUT;
    }
    else if (pwm_output < MIN_OUTPUT)
    {
        // Can't go lower than the minimum output value.
        pwm_output = MIN_OUTPUT;
    }

    // Return the PID output.
    return (int32_t) pwm_output;
}

