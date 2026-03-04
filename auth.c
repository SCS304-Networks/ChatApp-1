#include "auth.h"
#include "utils.h" // Teammate's standardized utilities
#include <conio.h>
#include <ctype.h> // Required for whitespace checking
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// --- 1. Internal Security & Validation Helpers ---

/**
 * is_invalid_input - Checks for whitespace-only strings or the pipe character
 * @str: The string to validate
 * * Returns 1 if the input is blank, only spaces, or contains a pipe '|'.
 * Returns 0 if the input is valid.
 */
int is_invalid_input(const char *str) {
  if (strlen(str) == 0)
    return 1;

  int only_spaces = 1;
  for (int i = 0; i < (int)strlen(str); i++) {
    if (!isspace((unsigned char)str[i])) {
      only_spaces = 0;
    }
    if (str[i] == '|') { // Prevent corruption of the delimited file
      return 1;
    }
  }
  return only_spaces;
}

// XOR Symmetric Encryption
void toggle_encryption(char *data) {
  char key = 'X';
  for (int i = 0; i < (int)strlen(data); i++) {
    data[i] = data[i] ^ key;
  }
}

// Masked input using getch() for Windows
void get_masked_password(char *password, int max_len) {
  int i = 0;
  char ch;
  while (i < max_len - 1) {
    ch = getch();
    if (ch == 13)
      break;            // Enter key
    else if (ch == 8) { // Backspace logic
      if (i > 0) {
        i--;
        printf("\b \b");
      }
    } else if (ch >= 32 && ch <= 126) {
      password[i++] = ch;
      printf("*");
    }
  }
  password[i] = '\0';
  printf("\n");
}

// --- 2. Registration Module ---

void register_user() {
  User newUser;
  clear_screen(); //

  printf("--- User Registration ---\n");
  printf("Enter Username: ");
  fgets(newUser.username, 50, stdin);
  sanitize_input(newUser.username); //

  // Validate Username
  if (is_invalid_input(newUser.username)) {
    printf("\n[!] Error: Username cannot be blank or contain '|'.\n");
    printf("Press Enter to return...");
    getchar();
    return;
  }

  // Check for Duplicates
  if (validate_unique_user(newUser.username) == 0) {
    printf("\n[!] Error: Username '%s' is already taken.\n", newUser.username);
    printf("Press Enter to return...");
    getchar();
    return;
  }

  printf("Enter Password: ");
  get_masked_password(newUser.password, 50);

  // Validate Password
  if (is_invalid_input(newUser.password)) {
    printf("\n[!] Error: Password cannot be blank or contain '|'.\n");
    printf("Press Enter to return...");
    getchar();
    return;
  }

  strcpy(newUser.status, "OFFLINE"); // Managed in RAM only
  commit_user_to_disk(newUser);

  printf("\n[+] Success: Account created for %s.\n", newUser.username);
  printf("Press Enter to continue...");
  getchar();
}

int validate_unique_user(char name[]) {
  FILE *f = fopen(REGISTRY_FILE, "r");
  if (!f)
    return 1;

  char file_user[50], file_pass[50];

  // Scan the 2-column format
  while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
    if (strcmp(file_user, name) == 0) {
      fclose(f);
      return 0; // Match found
    }
  }
  fclose(f);
  return 1; // Unique
}

void commit_user_to_disk(User u) {
  FILE *f = fopen(REGISTRY_FILE, "a");
  if (f) {
    toggle_encryption(u.password);
    fprintf(f, "%s|%s\n", u.username, u.password); // strictly 2 columns
    fclose(f);
  }
}

// --- 3. Login Module ---

int authenticate_user(User *session) {
  char authBuffer_Name[50], authBuffer_Pass[50];
  char file_user[50], file_pass[50];
  int successFlag = 0;

  clear_screen();
  printf("--- Account Login ---\n");

  printf("Username: ");
  fgets(authBuffer_Name, 50, stdin);
  sanitize_input(authBuffer_Name);

  printf("Password: ");
  get_masked_password(authBuffer_Pass, 50);

  FILE *f = fopen(REGISTRY_FILE, "r");
  if (!f) {
    printf("[!] Error: No registry found. Please register first.\n");
    printf("Press Enter to return...");
    getchar();
    return 0;
  }

  while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
    toggle_encryption(file_pass);

    if (strcmp(authBuffer_Name, file_user) == 0 &&
        strcmp(authBuffer_Pass, file_pass) == 0) {

      successFlag = 1;
      strcpy(session->username, file_user);
      strcpy(session->password, authBuffer_Pass);
      strcpy(session->status, "ACTIVE"); // Session state in RAM
      break;
    }
  }
  fclose(f);

  if (successFlag) {
    printf("\nWelcome back, [%s]!\n", session->username);
    return 1;
  } else {
    printf("\n[!] Invalid Credentials.\nPress Enter to return...");
    getchar();
    return 0;
  }
}

// --- 4. Dashboard & Session ---

void initialize_dashboard(User u) {
  clear_screen();
  printf("================================\n");
  printf("        USER DASHBOARD          \n");
  printf("  User: %-15s Status: %s\n", u.username, u.status);
  printf("================================\n");
}

void terminate_session(User *u) {
  strcpy(u->status, "OFFLINE"); // Volatile state reset
  memset(u->username, 0, 50);
  memset(u->password, 0, 50);
}

// --- 5. Deregistration Module ---

void execute_account_wipe(User *u) {
  User registrySnapshot[100];
  int count = 0;
  char confirmToken;
  char file_user[50], file_pass[50];

  clear_screen();
  printf("[!] WARNING: This action is permanent.\n");
  printf("Confirm wipe (y/n): ");
  scanf(" %c", &confirmToken);
  getchar();

  if (confirmToken != 'y')
    return;

  FILE *f = fopen(REGISTRY_FILE, "r");
  if (!f)
    return;

  while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
    if (strcmp(file_user, u->username) != 0) {
      strcpy(registrySnapshot[count].username, file_user);
      strcpy(registrySnapshot[count].password, file_pass);
      count++;
    }
  }
  fclose(f);

  f = fopen(REGISTRY_FILE, "w");
  for (int i = 0; i < count; i++) {
    fprintf(f, "%s|%s\n", registrySnapshot[i].username,
            registrySnapshot[i].password);
  }
  fclose(f);

  terminate_session(u);
  printf("[+] SUCCESS: Your account has been permanently wiped.\n");
  printf("Press Enter to return...");
  getchar();
}