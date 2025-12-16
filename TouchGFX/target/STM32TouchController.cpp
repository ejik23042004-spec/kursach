/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : STM32TouchController.cpp
  ******************************************************************************
  * This file was created by TouchGFX Generator 4.26.0. This file is only
  * generated once! Delete this file from your project and re-generate code
  * using STM32CubeMX or change this file manually to update it.
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

/* USER CODE BEGIN STM32TouchController */

#include <STM32TouchController.hpp>

// Внешние переменные из main.c для передачи координат тача
extern volatile uint16_t touch_x;
extern volatile uint16_t touch_y;
extern volatile uint8_t touch_pressed;

void STM32TouchController::init()
{
    /**
     * Initialize touch controller and driver
     * Инициализация FT6336 выполняется в main.c в задаче TouchGFX_Task_custom
     */
}

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    /**
     * Читаем координаты тача из глобальных переменных,
     * которые обновляются задачей TouchTask каждые 10 мс
     */
    if (touch_pressed)
    {
        x = (int32_t)touch_x;
        y = (int32_t)touch_y;
        return true;
    }
    
    return false;
}

/* USER CODE END STM32TouchController */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
