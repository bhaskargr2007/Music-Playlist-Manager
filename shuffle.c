#include <stdio.h>
#include <stdlib.h>
#include "playlist.h"

/* Shuffle songs using Fisher-Yates algorithm */
void shufflePlaylist(Playlist *p)
{
    Song *temp;
    Song **songs;
    Song *swap;

    int count = 0;
    int i, j;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    /* Count the songs */
    temp = p->head;

    while (temp != NULL)
    {
        count++;
        temp = temp->next;
    }

    if (count < 2)
    {
        printf("\nNot enough songs to shuffle!\n");
        return;
    }

    /* Allocate memory for song pointers */
    songs = malloc((size_t)count * sizeof(Song *));

    if (songs == NULL)
    {
        printf("\nMemory allocation failed!\n");
        return;
    }

    /* Store node addresses */
    temp = p->head;

    for (i = 0; i < count; i++)
    {
        songs[i] = temp;
        temp = temp->next;
    }

    /* Fisher-Yates shuffle */
    for (i = count - 1; i > 0; i--)
    {
        j = rand() % (i + 1);

        swap = songs[i];
        songs[i] = songs[j];
        songs[j] = swap;
    }

    /* Reconnect previous and next pointers */
    for (i = 0; i < count; i++)
    {
        songs[i]->prev = (i == 0) ? NULL : songs[i - 1];
        songs[i]->next = (i == count - 1) ? NULL : songs[i + 1];
    }

    /* Update list boundaries and reset selection to the new head.
       The playing pointer remains valid because nodes are not freed. */
    p->head = songs[0];
    p->tail = songs[count - 1];
    p->current = p->head;

    free(songs);

    printf("\nPlaylist shuffled successfully!\n");
}
