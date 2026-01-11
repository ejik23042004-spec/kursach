/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "crc.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "app_touchgfx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "ft6336.h"
#include "ds18b20.h"
#include "FreeRTOS.h"
#include "queue.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
ds18b20_t ds18;

// Структура для передачи температуры через очередь
typedef struct {
    int16_t temp_cold;  // Температура в холодной зоне (в сотых градуса, например 1025 = 10.25°C)
    int16_t temp_warm;  // Температура в теплой зоне
} temp_data_t;

// Очередь для передачи температуры в TouchGFX (FreeRTOS очередь)
QueueHandle_t tempQueueHandle;

// Последние корректные значения температуры (для защиты от аномальных значений)
static temp_data_t last_valid_temp = {0, 0};

// Границы допустимых значений температуры (в сотых градуса)
// DS18B20 диапазон: -55°C до +125°C, используем более узкий диапазон для практических применений
#define TEMP_MIN_CENTIDEGREES  (-5000)  // -50.00°C
#define TEMP_MAX_CENTIDEGREES  (10000)  // +100.00°C

// Защита от зависания и отваливания датчиков
#define TEMP_VALID_DATA_TIMEOUT_MS    (30000)  // 30 секунд без валидных данных - переинициализация
#define TEMP_REINIT_MAX_ATTEMPTS      (3)      // Максимум попыток переинициализации

static uint32_t last_valid_data_time = 0;      // Время последнего успешного чтения
static uint32_t last_reinit_time = 0;          // Время последней переинициализации
static uint8_t reinit_attempts = 0;            // Счетчик попыток переинициализации

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */


void TouchGFX_Task_custom(void *argument);
void StartDisplayTask(void *argument);
void VSyncTask(void *argument);
void TouchTask(void *argument);
void Temp_Task(void *argument);
void TempControl_Task(void *argument);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_CRC_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TouchGFX_Init();
  /* Call PreOsInit function */
  MX_TouchGFX_PreOSInit();
  /* USER CODE BEGIN 2 */
  
  // Инициализация FT6336 тачскрина (до запуска RTOS)

  osThreadId_t ds1820bTaskHandle;
  osThreadId_t displayTaskHandle;
  osThreadId_t touchgfxTaskHandle;
  osThreadId_t vsyncTaskHandle;
  osThreadId_t touchTaskHandle;
  osThreadId_t tempControlTaskHandle;

  const osThreadAttr_t ds1820bTask_attributes = {
        .name = "ds1820bTask",
        .stack_size = 512 * 4,
        .priority = (osPriority_t) osPriorityNormal,
    };


  const osThreadAttr_t displayTask_attributes = {
      .name = "displayTask",
      .stack_size = 512 * 4,
      .priority = (osPriority_t) osPriorityNormal,
  };

  const osThreadAttr_t touchgfxTask_attributes = {
    .name = "TouchGFX",
    .stack_size = 512 * 4,      // увеличенный стек для TouchGFX
    .priority = (osPriority_t) osPriorityHigh
  };

  const osThreadAttr_t vsyncTask_attributes = {
    .name = "VSyncTask",
    .stack_size = 512 * 4,
    .priority = (osPriority_t) osPriorityAboveNormal
  };

  const osThreadAttr_t touchTask_attributes = {
    .name = "TouchTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityNormal
  };

  const osThreadAttr_t tempControlTask_attributes = {
    .name = "TempControlTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t) osPriorityNormal
  };

  touchgfxTaskHandle = osThreadNew(TouchGFX_Task_custom, NULL, &touchgfxTask_attributes);

  /* Периодический VSYNC в отдельном потоке (для тестов, без использования таймеров) */
  vsyncTaskHandle = osThreadNew(VSyncTask, NULL, &vsyncTask_attributes);

  /* Задача опроса тачскрина каждые 10 мс */
  touchTaskHandle = osThreadNew(TouchTask, NULL, &touchTask_attributes);

  // Создаем FreeRTOS очередь для передачи температуры (размер 5 элементов)
  tempQueueHandle = xQueueCreate(5, sizeof(temp_data_t));

  ds1820bTaskHandle = osThreadNew(Temp_Task, NULL, &touchTask_attributes);

  // Задача управления GPIO по температуре
  tempControlTaskHandle = osThreadNew(TempControl_Task, NULL, &tempControlTask_attributes);








  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

