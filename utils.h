#ifndef UTILS_H
#define UTILS_H

void clear_screen(void);
void sanitize_input(char *buffer);
void release_lock(void);
void generate_timestamp(char *buffer);
long fetch_byte_count(char *filename);

#endif /* UTILS_H */