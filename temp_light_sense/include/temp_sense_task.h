#ifndef __TEMP_SENSE_TASK_H
#define __TEMP_SENSE_TASK_H

#include "../main.h"

#define THRESHOLD_HIGH	20
#define THRESHOLD_LOW	15

#define RESOLUTION_MASK  0x03<<13
#define FAULT_BITS_MASK  0x03<<11
#define EM_MASK 0x01<<4
#define CONV_RATE_MASK 0x03<<6
#define CONV_RATE_SHIFT 6
#define SD_MASK 0x08

void *temp_sense_task_thread(void *args);
void service_request(mq_temp_light_payload_t *, char *, int);

#endif
