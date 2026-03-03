#ifndef TASK_HEADFILE
#define TASK_HEADFILE

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "event_groups.h"

extern TaskHandle_t startTask_handle;
extern TaskHandle_t doorLockControlTask_handle;
extern TaskHandle_t fingerTask_handle;
extern TaskHandle_t keypadTask_handle;
extern TaskHandle_t IWDGRefreshTask_handle;
extern TaskHandle_t networkTask_handle;
extern TaskHandle_t NFCTask_handle;
extern TaskHandle_t verifyTask_handle;
extern SemaphoreHandle_t xSemphore_Display_Mutex;

#endif /* TASK_HEADFILE */
