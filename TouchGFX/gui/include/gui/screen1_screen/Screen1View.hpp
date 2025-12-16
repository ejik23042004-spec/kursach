#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

// Внешние переменные координат тача из main.c
extern volatile uint16_t touch_x;
extern volatile uint16_t touch_y;

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();

    void setText(const char* txt);

protected:
    uint8_t counter;
    bool flag;
    static const uint16_t TEXT_BUFFER_SIZE = 30;
    touchgfx::Unicode::UnicodeChar textBuffer[TEXT_BUFFER_SIZE];
};

#endif // SCREEN1VIEW_HPP
