/*
 * button.h
 *
 *  Created on: 21 de jul. de 2024
 *      Author: FelipeAlmeida
 */

#ifndef MAIN_BUTTON_BUTTON_H_
#define MAIN_BUTTON_BUTTON_H_



#define BUTTON_GPIO 9
#define DEBOUNCE_DELAY_MS 50
void button_init();
int read_button_debounced();

#endif /* MAIN_BUTTON_BUTTON_H_ */
