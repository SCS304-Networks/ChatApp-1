#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h> // Required for the bool return type

void clear_screen(void);
void sanitize_input(char *buffer);
void release_lock(void);
void generate_timestamp(char *buffer);
long fetch_byte_count(char *filename);

void acquire_lock(void);
bool input_available(void);

#endif /* UTILS_H */