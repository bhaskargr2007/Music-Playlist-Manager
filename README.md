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
gcc -Wall -Wextra -std=c11 main.c playlist.c navigation.c search.c shuffle.c storage.c -o music_playlist
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
