#include "lcdui_list.h"
#include "lcd_graphics.h"
#include "keyboard.h"
#include "lcd.h"

#include <stdio.h>

static char temp_s1[] = {0, 0};

void lcdui_list__paint(struct lcdui__list *list) {
    unsigned int index = list->first_visible_item;
    unsigned int y = list->y;
    unsigned int size = list->size();
    for (unsigned int i = 0; i < list->visible_items; i++) {
        if (index >= size) break;

        unsigned char color = (index == list->selected_item) ? LCD_COLOR_WHITE : LCD_COLOR_BLACK;
        lcd_graphics__setColor(color);
        lcd_graphics__fillRect(list->x, y, list->width, LCDUI_LIST__ITEM_HEIGHT);

        lcd_graphics__setColor(color == LCD_COLOR_BLACK ? LCD_COLOR_WHITE : LCD_COLOR_BLACK);
        unsigned int x = LCDUI_LIST__ITEM_LEFT_MARGIN;
        if (list->icon != NULL) {
            char icon = list->icon(index);
            if (icon != 0) {
                temp_s1[0] = icon;
                lcd_graphics__fillText(temp_s1, x, y + LCDUI_LIST__ITEM_TOP_MARGIN);
                x += lcd_graphics__glyph_width(icon);
            }
        }
        lcd_graphics__fillText(list->item(index), x, y + LCDUI_LIST__ITEM_TOP_MARGIN);

        y += LCDUI_LIST__ITEM_HEIGHT;
        ++index;
    }

    // TODO: paint remaining items
}


unsigned char lcdui_list__handleEvent(struct lcdui__list *list, unsigned char event) {
    if (event == VK_UP) {
        if (list->selected_item > 0) {
            list->selected_item--;
            if (list->selected_item < list->first_visible_item)
                list->first_visible_item--;
            lcdui_list__paint(list);
            lcd_graphics__flush_all();
        }
        return 1;
    }
    else if (event == VK_DOWN) {
        if (list->selected_item < list->size() - 1) {
            list->selected_item++;
            if (list->selected_item >= list->first_visible_item + list->visible_items)
                list->first_visible_item++;
            lcdui_list__paint(list);
            lcd_graphics__flush_all();
        }
        return 1;
    }
    else return 0;
}
