#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "playlist.h"

/* Read one complete line and discard excess characters safely. */
static int readLine(char *buffer, size_t size)
{
    int ch;

    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    if (strchr(buffer, '\n') == NULL)
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }

    buffer[strcspn(buffer, "\r\n")] = '\0';
    return 1;
}

/* Remove leading and trailing whitespace from user-entered text. */
static void trimText(char *text)
{
    char *start = text;
    size_t length;

    while (isspace((unsigned char)*start))
        start++;

    if (start != text)
        memmove(text, start, strlen(start) + 1);

    length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1]))
        text[--length] = '\0';
}

/* Case-insensitive comparison; titles and artists are compared separately. */
static int sameText(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0')
    {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

/* Find a song using both title and artist. */
static Song *findSong(const Playlist *p, const char *title, const char *artist)
{
    Song *temp;

    for (temp = p->head; temp != NULL; temp = temp->next)
    {
        if (sameText(temp->title, title) && sameText(temp->artist, artist))
            return temp;
    }

    return NULL;
}

/* Remove a specific node while preserving selected/playing pointer safety. */
static void unlinkAndFreeSong(Playlist *p, Song *song)
{
    if (song == NULL)
        return;

    if (p->current == song)
        p->current = (song->next != NULL) ? song->next : song->prev;

    if (p->playing == song)
        p->playing = NULL;

    if (song->prev != NULL)
        song->prev->next = song->next;
    else
        p->head = song->next;

    if (song->next != NULL)
        song->next->prev = song->prev;
    else
        p->tail = song->prev;

    free(song);
    p->dirty = 1;
}

/* Ask for confirmation before removing the supplied song node. */
static int confirmRemoval(const Song *song)
{
    char confirmation[16];

    printf("\nSong:   %s\nArtist: %s\n", song->title, song->artist);
    printf("Are you sure you want to remove this song? (y/n): ");

    if (!readLine(confirmation, sizeof(confirmation)))
        return 0;

    return confirmation[0] == 'y' || confirmation[0] == 'Y';
}

/* Function to add a song. Duplicate title + artist pairs are not allowed. */
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
    if (!readLine(newSong->title, sizeof(newSong->title)))
    {
        free(newSong);
        return;
    }

    printf("Enter artist name: ");
    if (!readLine(newSong->artist, sizeof(newSong->artist)))
    {
        free(newSong);
        return;
    }

    trimText(newSong->title);
    trimText(newSong->artist);

    if (newSong->title[0] == '\0' || newSong->artist[0] == '\0')
    {
        printf("\nSong title and artist cannot be empty!\n");
        free(newSong);
        return;
    }

    if (findSong(p, newSong->title, newSong->artist) != NULL)
    {
        printf("\n\033[31mThis song by this artist already exists in the current playlist!\033[0m\n");
        free(newSong);
        return;
    }

    newSong->prev = NULL;
    newSong->next = NULL;

    if (p->head == NULL)
    {
        p->head = newSong;
        p->tail = newSong;
        p->current = newSong;
    }
    else
    {
        newSong->prev = p->tail;
        p->tail->next = newSong;
        p->tail = newSong;
    }

    p->dirty = 1;
    printf("\n\033[32mSong added successfully!\033[0m\n");
}

/* Option 2: three ways to remove a song from the current playlist. */
void removeSong(Playlist *p)
{
    char input[32];
    char title[100];
    char artist[100];
    char *end;
    long choice;
    long trackNumber;
    long index;
    Song *temp;
    Song *target;
    int count;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    printf("\n\033[35m========== REMOVE SONG ==========\033[0m\n");
    printf("1. Remove Selected Song\n");
    printf("2. Choose Song from Playlist\n");
    printf("3. Remove by Song Name and Artist\n");
    printf("0. Return\n");
    printf("\nEnter your choice: ");

    if (!readLine(input, sizeof(input)))
        return;

    choice = strtol(input, &end, 10);
    while (isspace((unsigned char)*end))
        end++;

    if (end == input || *end != '\0' || choice < 0 || choice > 3)
    {
        printf("\nInvalid choice!\n");
        return;
    }

    if (choice == 0)
    {
        printf("\nReturning without changes.\n");
        return;
    }

    target = NULL;

    if (choice == 1)
    {
        target = p->current;
        if (target == NULL)
        {
            printf("\nNo song is currently selected. Select a song first.\n");
            return;
        }
        printf("\n\033[35mREMOVE SELECTED SONG\033[0m");
    }
    else if (choice == 2)
    {
        printf("\n\033[35mCHOOSE A SONG TO REMOVE\033[0m\n");
        count = 0;
        for (temp = p->head; temp != NULL; temp = temp->next)
        {
            count++;
            printf("%d. %s - %s", count, temp->title, temp->artist);
            if (temp == p->current)
                printf("  [NOW SELECTED]");
            if (temp == p->playing)
                printf("  [NOW PLAYING]");
            printf("\n");
        }

        printf("\nEnter track number (0 to cancel): ");
        if (!readLine(input, sizeof(input)))
            return;

        trackNumber = strtol(input, &end, 10);
        while (isspace((unsigned char)*end))
            end++;

        if (end == input || *end != '\0' || trackNumber < 0 || trackNumber > count)
        {
            printf("\nInvalid track number!\n");
            return;
        }
        if (trackNumber == 0)
        {
            printf("\nRemoval cancelled.\n");
            return;
        }

        target = p->head;
        for (index = 1; index < trackNumber; index++)
            target = target->next;
    }
    else
    {
        printf("\nEnter song title: ");
        if (!readLine(title, sizeof(title)))
            return;
        printf("Enter artist name: ");
        if (!readLine(artist, sizeof(artist)))
            return;

        trimText(title);
        trimText(artist);

        if (title[0] == '\0' || artist[0] == '\0')
        {
            printf("\nSong title and artist cannot be empty!\n");
            return;
        }

        target = findSong(p, title, artist);
        if (target == NULL)
        {
            printf("\n\033[31mSong not found. Check both the song title and artist.\033[0m\n");
            return;
        }

        printf("\n\033[35mREMOVE SONG BY NAME AND ARTIST\033[0m");
    }

    if (!confirmRemoval(target))
    {
        printf("\nRemoval cancelled.\n");
        return;
    }

    unlinkAndFreeSong(p, target);

    printf("\n\033[32mSong removed successfully!\033[0m\n");
    if (p->current != NULL)
        printf("NOW SELECTED: %s - %s\n", p->current->title, p->current->artist);
    else
        printf("NOW SELECTED: None (playlist is empty)\n");

    if (p->playing != NULL)
        printf("NOW PLAYING: %s - %s\n", p->playing->title, p->playing->artist);
    else
        printf("NOW PLAYING: Nothing playing\n");
}

/* Interactive playlist browser: choose tracks repeatedly, or enter 0 to return. */
void displayPlaylist(Playlist *p)
{
    char input[32];
    char *end;
    long choice;
    int count;
    int ch;
    Song *temp;
    Song *selected;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty! Add or load songs first.\n");
        printf("Press ENTER to return...");
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
        return;
    }

    for (;;)
    {
        count = 0;
        temp = p->head;

        printf("\033[2J\033[H");
        printf("\033[36m+--------------------------------------------------------------------------+\033[0m\n");
        printf("\033[1m                         MUSIC PLAYLIST - SELECT A SONG\033[0m\n");
        printf("\033[36m+--------------------------------------------------------------------------+\033[0m\n\n");
        printf("Enter a track number to select it. Enter 0 to return to the dashboard.\n\n");

        while (temp != NULL)
        {
            count++;
            if (temp == p->current)
                printf("\033[35m> ");
            else
                printf("  ");

            printf("%d. %-32s - %s", count, temp->title, temp->artist);

            if (temp == p->current)
                printf("  \033[35m[ NOW SELECTED ]\033[0m");
            if (temp == p->playing)
                printf("  \033[32m[ NOW PLAYING ]\033[0m");
            printf("\n");
            temp = temp->next;
        }

        printf("\nTotal Songs: %d\n", count);
        printf("\033[36m--------------------------------------------------------------------------\033[0m\n");
        printf("\nSelected: ");
        if (p->current != NULL)
            printf("%s - %s\n", p->current->title, p->current->artist);
        else
            printf("None\n");
        printf("Now Playing: ");
        if (p->playing != NULL)
            printf("%s - %s\n", p->playing->title, p->playing->artist);
        else
            printf("Nothing playing\n");

        printf("\nChoose track number (0 to return): ");
        if (fgets(input, sizeof(input), stdin) == NULL)
            return;

        if (strchr(input, '\n') == NULL)
        {
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
        }

        choice = strtol(input, &end, 10);
        while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')
            end++;

        if (end == input || *end != '\0' || choice < 0 || choice > count)
        {
            printf("\nInvalid track number. Press ENTER to try again...");
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;
            continue;
        }

        if (choice == 0)
            return;

        selected = p->head;
        for (long i = 1; i < choice; i++)
            selected = selected->next;

        p->current = selected;
        printf("\n\033[32mSelected: %s - %s\033[0m\n", selected->title, selected->artist);
        printf("Now Playing remains unchanged. Press [4] on the dashboard to play this selection.\n");
    }
}
