/* music player command (mpc)
 * Copyright (C) 2003-2008 Warren Dukes <warren.dukes@gmail.com>,
				Eric Wong <normalperson@yhbt.net>,
				Daniel Brown <danb@cs.utexas.edu>
 * Copyright (C) 2008-2010 Max Kellermann <max@duempel.org>
 * Project homepage: http://musicpd.org

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "list.h"
#include "charset.h"
#include "password.h"
#include "util.h"
#include "status.h"
#include "command.h"
#include "sticker.h"
#include "idle.h"
#include "search.h"
#include "mpc.h"
#include "options.h"

#include <mpd/client.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "lcd_graphics.h"
#include "lcdui_display.h"
#include "font_v8_var.h"

#include "form__playlists.h"

#include "model.h"
#include "keyboard.h"

static char* command_load_playlist_1[] = {"1"};
static char* command_load_playlist_2[] = {"2"};
static char* command_load_playlist_3[] = {"3"};
static char* command_load_playlist_4[] = {"4"};
static char* command_load_playlist_5[] = {"5"};
static char* command_load_playlist_6[] = {"6"};
static char* command_load_playlist_7[] = {"7"};
static char* command_load_playlist_8[] = {"8"};
static char* command_load_playlist_9[] = {"9"};

static char* command_volume_up[]   = {"+10"};
static char* command_volume_down[] = {"-10"};

cmdhandler get_command(int* argc, char*** argv) {
    while (1) {
        int c = read_key();
        printf("%d\n", c);
        switch (c) {
        case VK_STATUS:
            printf("status\n");
            *argc = 2;
            return cmd_status;
        case VK_TOGGLE:
            printf("toggle\n");
            *argc = 0;
            return cmd_toggle;
        case VK_NEXT:
            printf("next\n");
            *argc = 0;
            return cmd_next;
        case VK_PREVIOUS:
            printf("previous\n");
            *argc = 0;
            return cmd_prev;
        case VK_VOLUME_UP:
            printf("volume up");
            *argc = 1;
            *argv = command_volume_up;
            return cmd_volume;
        case VK_VOLUME_DOWN:
            printf("volume down");
            *argc = 1;
            *argv = command_volume_down;
            return cmd_volume;
        case VK_QUIT:
            printf("quit\n");
            return NULL;
        case VK_1:
            *argc = 1;
            *argv = command_load_playlist_1;
            return cmd_load;
        case VK_2:
            *argc = 1;
            *argv = command_load_playlist_2;
            return cmd_load;
        case VK_3:
            *argc = 1;
            *argv = command_load_playlist_3;
            return cmd_load;
        case VK_4:
            *argc = 1;
            *argv = command_load_playlist_4;
            return cmd_load;
        case VK_5:
            *argc = 1;
            *argv = command_load_playlist_5;
            return cmd_load;
        case VK_6:
            *argc = 1;
            *argv = command_load_playlist_6;
            return cmd_load;
        case VK_7:
            *argc = 1;
            *argv = command_load_playlist_7;
            return cmd_load;
        case VK_8:
            *argc = 1;
            *argv = command_load_playlist_8;
            return cmd_load;
        case VK_9:
            *argc = 1;
            *argv = command_load_playlist_9;
            return cmd_load;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct command {
	const char *command;
	const int min, max;   /* min/max arguments allowed, -1 = unlimited */
	int pipe;             /**
	                       * 1: implicit pipe read, `-' optional as argv[2]
	                       * 2: explicit pipe read, `-' needed as argv[2]
	                       *
	                       * multipled by -1 if used, so that it can signal
	                       * a free() before the program exits
	                       */
	cmdhandler handler;
	const char *usage;

	/** NULL means they won't be shown in help */
	const char *help;
}

 mpc_table [] = {
	/* command,     min, max, pipe, handler,         usage, help */
	{"add",         0,   -1,  1,    cmd_add,         "<file>", "Add a song to the current playlist"},
	{"crop",        0,   0,   0,    cmd_crop,        "", "Remove all but the currently playing song"},
	{ "current", 0, 0, 0, cmd_current,
	  "", "Show the currently playing song"},
	{"del",         0,   -1,  1,    cmd_del,         "<position>", "Remove a song from the current playlist"},
	{"play",        0,   -1,  2,    cmd_play,        "[<position>]", "Start playing at <position> (default: 1)"},
	{"next",        0,   0,   0,    cmd_next,        "", "Play the next song in the current playlist"},
	{"prev",        0,   0,   0,    cmd_prev,        "", "Play the previous song in the current playlist"},
	{"pause",       0,   0,   0,    cmd_pause,       "", "Pauses the currently playing song"},
	{"toggle",      0,   0,   0,    cmd_toggle,      "", "Toggles Play/Pause, plays if stopped"},
	{"stop",        0,   0,   0,    cmd_stop,        "", "Stop the currently playing playlists"},
	{"seek",        1,   1,   0,    cmd_seek,        "[+-][HH:MM:SS]|<0-100>%", "Seeks to the specified position"},
	{"clear",       0,   0,   0,    cmd_clear,       "", "Clear the current playlist"},
	{"outputs",     0,   0,   0,    cmd_outputs,     "", "Show the current outputs"},
	{"enable",      1,   1,   0,    cmd_enable,      "<output #>", "Enable a output"},
	{"disable",     1,   1,   0,    cmd_disable,     "<output #>", "Disable a output"},
	{"shuffle",     0,   0,   0,    cmd_shuffle,     "", "Shuffle the current playlist"},
	{"move",        2,   2,   0,    cmd_move,        "<from> <to>", "Move song in playlist"},
	{"playlist",    0,   0,   0,    cmd_playlist,    "", "Print the current playlist"},
	{"listall",     0,   -1,  2,    cmd_listall,     "[<file>]", "List all songs in the music dir"},
	{"ls",          0,   -1,  2,    cmd_ls,          "[<directory>]", "List the contents of <directory>"},
	{"lsplaylists", 0,   -1,  2,    cmd_lsplaylists, "", "List currently available playlists"},
	{"load",        0,   -1,  1,    cmd_load,        "<file>", "Load <file> as a playlist"},
	{"save",        1,   1,   0,    cmd_save,        "<file>", "Save a playlist as <file>"},
	{"rm",          1,   1,   0,    cmd_rm,          "<file>", "Remove a playlist"},
	{"volume",      0,   1,   0,    cmd_volume,      "[+-]<num>", "Set volume to <num> or adjusts by [+-]<num>"},
	{"repeat",      0,   1,   0,    cmd_repeat,      "<on|off>", "Toggle repeat mode, or specify state"},
	{"random",      0,   1,   0,    cmd_random,      "<on|off>", "Toggle random mode, or specify state"},
	{"single",      0,   1,   0,    cmd_single,      "<on|off>", "Toggle single mode, or specify state"},
	{"consume",     0,   1,   0,    cmd_consume,     "<on|off>", "Toggle consume mode, or specify state"},
	{"search",      2,   -1,  0,    cmd_search,      "<type> <query>", "Search for a song"},
	{"find",        2,   -1,  0,    cmd_find,        "<type> <query>", "Find a song (exact match)"},
	{"findadd", 2, -1, 0, cmd_findadd, "<type> <query>",
	 "Find songs and add them to the current playlist"},
	{"list",        1,   -1,  0,    cmd_list,        "<type> [<type> <query>]", "Show all tags of <type>"},
	{"crossfade",   0,   1,   0,    cmd_crossfade,   "[<seconds>]", "Set and display crossfade settings"},
#if LIBMPDCLIENT_CHECK_VERSION(2,4,0)
	{"clearerror",  0,   0,   0,    cmd_clearerror,  "", "Clear the current error"},
#endif
#if LIBMPDCLIENT_CHECK_VERSION(2,2,0)
	{"mixrampdb",   0,   1,   0,    cmd_mixrampdb,   "[<dB>]", "Set and display mixrampdb settings"},
	{"mixrampdelay",0,   1,   0,    cmd_mixrampdelay,"[<seconds>]", "Set and display mixrampdelay settings"},
#endif
	{"update",      0,   -1,  2,    cmd_update,      "[<path>]", "Scan music directory for updates"},
#if LIBMPDCLIENT_CHECK_VERSION(2,1,0)
	{"sticker",     1,   -1,  0,    cmd_sticker,     "<uri> <get|set|list|del> <args..>", "Sticker management"},
#endif
	{"stats",       0,   -1,  0,    cmd_stats,       "", "Display statistics about MPD"},
	{"version",     0,   0,   0,    cmd_version,     "", "Report version of MPD"},
	/* loadtab, lstab, and tab used for completion-scripting only */
	{"loadtab",     0,   1,   0,    cmd_loadtab,     "<directory>", NULL},
	{"lstab",       0,   1,   0,    cmd_lstab,       "<directory>", NULL},
	{"tab",         0,   1,   0,    cmd_tab,         "<path>", NULL},
	/* status was added for pedantic reasons */
	{"status",      0,   -1,  0,    cmd_status,      "", NULL},
	{ "idle", 0, -1, 0, cmd_idle, "[events]",
	  "Idle until an event occurs" },
	{ "idleloop", 0, -1, 0, cmd_idleloop, "[events]",
	  "Continuously idle until an event occurs" },
	{ "replaygain", 0, -1, 0, cmd_replaygain, "[off|track|album]",
	  "Set or display the replay gain mode" },

	/* don't remove this, when mpc_table[i].command is NULL it will terminate the loop */
	{ .command = NULL }
};


