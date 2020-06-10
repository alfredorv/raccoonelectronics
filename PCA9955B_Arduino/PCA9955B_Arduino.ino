#include <Wire.h>

unsigned char i = 0;
unsigned char j = 0;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(); // join i2c bus (address optional for master)
  init_led_drivers();
  set_led_output_current(5);
  set_pwm_mode();
}

void loop() {
  awesome_example();
}

void write_byte(unsigned char register_address, unsigned char data) {
    Wire.beginTransmission(0x3F);
    Wire.write(register_address);
    Wire.write(data);  
    Wire.endTransmission();
}

void init_led_drivers(void) {
    unsigned char i = 0;

    for(i = 0x02; i <= 0x05; i++) {
        write_byte(i, 0x00);
    }
}

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

void set_pwm_mode(void) {
    unsigned char i = 0;

    for(i = 0x08; i < 0x18; i++) {              // Set all PWM values to 0%
        write_byte(i, 0x00);
    }

    for(i = 0x02; i <= 0x05; i++) {             // Set all drivers in PWM mode
        write_byte(i, 0xAA);
    }
}

void led_pwm(unsigned char led, unsigned char duty_cycle) {
    unsigned char register_address = led + 8;

    write_byte(register_address, duty_cycle);
}

void awesome_example(void) {
    for(i = 0; i < 255; i++) {
        led_pwm(0, i);
        led_pwm(3, i);
        led_pwm(6, i);
        led_pwm(9, i);
        led_pwm(12, i);
        delay(1);
    }
    for(i = 255; i > 0; i--) {
        led_pwm(0, i);
        led_pwm(3, i);
        led_pwm(6, i);
        led_pwm(9, i);
        led_pwm(12, i);
        delay(1);
    }
    for(i = 0; i < 255; i++) {
        led_pwm(1, i);
        led_pwm(4, i);
        led_pwm(7, i);
        led_pwm(10, i);
        led_pwm(13, i);
        delay(1);
    }
    for(i = 255; i > 0; i--) {
        led_pwm(1, i);
        led_pwm(4, i);
        led_pwm(7, i);
        led_pwm(10, i);
        led_pwm(13, i);
        delay(1);
    }
    for(i = 0; i < 255; i++) {
        led_pwm(2, i);
        led_pwm(5, i);
        led_pwm(8, i);
        led_pwm(11, i);
        led_pwm(14, i);
        delay(1);
    }
    for(i = 255; i > 0; i--) {
        led_pwm(2, i);
        led_pwm(5, i);
        led_pwm(8, i);
        led_pwm(11, i);
        led_pwm(14, i);
        delay(1);
    }
    for(i = 0; i < 15; i++) {
        for(j = 0; j < 255; j++) {
            led_pwm(i, j);
            delay(1);
        }
    }
    for(i = 0; i < 15; i++) {
        for(j = 255; j > 0; j--) {
            led_pwm(i, j);
            delay(1);
        }
    }
}
