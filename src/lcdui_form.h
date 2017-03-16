#ifndef __LCDUI_FORM_H__
#define __LCDUI_FORM_H__

#include "font_v8_var.h"
#include "lcd_graphics.h"
#include "lcd.h"

#define LCDUI_FORM__TITLE_HEIGHT (11)
#define LCDUI_FORM__TITLE_TOP_MARGIN (0)
#define LCDUI_FORM__TITLE_LEFT_MARGIN (1)

#define LCDUI_FORM__SOFTBUTTON_WIDTH (LCD_WIDTH_PIXELS/2 - 1)
#define LCDUI_FORM__SOFTBUTTON_HEIGHT (9)
#define LCDUI_FORM__SOFTBUTTON_Y (LCD_HEIGHT_PIXELS - LCDUI_FORM__SOFTBUTTON_HEIGHT)
#define LCDUI_FORM__LEFT_SOFTBUTTON (0)
#define LCDUI_FORM__RIGHT_SOFTBUTTON (LCD_WIDTH_PIXELS - LCDUI_FORM__SOFTBUTTON_WIDTH)
#define LCDUI_FORM__SOFTBUTTON_TOP_MARGIN (1)
#define LCDUI_FORM__SOFTBUTTON_LEFT_MARGIN (1)


void lcdui_form__paintTitle(char* text);
void lcdui_form__paintSoftButton(char* text, int position);

#endif
