#ifndef PLAYLIST_H
#define PLAYLIST_H

/* Structure for each song */
typedef struct Song
{
    char title[100];
    char artist[100];

    struct Song *prev;
    struct Song *next;

} Song;

typedef enum RepeatMode
{
    REPEAT_OFF = 0,
    REPEAT_ALL,
    REPEAT_ONE
} RepeatMode;

/* Structure for the playlist */
typedef struct Playlist
{
    Song *head;
    Song *tail;
    Song *current;       /* Song highlighted for navigation */
    Song *playing;       /* Song currently in simulated playback */
    RepeatMode repeatMode; /* Repeat OFF, ALL, or ONE */
    char currentPlaylistName[100]; /* Empty means not yet saved */
    int dirty; /* Song data changed since last save/load */

} Playlist;

/* Member 1: Playlist Management */
void addSong(Playlist *p);
void removeSong(Playlist *p);
void displayPlaylist(Playlist *p);

/* Member 2: Playback and Navigation */
void playSong(Playlist *p);
void nextSong(Playlist *p);
void previousSong(Playlist *p);
void playFromBeginning(Playlist *p);
void playFromEnding(Playlist *p);

/* Member 3: Search and Shuffle */
void searchSong(Playlist *p);
void shufflePlaylist(Playlist *p);

/* Member 4: File Handling and Main Menu */
void savePlaylist(Playlist *p);
void loadPlaylist(Playlist *p);

#endif
