/*
 * LiquidCrystalDisplay.h
 *
 *  Created on: 09/06/2020
 *      Author: arent
 */

#ifndef LIQUIDCRYSTALDISPLAY_H_
#define LIQUIDCRYSTALDISPLAY_H_

void espera_n_ms(unsigned int milisegundos);
void envia_dato(unsigned char dato);
void envia_comando(unsigned char comando);
void print_caracter(unsigned char dato);
void inicializar_lcd(void);
void print(char *cadena);
void crear_caracter(unsigned char *arreglo, unsigned char posicion_cgram);



#endif /* LIQUIDCRYSTALDISPLAY_H_ */
