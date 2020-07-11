#include <msp430.h> 

/*
 * main.c
 *
 *
 * Autor: Ing. Alfredo Rentería Villanueva
 * Fecha: 10/07/2020
 *
 * Este código controla un shift register 74HC595 para pintar números en un display de 7 segmentos
 * de Cátodo Común; si se quisiera controlar uno de Ánodo Común, basta con invertir el valor binario
 * de cada dígito, por ejemplo: ~(0b01111111).
 */

unsigned char i = 0;
unsigned char arreglo[11] = {
                             0b00111111,                            // 0
                             0b00000110,                            // 1
                             0b01011011,                            // 2
                             0b01001111,                            // 3
                             0b01100110,                            // 4
                             0b01101101,                            // 5
                             0b01111101,                            // 6
                             0b00000111,                            // 7
                             0b01111111,                            // 8
                             0b01101111,                            // 9
                             0b10000000                             // punto
};

void imprime_digito(unsigned char data) {
    unsigned char j = 0;
    unsigned int mask = 0x80;
    unsigned char estado_del_puerto = 0;


    P1OUT |= 0x04;  // Pone OE en alto, por lo que las salidas del shift register están apagadas

    for(j = 0; j <= 8; j++) {
        estado_del_puerto = P1IN;               // Lee el estado actual del puerto

        // Escribe la salida respetando el estado del puerto. Para esto,
        // se hace un corrimiento de bits y se le aplica una operación AND con 'mask' para tomar
        // un bit a la vez. Una vez seleccionado el bit con la máscara, se hace otro corrimiento
        // de 7 bits a la derecha para escribir ese valor en el P1.0.
        P1OUT = (estado_del_puerto & 0xFC) | (mask & (data << j)) >> 7;

        // Genera pulso de reloj:
        P1OUT |= 0x02;
        _delay_cycles(160);
        P1OUT &= ~0x02;
    }

    P1OUT &= ~0x04;     // Pone OE en bajo, por lo que las salidas del shift register se activan
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// Detener watchdog timer
    DCOCTL = CALDCO_16MHZ;      // Configurar oscilador interno a 16MHz
    BCSCTL1 = CALBC1_16MHZ;     // Configurar oscilador interno a 16MHz

    P1OUT = 0x00;   // Apaga todas las salidas del puerto 1
    P1DIR = 0x07;   // P1.0 = DS, P1.1 = SHCP/SCTP, P1.2 = OE
    P1OUT |= 0x04;  // Pone OE en alto, por lo que las salidas del shift register están apagadas


    while(1) {
        // Imprime desde el 0 hasta el 9 y después el punto.
        // Cambia cada 0.25 segundos
        for(i = 0; i < 11; i++) {
            imprime_digito(arreglo[i]);

            __delay_cycles(4000000);
        }
    }
	
	return 0;
}
