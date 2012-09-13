/*
 * packetcommands.h
 *
 *  Created on: 2 Sep 2012
 *      Author: baz
 */

#ifndef PACKETCOMMANDS_H_
#define PACKETCOMMANDS_H_


#define TWI_CMD_RESET                   0x80        // Reset command
#define TWI_CMD_CHECKED_TXN             0x81        // Read/Write registers with simple checksum
#define TWI_CMD_PWM_ENABLE              0x82        // Enable PWM to motors
#define TWI_CMD_PWM_DISABLE             0x83        // Disable PWM to servo motors
#define TWI_CMD_WRITE_ENABLE            0x84        // Enable write of safe read/write registers
#define TWI_CMD_WRITE_DISABLE           0x85        // Disable write of safe read/write registers
#define TWI_CMD_REGISTERS_SAVE          0x86        // Save safe read/write registers fo EEPROM
#define TWI_CMD_REGISTERS_RESTORE       0x87        // Restore safe read/write registers from EEPROM
#define TWI_CMD_REGISTERS_DEFAULT       0x88        // Restore safe read/write registers to defaults
#define TWI_CMD_EEPROM_ERASE            0x89        // Erase the EEPROM.
#define TWI_CMD_PWM_BRAKE_ENABLE        0x8A        // Enable PWM H-bridge braking
#define TWI_CMD_PWM_BRAKE_DISABLE       0x8B        // Disable PWM H-bridge braking
#define TWI_CMD_RESERVED_FREE           0x90        // Available for use
#define TWI_CMD_CURVE_MOTION_ENABLE     0x91        // Enable curve motion processing.
#define TWI_CMD_CURVE_MOTION_DISABLE    0x92        // Disable curve motion processing.
#define TWI_CMD_CURVE_MOTION_RESET      0x93        // Reset the curve motion buffer.
#define TWI_CMD_CURVE_MOTION_APPEND     0x94        // Append curve motion data.
#define TWI_CMD_GCALL_ENABLE            0x95        // Enable General Call
#define TWI_CMD_GCALL_DISABLE           0x96        // Disable General Call
#define TWI_CMD_GCALL_START_WAIT        0x97        // Wait for the syncro move
#define TWI_CMD_GCALL_START_MOVE        0x98        // Start the General call syncro move

 uint8_t packet_registers_read(uint8_t address);
 void packet_registers_write(uint8_t address, uint8_t data);


#endif /* PACKETCOMMANDS_H_ */
