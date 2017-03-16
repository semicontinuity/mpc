#include "lcdui_label.h"
#include "lcd_graphics.h"


void lcdui_label__paint(const char *text, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    lcd_graphics__setColor(1);
    lcd_graphics__fillRect(x, y, width, height);
    lcd_graphics__setColor(0);
    lcd_graphics__fillText(text, x, y);
}
