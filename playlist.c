#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

/* Function to add a song */
void addSong(Playlist *p)
{
    Song *newSong;

    if (p == NULL)
    {
        printf("\nInvalid playlist!\n");
        return;
    }

    newSong = (Song *)malloc(sizeof(Song));

    if (newSong == NULL)
    {
        printf("\nMemory allocation failed!\n");
        return;
    }

    printf("\nEnter song title: ");
    if (fgets(newSong->title, sizeof(newSong->title), stdin) == NULL)
    {
        free(newSong);
        return;
    }

    newSong->title[strcspn(newSong->title, "\n")] = '\0';

    printf("Enter artist name: ");
    if (fgets(newSong->artist, sizeof(newSong->artist), stdin) == NULL)
    {
        free(newSong);
        return;
    }

    newSong->artist[strcspn(newSong->artist, "\n")] = '\0';

    if (strlen(newSong->title) == 0 || strlen(newSong->artist) == 0)
    {
        printf("\nSong title and artist cannot be empty!\n");
        free(newSong);
        return;
    }

    newSong->prev = NULL;
    newSong->next = NULL;

    /* If playlist is empty */
    if (p->head == NULL)
    {
        p->head = newSong;
        p->tail = newSong;
        p->current = newSong;
    }
    else
    {
        /* Insert song at the end */
        newSong->prev = p->tail;
        p->tail->next = newSong;
        p->tail = newSong;
    }

    printf("\nSong added successfully!\n");
}

/* Function to remove a song */
void removeSong(Playlist *p)
{
    char songTitle[100];
    Song *temp;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    printf("\nEnter song title to remove: ");

    if (fgets(songTitle, sizeof(songTitle), stdin) == NULL)
        return;

    songTitle[strcspn(songTitle, "\n")] = '\0';

    temp = p->head;

    /* Search for the song */
    while (temp != NULL && strcmp(temp->title, songTitle) != 0)
        temp = temp->next;

    /* Song not found */
    if (temp == NULL)
    {
        printf("\nSong not found!\n");
        return;
    }

    /* Update pointers before freeing the node. */
    if (p->current == temp)
    {
        if (temp->next != NULL)
            p->current = temp->next;
        else
            p->current = temp->prev;
    }

    /* Never leave playing pointing to a node that is about to be freed. */
    if (p->playing == temp)
        p->playing = NULL;

    /* Update previous node */
    if (temp->prev != NULL)
        temp->prev->next = temp->next;
    else
        p->head = temp->next;

    /* Update next node */
    if (temp->next != NULL)
        temp->next->prev = temp->prev;
    else
        p->tail = temp->prev;

    free(temp);

    printf("\nSong removed successfully!\n");
}

/* Function to display the playlist */
void displayPlaylist(Playlist *p)
{
    Song *temp;
    int count = 1;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    temp = p->head;

    printf("\n========== MUSIC PLAYLIST ==========\n");

    while (temp != NULL)
    {
        printf("\n%d. Song Title : %s\n", count, temp->title);
        printf("   Artist     : %s\n", temp->artist);

        if (temp == p->current)
            printf("   [Now Selected]\n");

        if (temp == p->playing)
            printf("   [Now Playing - Visual]\n");

        temp = temp->next;
        count++;
    }

    printf("\nTotal Songs: %d\n", count - 1);
    printf("====================================\n");
}
