#if defined(LCD_DUMMY)

#include "lcd.h"
#include <string.h>
#include <stdio.h>

static unsigned short lcd_offset;
unsigned char lcd_memory[LCD_STRIDE * LCD_HEIGHT_PAGES];

void lcd_init(void) {
    memset(lcd_memory, 0, sizeof(lcd_memory));
}


void lcd_deinit(void) {
}


void lcd_seek(unsigned short offset) {
    lcd_offset = offset;
}


unsigned char lcd_read() {
    return lcd_memory[lcd_offset];
}


/** Write 8 vertical pixels to the (page, column) and move to the next position */
void lcd_write(unsigned char data) {
    lcd_memory[lcd_offset++] = data;
}


/**
 * Flush the region of the internal in-memory buffer (if any) to the LCD
 * @param last_page exclusive
 * @param last_column exclusive
 */
void lcd_flush(unsigned char first_page, unsigned char first_column, unsigned char last_page, unsigned char last_column) {
    printf("@ lcd_flush %d %d %d %d\n", first_page, first_column, last_page, last_column);
}

#endif