static struct mpd_connection *
setup_connection(void)
{
	struct mpd_connection *conn;

	conn = mpd_connection_new(options.host, options.port, 0);
	if (conn == NULL) {
		fputs("Out of memory\n", stderr);
		exit(EXIT_FAILURE);
	}

	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
		printErrorAndExit(conn);

	if(options.password)
		send_password(options.password, conn);

	return conn;
}


int main(int argc, char ** argv)
{
	int ret = 0;
        
        int command_argc;
        char** command_argv;
        cmdhandler command_handler;

	parse_options(&argc, argv);

	/* parse command and arguments */


	/* initialization */
	charset_init(true, true);
        keyboard_init();
        conn = setup_connection();
        conn2 = setup_connection();

        model_refresh(0xFFFF);
        model_init();

        lcd_graphics__init();
        lcd_graphics__setFont(&FONT_V8_6P_VAR);

        lcdui_display__setCurrent(&form__playlists);

	/* run */
        while (1) {
            int c = read_key();
            lcdui_display__handleEvent(c);

//            command_handler = get_command(&command_argc, &command_argv);
//            if (command_handler == NULL) break;

//            ret = command_handler(command_argc, command_argv, conn);
//            if (ret != 0 && options.verbosity > V_QUIET) {
//                print_status(conn);
//            }
        }

        lcd_graphics__deinit();

        model_deinit();
	/* cleanup */
        mpd_connection_free(conn2);
        mpd_connection_free(conn);
        printf("disconnected\n");
        keyboard_deinit();
	charset_deinit();

        printf("exit\n");
	return ret;
}
