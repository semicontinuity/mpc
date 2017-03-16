#if !defined(__arm__) && !defined(LCD_DUMMY)
// LCD emulator using curses

#include "lcd.h"

#include "curses.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static WINDOW * mainwin;
static unsigned short lcd_offset;
unsigned char lcd_memory[LCD_STRIDE * LCD_HEIGHT_PAGES];

void lcd_init(void) {
	/*  Initialize ncurses  */
    memset(lcd_memory, 0, sizeof(lcd_memory));
    if ( (mainwin = initscr()) == NULL ) {
		fprintf(stderr, "Error initialising ncurses.\n");
		exit(EXIT_FAILURE);
    }

    start_color();                    /*  Initialize colours  */
    /*  Make sure we are able to do what we want. If
	has_colors() returns FALSE, we cannot use colours.
	COLOR_PAIRS is the maximum number of colour pairs
	we can use. We use 13 in this program, so we check
	to make sure we have enough available.               */

	if (!has_colors() || COLOR_PAIRS <= 1 ) {
		fprintf(stderr, "Color not supported.\n");
		exit(EXIT_FAILURE);
	}

    init_pair(1,  COLOR_GREEN,   COLOR_GREEN); // for bit=0
}


void lcd_deinit(void) {
	/*  Clean up after ourselves  */
    delwin(mainwin);
    endwin();
    refresh();
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
        while (column < last_column) {
            int offset = LCD_STRIDE*page + column;
            unsigned char data = lcd_memory[offset];
            int i = 0;
            do {
                int color = (data & 0x1) ? 0 : 1;
                color_set(color, NULL);
                mvaddstr((page<<3) + i, column, " ");
		
                data >>= 1;
                i++;
            }
            while (i < 8);

            ++column;
        }
        ++page;
    }

    refresh();
}

#endif
