#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>
#include <touchgfx/Callback.hpp>

// Внешние переменные координат тача из main.c
extern volatile uint16_t touch_x;
extern volatile uint16_t touch_y;

// Структура для передачи температуры
typedef struct {
    int16_t temp_cold;  // Температура в холодной зоне (в сотых градуса)
    int16_t temp_warm;  // Температура в теплой зоне
} temp_data_t;

// Функция для получения температуры из FreeRTOS очереди
// Возвращает pdTRUE (1) при успехе, pdFALSE (0) при ошибке
extern "C" {
    typedef int BaseType_t;
    #define pdTRUE  1
    #define pdFALSE 0
    BaseType_t getTemperatureFromQueue(temp_data_t *data);
    void togglePB10(void);
}

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void setText(const char* txt);
    
    // Обработчик нажатия на кнопку
    void buttonClickedCallback(const touchgfx::AbstractButton& src);

protected:
    uint8_t counter;
    bool flag;
    static const uint16_t TEXT_BUFFER_SIZE = 30;
    touchgfx::Unicode::UnicodeChar textBuffer[TEXT_BUFFER_SIZE];
    
    // Буферы для второго wildcard'а textArea1 и textArea2 (дробная часть температуры)
    static const uint16_t TEXTAREA1BUFFER2_SIZE = 10;
    touchgfx::Unicode::UnicodeChar textArea1Buffer2[TEXTAREA1BUFFER2_SIZE];
    static const uint16_t TEXTAREA2BUFFER2_SIZE = 10;
    touchgfx::Unicode::UnicodeChar textArea2Buffer2[TEXTAREA2BUFFER2_SIZE];
    
    // Callback для обработки нажатия на кнопку
    touchgfx::Callback<Screen1View, const touchgfx::AbstractButton&> buttonClickedCallbackWrapper;
};

#endif // SCREEN1VIEW_HPP
