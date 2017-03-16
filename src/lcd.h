#ifndef __LCD_H__
#define __LCD_H__

#define LCD_WIDTH_PIXELS (128)
#define LCD_HEIGHT_PIXELS (64)
#define LCD_HEIGHT_PAGES (LCD_HEIGHT_PIXELS/8)
#define LCD_STRIDE (LCD_WIDTH_PIXELS)

#define LCD_COLOR_WHITE ((unsigned char)0)
#define LCD_COLOR_BLACK ((unsigned char)1)

__attribute__((used))
void lcd_init(void);

__attribute__((used))
void lcd_deinit(void);

__attribute__((used))
void lcd_clear(void);

void lcd_seek(unsigned short offset);

/** Write 8 vertical pixels to the (page, column) and move to the next position */
void lcd_write(unsigned char data);

/** Read 8 vertical pixels from the current position, don't move the position*/
unsigned char lcd_read(void);


/**
 * Flush the region of the internal in-memory buffer (if any) to the LCD
 * @param last_page exclusive
 * @param last_column exclusive
 */
void lcd_flush(unsigned char first_page, unsigned char first_column, unsigned char last_page, unsigned char last_column);


#endif
