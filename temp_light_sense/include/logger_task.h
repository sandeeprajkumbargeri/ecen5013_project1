#ifndef __LOGGER_TASK_H
#define __LOGGER_TASK_H

#include <time.h>
#include "../main.h"

#define ({
		})

void *logger_task_thread(void *);
void *heartbeat_notifier(void *);
void update_time(char *, size_t);


#endif
