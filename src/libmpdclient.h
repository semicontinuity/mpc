/* libmpdclient
 * (c)2003-2004 by Warren Dukes (shank@mercury.chem.pitt.edu)
 * This project's homepage is: http://www.musicpd.org
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LIBMPDCLIENT_H
#define LIBMPDCLIENT_H

#include <sys/param.h>
#include <sys/time.h>

#define MPD_BUFFER_MAX_LENGTH	50000
#define MPD_WELCOME_MESSAGE	"OK MPD "

#define MPD_ERROR_TIMEOUT	10 /* timeout trying to talk to mpd */
#define MPD_ERROR_SYSTEM	11 /* system error */
#define MPD_ERROR_UNKHOST	12 /* unknown host */
#define MPD_ERROR_CONNPORT	13 /* problems connecting to port on host */
#define MPD_ERROR_NOTMPD	14 /* mpd not running on port at host */
#define MPD_ERROR_NORESPONSE	15 /* no response on attempting to connect */
#define MPD_ERROR_SENDING	16 /* error sending command */
#define MPD_ERROR_CONNCLOSED	17 /* connection closed by mpd */
#define MPD_ERROR_ACK		18 /* ACK returned! */
#define MPD_ERROR_BUFFEROVERRUN	19 /* Buffer was overrun! */

