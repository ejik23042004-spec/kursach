#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Unicode.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/Callback.hpp>
#include <stdio.h>

Screen1View::Screen1View()
    : counter(0), flag(true),
      buttonClickedCallbackWrapper(this, &Screen1View::buttonClickedCallback)
{
    // Привязываем буферы к textArea1 (холодная зона) - два wildcard'а: целая и дробная части
    textArea1.setWildcard1(textArea1Buffer1);  // Целая часть
    textArea1.setWildcard2(textArea1Buffer2);  // Дробная часть
    Unicode::snprintf(textArea1Buffer1, TEXTAREA1BUFFER1_SIZE, "%d", 0);
    Unicode::snprintf(textArea1Buffer2, TEXTAREA1BUFFER2_SIZE, "%02d", 0);
    textArea1.invalidate();

    // Привязываем буферы к textArea2 (теплая зона) - два wildcard'а: целая и дробная части
    textArea2.setWildcard1(textArea2Buffer1);  // Целая часть
    textArea2.setWildcard2(textArea2Buffer2);  // Дробная часть
    Unicode::snprintf(textArea2Buffer1, TEXTAREA2BUFFER1_SIZE, "%d", 0);
    Unicode::snprintf(textArea2Buffer2, TEXTAREA2BUFFER2_SIZE, "%02d", 0);
    textArea2.invalidate();

    // Привязываем ОБА буфера к textArea3 (для двух wildcards)
    // textArea3Buffer1 и textArea3Buffer2 уже определены в Screen1ViewBase
    textArea3.setWildcard1(textArea3Buffer1);
    textArea3.setWildcard2(textArea3Buffer2);
    
    // Инициализируем оба буфера
    Unicode::snprintf(textArea3Buffer1, TEXTAREA3BUFFER1_SIZE, "%d", 0);
    Unicode::snprintf(textArea3Buffer2, TEXTAREA3BUFFER2_SIZE, "%d", 0);
    textArea3.invalidate();
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    
    // Устанавливаем обработчик нажатия на кнопку button1
    button1.setAction(buttonClickedCallbackWrapper);
}

void Screen1View::handleTickEvent()
{
    temp_data_t temp_data;
    
    // Пытаемся получить данные о температуре из FreeRTOS очереди (неблокирующий вызов)
    if (getTemperatureFromQueue(&temp_data) == pdTRUE)
    {
        // Обновляем textArea1 (холодная зона)
        // Температура приходит в сотых градуса (например, 2818 = 28.18°C, -125 = -1.25°C)
        // Используем два wildcard'а: целая часть и дробная часть
        int16_t temp_cold_abs = temp_data.temp_cold;
        if (temp_cold_abs < 0) {
            temp_cold_abs = -temp_cold_abs;
        }
        int16_t temp_cold_int = temp_data.temp_cold / 100;  // Целая часть (с учетом знака)
        uint16_t temp_cold_frac = temp_cold_abs % 100;  // Дробная часть (всегда положительная)
        
        // Обновляем wildcard'ы: целая часть в Buffer1, дробная часть в Buffer2
        Unicode::snprintf(textArea1Buffer1, TEXTAREA1BUFFER1_SIZE, "%d", temp_cold_int);
        Unicode::snprintf(textArea1Buffer2, TEXTAREA1BUFFER2_SIZE, "%02d", temp_cold_frac);
        textArea1.invalidate();
        
        // Обновляем textArea2 (теплая зона)
        int16_t temp_warm_abs = temp_data.temp_warm;
        if (temp_warm_abs < 0) {
            temp_warm_abs = -temp_warm_abs;
        }
        int16_t temp_warm_int = temp_data.temp_warm / 100;  // Целая часть (с учетом знака)
        uint16_t temp_warm_frac = temp_warm_abs % 100;  // Дробная часть (всегда положительная)
        
        // Обновляем wildcard'ы: целая часть в Buffer1, дробная часть в Buffer2
        Unicode::snprintf(textArea2Buffer1, TEXTAREA2BUFFER1_SIZE, "%d", temp_warm_int);
        Unicode::snprintf(textArea2Buffer2, TEXTAREA2BUFFER2_SIZE, "%02d", temp_warm_frac);
        textArea2.invalidate();
    }
    
    // Обновляем textArea3 с координатами X и Y
    // Wildcard1 = X координата
    Unicode::snprintf(textArea3Buffer1, TEXTAREA3BUFFER1_SIZE, "%d", touch_x);
    // Wildcard2 = Y координата
    Unicode::snprintf(textArea3Buffer2, TEXTAREA3BUFFER2_SIZE, "%d", touch_y);
    
    // Обновляем отображение
    textArea3.invalidate();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::setText(const char* txt)
{
    touchgfx::Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%s", txt);
    textArea1.invalidate();
    textArea2.invalidate();
}

void Screen1View::buttonClickedCallback(const touchgfx::AbstractButton& src)
{
    if (&src == &button1)
    {
        // Переключаем состояние PB10
        togglePB10();
    }
}
