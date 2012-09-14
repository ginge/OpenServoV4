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
*/

#include "OpenServoCommon.h"
#include "setup.h"
#include "pid.h"
#include "registers.h"
#include "banks.h"
#include "i2c_module.h"
#include "packetcommands.h"

extern xQueueHandle xQueuePrint;

i2c_msg msgs;
uint8 buf[255];
i2c_msg *pmsg = &msgs;

uint8 address = 0;

void i2c_module_init()
{
    msgs.data = buf;

    i2c_master_enable(I2C1, I2C_FAST_MODE | I2C_SLAVE_GENERAL_CALL | I2C_SLAVE_USE_RX_BUFFER);
    i2c_slave_attach_recv_handler(I2C1, pmsg, funcrx);
    i2c_slave_attach_transmit_handler(I2C1, pmsg, functx);
    i2c_slave_set_own_address(I2C1, 0x10); //banks_read_byte(POS_PID_BANK, REG_TWI_ADDRESS));
    //i2c_slave_set_own_address2(I2C1, 0x0020);
}

void funcrx(i2c_msg *msg)
{
    portBASE_TYPE xHigherPriorityTaskWoken;
    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    struct PMessage *pxMessage;

    pxMessage = & xMessage;

    address = msg->data[0];

    if (msg->length > 1)
    {
        for (int i = 1; i < msg->length -1; i++)
        {
            packet_registers_write(address + i - 1, msg->data[i]);
        }
    }


    sprintf(pxMessage->data, "Received: %02x", msg->data[0]);
    xQueueSendFromISR( xQueuePrint, ( void * ) &pxMessage, &xHigherPriorityTaskWoken );


    /* Now the buffer is empty we can switch context if necessary. */
    if( xHigherPriorityTaskWoken )
    {
        /* Actual macro used here is port specific. */
        taskYIELD ();
    }
}

void functx(i2c_msg *msg)
{
    i2c_write(I2C1, packet_registers_read(address++));
}


