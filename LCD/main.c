#include <msp430.h> 
#include "LiquidCrystalDisplay.h"

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
	P1DIR = 0x3F;                // P1.0, 1, 2, 3, 4, 5 Configurados como salidas

	inicializar_lcd();
  envia_comando(0x84);         // Comando para colocar cursor en el primer renglón, quinto espacio
	print("Raccoon");
	envia_comando(0xC2);         // Comando para colocar cursor en el seegundo renglón, tercer espacio
	print("Electronics!");


	while(1) {

	}
}
