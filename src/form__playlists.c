#include "form__playlists.h"
#include "lcd.h"
#include "lcdui_display.h"
#include "lcdui_form.h"
#include "lcdui_list.h"
#include "model.h"
#include "controller.h"
#include <mpd/status.h>
#include <mpd/player.h>
#include <mpd/playlist.h>
#include <mpd/queue.h>
#include "keyboard.h"

#define GAP (2)

static struct lcdui__list list = {
    .x=0,
    .y=LCDUI_FORM__TITLE_HEIGHT + GAP,
    .width=LCD_WIDTH_PIXELS,
    .visible_items=4,
    .first_visible_item=0,
    .selected_item=0,
    mpc_playlists_size, mpc_playlists_item, NULL
};


static void form__playlists__refresh(__attribute__((used))int flags) {
    lcdui_list__paint(&list);
}


static void form__playlists__paint() {
    lcdui_form__paintTitle("Playlists");
    lcdui_form__paintSoftButton("Play", LCDUI_FORM__RIGHT_SOFTBUTTON);
    lcdui_form__paintSoftButton("Add", LCDUI_FORM__LEFT_SOFTBUTTON);
    lcdui_list__paint(&list);
}

static void play_list(int item) {
    if (mpc_playlists_size() > item) {
        mpd_run_stop(conn2);
        mpd_run_clear(conn2);
        mpd_run_load(conn2, mpc_playlists_item(item));
        mpc_play();
    }
}

static unsigned char form__playlists__handleEvent(unsigned char event) {
    if (event == VK_RIGHT_SELECT) {
        if (mpc_playlists_size() > 0) {
            mpd_run_stop(conn2);
            mpd_run_clear(conn2);
            mpd_run_load(conn2, mpc_playlists_item(list.selected_item));
            mpc_play();
        }
        return 1;
    }
    else if (event == VK_LEFT_SELECT) {
        if (mpc_playlists_size() > 0) {
            mpd_run_load(conn2, mpc_playlists_item(list.selected_item));
        }
        return 1;
    }
    else if (event == VK_1) { play_list(0); }
    else if (event == VK_2) { play_list(1); }
    else if (event == VK_3) { play_list(2); }
    else if (event == VK_4) { play_list(3); }
    else if (event == VK_5) { play_list(4); }
    else if (event == VK_6) { play_list(5); }
    else if (event == VK_7) { play_list(6); }
    else if (event == VK_8) { play_list(7); }
    else if (event == VK_9) { play_list(8); }
    else {
        int result = lcdui_list__handleEvent(&list, event);
        return result ? (unsigned char) result : default_handle_event(event);
    }
}


struct lcdui__displayable form__playlists = {
    form__playlists__paint,
    form__playlists__refresh,
    form__playlists__handleEvent
};


