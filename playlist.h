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

/* Structure for the playlist */
typedef struct Playlist
{
    Song *head;
    Song *tail;
    Song *current;  /* Song currently selected in the playlist */
    Song *playing;  /* Song most recently started for visual playback */

} Playlist;

/* Member 1: Playlist Management */
void addSong(Playlist *p);
void removeSong(Playlist *p);
void displayPlaylist(Playlist *p);

/* Member 2: Playback and Navigation */
void playSong(Playlist *p);
void nextSong(Playlist *p);
void previousSong(Playlist *p);

/* Member 3: Search and Shuffle */
void searchSong(Playlist *p);
void shufflePlaylist(Playlist *p);

/* Member 4: File Handling and Main Menu */
void savePlaylist(Playlist *p);
void loadPlaylist(Playlist *p);

#endif
