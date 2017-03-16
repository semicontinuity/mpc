/**
 * Implementation of lcd_graphics.h for NCurses (emulator)
 */

//#include "config.h"

#include "lcd_graphics.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>

/* 0 = black */
static unsigned char current_color;
static struct font_v8_var *current_font;


#if CONFIG_LCD_BUFFERED == 1
unsigned char dirty__first_page;
unsigned char dirty__first_column;
unsigned char dirty__last_page;
unsigned char dirty__last_column;

void lcd_graphics__reset_dirty()
{
    dirty__first_page = 0;
    dirty__first_column = 0;
    dirty__last_page = 0; /* exclusive */
    dirty__last_column = 0; /* exclusive */
}

#endif


void lcd_graphics__init(void) __attribute__((used))
{
    lcd_init();
#if CONFIG_LCD_BUFFERED == 1
    lcd_graphics__reset_dirty();
#endif
}


void lcd_graphics__deinit(void) __attribute__((used))
{
    lcd_deinit();
}


static void fill(int position, int width, unsigned char value)
{
    lcd_seek((unsigned short) position);
    for (int i = 0; i < width; i++) lcd_write(value);
}

static void andWith(int position, int width, unsigned char mask)
{
    lcd_seek((unsigned short) position);
    for (int i = 0; i < width; i++) lcd_write(lcd_read() & mask);
}

static void orWith(int position, int width, unsigned char mask)
{
    lcd_seek((unsigned short) position);
    for (int i = 0; i < width; i++) lcd_write(lcd_read() | mask);
}

void lcd_graphics__clear(void)
{
    for (int p = 0; p < LCD_HEIGHT_PAGES; p++) {
        fill(p * LCD_STRIDE, LCD_STRIDE, 0);
    }
#if CONFIG_LCD_BUFFERED == 1
    dirty__first_page = 0;
    dirty__first_column = 0;
    dirty__last_page = LCD_HEIGHT_PAGES;
    dirty__last_column = LCD_WIDTH_PIXELS;
#endif
}


void lcd_graphics__flush(void) __attribute__((used))
{
#if CONFIG_LCD_BUFFERED == 1
    if (dirty__last_page > dirty__first_page && dirty__last_column > dirty__first_column) {
        lcd_flush(dirty__first_page, dirty__first_column, dirty__last_page, dirty__last_column);
    }
#endif
}


void lcd_graphics__flush_all(void)
{
    lcd_flush(0, 0, LCD_HEIGHT_PAGES, LCD_WIDTH_PIXELS);
}


/*
 * ===============================================================================================
 * BitCopy combiner for BitBlt.
 * Effectively performs dst=src operation
 * ===============================================================================================
 */

void lcd_graphics__combiner_bit_copy__bltLeftShift(
    unsigned char *src,
    int srcOffset,
    int length,
    int shift,
    int mask)
{

    const int finalSrcOffset = srcOffset + length;
    for (int i = srcOffset; i < finalSrcOffset; i++) {
        lcd_write((unsigned char) ((lcd_read() & ~mask) | ((src[i] << shift) & mask)));
    }
}


void lcd_graphics__combiner_bit_copy__bltRightShift(
    unsigned char *src,
    int srcOffset,
    int length,
    int shift,
    int mask)
{

    const int finalSrcOffset = srcOffset + length;
    for (int i = srcOffset; i < finalSrcOffset; i++) {
        lcd_write((unsigned char) ((lcd_read() & ~mask) | ((src[i] >> shift) & mask)));
    }
}


struct lcd_graphics__combiner lcd_graphics__combiner_bit_copy __attribute__((used)) = {
    lcd_graphics__combiner_bit_copy__bltLeftShift,
    lcd_graphics__combiner_bit_copy__bltRightShift
};


/*
 * ===============================================================================================
 * BitSet combiner for BitBlt.
 * Effectively performs dst=dst OR src operation: for every 1 bit in src[i],
 * appropriate bit in dst[i] will be set.
 * If bit src[i] is 0, appropriate bit in dst[i] will left intact.
 * ===============================================================================================
 */

void lcd_graphics__combiner_bit_set__bltLeftShift(
    unsigned char *src,
    int srcOffset,
    int length,
    int shift,
    int mask)
{

    const int finalSrcOffset = srcOffset + length;
    for (int i = srcOffset; i < finalSrcOffset; i++) {
        lcd_write((unsigned char) (lcd_read() | ((src[i] << shift) & mask)));
    }
}


void lcd_graphics__combiner_bit_set__bltRightShift(
    unsigned char *src,
    int srcOffset,
    int length,
    int shift,
    int mask)
{

    const int finalSrcOffset = srcOffset + length;
    for (int i = srcOffset; i < finalSrcOffset; i++) {
        lcd_write((unsigned char) (lcd_read() | ((src[i] >> shift) & mask)));
    }
}


struct lcd_graphics__combiner lcd_graphics__combiner_bit_set = {
    lcd_graphics__combiner_bit_set__bltLeftShift,
    lcd_graphics__combiner_bit_set__bltRightShift
};

