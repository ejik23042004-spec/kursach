/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : TouchGFXHAL.cpp
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

#include <TouchGFXHAL.hpp>

/* USER CODE BEGIN TouchGFXHAL.cpp */

using namespace touchgfx;

/* ******************************************************
 * Functions required by Partial Frame Buffer Strategy
 * ******************************************************
 *
 *  int touchgfxDisplayDriverTransmitActive() must return whether or not data is currently being transmitted, over e.g. SPI.
 *  void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels, uint16_t x, uint16_t y, uint16_t w, uint16_t h) will be called
 *  when the framework wants to send a block. The user must then transfer the data represented by the arguments.
 *
 *  A user must call touchgfx::startNewTransfer(); once touchgfxDisplayDriverTransmitBlock() has successfully sent a block.
 *  E.g. if using DMA to transfer the block, this could be called in the "Transfer Completed" interrupt handler.
 *
 */
#warning "A user must call touchgfx::startNewTransfer(); once touchgfxDisplayDriverTransmitBlock() has succesfully sent a block."
#warning "A user must implement C-methods touchgfxDisplayDriverTransmitActive() and touchgfxDisplayDriverTransmitBlock() used by the Partial Framebuffer Strategy."

void TouchGFXHAL::initialize()
{
    // Calling parent implementation of initialize().
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.
    // Please note, HAL::initialize() must be called to initialize the framework.

    TouchGFXGeneratedHAL::initialize();
}

/**
 * Gets the frame buffer address used by the TFT controller.
 *
 * @return The address of the frame buffer currently being displayed on the TFT.
 */
uint16_t* TouchGFXHAL::getTFTFrameBuffer() const
{
    // Calling parent implementation of getTFTFrameBuffer().
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.

    return TouchGFXGeneratedHAL::getTFTFrameBuffer();
}

/**
 * Sets the frame buffer address used by the TFT controller.
 *
 * @param [in] address New frame buffer address.
 */
void TouchGFXHAL::setTFTFrameBuffer(uint16_t* address)
{
    // Calling parent implementation of setTFTFrameBuffer(uint16_t* address).
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.

    TouchGFXGeneratedHAL::setTFTFrameBuffer(address);
}

/**
 * This function is called whenever the framework has performed a partial draw.
 *
 * @param rect The area of the screen that has been drawn, expressed in absolute coordinates.
 *
 * @see flushFrameBuffer().
 */
void TouchGFXHAL::flushFrameBuffer(const touchgfx::Rect& rect)
{
    // Calling parent implementation of flushFrameBuffer(const touchgfx::Rect& rect).
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.
    // Please note, HAL::flushFrameBuffer(const touchgfx::Rect& rect) must
    // be called to notify the touchgfx framework that flush has been performed.
    // To calculate the start address of rect,
    // use advanceFrameBufferToRect(uint8_t* fbPtr, const touchgfx::Rect& rect)
    // defined in TouchGFXGeneratedHAL.cpp

    TouchGFXGeneratedHAL::flushFrameBuffer(rect);
}

bool TouchGFXHAL::blockCopy(void* RESTRICT dest, const void* RESTRICT src, uint32_t numBytes)
{
    return TouchGFXGeneratedHAL::blockCopy(dest, src, numBytes);
}

/**
 * Configures the interrupts relevant for TouchGFX. This primarily entails setting
 * the interrupt priorities for the DMA and LCD interrupts.
 */
void TouchGFXHAL::configureInterrupts()
{
    // Calling parent implementation of configureInterrupts().
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.

    TouchGFXGeneratedHAL::configureInterrupts();
}

/**
 * Used for enabling interrupts set in configureInterrupts()
 */
void TouchGFXHAL::enableInterrupts()
{
    // Calling parent implementation of enableInterrupts().
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.

    TouchGFXGeneratedHAL::enableInterrupts();
}

/**
 * Used for disabling interrupts set in configureInterrupts()
 */
void TouchGFXHAL::disableInterrupts()
{
    // Calling parent implementation of disableInterrupts().
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.

    TouchGFXGeneratedHAL::disableInterrupts();
}

/**
 * Configure the LCD controller to fire interrupts at VSYNC. Called automatically
 * once TouchGFX initialization has completed.
 */
void TouchGFXHAL::enableLCDControllerInterrupt()
{
    // Calling parent implementation of enableLCDControllerInterrupt().
    //
    // To overwrite the generated implementation, omit the call to the parent function
    // and implement the needed functionality here.

    TouchGFXGeneratedHAL::enableLCDControllerInterrupt();
}

bool TouchGFXHAL::beginFrame()
{
    return TouchGFXGeneratedHAL::beginFrame();
}

void TouchGFXHAL::endFrame()
{
    TouchGFXGeneratedHAL::endFrame();
}

#include <stdbool.h>
#include <stdint.h>

// Forward declaration для startNewTransfer() из TouchGFXGeneratedHAL.cpp
namespace touchgfx {
    void startNewTransfer();
}

extern "C" {

// твоя функция из библиотеки дисплея
void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);

// флаг занятости передачи (нужен для touchgfxDisplayDriverTransmitActive)
static volatile bool isTransmittingBlock = false;

// Буфер для инверсии цветов. Размер должен быть не меньше block size
// из TouchGFXGeneratedHAL.cpp (ManyBlockAllocator<10000, ...>).
static uint16_t invertedBlockBuffer[10000];

/**
 * TouchGFX calls this when it has rendered a block that must be transferred to the display.
 * pixels is RGB565 (16bpp).
 */
void touchgfxDisplayDriverTransmitBlock(const uint8_t* pixels,
                                        uint16_t x, uint16_t y,
                                        uint16_t w, uint16_t h)
{
    isTransmittingBlock = true;

    // ВАЖНО: TouchGFX отдаёт uint8_t*, но это RGB565 => трактуем как uint16_t*
    const uint16_t* src = (const uint16_t*)pixels;

    // Количество пикселей в блоке
    uint32_t count = (uint32_t)w * (uint32_t)h;
    if (count > (uint32_t)(sizeof(invertedBlockBuffer) / sizeof(invertedBlockBuffer[0])))
    {
        // Защита от переполнения буфера: если что-то пошло не так, просто обрежем блок
        count = (uint32_t)(sizeof(invertedBlockBuffer) / sizeof(invertedBlockBuffer[0]));
    }

    // Инвертируем цвета RGB565: каждый пиксель XOR с 0xFFFF
    for (uint32_t i = 0; i < count; i++)
    {
        invertedBlockBuffer[i] = (uint16_t)(src[i] ^ 0xFFFF);
    }

    // Передаём инвертированный блок на дисплей
    ILI9341_DrawImage(x, y, w, h, invertedBlockBuffer);

    // Т.к. ILI9341_DrawImage у нас блокирующая (без DMA), передача уже завершена.
    // Сообщаем TouchGFX, что можно начинать следующий блок.
    isTransmittingBlock = false;
    
    // ВАЖНО: вызываем startNewTransfer(), который освободит текущий блок
    // и запустит передачу следующего блока, если он готов
    touchgfx::startNewTransfer();
}

/**
 * TouchGFX polls this to know if the driver is still sending a block.
 */
int touchgfxDisplayDriverTransmitActive(void)
{
    return isTransmittingBlock ? 1 : 0;
}

/**
 * Optional in some setups; safest stub: always allow.
 * (If your build требует — лучше иметь.)
 */
int touchgfxDisplayDriverShouldTransferBlock(uint16_t bottom)
{
    (void)bottom;
    return 1;
}

} // extern "C"



/* USER CODE END TouchGFXHAL.cpp */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
