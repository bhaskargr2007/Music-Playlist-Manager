#include <stdio.h>
#include "playlist.h"

/* Start simulated playback for the selected song. */
void playSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    if (p->current == NULL)
        p->current = p->head;

    /* Playback changes only when the user explicitly presses Play. */
    p->playing = p->current;

    printf("\n---------- NOW PLAYING ----------\n");
    printf("Song Title : %s\n", p->playing->title);
    printf("Artist     : %s\n", p->playing->artist);
    printf("---------------------------------\n");
    printf("Simulated playback started!\n");
}

/* Navigation changes selection only; it does not change playback. */
void nextSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    if (p->current == NULL)
        p->current = p->head;
    else if (p->current->next == NULL)
    {
        printf("\nYou are already at the last song!\n");
        return;
    }
    else
        p->current = p->current->next;

    printf("\nSelected: %s — %s\n", p->current->title, p->current->artist);
    printf("Now Playing remains unchanged. Press [4] to play this selection.\n");
}

void previousSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    if (p->current == NULL)
        p->current = p->head;
    else if (p->current->prev == NULL)
    {
        printf("\nYou are already at the first song!\n");
        return;
    }
    else
        p->current = p->current->prev;

    printf("\nSelected: %s — %s\n", p->current->title, p->current->artist);
    printf("Now Playing remains unchanged. Press [4] to play this selection.\n");
}

/* Start simulated playback from the first playlist node. */
void playFromBeginning(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    p->current = p->head;
    p->playing = p->head;
    printf("\nPlayback started from the beginning: %s — %s\n",
           p->playing->title, p->playing->artist);
}

/* Start simulated playback from the final node; it has no following track. */
void playFromEnding(Playlist *p)
{
    if (p == NULL || p->tail == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    p->current = p->tail;
    p->playing = p->tail;
    printf("\nPlayback started from the ending: %s — %s\n",
           p->playing->title, p->playing->artist);
    printf("This is the last track. The simulated playlist has no next song.\n");
}
