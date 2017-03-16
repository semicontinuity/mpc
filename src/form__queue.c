#include "form__now_playing.h"
#include "lcd.h"
#include "lcdui_display.h"
#include "lcdui_form.h"
#include "lcdui_list.h"
#include "model.h"
#include "controller.h"
#include <mpd/status.h>
#include <mpd/player.h>
#include <mpd/queue.h>
#include "keyboard.h"
#include <stdio.h>

#define GAP (2)

char queue_icon(int index) {
    if (is_current(index)) {
        if (get_state() == MPD_STATE_PLAY) {
            return 28;
        }
        else if (get_state() == MPD_STATE_PAUSE) {
            return 29;
        }
    }
    return 30;
}


static struct lcdui__list list = {
    .x=0,
    .y=LCDUI_FORM__TITLE_HEIGHT + GAP,
    .width=LCD_WIDTH_PIXELS,
    .visible_items=4,
    .first_visible_item=0,
    .selected_item=0,
    queue_size, queue_item, queue_icon
};


static void form__queue__paint() {
    lcdui_form__paintTitle("Queue");
    lcdui_form__paintSoftButton("Delete", LCDUI_FORM__LEFT_SOFTBUTTON);
    lcdui_form__paintSoftButton("Play", LCDUI_FORM__RIGHT_SOFTBUTTON);
    lcdui_list__paint(&list);
}


static void form__queue__refresh(int flags) {
    printf("@ form__queue__refresh, flags=%d\n", flags);
    lcd_graphics__clear();
    form__queue__paint();
}


static unsigned char form__queue__handleEvent(unsigned char event) {

    if (event == VK_LEFT_SELECT) {
        if (queue_size() > 0) mpd_run_delete(conn2, list.selected_item);
        return 1;
    }
    else if (event == VK_RIGHT_SELECT) {
        if (queue_size() > 0) mpd_run_play_pos(conn2, list.selected_item);
        return 1;
    }
    else {
        int result = lcdui_list__handleEvent(&list, event);
        return result ? (unsigned char) result : default_handle_event(event);
    }
}


struct lcdui__displayable form__queue = {
    form__queue__paint,
    form__queue__refresh,
    form__queue__handleEvent
};


