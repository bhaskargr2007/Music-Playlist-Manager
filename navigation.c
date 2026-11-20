#include <stdio.h>
#include "playlist.h"

/* Function 1: Play Current Song */
void playSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    /* Initialize current pointer */
    if (p->current == NULL)
        p->current = p->head;

    printf("\n---------- NOW PLAYING ----------\n");
    printf("Song Title : %s\n", p->current->title);
    printf("Artist     : %s\n", p->current->artist);
    printf("---------------------------------\n");
    printf("Simulated playback started!\n");
}

/* Function 2: Play Next Song */
void nextSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    /* If current pointer is NULL */
    if (p->current == NULL)
    {
        p->current = p->head;
        playSong(p);
        return;
    }

    /* Check last song condition */
    if (p->current->next == NULL)
    {
        printf("\nYou are already at the last song!\n");
        return;
    }

    /* Move to next node */
    p->current = p->current->next;
    playSong(p);
}

/* Function 3: Play Previous Song */
void previousSong(Playlist *p)
{
    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    /* If current pointer is NULL */
    if (p->current == NULL)
    {
        p->current = p->head;
        playSong(p);
        return;
    }

    /* Check first song condition */
    if (p->current->prev == NULL)
    {
        printf("\nYou are already at the first song!\n");
        return;
    }

    /* Move to previous node */
    p->current = p->current->prev;
    playSong(p);
}
