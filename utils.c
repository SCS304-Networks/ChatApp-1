#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "utils.h"

/* Platform-specific includes for input_available */
#ifdef _WIN32
    #include <conio.h>    /* For _kbhit() on Windows */
#else
    #include <sys/select.h> /* For select() on Linux/macOS */
    #include <unistd.h>
#endif

/**
 * clear_screen - Clears the terminal screen
 *
 * Description: Handles the clear screen command for a clean CLI environment.
 * Return: void
 */
void clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/**
 * sanitize_input - Replaces trailing newline character with a null terminator
 * @buffer: The string array captured by fgets
 *
 * Description: Prevents newline characters from corrupting the delimited file.
 * Return: void
 */
void sanitize_input(char *buffer)
{
    buffer[strcspn(buffer, "\n")] = '\0';
}

/**
 * acquire_lock - Creates a lock file to manage concurrency
 *
 * Description: Wait (busy-wait) until 'messages.lock' is gone, 
 * then creates it to claim exclusive access to the database.
 * Return: void
 */
void acquire_lock(void)
{
    FILE *file;
    /* Busy-wait: Loop as long as the lock file exists */
    while ((file = fopen("messages.lock", "r")) != NULL) {
        fclose(file);
    }

    /* Create the lock file to signal other processes to wait */
    file = fopen("messages.lock", "w");
    if (file != NULL) {
        fprintf(file, "locked");
        fclose(file);
    }
}

/**
 * release_lock - Deletes the concurrency lock file
 *
 * Description: Invokes remove() on the messages.lock file.
 * Return: void
 */
void release_lock(void)
{
    remove("messages.lock");
}

/**
 * input_available - Checks if a key has been pressed without blocking
 *
 * Description: Monitors the input buffer. This allows the chat UI to 
 * refresh with new messages even if the user isn't currently typing.
 * Return: true if data is waiting in stdin, false otherwise.
 */
bool input_available(void)
{
#ifdef _WIN32
    return _kbhit(); 
#else
    struct timeval tv = {0L, 0L}; 
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0;
#endif
}

/**
 * generate_timestamp - Retrieves and formats the current system time
 * @buffer: The character array to hold the formatted time string
 *
 * Description: Formats time into a 20-character ISO-like HH:MM string.
 * Return: void
 */
void generate_timestamp(char *buffer)
{
    time_t t;
    struct tm *tm_info;

    t = time(NULL);
    tm_info = localtime(&t);
    strftime(buffer, 20, "%H:%M", tm_info);
}

/**
 * fetch_byte_count - Captures the exact file size in bytes
 * @filename: The string path of the database file
 *
 * Description: Moves cursor to the end of the file and checks position.
 * Return: The size of the file in bytes, or -1 if the file cannot be opened.
 */
long fetch_byte_count(char *filename)
{
    FILE *file;
    long size;

    file = fopen(filename, "r");
    if (file == NULL)
        return (-1);

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fclose(file);

    return (size);
}