/*
 * ===============================================================================================
 * BitReset combiner for BitBlt.
 * Effectively performs dst=dst AND NOT src operation: for every 1 bit in src[i],
 * appropriate bit in dst[i] will be cleared.
 * If bit src[i] is 0, appropriate bit in dst[i] will left intact.
 * ===============================================================================================
 */

void lcd_graphics__combiner_bit_reset__bltLeftShift(
    unsigned char *src,
    int srcOffset,
    int length,
    int shift,
    int mask)
{

    const int finalSrcOffset = srcOffset + length;
    for (int i = srcOffset; i < finalSrcOffset; i++) {
        lcd_write((unsigned char) (lcd_read() & (~(src[i] << shift) | ~mask)));
    }
}


void lcd_graphics__combiner_bit_reset__bltRightShift(
    unsigned char *src,
    int srcOffset,
    int length,
    int shift,
    int mask)
{

    const int finalSrcOffset = srcOffset + length;
    for (int i = srcOffset; i < finalSrcOffset; i++) {
        lcd_write((unsigned char) (lcd_read() & (~(src[i] >> shift) | ~mask)));
    }
}


struct lcd_graphics__combiner lcd_graphics__combiner_bit_reset = {
    lcd_graphics__combiner_bit_reset__bltLeftShift,
    lcd_graphics__combiner_bit_reset__bltRightShift
};


static inline int mask(int topBit, int bottomBit)
{
    return 0xFF >> (8 - bottomBit) & (0xFF << topBit);
}

static inline unsigned char min(unsigned char a, unsigned char b)
{
    return a > b ? b : a;
}

static inline unsigned char max(unsigned char a, unsigned char b)
{
    return a > b ? a : b;
}


/**
 * Bit block transfer utility for V8 bitmaps
 * (Bitmaps optimized for LCD screens with video memory organized into 8-pixel high "rows")
 * <pre>
 *         X-->
 *
 *           0          1                 127
 *
 *        addr: 0    addr: 1      ...   addr: 127
 *        +-----+    +-----+            +-----+
 * Y   0  |bit 0|    |bit 0|            |bit 0|
 * |   1  |bit 1|    |bit 1|            |bit 1|
 * |   2  |bit 2|    |bit 2|            |bit 2|
 * V   .
 *     7  |bit 7|    |bit 7|            |bit 7|
 *        +-----+    +-----+            +-----+
 *
 *        addr: 128    addr: 129  ...   addr: 255
 *        +-----+    +-----+            +-----+
 *     8  |bit 0|    |bit 0|            |bit 0|
 *     9  |bit 1|    |bit 1|            |bit 1|
 *     10 |bit 2|    |bit 2|            |bit 2|
 *     .
 *     15 |bit 7|    |bit 7|            |bit 7|
 *        +-----+    +-----+            +-----+
 *
 *        addr: 896    addr: 897  ...   addr: 1023
 *        +-----+    +-----+            +-----+
 *     56 |bit 0|    |bit 0|            |bit 0|
 *     57 |bit 1|    |bit 1|            |bit 1|
 *     58 |bit 2|    |bit 2|            |bit 2|
 *     .
 *     63 |bit 7|    |bit 7|            |bit 7|
 *        +-----+    +-----+            +-----+

 *
 * </pre>
 * This is typical for many 128x64, 122x32 graphical LCD displays (MT12864A, etc.)
 */


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
    const int srcInitialOffset)
{
    int dstRow = dstY >> 3;
    int finalDstRow = (dstY + height - 1) >> 3;
    int dstOffset = dstRow * LCD_STRIDE + dstX;
    int srcRow = srcY >> 3;
    int srcOffset = srcRow * srcStride + srcX + srcInitialOffset;

    // TODO optimize and make clear (e.g. with (x & 0xF8))
    int diff = (dstY - (dstRow << 3)) - (srcY - (srcRow << 3));
    int pivotBit = diff > 0 ? diff : diff + 8;
    int topShift = diff > 0 ? 8 - diff : -diff;
    int bottomShift = diff > 0 ? diff : 8 + diff;

    while (dstRow <= finalDstRow) {
        // TODO optimize and make clear (e.g. with (x & 0xF8))
        const int topBit = (dstY - (dstRow << 3)) > 0 ? (dstY - (dstRow << 3)) : 0;
        // TODO optimize and make clear(e.g. with (x & 0xF8))
        const int bottomBit =
            ((dstY + height) - (dstRow << 3)) < 8 ?
            ((dstY + height) - (dstRow << 3)) :
            8;

        if ((pivotBit == 0 || topBit < pivotBit) && topShift < 8) {
            lcd_seek((unsigned short) dstOffset);
            combiner->bltRightShift(
                src, srcOffset,
                width,
                topShift,
                mask(topBit, min((unsigned char) bottomBit, (unsigned char) pivotBit)));
            srcOffset += srcStride;
        }
        if (bottomBit > pivotBit && pivotBit != 0) {
            lcd_seek((unsigned short) dstOffset);
            combiner->bltLeftShift(
                src, srcOffset,
                width,
                bottomShift,
                mask(max((unsigned char) topBit, (unsigned char) pivotBit), bottomBit));
        }

        dstOffset += LCD_STRIDE;
        dstRow++;
    }
}


