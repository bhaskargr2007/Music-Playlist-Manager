#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "playlist.h"

/* Case-insensitive substring search. */
static int containsIgnoreCase(const char *text, const char *keyword)
{
    int i, j;

    for (i = 0; text[i] != '\0'; i++)
    {
        for (j = 0; keyword[j] != '\0'; j++)
        {
            if (text[i + j] == '\0' ||
                tolower((unsigned char)text[i + j]) !=
                tolower((unsigned char)keyword[j]))
                break;
        }

        if (keyword[j] == '\0')
            return 1;
    }

    return 0;
}

/* Search by title or artist, then make the chosen result NOW SELECTED. */
void searchSong(Playlist *p)
{
    Song *temp;
    Song *selected = NULL;
    char keyword[100];
    char input[32];
    char *end;
    long choice;
    int matches = 0;
    int ch;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    printf("\nEnter song title or artist to search: ");
    if (fgets(keyword, sizeof(keyword), stdin) == NULL)
        return;

    keyword[strcspn(keyword, "\n")] = '\0';

    if (keyword[0] == '\0')
    {
        printf("\nSearch keyword cannot be empty!\n");
        return;
    }

    /* First pass: count matching songs. */
    for (temp = p->head; temp != NULL; temp = temp->next)
    {
        if (containsIgnoreCase(temp->title, keyword) ||
            containsIgnoreCase(temp->artist, keyword))
            matches++;
    }

    if (matches == 0)
    {
        printf("\nNo matching song found!\n");
        return;
    }

    printf("\nSearch results for \"%s\":\n", keyword);

    /* Second pass: display the matching songs with result numbers. */
    {
        int resultNumber = 0;

        for (temp = p->head; temp != NULL; temp = temp->next)
        {
            if (containsIgnoreCase(temp->title, keyword) ||
                containsIgnoreCase(temp->artist, keyword))
            {
                resultNumber++;
                printf("  [%d] %s - %s", resultNumber, temp->title, temp->artist);
                if (temp == p->current)
                    printf("  [CURRENTLY SELECTED]");
                printf("\n");

                if (matches == 1)
                    selected = temp;
            }
        }
    }

    /* A single result is selected automatically. */
    if (matches == 1)
    {
        p->current = selected;
        printf("\nSelected automatically: %s - %s\n", selected->title, selected->artist);
        printf("Now Playing remains unchanged. Press [4] to play this selection.\n");
        return;
    }

    /* Multiple results: let the user choose which result to select. */
    printf("\nEnter the result number to select (0 to cancel): ");
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

    if (end == input || *end != '\0' || choice < 0 || choice > matches)
    {
        printf("\nInvalid result number. No change to selection.\n");
        return;
    }

    if (choice == 0)
    {
        printf("\nSearch cancelled. Current selection unchanged.\n");
        return;
    }

    {
        int resultNumber = 0;

        for (temp = p->head; temp != NULL; temp = temp->next)
        {
            if (containsIgnoreCase(temp->title, keyword) ||
                containsIgnoreCase(temp->artist, keyword))
            {
                resultNumber++;
                if (resultNumber == choice)
                {
                    selected = temp;
                    break;
                }
            }
        }
    }

    if (selected != NULL)
    {
        p->current = selected;
        printf("\nSelected: %s - %s\n", selected->title, selected->artist);
        printf("Now Playing remains unchanged. Press [4] to play this selection.\n");
    }
}
