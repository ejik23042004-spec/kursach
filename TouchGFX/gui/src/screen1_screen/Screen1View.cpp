#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Unicode.hpp>

Screen1View::Screen1View()
    : counter(0), flag(true)
{
    // Привязываем наш буфер к textArea1 (если в Designer стоит Wildcard)
    textArea1.setWildcard1(textBuffer);
    Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%d", 0);
    textArea1.invalidate();


    // Привязываем наш буфер к textArea1 (если в Designer стоит Wildcard)
    textArea2.setWildcard1(textBuffer);
    Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%d", 0);
    textArea2.invalidate();


}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::handleTickEvent()
{
    counter++;

    // раз в 60 тиков (если tick ~60Hz, то раз в секунду)
    if ((counter % 1) == 0)
    {
        touchgfx::Unicode::snprintf(textBuffer, TEXT_BUFFER_SIZE, "%d", counter);
        textArea1.invalidate();
        textArea2.invalidate();
    }

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
