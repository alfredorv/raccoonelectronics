/*
 * LiquidCrystalDisplay.c
 *
 *  Created on: 09/06/2020
 *      Author: Ing. Alfredo Rentería Villanueva
 */

#include  <msp430.h>

#define RS              0x10
#define E               0x20

unsigned char tabla_nt7605[] = { 0x02, 0x02, 0x0C, 0x00, 0x0C, 0x00, 0x01, 0x00, 0x06 };

/*
 * nombre: espera_n_ms
 * params: milisegundos
 * return: none
 */
void espera_n_ms(unsigned int milisegundos) {
    unsigned int i = 0;

    for(i = 0; i < milisegundos; i++) {
        __delay_cycles(16000);             // Si el oscilador es de 16MHz, genera una espera de 1ms
    }
}

/*
 * nombre: prepara_dato
 * params: dato
 * return: none
 */
void envia_dato(unsigned char dato) {
    unsigned char estado_actual;

    dato &= 0x0F;                     // Toma solamente los 4 primeros bits
    estado_actual = (P1OUT & 0xF0);   // Lee solamente los 4 bits altos del puerto 1

    P1OUT = estado_actual | dato;     // Escribe las salidas respetando los bits altos del puerto 1

    P1OUT ^= E;                       // E = HIGH
    espera_n_ms(1);                   // Espera 1ms
    P1OUT ^= E;                       // E = LOW
}

/*
 * nombre: envia_comando
 * params: comando
 * return: none
 */
void envia_comando(unsigned char comando) {
    P1OUT &= ~RS;                     // RS = LOW para indicar que se trata de un comando
    espera_n_ms(1);                   // Espera 1ms para asegurar que RS está en LOW

    envia_dato(comando >> 4);
    envia_dato(comando);
}

/*
 * nombre: print_caracter
 * params: dato
 * return: none
 */
void print_caracter(unsigned char dato) {
    P1OUT |= RS;                      // RS = HIGH para indicar que se trata de un dato
    espera_n_ms(1);                   // Espera 1ms para asegurar que RS está en LOW

    envia_dato(dato >> 4);
    envia_dato(dato);
}

/*
 * nombre: inicializar_lcd
 * params: controlador
 * return: none
 */
void inicializar_lcd(void) {
    unsigned char i = 0;

    espera_n_ms(35);                // Espera más de 30ms, según la datasheet del driver NT7605

    P1OUT &= ~RS;                   // RS = LOW para indicar que se trata de un comando

    for(i = 0; i < sizeof(tabla_nt7605); i++) {
        envia_dato(tabla_nt7605[i]);             // Envia cada dato de la lista de inicialización
        espera_n_ms(1);
    }
}

/*
 * nombre: print
 * params: dato
 * return: none
 */
void print(char *cadena) {
    char *puntero;

    puntero = cadena;

    P1OUT |= RS;
    espera_n_ms(1);                   // Espera 1ms para asegurar que RS está en HIGH

    while(*puntero != '\0') {
        print_caracter(*puntero);

        espera_n_ms(1);

        puntero++;
    }
}

/*
 * nombre: print
 * params: arreglo, posicion_memoria
 * return: none
 */
void crear_caracter(unsigned char *arreglo, unsigned char posicion_cgram) {
    unsigned char i = 0;

    if(posicion_cgram > 7) posicion_cgram = 7;      // Limita la dirección de la RAM

    envia_comando(0x40 | (posicion_cgram * 8));     // Selecciona CGRAM e indica la dirección

    P1OUT |= RS;                                    // Indica que se enviarán datos (RS = HIGH)
    espera_n_ms(1);                                 // Espera 1ms para asegurar que RS está en HIGH

    for(i = 0; i < 8; i++) {
        print_caracter(arreglo[i]);                     // Envía el caracter personalizado
    }
}
