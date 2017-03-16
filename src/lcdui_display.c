#include "lcdui_display.h"
#include "lcd_graphics.h"
#include "stdio.h"

static struct lcdui__displayable *current_displayable;


void lcdui_display__setCurrent(struct lcdui__displayable *displayable)
{
    current_displayable = displayable;
    lcd_graphics__clear();
    current_displayable->paint();
    lcd_graphics__flush_all();
}


void lcdui_display__refresh(int flags)
{
    printf("> lcdui_display__refresh\n");
    if (current_displayable && current_displayable->refresh != 0) {
        printf("| has current displayable\n");
        current_displayable->refresh(flags);
        lcd_graphics__flush_all();
    }
    printf("< lcdui_display__refresh\n");
}


unsigned char lcdui_display__handleEvent(unsigned char event) __attribute__((used))
{
    if (current_displayable->handleEvent != 0) {
        unsigned char result = current_displayable->handleEvent(event);
        if (result) return result;
    }
    return 0;
}
