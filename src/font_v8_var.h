#ifndef __FONT_V8_VAR_H__
#define __FONT_V8_VAR_H__

#include "myconfig.h"

struct font_v8_var {
    unsigned char *glyphs;
    unsigned char *widths;
    unsigned char width;
    unsigned char height;
    unsigned char first_glyph;
    __attribute__((used))
    unsigned char number_of_glyphs;
};


#if CONFIG_FONT_V8_6P_VAR==1
__attribute__((used))
extern struct font_v8_var FONT_V8_6P_VAR;
#endif


#endif