//  ILI9341_Init();
//  HAL_Delay (1000);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET); // CS = HIGH
//


  while (1)
  {

//	    ILI9341_FillScreen(0xF81F);   // Фиолетовый
//	    HAL_Delay(500);
//	    ILI9341_FillScreen(0x001F);   // Синий
//	    HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#include <math.h>


void TouchGFX_Task_custom(void *argument)
{
    /* Поток TouchGFX: внутри MX_TouchGFX_Process() вызывается touchgfx_taskEntry(),
     * который содержит свой собственный бесконечный цикл. Вызывать его в for(;;)
     * нельзя — достаточно одного вызова.
     */
    (void)argument;
    MX_TouchGFX_Process();  // не возвращает управление
}




void ds18_tim_cb(TIM_HandleTypeDef *htim)
{
    ow_callback(&ds18.ow);
}

// Функция переинициализации датчиков
static void reinit_ds18b20_sensors(void)
{
    ow_init_t ow_init_struct;
    ow_init_struct.tim_handle = &htim2;
    ow_init_struct.gpio = GPIOA;
    ow_init_struct.pin = GPIO_PIN_9;
    ow_init_struct.tim_cb = ds18_tim_cb;
    ow_init_struct.done_cb = NULL;   // Optional
    ow_init_struct.rom_id_filter = DS18B20_ID;

    ds18b20_init(&ds18, &ow_init_struct);

    // Update ROM IDs for all devices
    ds18b20_update_rom_id(&ds18);
    while(ds18b20_is_busy(&ds18));

    // Configure alarm thresholds and resolution
    ds18b20_config_t ds18_conf = {
        .alarm_high = 50,
        .alarm_low = -50,
        .cnv_bit = DS18B20_CNV_BIT_12
    };
    ds18b20_conf(&ds18, &ds18_conf);
    while(ds18b20_is_busy(&ds18));
}

void Temp_Task(void *argument)
{
    (void)argument;
    ow_init_t ow_init_struct;
    ow_init_struct.tim_handle = &htim2;
    ow_init_struct.gpio = GPIOA;
    ow_init_struct.pin = GPIO_PIN_9;
    ow_init_struct.tim_cb = ds18_tim_cb;
    ow_init_struct.done_cb = NULL;   // Optional
    ow_init_struct.rom_id_filter = DS18B20_ID;

    ds18b20_init(&ds18, &ow_init_struct);

    // Update ROM IDs for all devices
    ds18b20_update_rom_id(&ds18);
    while(ds18b20_is_busy(&ds18));

    // Configure alarm thresholds and resolution
    ds18b20_config_t ds18_conf = {
        .alarm_high = 50,
        .alarm_low = -50,
        .cnv_bit = DS18B20_CNV_BIT_12
    };
    ds18b20_conf(&ds18, &ds18_conf);
    while(ds18b20_is_busy(&ds18));

    // Инициализируем время последнего успешного чтения
    last_valid_data_time = HAL_GetTick();
    last_reinit_time = HAL_GetTick();
    reinit_attempts = 0;

    temp_data_t temp_data;
    uint32_t current_time;
    uint32_t time_since_valid_data;
    uint8_t both_valid = 0;

    for (;;)
    {
        current_time = HAL_GetTick();
        
        // Запускаем конвертацию температуры на всех датчиках
    	ds18b20_cnv(&ds18);
    	while(ds18b20_is_busy(&ds18));
    	while(!ds18b20_is_cnv_done(&ds18));

        // Читаем температуру с первого датчика (холодная зона, индекс 0)
    	ds18b20_req_read(&ds18, 0);
    	while(ds18b20_is_busy(&ds18));
    	temp_data.temp_cold = ds18b20_read_c(&ds18);
    	osDelay(10);
        // Читаем температуру со второго датчика (теплая зона, индекс 1)
    	ds18b20_req_read(&ds18, 1);
    	while(ds18b20_is_busy(&ds18));
    	temp_data.temp_warm = ds18b20_read_c(&ds18);

        // Защита от аномальных значений
        both_valid = 1;
        
        // Проверяем диапазон для холодной зоны
        if (temp_data.temp_cold < TEMP_MIN_CENTIDEGREES || temp_data.temp_cold > TEMP_MAX_CENTIDEGREES) {
            // Используем последнее корректное значение
            temp_data.temp_cold = last_valid_temp.temp_cold;
            both_valid = 0;  // Данные невалидны
        } else {
            // Сохраняем корректное значение
            last_valid_temp.temp_cold = temp_data.temp_cold;
        }

        // Проверяем диапазон для теплой зоны
        if (temp_data.temp_warm < TEMP_MIN_CENTIDEGREES || temp_data.temp_warm > TEMP_MAX_CENTIDEGREES) {
            // Используем последнее корректное значение
            temp_data.temp_warm = last_valid_temp.temp_warm;
            both_valid = 0;  // Данные невалидны
        } else {
            // Сохраняем корректное значение
            last_valid_temp.temp_warm = temp_data.temp_warm;
        }

        // Если оба значения валидны, обновляем время последнего успешного чтения
        if (both_valid) {
            last_valid_data_time = current_time;
            reinit_attempts = 0;  // Сбрасываем счетчик попыток при успешном чтении
        }

        // Проверяем, не прошло ли слишком много времени без валидных данных
        time_since_valid_data = current_time - last_valid_data_time;
        
        if (time_since_valid_data > TEMP_VALID_DATA_TIMEOUT_MS) {
            // Долгое время нет валидных данных - переинициализируем датчики
            reinit_attempts++;
            last_reinit_time = current_time;
            reinit_ds18b20_sensors();
            
            // Если переинициализация не помогла после нескольких попыток - перезапускаем задачу
            if (reinit_attempts >= TEMP_REINIT_MAX_ATTEMPTS) {
                // Сбрасываем все счетчики и переинициализируем задачу
                last_valid_data_time = current_time;
                last_reinit_time = current_time;
                reinit_attempts = 0;
                
                // Переинициализируем датчики еще раз
                reinit_ds18b20_sensors();
                
                // Небольшая задержка перед продолжением работы
                osDelay(1000);
            }
        }

        // Отправляем данные в FreeRTOS очередь для TouchGFX
        // Используем xQueueSend с таймаутом 0 (неблокирующий)
        xQueueSend(tempQueueHandle, &temp_data, 0);

        osDelay(1000);
    }
}

// Функция для получения температуры из FreeRTOS очереди (для использования в C++)
// Возвращает pdTRUE (1) при успехе, pdFALSE (0) при ошибке
BaseType_t getTemperatureFromQueue(temp_data_t *data)
{
    if (tempQueueHandle != NULL) {
        return xQueueReceive(tempQueueHandle, data, 0);  // Таймаут 0 = неблокирующий
    }
    return pdFALSE;
}

// Функция toggle для PB10 (для использования в C++)
void togglePB10(void)
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
}

