#include <msp430.h>
#include "PCA9955B_msp430x2xx.h"

#define I2C_SLAVE_ADDRESS      0x3F

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    DCOCTL = CALDCO_16MHZ;
    BCSCTL1 = CALBC1_16MHZ;

    init_PCA9955B(I2C_SLAVE_ADDRESS, 5);        // Set slave address and current in mA
    set_pwm_mode();

    while(1) {
        awesome_example();
    }
}
