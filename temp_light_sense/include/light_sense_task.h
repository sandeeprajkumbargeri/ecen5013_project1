#ifndef __LIGHT_SENSE_TASK_H
#define __LIGHT_SENSE_TASK_H

float calculate_ambient_lux(uint16_t adc_channel_out0, uint16_t adc_channel_out1);
void *light_sense_task_thread(void *args);
#endif
