#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "playlist.h"

#define SAVE_HEADER "# MUSIC_PLAYLIST_MANAGER_V1"

/* Read one complete line. Returns 1=line, 0=EOF, -1=too long/error. */
static int readLine(FILE *file, char *buffer, size_t size)
{
    size_t len;
    int ch;

    if (fgets(buffer, (int)size, file) == NULL)
        return feof(file) ? 0 : -1;

    len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[--len] = '\0';
    else if (!feof(file))
    {
        while ((ch = fgetc(file)) != '\n' && ch != EOF)
            ;
        return -1;
    }

    if (len > 0 && buffer[len - 1] == '\r')
        buffer[--len] = '\0';

    return 1;
}

static void freeSongs(Playlist *p)
{
    Song *node = p->head;
    while (node != NULL)
    {
        Song *next = node->next;
        free(node);
        node = next;
    }
    p->head = p->tail = p->current = p->playing = NULL;
}

static int appendSong(Playlist *p, const char *title, const char *artist)
{
    Song *node;

    if (title[0] == '\0' || artist[0] == '\0' ||
        strlen(title) >= sizeof(((Song *)0)->title) ||
        strlen(artist) >= sizeof(((Song *)0)->artist))
        return 0;

    node = malloc(sizeof(*node));
    if (node == NULL)
        return 0;

    strcpy(node->title, title);
    strcpy(node->artist, artist);
    node->prev = p->tail;
    node->next = NULL;

    if (p->tail != NULL)
        p->tail->next = node;
    else
        p->head = node;

    p->tail = node;
    if (p->current == NULL)
        p->current = node;
    return 1;
}

/* Multiple named playlists are stored as Playlists/<name>.txt. */
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#define PLAYLIST_DIR "Playlists"

static int makePlaylistPath(const char *name, char *path, size_t pathSize)
{
    char safe[100];
    size_t i, j = 0;
    if (name == NULL || name[0] == '\0') return 0;
    for (i = 0; name[i] != '\0' && j < sizeof(safe) - 1; i++)
    {
        unsigned char c = (unsigned char)name[i];
        if (isalnum(c) || c == '_' || c == '-') safe[j++] = (char)c;
        else if (c == ' ' || c == '.') safe[j++] = '_';
        else return 0;
    }
    safe[j] = '\0';
    if (j == 0 || strcmp(safe, ".") == 0 || strcmp(safe, "..") == 0) return 0;
    if (snprintf(path, pathSize, "%s/%s.txt", PLAYLIST_DIR, safe) >= (int)pathSize) return 0;
    return 1;
}

static int ensurePlaylistDirectory(void)
{
    struct stat info;
    if (mkdir(PLAYLIST_DIR, 0755) != 0 && errno != EEXIST)
    {
        printf("\nUnable to create %s directory.\n", PLAYLIST_DIR);
        return 0;
    }
    /* Preserve and migrate the project's original fixed playlist.txt once. */
    if (stat("playlist.txt", &info) == 0 && stat(PLAYLIST_DIR "/Imported_Playlist.txt", &info) != 0)
    {
        if (rename("playlist.txt", PLAYLIST_DIR "/Imported_Playlist.txt") == 0)
            printf("\nExisting playlist.txt migrated to %s/Imported_Playlist.txt.\n", PLAYLIST_DIR);
    }
    return 1;
}

static void readName(const char *prompt, char *name, size_t size)
{
    printf("%s", prompt);
    if (fgets(name, (int)size, stdin) == NULL) { name[0] = '\0'; return; }
    name[strcspn(name, "\r\n")] = '\0';
}

static int askYesNo(const char *prompt)
{
    char answer[16];
    readName(prompt, answer, sizeof(answer));
    return answer[0] == 'y' || answer[0] == 'Y';
}

