#include "myconfig.h"
#include "font_v8_var.h"



#if CONFIG_FONT_V8_6P_VAR==1

#include "font_v8_6p_var.h"

__attribute__((used))
struct font_v8_var FONT_V8_6P_VAR = {
    font_v8_6p_var__glyphs,
    font_v8_6p_var__widths,
    FONT_V8_6P_VAR__WIDTH,
    FONT_V8_6P_VAR__HEIGHT,
    FONT_V8_6P_VAR__FIRST_GLYPH,
    FONT_V8_6P_VAR__NUMBER_OF_GLYPHS
};

#endif
