#include <msp430.h>

/*
 * PCA9955B_msp430xfxx.c
 *
 *  Created on: 21/05/2020
 *      Author: arent
 */

/*
 * Variables
 */
unsigned char i = 0;
unsigned int j = 0;

/*
 * ------------------------------------------------------------------------------------------------
 *------------------------------------------I2C FUNCTIONS------------------------------------------
 * ------------------------------------------------------------------------------------------------
 */

/*
 *  Name: set_i2c_slave_address
 *  Params: slave_address
 *  Return: none
 */
void set_i2c_slave_address(unsigned char slave_address) {
    UCB0I2CSA = slave_address;               // Set slave address
}

/*
 *  Name: configure_i2c
 *  Params: none
 *  Return: none
 */
void configure_i2c(void) {
    P3SEL |= 0x06;                            // Configure UCB1 - P3.1 = SDA, P3.2 = SCL

    UCB0CTL1 |= UCSWRST;                      // Hold the state machine in reset
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;     // I2C Master mode | select I2C | synchronous mode
    UCB0CTL1 |= UCSSEL_2;                     // Use SMCK as clock
    UCB0BR0 = 40;                             // Set the divider in order to reach ~400kHz
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;                     // Start the state machine
}

/*
 *  Name: read_byte
 *  Params: register_address
 *  Return: unsigned char
 */
unsigned char read_byte(unsigned char register_address) {
    while (UCB0CTL1 & UCTXSTP);               // Ensure stop condition got sent
    UCB0CTL1 |= UCTR | UCTXSTT;               // I2C start condition
    while(!(IFG2 & UCB0TXIFG));               // Wait for any pending bytes on the TX buffer
    UCB0TXBUF = register_address;             // Set the register address
    while(!(IFG2 & UCB0TXIFG));               // Wait for any pending bytes on the TX buffer

    /*
    * It is necessary to set the STOP condition right after the START condition in receiver mode.
    * In case it is not sent, the MSP430 reads an extra byte (0x00).
    */
    UCB0CTL1 &= ~UCTR;                        // Receiver mode
    UCB0CTL1 |= UCTXSTT;                      // I2C start condition
    while(UCB0CTL1 & UCTXSTT);                // Make sure start has been cleared
    UCB0CTL1 |= UCTXSTP;                      // Generate I2C stop condition

    while(!(IFG2 & UCB0RXIFG));               // Wait for response
    return UCB0RXBUF;
}

/*
 *  Name: write_byte
 *  Params: register_address, data
 *  Return: none
 */
void write_byte(unsigned char register_address, unsigned char data) {
    while (UCB0CTL1 & UCTXSTP);             // Ensure stop condition got sent
    UCB0CTL1 |= UCTR | UCTXSTT;             // I2C start condition
    while(!(IFG2 & UCB0TXIFG));             // Wait for any pending bytes on the TX buffer
    UCB0TXBUF = register_address;           // Send register address
    while(!(IFG2 & UCB0TXIFG));             // Wait for any pending bytes on the TX buffer
    UCB0TXBUF = data;                       // Send data byte
    while (!(IFG2&UCB0TXIFG));
    UCB0CTL1 |= UCTXSTP;                    // Generate I2C stop condition
}

/*
 * ------------------------------------------------------------------------------------------------
 *------------------------------------------PCA995B FUNCTIONS--------------------------------------
 * ------------------------------------------------------------------------------------------------
 */

/*
 *  Name: init_PCA9955B
 *  Params: none
 *  Return: none
 */
void set_led_output_current(unsigned char current_ma) {
    float binary_current = 0;
    unsigned char i = 0;

    if(current_ma > 57) {                                   // Ensure maximum value is 57mA
        current_ma = 57;
    }

    binary_current = ((float)current_ma / 1000) * 4444;     // Calculate equivalent binary value

    for(i = 0x18; i <= 0x27; i++) {
        write_byte(i, (unsigned char)binary_current);       // Set all LED outputs to current_ma
    }
}

/*
 *  Name: set_pwm_mode
 *  Params: none
 *  Return: none
 */
void set_pwm_mode(void) {
    unsigned char i = 0;

    for(i = 0x08; i < 0x18; i++) {              // Set all PWM values to 0%
        write_byte(i, 0x00);
    }

    for(i = 0x02; i <= 0x05; i++) {             // Set all drivers in PWM mode
        write_byte(i, 0xAA);
    }
}

/*
 *  Name: led_pwm
 *  Params: duty_cycle
 *  Return: none
 */
void led_pwm(unsigned char led, unsigned char duty_cycle) {
    unsigned char register_address = led + 8;

    write_byte(register_address, duty_cycle);
}

/*
 *  Name: init_led_drivers
 *  Params: none
 *  Return: none
 */
void init_led_drivers(void) {
    unsigned char i = 0;

    for(i = 0x02; i <= 0x05; i++) {
        write_byte(i, 0x00);
    }
}

/*
 *  Name: init_PCA9955B
 *  Params: none
 *  Return: none
 */
void init_PCA9955B(unsigned char slave_address, unsigned char current_ma) {
    __delay_cycles(2000);
    set_i2c_slave_address(slave_address);
    configure_i2c();
    init_led_drivers();
    set_led_output_current(current_ma);                     // Set LED maximum output current in mA
}

/*
 *  Name: awesome_example
 *  Params: none
 *  Return: none
 */
void awesome_example(void) {
    for(i = 0; i < 255; i++) {
        led_pwm(0, i);
        led_pwm(3, i);
        led_pwm(6, i);
        led_pwm(9, i);
        led_pwm(12, i);
        __delay_cycles(10000);
    }
    for(i = 255; i > 0; i--) {
        led_pwm(0, i);
        led_pwm(3, i);
        led_pwm(6, i);
        led_pwm(9, i);
        led_pwm(12, i);
        __delay_cycles(10000);
    }
    for(i = 0; i < 255; i++) {
        led_pwm(1, i);
        led_pwm(4, i);
        led_pwm(7, i);
        led_pwm(10, i);
        led_pwm(13, i);
        __delay_cycles(10000);
    }
    for(i = 255; i > 0; i--) {
        led_pwm(1, i);
        led_pwm(4, i);
        led_pwm(7, i);
        led_pwm(10, i);
        led_pwm(13, i);
        __delay_cycles(10000);
    }
    for(i = 0; i < 255; i++) {
        led_pwm(2, i);
        led_pwm(5, i);
        led_pwm(8, i);
        led_pwm(11, i);
        led_pwm(14, i);
        __delay_cycles(10000);
    }
    for(i = 255; i > 0; i--) {
        led_pwm(2, i);
        led_pwm(5, i);
        led_pwm(8, i);
        led_pwm(11, i);
        led_pwm(14, i);
        __delay_cycles(10000);
    }
    for(i = 0; i < 14; i++) {
        for(j = 0; j < 255; j++) {
            led_pwm(i, j);
            __delay_cycles(1000);
        }
    }
    for(i = 0; i < 14; i++) {
        for(j = 255; j > 0; j--) {
            led_pwm(i, j);
            __delay_cycles(1000);
        }
    }
}