static int writeNamedPlaylist(Playlist *p, const char *name, int overwrite)
{
    FILE *file;
    Song *node;
    char path[160];
    const char *base;
    size_t len;

    if (name[0] == '\0' || !makePlaylistPath(name, path, sizeof(path)))
    {
        printf("Invalid name. Use letters, numbers, spaces, _ or -.\n");
        return 0;
    }

    file = fopen(path, "r");
    if (file != NULL)
    {
        fclose(file);
        if (!overwrite)
        {
            printf("A playlist with that name already exists. Choose another name.\n");
            return 0;
        }
        if (!askYesNo("This will replace the existing playlist. Continue? (y/n): "))
        {
            printf("Save cancelled.\n");
            return 0;
        }
    }

    file = fopen(path, "w");
    if (file == NULL)
    {
        printf("Unable to save to %s.\n", path);
        return 0;
    }

    if (fprintf(file, "%s\n", SAVE_HEADER) < 0)
    {
        fclose(file);
        printf("Write error.\n");
        return 0;
    }

    for (node = p->head; node != NULL; node = node->next)
    {
        if (fprintf(file, "%s\n%s\n", node->title, node->artist) < 0)
        {
            fclose(file);
            printf("Write error.\n");
            return 0;
        }
    }

    if (fclose(file) != 0)
    {
        printf("Error closing playlist file.\n");
        return 0;
    }

    base = strrchr(path, '/');
    base = base ? base + 1 : path;
    len = strlen(base);
    if (len > 4) len -= 4; else len = 0;
    if (len >= sizeof(p->currentPlaylistName))
        len = sizeof(p->currentPlaylistName) - 1;
    memcpy(p->currentPlaylistName, base, len);
    p->currentPlaylistName[len] = '\0';
    p->dirty = 0;

    printf("\nPlaylist saved successfully: %s\n", path);
    return 1;
}

void savePlaylist(Playlist *p)
{
    char input[32];
    char name[100];
    int choice, ch;
    char path[160];

    if (p == NULL) { printf("\nInvalid playlist!\n"); return; }
    if (p->head == NULL) { printf("\nNothing to save: playlist is empty.\n"); return; }
    if (!ensurePlaylistDirectory()) return;

    printf("\nSAVE PLAYLIST\n");
    printf("[1] Create New Playlist\n");
    printf("[2] Update Current Playlist\n");
    printf("[3] Save As (Different Name)\n");
    printf("[0] Return\n");
    if (p->currentPlaylistName[0] != '\0')
        printf("Current playlist: %s\n", p->currentPlaylistName);
    else
        printf("Current playlist: (not saved yet)\n");
    printf("Choose option: ");

    if (fgets(input, sizeof(input), stdin) == NULL) return;
    if (strchr(input, '\n') == NULL)
        while ((ch = getchar()) != '\n' && ch != EOF) ;

    if (sscanf(input, "%d", &choice) != 1 || choice < 0 || choice > 3)
    {
        printf("Invalid choice.\n");
        return;
    }
    if (choice == 0) { printf("Save menu closed.\n"); return; }

    if (choice == 1)
    {
        readName("Enter new playlist name: ", name, sizeof(name));
        if (name[0] == '\0' || !makePlaylistPath(name, path, sizeof(path)))
        {
            printf("Invalid playlist name.\n");
            return;
        }
        if (writeNamedPlaylist(p, name, 0))
            printf("New playlist created.\n");
        return;
    }

    if (choice == 2)
    {
        if (p->currentPlaylistName[0] == '\0')
        {
            printf("This playlist has no saved name yet. Choose [1] Create New Playlist or [3] Save As.\n");
            return;
        }
        (void)writeNamedPlaylist(p, p->currentPlaylistName, 1);
        return;
    }

    readName("Enter new playlist name: ", name, sizeof(name));
    if (name[0] == '\0' || !makePlaylistPath(name, path, sizeof(path)))
    {
        printf("Invalid playlist name.\n");
        return;
    }
    (void)writeNamedPlaylist(p, name, 0);
}

