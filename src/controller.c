#include "controller.h"
#include "model.h"
#include "keyboard.h"
#include "util.h"

#include "lcd_graphics.h"
#include "lcdui_display.h"
#include "form__now_playing.h"
#include "form__queue.h"
#include "form__playlists.h"
#include <mpd/player.h>
#include <mpd/response.h>

unsigned char default_handle_event(unsigned char event) {
    if (event == VK_QUIT) {
        lcd_graphics__clear();
        lcd_graphics__flush_all();
    }
    if (event == VK_VOLUME_UP) {
        int new_volume = get_volume() + 7;
        if (new_volume > 100) new_volume = 100;
        set_volume(new_volume);
    }
    else if (event == VK_VOLUME_DOWN) {
        int new_volume = get_volume() - 7;
        if (new_volume < 0) new_volume = 0;
        set_volume(new_volume);
    }
    else if (event == VK_TOGGLE) {
        mpc_toggle();
    }
    else if (event == VK_NOW_PLAYING) {
        lcdui_display__setCurrent(&form__now_playing);
    }
    else if (event == VK_QUEUE) {
        lcdui_display__setCurrent(&form__queue);
    }
    else if (event == VK_PLAYLISTS) {
        lcdui_display__setCurrent(&form__playlists);
    }
    else if (event == VK_NEXT) {
        mpd_run_next(conn2);
        if (!mpd_response_finish(conn2))
            printErrorAndExit(conn2);
    }
    else if (event == VK_PREVIOUS) {
        mpd_run_previous(conn2);
        if (!mpd_response_finish(conn2))
            printErrorAndExit(conn2);
    }
    return 1;
}
