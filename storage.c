#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

#define PLAYLIST_FILE "playlist.txt"

/* Read one line, reject lines that exceed the Song field capacity. */
static int readSongField(FILE *file, char *field, size_t size)
{
    size_t length;

    if (fgets(field, (int)size, file) == NULL)
        return feof(file) ? 0 : -1;

    length = strlen(field);

    if (length > 0 && field[length - 1] == '\n')
    {
        field[length - 1] = '\0';
        if (length > 1 && field[length - 2] == '\r')
            field[length - 2] = '\0';
        return 1;
    }

    if (!feof(file))
    {
        int ch;
        while ((ch = fgetc(file)) != '\n' && ch != EOF)
            ;
        return -1;
    }

    if (length > 0 && field[length - 1] == '\r')
        field[length - 1] = '\0';

    return 1;
}

/* Release every node in a playlist. */
static void freeSongs(Playlist *p)
{
    Song *temp;
    Song *nextSong;

    temp = p->head;

    while (temp != NULL)
    {
        nextSong = temp->next;
        free(temp);
        temp = nextSong;
    }

    p->head = NULL;
    p->tail = NULL;
    p->current = NULL;
    p->playing = NULL;
}

/* Save each song as two lines: title followed by artist. */
void savePlaylist(Playlist *p)
{
    FILE *file;
    Song *temp;

    if (p == NULL)
    {
        printf("\nInvalid playlist!\n");
        return;
    }

    file = fopen(PLAYLIST_FILE, "w");

    if (file == NULL)
    {
        printf("\nUnable to open %s for saving.\n", PLAYLIST_FILE);
        return;
    }

    temp = p->head;

    while (temp != NULL)
    {
        if (fprintf(file, "%s\n%s\n", temp->title, temp->artist) < 0)
        {
            printf("\nError while writing the playlist file.\n");
            fclose(file);
            return;
        }

        temp = temp->next;
    }

    if (fclose(file) != 0)
    {
        printf("\nError while closing the playlist file.\n");
        return;
    }

    printf("\nPlaylist saved successfully to %s!\n", PLAYLIST_FILE);
}

/* Load songs into a temporary list before replacing the current playlist. */
void loadPlaylist(Playlist *p)
{
    FILE *file;
    Playlist loaded = {NULL, NULL, NULL, NULL};
    Song *newSong;
    int titleStatus;
    int artistStatus;
    int count = 0;

    if (p == NULL)
    {
        printf("\nInvalid playlist!\n");
        return;
    }

    file = fopen(PLAYLIST_FILE, "r");

    if (file == NULL)
    {
        printf("\nNo saved playlist found (%s).\n", PLAYLIST_FILE);
        return;
    }

    while (1)
    {
        char title[100];
        char artist[100];

        titleStatus = readSongField(file, title, sizeof(title));

        if (titleStatus == 0)
            break;

        if (titleStatus < 0)
        {
            printf("\nPlaylist file contains an invalid or overlong title. Load cancelled.\n");
            freeSongs(&loaded);
            fclose(file);
            return;
        }

        artistStatus = readSongField(file, artist, sizeof(artist));

        if (artistStatus != 1)
        {
            printf("\nPlaylist file is incomplete or contains an invalid artist. Load cancelled.\n");
            freeSongs(&loaded);
            fclose(file);
            return;
        }

        if (title[0] == '\0' || artist[0] == '\0')
        {
            printf("\nPlaylist file contains an empty title or artist. Load cancelled.\n");
            freeSongs(&loaded);
            fclose(file);
            return;
        }

        newSong = malloc(sizeof(Song));

        if (newSong == NULL)
        {
            printf("\nMemory allocation failed. Load cancelled.\n");
            freeSongs(&loaded);
            fclose(file);
            return;
        }

        strcpy(newSong->title, title);
        strcpy(newSong->artist, artist);

        newSong->prev = loaded.tail;
        newSong->next = NULL;

        if (loaded.head == NULL)
        {
            loaded.head = newSong;
            loaded.current = newSong;
        }
        else
        {
            loaded.tail->next = newSong;
        }

        loaded.tail = newSong;
        count++;
    }

    if (ferror(file))
    {
        printf("\nError while reading the playlist file. Load cancelled.\n");
        freeSongs(&loaded);
        fclose(file);
        return;
    }

    fclose(file);

    /* Replace the old playlist only after a complete successful load. */
    freeSongs(p);
    p->head = loaded.head;
    p->tail = loaded.tail;
    p->current = loaded.current;
    p->playing = NULL;

    printf("\nPlaylist loaded successfully! Songs loaded: %d\n", count);
}
