#include "init.h"
#include "AS608.h"
#include "RC522.h"
#include "OLED_I2C.h"
#include "Servo.h"
#include "AT24Cxx.h"
#include "ESP8266.h"
#include "debug.h"

/* 门锁控制任务配置 */
#define DOORLOCKCONTROL_TASK_STACK    256
#define DOORLOCKCONTROL_TASK_PRIORITY 4
TaskHandle_t doorLockControlTask_handle;
void vDoorLockControlTask(void *pvParameters);
/* 指纹识别任务配置 */
#define FINGER_TASK_STACK    128
#define FINGER_TASK_PRIORITY 2
TaskHandle_t fingerTask_handle;
void vFingerTask(void *pvParameters);
/* 喂狗任务配置 */
#define IWDGREFRESH_TASK_STACK    128
#define IWDGREFRESH_TASK_PRIORITY 5
TaskHandle_t IWDGRefreshTask_handle;
void vIWDGRefreshTask(void *pvParameters);
/* 键盘任务配置 */
#define KEYPAD_TASK_STACK    128
#define KEYPAD_TASK_PRIORITY 2
TaskHandle_t keypadTask_handle;
void vKeypadTask(void *pvParameters);
/* 网络任务配置 */
#define NETWORK_TASK_STACK    384
#define NETWORK_TASK_PRIORITY 1
TaskHandle_t networkTask_handle;
void vNetworkTask(void *pvParameters);
/* NFC识别任务配置 */
#define NFC_TASK_STACK    160
#define NFC_TASK_PRIORITY 2
TaskHandle_t NFCTask_handle;
void vNFCTask(void *pvParameters);
/* 认证任务配置 */
#define VERIFY_TASK_STACK    256
#define VERIFY_TASK_PRIORITY 3
TaskHandle_t verifyTask_handle;
void vVerifyTask(void *pvParameters);

SemaphoreHandle_t xSemphore_Display_Mutex; // 显示互斥信号量

AS608_HandleTypeDef has608;
RC522_HandleTypeDef hrc522;

// 系统初始化
void System_Init(void)
{
    /* 硬件初始化 */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();        // MFRC522
    MX_TIM2_Init();        // Servo
    MX_USART1_UART_Init(); // 调试串口
    MX_USART2_UART_Init(); // ESP8266串口
    MX_USART3_UART_Init(); // AS608串口
    MX_IWDG_Init();

    /* 驱动初始化 */
    HAL_Delay(500);
    OLED_Init();
    Servo_Init();
    
    AS608_Init(&has608, &huart3, AS608_IRQ_GPIO_Port, AS608_IRQ_Pin);
    MFRC522_Init(&hrc522, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin);

    /* FreeRTOS初始化 */
    // 创建队列
    xSemphore_Display_Mutex = xSemaphoreCreateMutex();
    // 创建任务
    xTaskCreate(vDoorLockControlTask, "vDoorLockControlTask", DOORLOCKCONTROL_TASK_STACK, NULL, DOORLOCKCONTROL_TASK_PRIORITY, &doorLockControlTask_handle);
    xTaskCreate(vFingerTask, "vFingerTask", FINGER_TASK_STACK, NULL, FINGER_TASK_PRIORITY, &fingerTask_handle);
    xTaskCreate(vKeypadTask, "vKeypadTask", KEYPAD_TASK_STACK, NULL, KEYPAD_TASK_PRIORITY, &keypadTask_handle);
    xTaskCreate(vNetworkTask, "vNetworkTask", NETWORK_TASK_STACK, NULL, NETWORK_TASK_PRIORITY, &networkTask_handle);
    xTaskCreate(vNFCTask, "vNFCTask", NFC_TASK_STACK, NULL, NFC_TASK_PRIORITY, &NFCTask_handle);
    xTaskCreate(vVerifyTask, "vVerifyTask", VERIFY_TASK_STACK, NULL, VERIFY_TASK_PRIORITY, &verifyTask_handle);
    xTaskCreate(vIWDGRefreshTask, "vIWDGRefreshTask", IWDGREFRESH_TASK_STACK, NULL, IWDGREFRESH_TASK_PRIORITY, &IWDGRefreshTask_handle);
}

// 系统时钟配置
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.LSIState       = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}
