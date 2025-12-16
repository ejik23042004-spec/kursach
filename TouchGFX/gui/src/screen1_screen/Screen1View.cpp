#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Unicode.hpp>

Screen1View::Screen1View()
    : counter(0), flag(true)
{
    // Привязываем буферы к textArea1 и textArea2
    textArea1.setWildcard1(textBuffer);
    Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%d", 0);
    textArea1.invalidate();

    textArea2.setWildcard1(textBuffer);
    Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%d", 0);
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
}

void Screen1View::handleTickEvent()
{
    // Обновляем textArea1 и textArea2 (если нужно)
    touchgfx::Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%d", touch_y);
    textArea1.invalidate();
    textArea2.invalidate();
    
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