// Задача управления GPIO PA8 по температуре
// Если температура < 32°C, подтягивает PA8 к земле (GPIO_PIN_RESET)
// Если температура >= 32°C, отпускает PA8 (GPIO_PIN_SET для OPEN_DRAIN)
void TempControl_Task(void *argument)
{
    (void)argument;
    temp_data_t temp_data;
    const int16_t TEMP_THRESHOLD_CENTIDEGREES = 3200;  // 32.00°C в сотых градуса
    
    for (;;)
    {
        // Получаем данные температуры из очереди БЕЗ удаления (xQueuePeek)
        // Это позволяет TouchGFX также получить эти данные
        if (xQueuePeek(tempQueueHandle, &temp_data, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
            // Проверяем температуру холодной зоны (можно использовать любую зону или обе)
            // Используем температуру холодной зоны для управления
            if (temp_data.temp_cold < TEMP_THRESHOLD_CENTIDEGREES)
            {
                // Температура ниже 32°C - подтягиваем PA8 к земле
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
            }
            else
            {
                // Температура >= 32°C - отпускаем PA8 (для OPEN_DRAIN это HIGH)
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
            }
        }
        // Если данные не получены (таймаут), продолжаем цикл
    }
}

void VSyncTask(void *argument)
{
    (void)argument;

    /* Простой тестовый вариант: генерируем VSYNC из потока с периодом ~16 мс (~60 Гц) */
    extern void touchgfxSignalVSync(void);


    ILI9341_Init();
    //ILI9341_FillScreen(0xFFFF); // Черный

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    //osDelay(1000);
    ILI9341_InvertColors(1);
//    ILI9341_FillScreen(ILI9341_BLACK);
//    HAL_Delay(300);
//    ILI9341_FillScreen(ILI9341_WHITE);
//    HAL_Delay(300);
//    ILI9341_FillScreen(ILI9341_RED);
//    HAL_Delay(300);
//    ILI9341_FillScreen(ILI9341_GREEN);
//     HAL_Delay(300);
//    ILI9341_FillScreen(ILI9341_YELLOW);
//    HAL_Delay(300);
//    ILI9341_FillScreen(ILI9341_BLUE);


    for (;;)
    {
        touchgfxSignalVSync();
        osDelay(10);
    }
}




#define M_PI 3.14159265358979323846
float sine_function(float x, uint16_t amplitude, uint16_t phase_shift)
{
    // Нормализуем x к диапазону [0, 1]
    float normalized = x / 320.0f;

    // Преобразуем в фазу для полного цикла синуса [0, 2π]
    float phase = normalized * 5.0f * M_PI + (phase_shift/1);

    // Вычисляем синус (диапазон [-1, 1])
    float sine_value = (float)sin((double)phase);

    // Масштабируем к диапазону [0, 240]
    float result = (sine_value + 1.0f) * amplitude+(120 - amplitude);

    return result;
}


typedef struct {
    float y_top;
    float y_bottom;
} all_y_for_circle;

all_y_for_circle circle_function(float x, uint16_t a, uint16_t b, uint16_t r)
{
	all_y_for_circle points;

	 // Проверяем, находится ли x в пределах круга
	    if (x < a - r || x > a + r) {
	        points.y_top = NAN;
	        points.y_bottom = NAN;
	        return points;
	    }
    // Вычисляем y для верхней полуокружности
	 points.y_top = b + sqrt(r*r - (x-a)*(x-a));

    // Или для нижней полуокружности, в зависимости от вашей логики
	 points.y_bottom = b - sqrt(r*r - (-x+a)*(-x+a));

    return points;
}

/* USER CODE BEGIN TouchData */
// Глобальные переменные для передачи координат тача в TouchGFX
volatile uint16_t touch_x = 0;
volatile uint16_t touch_y = 0;
volatile uint8_t touch_pressed = 0;
/* USER CODE END TouchData */

/**
 * @brief Задача опроса тачскрина FT6336
 * @param argument Не используется
 * @retval None
 */
void TouchTask(void *argument)
{
    (void)argument;
    uint16_t x, y;
    uint8_t touch_status;
    
    // Небольшая задержка для инициализации системы
    osDelay(100);
    
    if (ft6336_init() != 0)
      {
          // Ошибка инициализации FT6336
          Error_Handler();
      }


    for (;;)
    {
        // Проверяем статус касания
        touch_status = ft6336_get_td_status();
        
        if (touch_status > 0)
        {
            // Есть касание - читаем координаты
            // Меняем местами X и Y, т.к. они перепутаны в чипе
            ft6336_get_touch1_position(&y, &x);
            
            // Сохраняем координаты в глобальные переменные для TouchGFX
            touch_x = x;
            touch_y = y;
            touch_pressed = 1;
            
            // Очищаем флаг прерывания (если используется)
            ft6336_clear_interrupt();
        }
        else
        {
            // Нет касания
            touch_pressed = 0;
        }
        
        // Опрос каждые 10 мс (100 Гц)
        osDelay(10);
    }
}




void StartDisplayTask(void *argument)
{
    // Перед работой с дисплеем подождем инициализацию системы
    osDelay(100);

    // Инициализация дисплея

    char cmd = 0x04;
    char buf[2] = {0x00, 0x00};



    ILI9341_Init();
    ILI9341_FillScreen(0xFFFF); // Черный
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    uint16_t current_color = 0;
   	uint16_t x = 0;
   	uint16_t y = 0 , y2 = 0;
   	uint16_t a = 1;
   	uint16_t current_state = 1;
   	uint16_t phase = 0;
	uint16_t amplitude = 0;
    for(;;)
    {
    	if(x==320)
    	{
    		amplitude = amplitude + current_state;
    		phase = phase + 1;
    		x=0;
    		y=0;

    		current_color = 0x0000;
    		//ILI9341_DrawPixel(x, y, 0x0000);
    	}
    	if(amplitude==118){

    		current_state = -1;

    	}
    	if(amplitude==2){

    	    		current_state = 1;

    	    	}



//    	if(x==320 && current_state == 1)
//    	    	{
//    				//phase++;
//    	    		x=0;
//    	    		y=0;
//    	    		current_state = 0;
//    	    		current_color = 0xFFFF;
//    	    		//ILI9341_DrawPixel(x, y, 0x0000);
//    	    	}

    //	osDelay(10);


    	x++;

    	y=sine_function(x,amplitude,phase);
    	y2=sine_function(x,amplitude - current_state,phase - 1);
    	uint16_t a1 = 320/2;
    	uint16_t b1 = 240/2;
    	uint16_t r1 = 50;
    	uint16_t y3 = 0;

    	//y3=sine_function(x,amplitude - current_state-1,phase - 1);
    	//HAL_UART_Transmit (&huart2, "Current y: %u", y,100);

    	for (uint8_t i =0; i < a; i++){
        //ILI9341_DrawPixel(x, y2+i + a/2, 0xFFFF);
        //ILI9341_DrawPixel(x, y2+i - a/2, 0xFFFF);
        //ILI9341_DrawPixel(x, y2+i+1 - a/2, 0xFFFF);
    	//ILI9341_DrawPixel(x, y+i - a/2, 0x0000);
    		for (float x = a1 - r1; x <= a1 + r1; x += 1.0f) {
    		        // Получаем точки окружности для текущего X
    		        all_y_for_circle points = circle_function(x, a1, b1, r1);




    		            ILI9341_DrawPixel((uint16_t)round(x), (uint16_t)round(points.y_top)+ a/2, 0x0000);
    		            ILI9341_DrawPixel((uint16_t)round(x), (uint16_t)round(points.y_bottom)+ a/2, 0x0000);

    		        osDelay(5);

    		          //if (x > a1 + r1) {
    		          //ILI9341_DrawPixel((uint16_t)round(x), (uint16_t)round(points.y_top)+ a/2, 0xFFFF);
    		          //}


    		          //if (x > a1 - r1) {
    		          //LI9341_DrawPixel((uint16_t)round(x), (uint16_t)round(points.y_bottom)+ a/2, 0xFFFF);
    		          //}
    		    }
    	}
        //ILI9341_FillScreen(0xFFFF); // Черный
    }
}



/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
