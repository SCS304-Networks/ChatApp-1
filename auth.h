
#ifndef AUTH_H
#define AUTH_H
#define REGISTRY_FILE "data/users.txt"
#include "models.h"
#include "utils.h"

// Registration Module
void register_user();
int validate_unique_user(char name[]);
void commit_user_to_disk(User u);

// Login Module
int authenticate_user(User *session);
void initialize_dashboard(User u);

// --- Encryption Module ---
void toggle_encryption(char *data);
void get_masked_password(char *password, int max_len);

// Deregistration Module
void execute_account_wipe(User *u);
void terminate_session(User *u);

// Utility function mentioned in flow
void strip_newline(char *str);

#endif