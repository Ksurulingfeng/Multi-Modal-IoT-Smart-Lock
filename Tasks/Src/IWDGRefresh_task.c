#include "task_headfile.h"
#include "sys.h"
#include "iwdg.h"

void vIWDGRefreshTask(void *pvParameters)
{
    while(1) {
		HAL_IWDG_Refresh(&hiwdg);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