void loadPlaylist(Playlist *p)
{
    DIR *dir;
    struct dirent *entry;
    char names[200][100];
    int count = 0, choice, i;
    char input[32], path[160];
    FILE *file;
    Playlist loaded = {NULL, NULL, NULL, NULL, REPEAT_OFF, "", 0};
    char first[256];
    int status, songs = 0, lineFormat = 0, headerFound = 0;

    if (p == NULL) { printf("\nInvalid playlist!\n"); return; }
    if (!ensurePlaylistDirectory()) return;
    dir = opendir(PLAYLIST_DIR);
    if (dir == NULL) { printf("Unable to open playlist directory.\n"); return; }
    while ((entry = readdir(dir)) != NULL && count < 200)
    {
        size_t len = strlen(entry->d_name);
        if (len > 4 && strcmp(entry->d_name + len - 4, ".txt") == 0)
        {
            size_t stem = len - 4;
            if (stem >= sizeof(names[0])) continue;
            memcpy(names[count], entry->d_name, stem);
            names[count][stem] = '\0';
            count++;
        }
    }
    closedir(dir);
    if (count == 0) { printf("\nNo saved playlists found in %s/.\n", PLAYLIST_DIR); return; }
    printf("\nAVAILABLE PLAYLISTS\n");
    for (i = 0; i < count; i++) printf("[%d] %s\n", i + 1, names[i]);
    printf("[0] Cancel\nChoose playlist: ");
    if (fgets(input, sizeof(input), stdin) == NULL || sscanf(input, "%d", &choice) != 1 || choice < 0 || choice > count)
    { printf("Invalid choice.\n"); return; }
    if (choice == 0) { printf("Load cancelled.\n"); return; }
    if (p->dirty && p->head != NULL)
    {
        if (askYesNo("Current playlist has unsaved changes. Save before loading? (y/n): "))
            savePlaylist(p);
    }
    if (snprintf(path, sizeof(path), "%s/%s.txt", PLAYLIST_DIR, names[choice - 1]) >= (int)sizeof(path)) return;
    file = fopen(path, "r");
    if (file == NULL) { printf("Unable to open %s.\n", path); return; }
    status = readLine(file, first, sizeof(first));
    if (status != 1) { printf("Playlist file is empty or invalid.\n"); fclose(file); return; }
    if (strcmp(first, SAVE_HEADER) == 0) headerFound = 1;
    else if (strstr(first, " - ") != NULL) lineFormat = 1;
    if (lineFormat)
    {
        char line[256], *separator, *scan, *last;
        strcpy(line, first);
        for (;;)
        {
            separator = strstr(line, " - ");
            if (!separator) { printf("Invalid song line: %s\n", line); freeSongs(&loaded); fclose(file); return; }
            last = separator;
            scan = separator;
            while ((scan = strstr(scan + 3, " - ")) != NULL) last = scan;
            separator = last; *separator = '\0';
            if (!appendSong(&loaded, line, separator + 3)) { freeSongs(&loaded); fclose(file); printf("Invalid song or memory error.\n"); return; }
            songs++;
            status = readLine(file, line, sizeof(line));
            if (status == 0) break;
            if (status < 0) { freeSongs(&loaded); fclose(file); printf("Line too long.\n"); return; }
        }
    }
    else
    {
        char title[256], artist[256];
        if (headerFound) { status = readLine(file, title, sizeof(title)); if (status != 1) title[0] = '\0'; }
        else strcpy(title, first);
        while (title[0] != '\0')
        {
            if (readLine(file, artist, sizeof(artist)) != 1 || !appendSong(&loaded, title, artist))
            { freeSongs(&loaded); fclose(file); printf("Invalid title/artist pair. Load cancelled.\n"); return; }
            songs++;
            status = readLine(file, title, sizeof(title));
            if (status == 0) break;
            if (status < 0) { freeSongs(&loaded); fclose(file); printf("Line too long.\n"); return; }
        }
    }
    fclose(file);
    if (songs == 0) { freeSongs(&loaded); printf("Playlist contains no songs.\n"); return; }
    freeSongs(p);
    p->head = loaded.head; p->tail = loaded.tail; p->current = loaded.current; p->playing = NULL;
    p->repeatMode = REPEAT_OFF;
    strncpy(p->currentPlaylistName, names[choice - 1], sizeof(p->currentPlaylistName) - 1);
    p->currentPlaylistName[sizeof(p->currentPlaylistName) - 1] = '\0';
    p->dirty = 0;
    printf("\nLoaded '%s' successfully. Songs loaded: %d\n", p->currentPlaylistName, songs);
}
