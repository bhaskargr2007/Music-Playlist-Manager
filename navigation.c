#include <stdio.h>
#include "playlist.h"

/* Start visual playback for the currently selected song. */
void playSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    /* Select the first song if no song is currently selected. */
    if (p->current == NULL)
        p->current = p->head;

    /* Only this function changes the visual now-playing pointer. */
    p->playing = p->current;

    printf("\n---------- NOW PLAYING ----------\n");
    printf("Song Title : %s\n", p->playing->title);
    printf("Artist     : %s\n", p->playing->artist);
    printf("---------------------------------\n");
    printf("Visual playback started!\n");
}

/* Move the selection forward without changing the playing song. */
void nextSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    if (p->current == NULL)
    {
        p->current = p->head;
        printf("\nSelected: %s\n", p->current->title);
        return;
    }

    if (p->current->next == NULL)
    {
        printf("\nYou are already at the last song!\n");
        return;
    }

    p->current = p->current->next;
    printf("\nSelected next song: %s\n", p->current->title);
    printf("Press 4 to start visual playback of this song.\n");
}

/* Move the selection backward without changing the playing song. */
void previousSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    if (p->current == NULL)
    {
        p->current = p->head;
        printf("\nSelected: %s\n", p->current->title);
        return;
    }

    if (p->current->prev == NULL)
    {
        printf("\nYou are already at the first song!\n");
        return;
    }

    p->current = p->current->prev;
    printf("\nSelected previous song: %s\n", p->current->title);
    printf("Press 4 to start visual playback of this song.\n");
}
