#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "playlist.h"

#define INPUT_SIZE 64

/* ANSI terminal colors */
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define CYAN    "\033[36m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define RED     "\033[31m"
#define WHITE   "\033[97m"

/* Read and validate a menu choice without leaving characters in stdin. */
static int readMenuChoice(int *choice)
{
    char input[INPUT_SIZE];
    char *end;
    long value;
    int ch;

    if (fgets(input, sizeof(input), stdin) == NULL)
        return 0;

    if (strchr(input, '\n') == NULL)
    {
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
    }

    value = strtol(input, &end, 10);

    if (end == input)
        return -1;

    while (*end != '\0' && isspace((unsigned char)*end))
        end++;

    if (*end != '\0' || value < 0 || value > 10)
        return -1;

    *choice = (int)value;
    return 1;
}

/* Count nodes in the actual doubly linked list. */
static int songCount(const Playlist *p)
{
    const Song *temp;
    int count = 0;

    for (temp = p->head; temp != NULL; temp = temp->next)
        count++;

    return count;
}

/* Find a song's 1-based position in the playlist. */
static int songPosition(const Playlist *p, const Song *song)
{
    const Song *temp;
    int position = 1;

    if (song == NULL)
        return 0;

    for (temp = p->head; temp != NULL; temp = temp->next, position++)
    {
        if (temp == song)
            return position;
    }

    return 0;
}

static void clearScreen(void)
{
    printf("\033[2J\033[H");
}

static void drawLine(void)
{
    printf(CYAN "  +--------------------------------------------------------------------------+\n" RESET);
}

static void pauseScreen(void)
{
    int ch;

    printf("\n" DIM "  Press ENTER to return to the dashboard..." RESET);
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}

/* Dashboard shows selection separately from the visual playback state. */
static void showDashboard(const Playlist *p)
{
    int total = songCount(p);
    int selectedPosition = songPosition(p, p->current);
    int playingPosition = songPosition(p, p->playing);

    clearScreen();
    printf("\n");
    drawLine();
    printf(CYAN "  |" RESET BOLD WHITE "                           MUSIC PLAYLIST MANAGER                         " RESET CYAN "|\n" RESET);
    printf(CYAN "  |" RESET DIM "                         DOUBLY LINKED LIST  /  C PROJECT                 " RESET CYAN "|\n" RESET);
    drawLine();
    printf("\n");

    printf(MAGENTA BOLD "  NOW SELECTED\n" RESET);
    if (p->current != NULL)
    {
        printf("  " WHITE BOLD "%-48.48s" RESET DIM "Track %d of %d" RESET "\n",
               p->current->title, selectedPosition, total);
        printf("  " CYAN "%-48.48s" RESET "\n", p->current->artist);
        printf("  " GREEN "Ready for simulated playback" RESET "\n\n");
    }
    else
    {
        printf("  " YELLOW "No song selected — add or load songs to begin." RESET "\n\n");
    }

    printf(BLUE BOLD "  NOW PLAYING\n" RESET);
    if (p->playing != NULL && playingPosition != 0)
    {
        printf("  " WHITE BOLD "%-48.48s" RESET DIM "Track %d of %d" RESET "\n",
               p->playing->title, playingPosition, total);
        printf("  " CYAN "%-48.48s" RESET "\n", p->playing->artist);
        printf("  " GREEN "Visual playback started" RESET "\n\n");
    }
    else
    {
        printf("  " YELLOW "Nothing playing — press 4 to play selected song." RESET "\n\n");
    }

    printf(BLUE BOLD "  LIBRARY OVERVIEW\n" RESET);
    printf("  " WHITE "♪  Total tracks" RESET "          : %d\n", total);
    printf("  " WHITE "♫  Current position" RESET "      : %s",
           selectedPosition ? "" : "Not selected\n");
    if (selectedPosition)
        printf("%d / %d\n", selectedPosition, total);
    printf("  " WHITE "▣  Playlist structure" RESET "    : Doubly Linked List\n\n");

    drawLine();
    printf(BOLD WHITE "  MAIN MENU\n" RESET);
    printf("  " CYAN "[1]" RESET "  + Add Song              " CYAN "[2]" RESET "  - Remove Song\n");
    printf("  " CYAN "[3]" RESET "  ☷ Display Playlist      " CYAN "[4]" RESET "  ▶ Play Selected Song\n");
    printf("  " CYAN "[5]" RESET "  ⏭ Next Song             " CYAN "[6]" RESET "  ⏮ Previous Song\n");
    printf("  " CYAN "[7]" RESET "  Search Song             " CYAN "[8]" RESET "  ↻ Shuffle Playlist\n");
    printf("  " CYAN "[9]" RESET "  ▣ Save Playlist         " CYAN "[10]" RESET " ⇩ Load Playlist\n");
    printf("  " RED "[0]" RESET "  Exit\n");
    drawLine();
    printf("\n  " BOLD "Choose an option: " RESET);
}

/* Free all songs before the program exits. */
static void clearPlaylist(Playlist *p)
{
    Song *temp = p->head;
    Song *next;

    while (temp != NULL)
    {
        next = temp->next;
        free(temp);
        temp = next;
    }

    p->head = NULL;
    p->tail = NULL;
    p->current = NULL;
    p->playing = NULL;
}

int main(void)
{
    Playlist playlist = {NULL, NULL, NULL, NULL};
    int choice = -1;
    int inputStatus;

    srand((unsigned int)time(NULL));

    for (;;)
    {
        showDashboard(&playlist);
        inputStatus = readMenuChoice(&choice);

        if (inputStatus == 0)
        {
            printf("\n\nInput ended. Exiting program.\n");
            break;
        }

        if (inputStatus < 0)
        {
            printf("\n  " RED "Invalid choice! Enter a number from 0 to 10." RESET);
            pauseScreen();
            continue;
        }

        if (choice == 0)
        {
            clearScreen();
            printf("\n  " CYAN BOLD "Thanks for using Music Playlist Manager!" RESET "\n\n");
            break;
        }

        switch (choice)
        {
            case 1:  addSong(&playlist);         break;
            case 2:  removeSong(&playlist);      break;
            case 3:  displayPlaylist(&playlist); break;
            case 4:  playSong(&playlist);        break;
            case 5:  nextSong(&playlist);        break;
            case 6:  previousSong(&playlist);    break;
            case 7:  searchSong(&playlist);      break;
            case 8:  shufflePlaylist(&playlist); break;
            case 9:  savePlaylist(&playlist);    break;
            case 10: loadPlaylist(&playlist);    break;
        }

        pauseScreen();
    }

    clearPlaylist(&playlist);
    return 0;
}
