#include <msp430.h> 
#include "LiquidCrystalDisplay.h"

unsigned char i = 0;

unsigned char corazon[] = {
    0b00000,
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};

unsigned char cohete[] = {
    0b00100,
    0b01110,
    0b01110,
    0b11111,
    0b11111,
    0b01110,
    0b11111,
    0b10101
};

unsigned char jeringa[] = {
    0b11111,
    0b01010,
    0b11111,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b00100
};

unsigned char flor[] = {
    0b01010,
    0b01110,
    0b01110,
    0b10101,
    0b01110,
    0b00100,
    0b00100,
    0b00100
};


/**
 * main.c
 */
void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    DCOCTL = CALDCO_16MHZ;      // Configurar oscilador interno a 16MHz
    BCSCTL1 = CALBC1_16MHZ;     // Configurar oscilador interno a 16MHz

	/* Seleccionar salidas:
     * P1.0 = D4
     * P1.1 = D5
     * P1.2 = D6
     * P1.3 = D7
     * P1.4 = RS
     * P1.5 = E
     */
	P1DIR = 0x3F;               // P1.0, 1, 2, 3, 4, 5 Configurados como salidas

	inicializar_lcd();

    crear_caracter(corazon, 0);
    crear_caracter(cohete, 1);
    crear_caracter(jeringa, 2);
    crear_caracter(flor, 3);

    envia_comando(0x80);    // Colocar cursor en la posición 0 del primer renglón
    print_caracter(0);      // Imprime corazón
    print_caracter(2);      // Imprime jeringa
    print_caracter(3);      // Imprime flor
    print_caracter(1);      // Imprime Rocket Raccoon!
    print("Raccoon ");
    print_caracter(1);      // Imprime Rocket Raccoon!
    print_caracter(3);      // Imprime flor
    print_caracter(2);      // Imprime jeringa
    print_caracter(0);      // Imprime corazón
    envia_comando(0xC0);    // Colocar cursor en la posición 0 del segundo renglón
    print_caracter(1);      // Imprime Rocket Raccoon!
    print(" Electronics! ");
    print_caracter(1);      // Imprime Rocket Raccoon!

	while(1) {

	}
}
