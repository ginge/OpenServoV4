/*
 * i2c_module.h
 *
 *  Created on: 2 Sep 2012
 *      Author: baz
 */

#ifndef I2C_MODULE_H_
#define I2C_MODULE_H_

#include <libmaple/i2c.h>

void i2c_module_init();


void funcrx(i2c_msg *msg);
void functx(i2c_msg *msg);

#endif /* I2C_MODULE_H_ */
