/*
 * packetprocess.cpp
 *
 *  Created on: 1 Sep 2012
 *      Author: baz
 */
#include <inttypes.h>
#include <string.h>
#include "WProgram.h"

#include "OpenServoCommon.h"
#include "config.h"
//#include "eeprom.h"
#include "pid.h"
#include "pwm.h"
//#include "step.h"
#include "registers.h"
#include "banks.h"
#include "packetcommands.h"

 uint8_t packet_registers_read(uint8_t address)
// Read the byte from the specified register.  This function handles the
// reading of special registers such as unused registers, redirect and
// redirected registers.
{
    // Mask the most significant bit of the address.
    address &= 0x7F;

    // Are we reading a normal register?
    if (address <= MAX_WRITE_PROTECT_REGISTER)
    {
#if ROLLING_SUBTYPE
        if (address == REG_DEVICE_SUBTYPE)
        {
            return registers_subtype_cycle();
        }
#endif
        // Yes. Complete the read.
        return registers_read_byte(address);
    }

    // Are we reading an unused register.
    if (address <= MAX_UNUSED_REGISTER)
    {
        // Block the read.
        return 0;
    }

    // Are we reading a bank register?
    if (address <= MAX_BANK_REGISTER)
    {
        // Defaults to returning the register n the bank array
        return banks_read_byte(banks_selected_bank(), address-MIN_BANK_REGISTER);
    }

    // All other reads are blocked.
    return 0;
}


 void packet_registers_write(uint8_t address, uint8_t data)
// Write non-write protected registers.  This function handles the
// writing of special registers such as unused registers, redirect and
// redirected registers.
{
    // Mask the most significant bit of the address.
    address &= 0x7F;


    // Are we writing a read only register?
    if (address <= MAX_READ_ONLY_REGISTER)
    {
        // Yes. Block the write.
        return;
    }

    // Are we writing a read/write register?
    if (address <= MAX_READ_WRITE_REGISTER)
    {
        // Check that the bank selection is in range
        if (address == REG_BANK_SELECT)
        {
            if (data >= MAX_BANKS - 1)
            {
                // Set the bank number to the maximum
                data = MAX_BANKS - 1;
            }
        }

        // Yes. Complete the write.
        registers_write_byte(address, data);

        return;
    }

    // Is writing to the upper registers disabled?
    if (registers_is_write_disabled() && (address <= MAX_WRITE_PROTECT_REGISTER))
    {
        // Yes. Block the write.
        return;
    }

    // Are we writing a write protected register?
    if (address <= MAX_WRITE_PROTECT_REGISTER)
    {
        // Yes. Complete the write if writes are enabled.
        registers_write_byte(address, data);

        return;
    }

    // Are we writing an unused register.
    if (address <= MAX_UNUSED_REGISTER)
    {
        // Yes. Block the write.
        return;
    }

    // Are we writing a bank register?
    if (address <= MAX_BANK_REGISTER)
    {
        if (banks_selected_bank() == CONFIG_BANK)
        {
      // Are we writing to the configuration bank while writes are disabled?
      if (registers_is_write_disabled())
        return;
        }
        return banks_write_byte(banks_selected_bank(), address-MIN_BANK_REGISTER, data);
    }

    // All other writes are blocked.
    return;
}
