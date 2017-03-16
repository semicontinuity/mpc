#if defined(__arm__)
// LCD driver for NP210

#include "lcd.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

static int filedes;

static unsigned short lcd_offset;
unsigned char lcd_memory[LCD_STRIDE * LCD_HEIGHT_PAGES];


void lcd_init(void) {
    memset(lcd_memory, 0, sizeof(lcd_memory));
    filedes = open("/dev/lcd", O_WRONLY);
    // TODO: check result (-1)
}


void lcd_deinit(void) {
    /*  Clean up after ourselves  */
    close(filedes);
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
    int page = first_page;
    while (page < last_page) {
        int column = first_column;
        int offset = LCD_STRIDE*page + column;
        lseek(filedes, offset, SEEK_SET);
        write(filedes, lcd_memory + offset, last_column-first_column);
        ++page;
    }
}

#endif
