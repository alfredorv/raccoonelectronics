/*
 * LiquidCrystalDisplay.h
 *
 *  Created on: 09/06/2020
 *      Author: Ing. Alfredo Renter�a Villanueva
 */

#ifndef LIQUIDCRYSTALDISPLAY_H_
#define LIQUIDCRYSTALDISPLAY_H_

void espera_n_ms(unsigned int milisegundos);
void envia_dato(unsigned char dato);
void envia_comando(unsigned char comando);
void inicializar_lcd(void);
void print(char *cadena);

#endif /* LIQUIDCRYSTALDISPLAY_H_ */
