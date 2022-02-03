/**
 * @file module.c
 * @author Dmitrii Fedin 
 * @brief Абстрактный модуль для тестов
 * @version 0.1
 * @date 2022-01-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "module.h"
#include <stdlib.h>


#define PAYLOAD_DATA_LEN (50)

struct Payload {
    uint8_t data[PAYLOAD_DATA_LEN];
};
typedef struct Payload* Payload;


struct Message {
    uint32_t id;
    struct {
        message_p_count_t count;
        Payload elements;
    } payloads;
};


Message message_create(message_p_count_t payloads_count) {
    if (payloads_count == 0) {
        return NULL;
    }
    Message ptr = (Message)calloc(1, sizeof(struct Message));
    if (ptr == NULL) {
        return NULL;
    }
    ptr->payloads.elements = (Payload)calloc(payloads_count, sizeof(struct Payload));
    if ((ptr->payloads.elements) == NULL) {
        free(ptr);
        return NULL;
    }
    ptr->payloads.count = payloads_count;
    return ptr;
}



void message_free_s(Message* message) {
    if (message == NULL) {
        return;
    }
    Message ptr = *message;
    if (ptr == NULL) {
        return;
    }
    if ((ptr->payloads.elements)!= NULL) {
        free(ptr->payloads.elements);
        ptr->payloads.elements = NULL;
    }
    free(ptr);
    *message = NULL;
}