#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "playlist.h"

/* Case-insensitive substring search */
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

/* Search songs by title or artist */
void searchSong(Playlist *p)
{
    Song *temp;
    char keyword[100];
    int found = 0;

    if (p == NULL || p->head == NULL)
    {
        printf("\nPlaylist is empty!\n");
        return;
    }

    printf("\nEnter song title or artist to search: ");

    if (fgets(keyword, sizeof(keyword), stdin) == NULL)
        return;

    keyword[strcspn(keyword, "\n")] = '\0';

    if (strlen(keyword) == 0)
    {
        printf("\nSearch keyword cannot be empty!\n");
        return;
    }

    temp = p->head;

    while (temp != NULL)
    {
        if (containsIgnoreCase(temp->title, keyword) ||
            containsIgnoreCase(temp->artist, keyword))
        {
            printf("\nSong Found!\n");
            printf("Title  : %s\n", temp->title);
            printf("Artist : %s\n", temp->artist);
            found = 1;
        }

        temp = temp->next;
    }

    if (!found)
        printf("\nNo matching song found!\n");
}
