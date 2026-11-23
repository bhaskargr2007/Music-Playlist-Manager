# Music Playlist Manager Using Doubly Linked List in C

## Project files

- `playlist.h` - Shared Song and Playlist structures and function declarations.
- `playlist.c` - Member 1: add, remove, and display songs.
- `navigation.c` - Member 2: play, next, and previous song navigation.
- `search.c` - Member 3: case-insensitive search by title or artist.
- `shuffle.c` - Member 3: Fisher-Yates playlist shuffle.
- `storage.c` - Member 4: save and load playlist data using a text file.
- `main.c` - Member 4: colorful terminal dashboard, menu, input validation, module integration, and cleanup.

## Compile

Open a terminal in this folder and run:

```bash
gcc -Wall -Wextra -Werror -std=c11 main.c playlist.c navigation.c search.c shuffle.c storage.c -o music_playlist
```

## Run

```bash
./music_playlist
```

Use a terminal that supports ANSI escape sequences for the dashboard colors and screen clearing.

## Dashboard and menu options

The dashboard displays the currently selected song, artist, track position, total number of songs, and the complete menu. All dashboard information is read from the actual doubly linked list.

0. Exit
1. Add Song
2. Remove Song
3. Display Playlist
4. Play Selected Song
5. Next Song
6. Previous Song
7. Search Song
8. Shuffle Playlist
9. Save Playlist
10. Load Playlist
11. Repeat Mode (OFF / REPEAT ALL / REPEAT ONE)
12. Play from Beginning
13. Play from Ending

## File storage

The program saves data in `playlist.txt` in the current working directory. Each song is stored as two lines: title, then artist. Loading replaces the current in-memory playlist only after the saved file has been read successfully.

Keep `playlist.txt` in the same working directory when you want to load the saved playlist.

## Notes

- Playback is simulated; no audio files are played.
- Shuffle changes the linked-list order using the Fisher-Yates algorithm and resets the current song to the new first song.
- The program uses dynamic memory allocation and frees the playlist nodes on exit.
- Loading a saved playlist replaces the songs currently in memory.
- The original menu file was backed up as `main.c.backup-before-ui-20261120`.
- The separate `Music-Playlist-UI-Demo` folder remains unchanged.


## Dashboard playback indicators

The dashboard displays **NOW SELECTED** and **NOW PLAYING** separately. Next/Previous changes only the selected song. The Now Playing song remains unchanged until **Play Selected Song** is chosen. Playback is simulated; no audio is played.

- **Add Song**: prevents duplicate entries when both the song title and artist match an existing track. The comparison ignores letter case and leading/trailing spaces. The same title by a different artist is allowed.
- **Remove Song**: offers three methods: (1) remove the song highlighted in **NOW SELECTED**, (2) choose a track by its playlist number, or (3) enter the song title and artist. Every method asks for confirmation. If the selected song is removed, the next song becomes selected (or the previous song if the removed track was last). If the playing song is removed, **NOW PLAYING** is cleared safely. Other selected/playing pointers remain unchanged.

## Playback controls

- **Repeat OFF**: the queue preview ends after the final song.
- **Repeat ALL**: after the final song, the queue preview points to the first song.
- **Repeat ONE**: the queue preview points to the currently playing song.
- **Play from Beginning**: sets both the selected and playing song to the first track.
- **Play from Ending**: sets both the selected and playing song to the last track.
- **Play from Ending** uses stop-at-end behaviour; it does not wrap to the beginning.
- Playback remains simulated. There is no audio engine or automatic time-based track advancement; the repeat mode is reflected in the NEXT IN QUEUE preview.


## Named playlist saving

Menu option **[9] Save Playlist** now provides:
- **[1] Create New Playlist** — creates a new file and refuses to overwrite an existing name.
- **[2] Update Current Playlist** — updates the currently loaded/saved playlist.
- **[3] Save As (Different Name)** — creates another named playlist without overwriting an existing file.
- **[0] Return** — closes the save menu.

Menu option **[10] Load Playlist** remains for opening existing playlists only.
