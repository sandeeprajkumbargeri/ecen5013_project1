#ifndef __LIGHT_SENSE_TASK_H
#define __LIGHT_SENSE_TASK_H

#include "../main.h"

#define INTEGRATION_TIME_MASK 0x03
#define GAIN_MASK 0x01<<4
#define INT_MASK 0x11<<4

float calculate_ambient_lux(uint16_t, uint16_t);
void *light_sense_task_thread(void *);
void light_service_request(mq_temp_light_payload_t *, char *, int);

#endif
