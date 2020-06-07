/*
 * PCA9955B_msp430x2xx.h
 *
 *  Created on: 21/05/2020
 *      Author: arent
 */

#ifndef PCA9955B_MSP430X2XX_H_
#define PCA9955B_MSP430X2XX_H_

// I2C FUNCTIONS
void set_i2c_slave_address(unsigned char slave_address);
void configure_i2c(void);
unsigned char read_byte(unsigned char register_address);
void write_byte(unsigned char register_address, unsigned char data);

// PCA9955B FUNCTIONS
void init_PCA9955B(unsigned char slave_address, unsigned char current_ma);
void init_led_drivers(void);
void set_led_output_current(unsigned char current_ma);
void set_pwm_mode(void);
void led_pwm(unsigned char led, unsigned char duty_cycle);
void awesome_example(void);


#endif /* PCA9955B_MSP430X2XX_H_ */
