#ifndef __LCDUI_DISPLAY_H__
#define __LCDUI_DISPLAY_H__

struct lcdui__displayable {
    void (*paint)();

    void (*refresh)(int flags);

    unsigned char (*handleEvent)(unsigned char event);
};

void lcdui_display__setCurrent(struct lcdui__displayable *displayable);

void lcdui_display__refresh(int flags);

unsigned char lcdui_display__handleEvent(unsigned char event) __attribute__((used));

#endif
