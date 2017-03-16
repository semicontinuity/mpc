#ifndef __MODEL_H__
#define __MODEL_H__

#include <mpd/connection.h>
#include <mpd/song.h>


extern int shutting_down;


extern struct mpd_connection *conn;
extern struct mpd_connection *conn2;

void mpc_play(void);
void mpc_pause(void);
void mpc_toggle(void);

void set_volume(int volume);
int get_volume(void);
int get_state(void);
int is_current(int index);
int now_playing_index(void) __attribute__((used));

int queue_size(void);
const char* queue_item(int index);
struct mpd_song *queue_song(int index);
int mpc_queue_song_index(void);


int mpc_playlists_size(void);
const char* mpc_playlists_item(int index);


void model_refresh(int flags);


void model_init(void) __attribute__((used));

void model_deinit(void) __attribute__((used));

#endif
