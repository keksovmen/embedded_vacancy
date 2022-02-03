/**
 * @file main.c
 * @author Dmitrii Fedin
 * @brief Шаблон задачи по RTOS
 * @version 0.1
 * @date 2022-01-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */
 
 /*
	Данную задачу можно решить с использованием Queue,
	где в ISR будут толкаться нажатые кнопки,
	такое решение позволит обрабатывать 99% нажатий кнопок
	1% потеряется т.к. Queue слишком маленькая (overflow). 
	Но будет проблематично определить, 
	что несколько кнопок были нажаты одновременно, необходимо
	сохранять текущий момент времени или тик, затем анализировать их.
	
	Текущее решение при помощи семафора, но при помощи RTOS Task Notifications,
	что по документации быстрее и подходит для данной задачи, но 
	данный таск обработки нажатий имеет не максимальный приоритет, что
	не гарантирует обнаружения нажатия кнопки. Зато упрощает обнаружения
	одновременного нажатия кнопок.
	
	Решение задачи зависит от системы в которой она будет использоваться.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"


enum BUTTONS {
    OK_BUTTON, //пин 0
    CANCEL_BUTTON, //пин 1
    PREV_BUTTON, //пин 2
    NEXT_BUTTON //пин 3
};
#define BUTTONS_COUNT 4

// to notify
static TaskHandle_t h_buttons_task = NULL;


//couple of declarations
//можно сделать динамическими, передав второй аргумент длинны
void processGPIO_state(bool state[BUTTONS_COUNT]);
void fillGPIO_state(bool state[BUTTONS_COUNT]);


/**
 * @brief Эмуляция прерывания GPIO
 * 
 * @param pin_no номер пина. Может принимать значения от 0 до 3. 
 *
 */
void gpio_IRQ(uint8_t pin_no) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	vTaskNotifyGiveFromISR(, &xHigherPriorityTaskWoken);
	if (xHigherPriorityTaskWoken == pdTRUE){
		//Вернуть контекст, зависит от платформы
		portYIELD_FROM_ISR(h_buttons_task, xHigherPriorityTaskWoken);
	}else{
		//Не удалось запустить таск обработки
		//можно использовать Queue и толкнуть pin_no
	}
}



/**
 * @brief Задача, которая должна обрабатывать нажатия кнопок
 * 
 * Типы нажатий, которые могут обрабытываться 
 *  - одиночные
 *  - зажаты 2 кнопки. К примеру, OK и PREV
 * 
 *  В качестве действия можно использовать printf()
 * 
 * @param arg 
 */
void buttons_process_task(void* arg) {
	bool buttons_state[BUTTONS_COUNT];
	
    for(;;) {
		//ждем пока не случится эвент нажатия и scheduler не продолжит таск
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		//если мы здесь то смысла от текущего прерывания нет
		disableGPIO_IRQ();
		
		//считываем состояние пинов
		fillGPIO_state(buttons_state);
		//выводим текущие нажатия
		processGPIO_state(buttons_state);
		
		enableGPIO_IRQ();
		
    }
    vTaskDelete(NULL);
}


/**
 * @brief Функция для запуска 
 * 
 */
void start_buttons_processing() {
    
    //якобы создаём задачу
    xTaskCreate(_vBackground_Task, "buttons_task", 1024*4, NULL, 1, &h_buttons_task);

    //якобы включаем прерывания
    enableGPIO_IRQ();

    //можно считать, что прерывания можно останавливать, если необходимо
    //disableGPIO_IRQ();
}

//дополнительно доступные функции, которыми можно пользоваться при необходимости
bool gpio_read(uint8_t pin_no);
void enableGPIO_IRQ();
void disableGPIO_IRQ();


//definitions
void fillGPIO_state(bool state[BUTTONS_COUNT]){
	state[0] = gpio_read(OK_BUTTON);
	state[1] = gpio_read(CANCEL_BUTTON);
	state[2] = gpio_read(PREV_BUTTON);
	state[3] = gpio_read(NEXT_BUTTON);
}

void processGPIO_state(bool state[BUTTONS_COUNT]){
	bool at_least_one_pressed = false;
	for (int i = 0; i < BUTTONS_COUNT; i++){
		if(state[i] == true){
			if(at_least_one_pressed == false){
				at_least_one_pressed = true;
				printf("%s", "Currently pressed button(s): ");
			}
			printf("%d, ", i);
		}
	}
	if (at_least_one_pressed == true){
		printf("%s", "\n");
	}
}