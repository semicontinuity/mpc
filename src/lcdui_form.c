#include "lcdui_form.h"

void lcdui_form__paintTitle(char *text)
{
    lcd_graphics__setColor(1);
    lcd_graphics__fillRect(0, 0, LCD_WIDTH_PIXELS, LCDUI_FORM__TITLE_HEIGHT - 3);
    lcd_graphics__setColor(0);
    lcd_graphics__fillRect(0, LCDUI_FORM__TITLE_HEIGHT - 2, LCD_WIDTH_PIXELS, 1);
    lcd_graphics__fillText(text, LCDUI_FORM__TITLE_LEFT_MARGIN, LCDUI_FORM__TITLE_TOP_MARGIN);
}

void lcdui_form__paintSoftButton(char *text, int position)
{
    lcd_graphics__setColor(0);
    lcd_graphics__fillRect(position,
                           LCDUI_FORM__SOFTBUTTON_Y,
                           LCDUI_FORM__SOFTBUTTON_WIDTH,
                           LCDUI_FORM__SOFTBUTTON_HEIGHT);

    unsigned int text_x;
    if (position == LCDUI_FORM__LEFT_SOFTBUTTON) {
        text_x = LCDUI_FORM__LEFT_SOFTBUTTON + LCDUI_FORM__SOFTBUTTON_LEFT_MARGIN;
    }
    else {
        unsigned int text_width = lcd_graphics__text_width(text);
        text_x = LCD_WIDTH_PIXELS >= text_width ? LCD_WIDTH_PIXELS - text_width : 0;
    }
    lcd_graphics__setColor(1);
    lcd_graphics__fillText(text, text_x, LCDUI_FORM__SOFTBUTTON_Y + LCDUI_FORM__SOFTBUTTON_TOP_MARGIN);
}

