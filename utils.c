#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils.h"

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