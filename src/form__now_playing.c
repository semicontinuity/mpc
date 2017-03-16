#include "form__now_playing.h"
#include "lcd.h"
#include "lcdui_display.h"
#include "lcdui_form.h"
#include "lcdui_label.h"
#include "model.h"
#include "controller.h"

#include <stdio.h>

#define MARGIN_TOP  (5)
#define MARGIN_LEFT (1)
#define CONTENT_WIDTH (LCD_WIDTH_PIXELS - MARGIN_LEFT)

// TODO: use flags
static void form__now_playing__refresh(__attribute__((used)) int flags) {
    printf("@ form__now_playing__refresh\n");

    const char *text;
    int now_playing_index = mpc_queue_song_index();
    printf("| now_playing_index=%d\n", now_playing_index);
    if (now_playing_index == -1) {
        lcdui_label__paint("", MARGIN_LEFT, LCDUI_FORM__TITLE_HEIGHT + MARGIN_TOP, CONTENT_WIDTH, 4 * 8);
    }
    else {
        struct mpd_song *song = queue_song(now_playing_index);

        text = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
        if (text == NULL) text = "";
        lcdui_label__paint(text, MARGIN_LEFT, LCDUI_FORM__TITLE_HEIGHT + MARGIN_TOP, CONTENT_WIDTH, 8);

        text = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
        if (text == NULL) text = "";
        lcdui_label__paint(text, MARGIN_LEFT, LCDUI_FORM__TITLE_HEIGHT + MARGIN_TOP + 8, CONTENT_WIDTH, 8);

        text = mpd_song_get_tag(song, MPD_TAG_ALBUM, 0);
        if (text == NULL) text = "";
        lcdui_label__paint(text, MARGIN_LEFT, LCDUI_FORM__TITLE_HEIGHT + MARGIN_TOP + 24, CONTENT_WIDTH, 8);
    }
}


static void form__now_playing__paint() {
    lcdui_form__paintTitle("Now playing");
    form__now_playing__refresh(0xFFFF);
}


static unsigned char form__now_playing__handleEvent(unsigned char event) {
    return default_handle_event(event);
}


struct lcdui__displayable form__now_playing = {
    form__now_playing__paint,
    form__now_playing__refresh,
    form__now_playing__handleEvent
};
