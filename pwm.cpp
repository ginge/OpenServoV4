/*
    Copyright (c) 2009 Barry Carter <barry.carter@gmail.com>
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

    $Id: pwm-std.c,v 1.2 2009/04/04 11:51:58 ginge Exp $
*/

/*
 * Edit history:
 *
 *     26 Jan 2008        Retrofitted as much as possible of the H-bridge
 *                        "anti-[partial]-crobaring fix" from the code for
 *                        the V3 board to here (the code for the V2 board).
 */
#include <inttypes.h>
#include "WProgram.h"
#include "OpenServoCommon.h"

#include "config.h"
#include "pwm_os.h"
#include "registers.h"
#include "banks.h"


// Flags that indicate PWM output in A and B direction.
static uint16_t pwm_a;
static uint16_t pwm_b;

// Pwm frequency divider value.
static uint16_t pwm_div;
static uint16_t  pwm_max;

//
// The delay_loop function is used to provide a delay. The purpose of the delay is to
// allow changes asserted at the AVRs I/O pins to take effect in the H-bridge (for
// example turning on or off one of the MOSFETs). This is to prevent improper states
// from occurring in the H-bridge and leading to brownouts or "crowbaring" of the
// supply.
//
#define DELAYLOOP 8
inline static void delay_loop(int n)
{
    uint8_t i;
    for(i=0; i<n; i++)
    {
        asm("nop");
        //nop();
    }
}

//
//
//
static void pwm_dir_a(uint16_t pwm_duty)
// Send PWM signal for rotation with the indicated pwm ratio (0 - 255).
// This function is meant to be called only by pwm_update.
{
    // Determine the duty cycle value for the timer.
    uint16_t duty_cycle = pwm_duty;

    // Disable interrupts.
    //nvic_globalirq_disable();

    // Do we need to reconfigure PWM output?
    if (!pwm_a || pwm_b)
    {

        // Disable PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) output.
        // NOTE: Actually PWM_A should already be disabled...
        pinMode(6, OUTPUT); //PA8
        pinMode(7, OUTPUT); //PA9
    
        // Yes. Make sure PB1 and PB2 are zero.
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
        
        //
        // Give the H-bridge time to respond to the above changes
        //
        delay_loop(DELAYLOOP);

        // Reset the B direction flag.
        pwm_b = 0;
    }

    // Set the A direction flag.
    pwm_a = pwm_duty;
    SerialUSB.println(pwm_a);
    // Update the PWM duty cycle.
    pinMode(6, PWM); //PA8
    pwmWrite(6, pwm_a);
    
    // Restore interrupts.
   // nvic_globalirq_enable();

    // Save the pwm A and B duty values.
    registers_write_byte(REG_PWM_DIRA, pwm_a);
    registers_write_byte(REG_PWM_DIRB, pwm_b);
}


static void pwm_dir_b(uint16_t pwm_duty)
// Send PWM signal for rotation with the indicated pwm ratio (0 - 255).
// This function is meant to be called only by pwm_update.
{
    // Determine the duty cycle value for the timer.
    uint16_t duty_cycle = pwm_duty; //PWM_OCRN_VALUE(pwm_div, pwm_duty);

    // Disable interrupts.
    //nvic_globalirq_disable();

    // Do we need to reconfigure PWM output?
    if (!pwm_b || pwm_a)
    {

        // Disable PWM_A (PB1/OC1A) and PWM_B (PB2/OC1B) output.
        // NOTE: Actually PWM_B should already be disabled...
       pinMode(6, OUTPUT); //PA8
        pinMode(7, OUTPUT); //PA9
    
        // Yes. Make sure PB1 and PB2 are zero.
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);

        //
        // Give the H-bridge time to respond to the above changes
        //
        delay_loop(DELAYLOOP);

        // Reset the A direction flag.
        pwm_a = 0;
    }

    // Set the B direction flag.
    pwm_b = pwm_duty;
    SerialUSB.println(pwm_b);
    // Update the PWM duty cycle.
    pinMode(7, PWM); //PA9
    pwmWrite(7, pwm_b);
    
    // Restore interrupts.
    //nvic_globalirq_enable();

    // Save the pwm A and B duty values.
    registers_write_byte(REG_PWM_DIRA, pwm_a);
    registers_write_byte(REG_PWM_DIRB, pwm_b);
}


