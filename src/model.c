#include "model.h"

#include <mpd/client.h>

#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "lcdui_display.h"
#include "lcd_graphics.h"
#include "lcd.h"

int shutting_down = 0;


static pthread_t thread;
struct mpd_connection *conn;
struct mpd_connection *conn2;


enum mpd_state __state;

int __queue_song = -1;
int __song_id;
struct mpd_song *__queue[512];
int __queue_size = 0;

struct mpd_playlist *__playlists[512];
int __playlists_size = 0;

int __volume = 0;


void mpc_play(void) {
    mpd_run_play(conn2);
}

void mpc_pause() {
    mpd_send_pause(conn2, true);
    if (!mpd_response_finish(conn2))
        printErrorAndExit(conn2);
}

void mpc_toggle() {
    if (__state == MPD_STATE_PLAY) {
        mpc_pause();
    } else {
        mpc_play();
    }
}


void set_volume(int volume) {
    if (!mpd_run_set_volume(conn2, volume))
        printErrorAndExit(conn2);
    else
        __volume = volume;
}

int get_volume(void) {
    return __volume;
}

int get_state(void) {
    return __state;
}

int is_current(int index) {
    if (index < 0 || index > __queue_size) return 0;
    return mpd_song_get_id(__queue[index]) == __song_id;
}

int mpc_queue_song_index(void) { return __queue_song; }
int queue_size(void) { return __queue_size; }

struct mpd_song *queue_song(int index) {
    if (index < 0 || index > __queue_size) return NULL;
    return __queue[index];
}

// TODO: privide song
static char temp_s[64];
const char* queue_item(int index) {
    if (index < 0 || index > __queue_size) return NULL;
    char* s = songToFormatedString(__queue[index], "%title%", NULL);
    lcd_graphics__strcpy_with_ellipsis(temp_s, s, LCD_WIDTH_PIXELS);
    if (s != NULL) free(s);
    return temp_s;
}



int mpc_playlists_size(void) { return __playlists_size; }

const char* mpc_playlists_item(int index) {
    if (index < 0 || index > __playlists_size) return NULL;
    return mpd_playlist_get_path(__playlists[index]);
}



void model_refresh__status() {
    if(!mpd_send_status(conn))
        printErrorAndExit(conn);

    struct mpd_status *status;
    status = mpd_recv_status(conn);
    if (status == NULL)
        printErrorAndExit(conn);

    __state = mpd_status_get_state(status);

    __queue_song = mpd_status_get_song_pos(status);
    __song_id = mpd_status_get_song_id(status);
    if (__state == MPD_STATE_STOP) __queue_song = -1; // must be -1, but is not
    printf("song: %d\n", __queue_song);

    int volume = mpd_status_get_volume(status);
    if (volume >= 0) __volume = volume;

    mpd_status_free(status);
}


void model_refresh__queue() {
    printf("> model_refresh_queue\n");

    for (int i = 0; i < __queue_size; i++) mpd_song_free(__queue[i]);
    __queue_size = 0;

    struct mpd_song *song;

    if (!mpd_send_list_queue_meta(conn))
        printErrorAndExit(conn);

    int i = 0;
    while ((song = mpd_recv_song(conn)) != NULL) {
        __queue[i++] = song;
    }
    __queue_size = i;

    if (!mpd_response_finish(conn))
        printErrorAndExit(conn);

    printf("< model_refresh_queue\n");
}


void model_refresh__playlists() {
    printf("> model_refresh_playlists\n");

    for (int i = 0; i < __playlists_size; i++) mpd_playlist_free(__playlists[i]);
    __playlists_size = 0;

    struct mpd_playlist *playlist;

    if (!mpd_send_list_meta(conn, ""))
        printErrorAndExit(conn);

    int i = 0;
    while ((playlist = mpd_recv_playlist(conn)) != NULL) {
        __playlists[i++] = playlist;
    }
    __playlists_size = i;

    if (!mpd_response_finish(conn))
        printErrorAndExit(conn);

    printf("< model_refresh_playlists\n");
}


void model_refresh(int flags) {
    printf("> model_refresh\n");
    if (flags & MPD_IDLE_QUEUE) {
        model_refresh__queue();
    }
    if (flags & MPD_IDLE_PLAYER) {
        model_refresh__status();
    }
    if (flags & MPD_IDLE_STORED_PLAYLIST) {
        model_refresh__playlists();
    }
    printf("< model_refresh\n");
}


void model_run() {
    while(1) {
        printf("@ model_run: idle\n");
        enum mpd_idle idle = mpd_run_idle(conn);
        printf("@ model_run: idle returned %d\n", idle);
        if (idle == 0 || shutting_down) return;

        model_refresh(idle);
        lcdui_display__refresh(idle);
    }
}

void model_init() __attribute__((used)) {
    pthread_create (&thread, NULL, (void *) &model_run, (void *)NULL);
}

void model_deinit() __attribute__((used)) {
}
