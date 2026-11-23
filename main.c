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

    if (*end != '\0' || value < 0 || value > 13)
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

/* Find the current song's 1-based position. */
static int currentPosition(const Playlist *p)
{
    const Song *temp;
    int position = 1;

    if (p->current == NULL)
        return 0;

    for (temp = p->head; temp != NULL; temp = temp->next, position++)
    {
        if (temp == p->current)
            return position;
    }

    return 0;
}

/* Find the 1-based position of any song node. */
static int songPosition(const Playlist *p, const Song *target)
{
    const Song *temp;
    int position = 1;

    if (target == NULL)
        return 0;

    for (temp = p->head; temp != NULL; temp = temp->next, position++)
    {
        if (temp == target)
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

/* Determine the queue preview from the active playback node and repeat mode. */
static const Song *nextInQueue(const Playlist *p)
{
    if (p->playing == NULL)
        return NULL;

    if (p->repeatMode == REPEAT_ONE)
        return p->playing;

    if (p->playing->next != NULL)
        return p->playing->next;

    if (p->repeatMode == REPEAT_ALL)
        return p->head;

    return NULL;
}

static const char *repeatModeName(RepeatMode mode)
{
    switch (mode)
    {
        case REPEAT_ALL: return "REPEAT ALL";
        case REPEAT_ONE: return "REPEAT ONE";
        default: return "OFF";
    }
}

static void chooseRepeatMode(Playlist *p)
{
    char input[INPUT_SIZE];
    int ch;
    int mode;

    printf("\nREPEAT MODE\n");
    printf("[1] OFF\n[2] REPEAT ALL\n[3] REPEAT ONE\n[0] Cancel\n");
    printf("Current mode: %s\nChoose mode: ", repeatModeName(p->repeatMode));

    if (fgets(input, sizeof(input), stdin) == NULL)
        return;
    if (strchr(input, '\n') == NULL)
        while ((ch = getchar()) != '\n' && ch != EOF) ;

    if (sscanf(input, "%d", &mode) != 1 || mode < 0 || mode > 3)
    {
        printf("Invalid repeat mode.\n");
        return;
    }

    if (mode == 0)
    {
        printf("Repeat mode unchanged.\n");
        return;
    }

    p->repeatMode = (RepeatMode)(mode - 1);
    printf("Repeat mode set to %s.\n", repeatModeName(p->repeatMode));
}

/* Attractive dashboard backed by the real playlist data. */
static void showDashboard(const Playlist *p)
{
    int total = songCount(p);
    int position = currentPosition(p);
    const Song *queued = nextInQueue(p);

    clearScreen();
    printf("\n");
    drawLine();
    printf(CYAN "  |" RESET BOLD WHITE "                           MUSIC PLAYLIST MANAGER                         " RESET CYAN "|\n" RESET);
    printf(CYAN "  |" RESET DIM "                         DOUBLY LINKED LIST  /  C PROJECT                 " RESET CYAN "|\n" RESET);
    drawLine();
    printf("\n");

    /* Three fixed-width columns keep titles and status rows aligned. */
    printf("  " MAGENTA BOLD "%-25s" GREEN BOLD "%-25s" YELLOW BOLD "%-25s" RESET "\n",
           "NOW SELECTED", "NOW PLAYING", "NEXT IN QUEUE");

    printf("  " WHITE BOLD "%-25.25s" WHITE BOLD "%-25.25s" WHITE BOLD "%-25.25s" RESET "\n",
           p->current ? p->current->title : "No song selected",
           p->playing ? p->playing->title : "Nothing playing",
           queued ? queued->title : (p->playing ? "End of playlist" : "Press [4] to start"));

    printf("  " CYAN "%-25.25s" CYAN "%-25.25s" CYAN "%-25.25s" RESET "\n",
           p->current ? p->current->artist : "Add or load songs",
           p->playing ? p->playing->artist : "",
           queued ? queued->artist : "");

    {
        char selectedStatus[64];
        char playingStatus[64];
        char queueStatus[64];

        if (p->current != NULL)
            snprintf(selectedStatus, sizeof(selectedStatus), "Track %d of %d", position, total);
        else
            snprintf(selectedStatus, sizeof(selectedStatus), "Not selected");

        if (p->playing != NULL)
            snprintf(playingStatus, sizeof(playingStatus), "Playing - Track %d of %d",
                     songPosition(p, p->playing), total);
        else
            snprintf(playingStatus, sizeof(playingStatus), "Playback stopped");

        if (queued != NULL)
            snprintf(queueStatus, sizeof(queueStatus), "Track %d of %d",
                     songPosition(p, queued), total);
        else
            snprintf(queueStatus, sizeof(queueStatus), "%s",
                     p->playing != NULL ? "Last track" : "");

        printf("  " MAGENTA "%-25.25s" GREEN "%-25.25s" YELLOW "%-25.25s" RESET "\n",
               selectedStatus, playingStatus, queueStatus);
    }
    printf("  " YELLOW "Repeat mode: %s" RESET "\n\n", repeatModeName(p->repeatMode));

    printf(BLUE BOLD "  LIBRARY OVERVIEW\n" RESET);
    printf("  " WHITE "♪  Total tracks" RESET "          : %d\n", total);
    printf("  " WHITE "♫  Current position" RESET "      : %s",
           position ? "" : "Not selected\n");
    if (position)
        printf("%d / %d\n", position, total);
    printf("  " WHITE "▣  Playlist structure" RESET "    : Doubly Linked List\n\n");

    drawLine();
    printf(BOLD WHITE "  MAIN MENU\n" RESET);
    printf("  " CYAN "[1]" RESET "  + Add Song              " CYAN "[2]" RESET "  - Remove Song\n");
    printf("  " CYAN "[3]" RESET "  ☷ Display Playlist      " CYAN "[4]" RESET "  ▶ Play Selected Song\n");
    printf("  " CYAN "[5]" RESET "  ⏭ Next Song             " CYAN "[6]" RESET "  ⏮ Previous Song\n");
    printf("  " CYAN "[7]" RESET "  Search Song             " CYAN "[8]" RESET "  ↻ Shuffle Playlist\n");
    printf("  " CYAN "[9]" RESET "  ▣ Save Playlist         " CYAN "[10]" RESET " ⇩ Load Playlist\n");
    printf("  " CYAN "[11]" RESET " Repeat Mode           " CYAN "[12]" RESET " Play from Beginning\n");
    printf("  " CYAN "[13]" RESET " Play from Ending\n");
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
    Playlist playlist = {NULL, NULL, NULL, NULL, REPEAT_OFF, "", 0};
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
            printf("\n  " RED "Invalid choice! Enter a number from 0 to 13." RESET);
            pauseScreen();
            continue;
        }

        if (choice == 0)
        {
            clearScreen();
            printf("\n  " CYAN BOLD "Thanks for using Music Playlist Manager!" RESET "\n\n");
            break;
        }

        if (choice == 3)
        {
            /* The playlist browser manages its own screen and return action. */
            displayPlaylist(&playlist);
            continue;
        }

        switch (choice)
        {
            case 1:  addSong(&playlist);         break;
            case 2:  removeSong(&playlist);      break;
            case 4:  playSong(&playlist);        break;
            case 5:  nextSong(&playlist);        break;
            case 6:  previousSong(&playlist);    break;
            case 7:  searchSong(&playlist);      break;
            case 8:  shufflePlaylist(&playlist); break;
            case 9:  savePlaylist(&playlist);    break;
            case 10: loadPlaylist(&playlist);    break;
            case 11: chooseRepeatMode(&playlist); break;
            case 12: playFromBeginning(&playlist); break;
            case 13: playFromEnding(&playlist);    break;
        }

        pauseScreen();
    }

    clearPlaylist(&playlist);
    return 0;
}
