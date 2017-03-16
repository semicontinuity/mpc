#ifndef __LCD_GRAPHICS_H__
#define __LCD_GRAPHICS_H__

#include "font_v8_var.h"


void lcd_graphics__init(void) __attribute__((used));

void lcd_graphics__deinit(void) __attribute__((used));

void lcd_graphics__flush() __attribute__((used));

//debug
void lcd_graphics__flush_all();

void lcd_graphics__clear();

void lcd_graphics__fillRect(
    const int x,
    const int y,
    const int width,
    const int height);

void lcd_graphics__setFont(struct font_v8_var *font) __attribute__((used));

void lcd_graphics__setColor(unsigned char color);

void lcd_graphics__fillText(const char *text, unsigned int x, unsigned int y);

void lcd_graphics__fillTextLimited(
    char *text, unsigned char x, unsigned char y, unsigned char maxWidth) __attribute__((used));

int lcd_graphics__glyph_width(char c);

unsigned int lcd_graphics__text_width(char *text);

void lcd_graphics__strcpy_with_ellipsis(char *dest, char *src, int maxWidth);

/**
 * Abstract combiner for BitBlt, applied to the array of data.
 * Processed are 'length' bytes, starting at appropriate offsets.
 * (for performance reasons, not to call method in a loop).
 * To apply operation only to certain bits in dst, mask is used.
 * Operation is applied only if corresponding bit in mask is 1.
 * Operations are bit-level whereas data is stored as bytes, therefore a shift
 * is needed - either to the left or to the right.
 * 2 operations are defined for performance reasons - not to use extra 'if' in a loop.
 */
struct lcd_graphics__combiner {
    void (*bltLeftShift)(
        unsigned char *src,
        int srcOffset,
        int length,
        int shift,
        int mask);

    void (*bltRightShift)(
        unsigned char *src,
        int srcOffset,
        int length,
        int shift,
        int mask);
};

extern struct lcd_graphics__combiner lcd_graphics__combiner_bit_copy __attribute__((used));
extern struct lcd_graphics__combiner lcd_graphics__combiner_bit_set __attribute__((used));
extern struct lcd_graphics__combiner lcd_graphics__combiner_bit_reset __attribute__((used));

void lcd_graphics__bitBlt(
    unsigned char *src,
    const int srcStride,
    const int width,
    const int height,
    const int dstX,
    const int dstY,
    const int srcX,
    const int srcY,
    const struct lcd_graphics__combiner *const combiner,
    const int srcInitialOffset);


void lcd_graphics__fillRect(
    const int x,
    const int y,
    const int width,
    const int height);

#endif
