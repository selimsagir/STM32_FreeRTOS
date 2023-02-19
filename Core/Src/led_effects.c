/*
 * led_effects.c
 *
 *  Created on: Jan 29, 2023
 *      Author: selim
 */

#include "main.h"

void led_effect_stop(){
	for (int i = 0; i < 4; i++) {
		xTimerStop(handle_led_timer[i], portMAX_DELAY);
	}
}

void led_effect(uint8_t n){

	led_effect_stop();
	xTimerStart(handle_led_timer[n-1], portMAX_DELAY);
}


void turn_on_all_leds(){

	HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_SET);
}

void turn_off_all_leds(){

	HAL_GPIO_WritePin(LD10_GPIO_Port, LD10_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD7_GPIO_Port, LD7_Pin, GPIO_PIN_RESET);
}

void turn_on_odd_leds(){
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
}

void turn_off_odd_leds(){
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
}



void LED_effect1()
{
    // Declare a static variable "flag" with an initial value of 1
    static int flag = 1;

    // Perform the XOR operation on the "flag" variable with 1
    // This compares each bit of the first operand (flag) to the corresponding bit of the second operand (1)
    // If the bits are the same, the corresponding result bit is set to 0. If the bits are different, the corresponding result bit is set to 1.
    flag = flag ^ 1;

    // Check the value of "flag" after the XOR operation
    if (flag == 1) {
        // If "flag" is 1, call the function to turn on all the LEDs
        turn_on_all_leds();
    } else {
        // If "flag" is 0, call the function to turn off all the LEDs
        turn_off_all_leds();
    }
}


void LED_effect2()
{
    // Declare a static variable "flag" with an initial value of 1
    static int flag = 1;

    // Perform the XOR operation on the "flag" variable with 1
    // This compares each bit of the first operand (flag) to the corresponding bit of the second operand (1)
    // If the bits are the same, the corresponding result bit is set to 0. If the bits are different, the corresponding result bit is set to 1.
    flag = flag ^ 1;

    // Check the value of "flag" after the XOR operation
    if (flag == 1) {
        // If "flag" is 1, call the function to turn on all the LEDs
        turn_on_odd_leds();
    } else {
        // If "flag" is 0, call the function to turn off all the LEDs
        turn_off_odd_leds();
    }
}


void LED_effect3()
{
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

}
void LED_effect4()
{
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
}