void lcd_graphics__setFont(struct font_v8_var *font) __attribute__((used))
{
    current_font = font;
}


void lcd_graphics__setColor(unsigned char color)
{
    current_color = color;
}

int lcd_graphics__glyph_width(char c)
{
    return current_font->widths[c - current_font->first_glyph];
}

unsigned int lcd_graphics__text_width(char *text)
{
    unsigned int width = 0;
    char c;
    while ((c = *text++)) width += current_font->widths[c - current_font->first_glyph];
    return width;
}


// no checks for dest
void lcd_graphics__strcpy_with_ellipsis(char *dest, char *src, int maxWidth)
{
    size_t length = strlen(src);
    int fullWidth = lcd_graphics__text_width(src);
    if (fullWidth > maxWidth) {
        fullWidth += lcd_graphics__text_width("...");
        for (size_t i = length - 1; i >= 0; i--) {
            fullWidth -= lcd_graphics__glyph_width(src[i]);
            if (fullWidth <= maxWidth) {
                strncpy(dest, src, i);
                strcpy(dest + i, "...");
                break;
            }
        }
    }
    else {
        strcpy(dest, src);
    }
}


// TODO: checks for string length
static char temp_s[64];

void lcd_graphics__fillTextLimited(
    char *text,
    unsigned char x,
    unsigned char y,
    unsigned char maxWidth) __attribute__((used))
{
    lcd_graphics__strcpy_with_ellipsis(temp_s, text, maxWidth);
    lcd_graphics__fillText(temp_s, x, y);
}


void lcd_graphics__fillText(const char *text, unsigned int x, unsigned int y)
{
    const struct lcd_graphics__combiner *const combiner = current_color
                                                          ? &lcd_graphics__combiner_bit_reset
                                                          : &lcd_graphics__combiner_bit_set;
    const int fontHeight = current_font->height;
    const int srcStride = current_font->width * ((current_font->height + 7) >> 3);
    while (1) {
        char c = *text;
        if (!c) break;

        const int index = c - current_font->first_glyph;
        const int width = current_font->widths[index];
        lcd_graphics__bitBlt(
            current_font->glyphs + srcStride * index,
            srcStride,
            width,
            fontHeight,
            x,
            y,
            0,
            0,
            combiner,
            0);

        x += width;
        ++text;
    }
}


/**
 * Fill a rectangle at given coordinates with specified color.
 * Helpers {@link #fill}, {@link #andWith}, {@link #orWith} are
 * not implemented with interface like in @{link BitBlt},
 * because their invocation does not require many ifs (at least now).
 * This is a low-level function, not an public-API function
 * (Assumes that all arguments are pre-checked, within allowed ranges)
 * @param x         x coordinate of left border
 * @param y         x coordinate of top border
 * @param width     width of rectangle
 * @param height    height of rectangle
 */
void lcd_graphics__fillRect(
    const int x,
    const int y,
    const int width,
    const int height)
{
    int row = y >> 3;
    int position = (row * LCD_STRIDE) + x;

    const int topRowOffset = row << 3;

    // Optionally paint "first" row
    // If y is not divisible by 8, the first row is painted specially
    // Also the first row can be the last one (all pixels touched lie within one row)
    // The bit mask should be specially calculated.
    const int startOffsetInTopRow = y - topRowOffset;
    const int endOffsetInTopRow = startOffsetInTopRow + height;
    if (startOffsetInTopRow > 0 || endOffsetInTopRow <= 7) {
        if (!current_color) {
            int mask = 0xFF << startOffsetInTopRow;
            if (endOffsetInTopRow <= 7) {
                mask &= 0xFF >> (8 - endOffsetInTopRow);
            }
            orWith(position, width, (unsigned char) mask);
        }
        else {
            int mask = 0xFF << startOffsetInTopRow;
            if (endOffsetInTopRow <= 7) {
                mask &= (0xFF >> (8 - endOffsetInTopRow));
            }
            andWith(position, width, (unsigned char) ~mask);
        }
        row++;
        position += LCD_STRIDE;
    }

    // Paint "middle" rows - those completely filled
    // For this rows, appropriate bytes are simply filled with 0xFF or 0x00
    const int endRow = (y + height) >> 3;
    while (row < endRow) {
        fill(position, width, !current_color ? (unsigned char) 0xFF : (unsigned char) 0);
        row++;
        position += LCD_STRIDE;
    }

    // Optionally paint "last" row
    // The bit mask should be specially calculated, depending on remainingHeight
    const int remainingHeight = y + height - (row << 3);
    if (remainingHeight > 0) {
        if (!current_color) {
            orWith(position, width, (unsigned char) (0xFF >> (8 - remainingHeight)));
        }
        else {
            andWith(position, width, (unsigned char) ~(0xFF >> (8 - remainingHeight)));
        }
    }
}