#ifdef __cplusplus
extern "C" {
#endif

/* internal stuff don't touch this struct */
typedef struct _mpd_ReturnElement {
	char * name;
	char * value;
} mpd_ReturnElement;

/* mpd_Connection
 * holds info about connection to mpd
 * use error, and errorStr to detect errors
 */
typedef struct _mpd_Connection {
	/* use this to check the version of mpd */
	int version[3];
	/* IMPORTANT, you want to get the error messages from here */
	char errorStr[MPD_BUFFER_MAX_LENGTH+1];
	/* this will be set to 1 if there is an error, 0 if not */
	int error;
	/* DON'T TOUCH any of the rest of this stuff */
	int sock; 
	char buffer[MPD_BUFFER_MAX_LENGTH+1];
	int buflen;
	int bufstart;
	int doneProcessing;
	int commandList;
	mpd_ReturnElement * returnElement;
	struct timeval timeout;
} mpd_Connection;

/* mpd_newConnection
 * use this to open a new connection
 * you should use mpd_closeConnection, when your done with the connection,
 * even if an error has occurred
 * _host_ format can include the password for the host: password@host
 * _timeout_ is the connection timeout period in seconds
 */
mpd_Connection * mpd_newConnection(const char * host, int port, float timeout);

void mpd_setConnectionTimeout(mpd_Connection * connection, float timeout);

/* mpd_closeConnection
 * use this to close a connection and free'ing subsequent memory
 */
void mpd_closeConnection(mpd_Connection * connection);

/* mpd_clearError
 * clears error
 */
void mpd_clearError(mpd_Connection * connection);

/* STATUS STUFF */

/* use these with status.state to determine what state the player is in */
#define MPD_STATUS_STATE_UNKNOWN	0
#define MPD_STATUS_STATE_STOP		1
#define MPD_STATUS_STATE_PLAY		2
#define MPD_STATUS_STATE_PAUSE		3

/* us this with status.volume to determine if mpd has volume support */
#define MPD_STATUS_NO_VOLUME		-1

/* mpd_Status
 * holds info return from status command
 */
typedef struct mpd_Status {
	/* 0-100, or MPD_STATUS_NO_VOLUME when there is no volume support */
	int volume;
	/* 1 if repeat is on, 0 otherwise */
	int repeat;
	/* 1 if random is on, 0 otherwise */
	int random;
	/* playlist length */
	int playlistLength;
	/* playlist, use this to determine when the playlist has changed */
	long long playlist;
	/* use with MPD_STATUS_STATE_* to determine state of player */
	int state;
	/* crossfade setting in seconds */
	int crossfade;
	/* if in PLAY or PAUSE state, this is the number of the currently
	 * playing song in the playlist, beginning with 0
	 */
	int song;
	/* time in seconds that have elapsed in the currently playing/paused
	 * song
	 */
	int elapsedTime;
	/* length in seconds of the currently playing/paused song */
	int totalTime;
	/* current bit rate in kbs */
	int bitRate;
	/* audio sample rate */
	unsigned int sampleRate;
	/* audio bits */
	int bits;
	/* audio channels */
	int channels;
	/* error */
	char * error;
} mpd_Status;

/* mpd_getStatus
 * returns status info, be sure to free it with mpd_freeStatus()
 */
mpd_Status * mpd_getStatus(mpd_Connection * connection);

/* mpd_freeStatus
 * free's status info malloc'd and returned by mpd_getStatus
 */
void mpd_freeStatus(mpd_Status * status);

typedef struct _mpd_Stats {
	int numberOfArtists;
	int numberOfAlbums;
	int numberOfSongs;
	unsigned long uptime;
	unsigned long dbUpdateTime;
} mpd_Stats;

mpd_Stats * mpd_getStats(mpd_Connection * connection);

void mpd_freeStats(mpd_Stats * stats);

/* SONG STUFF */

#define MPD_SONG_NO_TIME	-1

/* mpd_Song
 * for storing song info returned by mpd
 */
typedef struct _mpd_Song {
	/* filename of song */
	char * file;
	/* artist, maybe NULL if there is no tag */
	char * artist;
	/* title, maybe NULL if there is no tag */
	char * title;
	/* album, maybe NULL if there is no tag */
	char * album;
	/* track, maybe NULL if there is no tag */
	char * track;
	/* length of song in seconds, check that it is not MPD_SONG_NO_TIME  */
	int time;
} mpd_Song;

/* mpd_newSong
 * use to allocate memory for a new mpd_Song
 * file, artist, etc all initialized to NULL
 * if your going to assign values to file, artist, etc
 * be sure to malloc or strdup the memory
 * use mpd_freeSong to free the memory for the mpd_Song, it will also
 * free memory for file, artist, etc, so don't do it yourself
 */
mpd_Song * mpd_newSong();

/* mpd_freeSong
 * use to free memory allocated by mpd_newSong
 * also it will free memory pointed to by file, artist, etc, so be careful
 */
void mpd_freeSong(mpd_Song * song);

/* mpd_songDup
 * works like strDup, but for a mpd_Song
 */
mpd_Song * mpd_songDup(mpd_Song * song);

/* DIRECTORY STUFF */

/* mpd_Directory
 * used to store info fro directory (right now that just the path)
 */
typedef struct _mpd_Directory {
	char * path;
} mpd_Directory;

/* mpd_newDirectory
 * allocates memory for a new directory
 * use mpd_freeDirectory to free this memory
 */
mpd_Directory * mpd_newDirectory ();

/* mpd_freeDirectory
 * used to free memory allocated with mpd_newDirectory, and it frees
 * path of mpd_Directory, so be careful
 */
void mpd_freeDirectory(mpd_Directory * directory);

/* mpd_directoryDup
 * works like strdup, but for mpd_Directory
 */
mpd_Directory * mpd_directoryDup(mpd_Directory * directory);

/* PLAYLISTFILE STUFF */

/* mpd_PlaylistFile
 * stores info about playlist file returned by lsinfo
 */
typedef struct _mpd_PlaylistFile {
	char * path;
} mpd_PlaylistFile;

/* mpd_newPlaylistFile
 * allocates memory for new mpd_PlaylistFile, path is set to NULL
 * free this memory with mpd_freePlaylistFile
 */
mpd_PlaylistFile * mpd_newPlaylistFile();

/* mpd_freePlaylist
 * free memory allocated for freePlaylistFile, will also free
 * path, so be careful
 */
void mpd_freePlaylistFile(mpd_PlaylistFile * playlist);

/* mpd_playlistFileDup
 * works like strdup, but for mpd_PlaylistFile
 */
mpd_PlaylistFile * mpd_playlistFileDup(mpd_PlaylistFile * playlist);

/* INFO ENTITY STUFF */

/* the type of entity returned from one of the commands that generates info
 * use in conjunction with mpd_InfoEntity.type
 */
#define MPD_INFO_ENTITY_TYPE_DIRECTORY		0
#define MPD_INFO_ENTITY_TYPE_SONG		1
#define MPD_INFO_ENTITY_TYPE_PLAYLISTFILE	2

/* mpd_InfoEntity
 * stores info on stuff returned info commands
 */
typedef struct mpd_InfoEntity {
	/* the type of entity, use with MPD_INFO_ENTITY_TYPE_* to determine
	 * what this entity is (song, directory, etc...)
	 */
	int type;
	/* the actual data you want, mpd_Song, mpd_Directory, etc */
	union {
		mpd_Directory * directory;
		mpd_Song * song;
		mpd_PlaylistFile * playlistFile;
	} info;
} mpd_InfoEntity;

mpd_InfoEntity * mpd_newInfoEntity();

void mpd_freeInfoEntity(mpd_InfoEntity * entity);

/* INFO COMMANDS AND STUFF */

/* use this function to loop over after calling Info/Listall functions */
mpd_InfoEntity * mpd_getNextInfoEntity(mpd_Connection * connection);

/* songNum of -1, means to display the whole list */
void mpd_sendPlaylistInfoCommand(mpd_Connection * connection, int songNum);

void mpd_sendListallCommand(mpd_Connection * connection, const char * dir);

void mpd_sendListallInfoCommand(mpd_Connection * connection, const char * dir);

void mpd_sendLsInfoCommand(mpd_Connection * connection, const char * dir);

#define MPD_TABLE_ARTIST	0
#define MPD_TABLE_ALBUM		1
#define MPD_TABLE_TITLE		2
#define MPD_TABLE_FILENAME	3

void mpd_sendSearchCommand(mpd_Connection * connection, int table, 
		const char * str);

void mpd_sendFindCommand(mpd_Connection * connection, int table, 
		const char * str);

/* LIST TAG COMMANDS */

/* use this function fetch next artist entry, be sure to free the returned 
 * string.  NULL means there are no more.  Best used with sendListArtists
 */
char * mpd_getNextArtist(mpd_Connection * connection);

char * mpd_getNextAlbum(mpd_Connection * connection);

/* list artist or albums by artist, arg1 should be set to the artist if
 * listing albums by a artist, otherwise NULL for listing all artists or albums
 */
void mpd_sendListCommand(mpd_Connection * connection, int table, 
		const char * arg1);

void mpd_sendListAlbumsCommand(mpd_Connection * connection, 
		const char * artist);

/* SIMPLE COMMANDS */

void mpd_sendAddCommand(mpd_Connection * connection, const char * file);

void mpd_sendDeleteCommand(mpd_Connection * connection, int songNum);

void mpd_sendSaveCommand(mpd_Connection * connection, const char * name);

void mpd_sendLoadCommand(mpd_Connection * connection, const char * name);

void mpd_sendRmCommand(mpd_Connection * connection, const char * name);

void mpd_sendShuffleCommand(mpd_Connection * connection);

void mpd_sendClearCommand(mpd_Connection * connection);

/* use this to start playing at the beginning, useful when in random mode */
#define MPD_PLAY_AT_BEGINNING	-1

void mpd_sendPlayCommand(mpd_Connection * connection, int songNum);

void mpd_sendStopCommand(mpd_Connection * connection);

void mpd_sendPauseCommand(mpd_Connection * connection);

void mpd_sendNextCommand(mpd_Connection * connection);

void mpd_sendPrevCommand(mpd_Connection * connection);

void mpd_sendMoveCommand(mpd_Connection * connection, int from, int to);

void mpd_sendSwapCommand(mpd_Connection * connection, int song1, int song2);

void mpd_sendSeekCommand(mpd_Connection * connection, int song, int time);

void mpd_sendRepeatCommand(mpd_Connection * connection, int repeatMode);

void mpd_sendRandomCommand(mpd_Connection * connection, int randomMode);

void mpd_sendSetvolCommand(mpd_Connection * connection, int volumeChange);

/* WARNING: don't use volume command, its depreacted */
void mpd_sendVolumeCommand(mpd_Connection * connection, int volumeChange);

void mpd_sendCrossfadeCommand(mpd_Connection * connection, int seconds);

int mpd_getCrossfade(mpd_Connection * connection);

void mpd_sendUpdateCommand(mpd_Connection * connection);

void mpd_sendPasswordCommand(mpd_Connection * connection, const char * pass);

/* after executing a command, when your done with it to get its status
 * (you want to check connection->error for an error)
 */
void mpd_finishCommand(mpd_Connection * connection);

/* command list stuff, use this to do things like add files very quickly */
void mpd_sendCommandListBegin(mpd_Connection * connection);

void mpd_sendCommandListEnd(mpd_Connection * connection);

#ifdef __cplusplus
}
#endif

#endif