void pwm_registers_defaults(void)
// Initialize the PWM algorithm related register values.  This is done
// here to keep the PWM related code in a single file.
{
    // PWM divider is a value between 1 and 1024.  This divides the fundamental
    // PWM frequency (500 kHz for 8MHz clock, 1250 kHz for 20MHz clock) by a
    // constant value to produce a PWM frequency suitable to drive a motor.  A
    // small motor with low inductance and impedance such as those found in an
    // RC servo will my typically use a divider value between 16 and 64.  A larger
    // motor with higher inductance and impedance may require a greater divider.
    banks_write_word(POS_PID_BANK, REG_PWM_FREQ_DIVIDER_HI, REG_PWM_FREQ_DIVIDER_LO, DEFAULT_PWM_FREQ_DIVIDER);

    // This sets the maximum PWM output as a percentage of the calculated PWM divider values
    banks_write_byte(POS_PID_BANK, REG_PWM_MAX, DEFAULT_PWM_MAX);

}


void pwm_init(void)
// Initialize the PWM module for controlling a DC motor.
{
    // Initialize the pwm frequency divider value.
    // Initialize the pwm frequency divider value.
    pwm_div = banks_read_word(POS_PID_BANK, REG_PWM_FREQ_DIVIDER_HI, REG_PWM_FREQ_DIVIDER_LO);
    pwm_max = banks_read_byte(CONFIG_BANK,  REG_PWM_MAX);

    
    asm("nop");
    asm("nop");
    asm("nop");


    pinMode(6, PWM); //PA8
    pinMode(7, PWM); //PA9
    pwmWrite(6, 0);
    pwmWrite(7, 0);
    

    // Update the pwm values.
    registers_write_byte(REG_PWM_DIRA, 0);
    registers_write_byte(REG_PWM_DIRB, 0);

#if STEP_ENABLE_BRIDGE_PIN
    // Set enable pin as output
    STEP_ENABLE_DDR |= (1<<STEP_ENABLE_PIN);

    step_enable_bridge();
#endif

}


int16_t pwm_sanitise(int16_t position, int16_t pwm)
{
    uint16_t min_position;
    uint16_t max_position;

    // Use the minimum and maximum position as is.

    // Get the minimum and maximum seek position.
    min_position = banks_read_word(CONFIG_BANK, REG_MIN_SEEK_HI, REG_MIN_SEEK_LO);
    max_position = banks_read_word(CONFIG_BANK, REG_MAX_SEEK_HI, REG_MAX_SEEK_LO);

    // Make sure these values are sane 10-bit values.
    if (min_position > MAX_POSITION) min_position = MAX_POSITION;
    if (max_position > MAX_POSITION) max_position = MAX_POSITION;

    // Disable clockwise movements when position is below the minimum position.
    //if ((position < min_position) && (pwm < 0)) pwm = 0;

    // Disable counter-clockwise movements when position is above the maximum position.
    //if ((position > max_position) && (pwm > 0)) pwm = 0;

    return pwm;
}

void pwm_update(uint16_t position, int32_t pwm)
// Update the PWM signal being sent to the motor.  The PWM value should be
// a signed integer in the range of -255 to -1 for clockwise movement,
// 1 to 255 for counter-clockwise movement or zero to stop all movement.
// This function provides a sanity check against the servo position and
// will prevent the servo from being driven past a minimum and maximum
// position.
{
    uint16_t pwm_width;


    // Determine if PWM is disabled in the registers.
    if (!(registers_read_byte(REG_FLAGS_LO) & (1<<FLAGS_LO_PWM_ENABLED))) pwm = 0;

    // Determine direction of servo movement or stop.
    if (pwm < 0)
    {
        // Less than zero. Turn clockwise.

        // Get the PWM width from the PWM value.
        pwm_width = (uint16_t) -pwm;

        // Turn clockwise.
#if SWAP_PWM_DIRECTION_ENABLED
        pwm_dir_a(pwm_width);
#else
        pwm_dir_b(pwm_width);
#endif
    }
    else if (pwm > 0)
    {
        // More than zero. Turn counter-clockwise.

        // Get the PWM width from the PWM value.
        pwm_width = (uint16_t) pwm;

        // Turn counter-clockwise.
#if SWAP_PWM_DIRECTION_ENABLED
        pwm_dir_b(pwm_width);
#else
        pwm_dir_a(pwm_width);
#endif

    }
    else
    {
        // Stop all PWM activity to the motor.
        pwm_stop();
    }
}


void pwm_stop(void)
// Stop all PWM signals to the motor.
{
    // Disable interrupts.
    //nvic_globalirq_disable();


    pinMode(6, OUTPUT); //PA8
    pinMode(7, OUTPUT); //PA9
    pwmWrite(6, 0); // set low
    pwmWrite(7, 0); // set low

    // Restore interrupts.
    //nvic_globalirq_enable();

    // Save the pwm A and B duty values.
    registers_write_byte(REG_PWM_DIRA, pwm_a);
    registers_write_byte(REG_PWM_DIRB, pwm_b);
}

