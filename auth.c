#include "auth.h"
#include "utils.h"
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/* --- 1. Internal Security & Validation Helpers --- */

/**
 * is_invalid_input - Checks for whitespace-only strings or the pipe character
 * @str: The string to validate
 *
 * Return: 1 if the input is blank, only spaces, or contains a pipe '|'.
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
    if (str[i] == '|') { /* Prevent corruption of the delimited file */
      return 1;
    }
  }
  return only_spaces;
}

/**
 * toggle_encryption - Applies XOR Symmetric Encryption to a string
 * @data: The string to encrypt or decrypt
 *
 * Return: void
 */
void toggle_encryption(char *data) {
  char key = 'X';
  for (int i = 0; i < (int)strlen(data); i++) {
    data[i] = data[i] ^ key;
  }
}

/**
 * get_masked_password - Reads masked input using getch() for Windows
 * @password: Buffer to store the password
 * @max_len: Maximum length of the password
 *
 * Return: void
 */
void get_masked_password(char *password, int max_len) {
  int i = 0;
  char ch;
  while (i < max_len - 1) {
    ch = getch();
    if (ch == 13)
      break;            /* Enter key */
    else if (ch == 8) { /* Backspace logic */
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

/* --- 2. Registration Module --- */

/**
 * register_user - Handles the user registration process
 *
 * Return: void
 */
void register_user() {
  User newUser;
  clear_screen(); /* */

  printf("--- User Registration ---\n");
  printf("Enter Username: ");
  fgets(newUser.username, 50, stdin);
  sanitize_input(newUser.username); /* */

  /* Validate Username */
  if (is_invalid_input(newUser.username)) {
    printf("\n[!] Error: Username cannot be blank or contain '|'.\n");
    printf("Press Enter to return...");
    getchar();
    return;
  }

  /* Check for Duplicates */
  if (validate_unique_user(newUser.username) == 0) {
    printf("\n[!] Error: Username '%s' is already taken.\n", newUser.username);
    printf("Press Enter to return...");
    getchar();
    return;
  }

  printf("Enter Password: ");
  get_masked_password(newUser.password, 50);

  /* Validate Password */
  if (is_invalid_input(newUser.password)) {
    printf("\n[!] Error: Password cannot be blank or contain '|'.\n");
    printf("Press Enter to return...");
    getchar();
    return;
  }

  strcpy(newUser.status, "OFFLINE"); /* Managed in RAM only */
  commit_user_to_disk(newUser);

  printf("\n[+] Success: Account created for %s.\n", newUser.username);
  printf("Press Enter to continue...");
  getchar();
}

/**
 * validate_unique_user - Checks if a username is unique in the registry
 * @name: The username to check
 *
 * Return: 0 if a match is found, 1 if unique
 */
int validate_unique_user(char name[]) {
  FILE *f = fopen(REGISTRY_FILE, "r");
  if (!f)
    return 1;

  char file_user[50], file_pass[50];

  /* Scan the 2-column format */
  while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
    if (strcmp(file_user, name) == 0) {
      fclose(f);
      return 0; /* Match found */
    }
  }
  fclose(f);
  return 1; /* Unique */
}

/**
 * commit_user_to_disk - Saves a user to the registry file
 * @u: The user struct to save
 *
 * Return: void
 */
void commit_user_to_disk(User u) {
  FILE *f = fopen(REGISTRY_FILE, "a");
  if (f) {
    toggle_encryption(u.password);
    fprintf(f, "%s|%s\n", u.username, u.password); /* strictly 2 columns */
    fclose(f);
  }
}

/* --- 3. Login Module --- */

/**
 * authenticate_user - Authenticates a user and starts a session
 * @session: Pointer to the session User struct to populate on success
 *
 * Return: 1 on success, 0 on failure
 */
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
      strcpy(session->status, "ACTIVE"); /* Session state in RAM */
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

/* --- 4. Dashboard & Session --- */

/**
 * initialize_dashboard - Prints the user dashboard
 * @u: The authenticated user
 *
 * Return: void
 */
void initialize_dashboard(User u) {
  clear_screen();
  printf("================================\n");
  printf("        USER DASHBOARD          \n");
  printf("  User: %-15s Status: %s\n", u.username, u.status);
  printf("================================\n");
}

/**
 * terminate_session - Logs out a user and clears volatile state
 * @u: Pointer to the user session
 *
 * Return: void
 */
void terminate_session(User *u) {
  strcpy(u->status, "OFFLINE"); /* Volatile state reset */
  memset(u->username, 0, 50);
  memset(u->password, 0, 50);
}

/* --- 5. Deregistration Module --- */

/**
 * execute_account_wipe - Permanently deletes an account from the registry
 * @u: Pointer to the user to delete
 *
 * Return: void
 */
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

/**
 * change_password - Changes the password for an authenticated user
 * @session: Pointer to the user session
 *
 * Return: void
 */
void change_password(User *session)
{
    char oldpass[50], newpass[50];

    clear_screen();
    printf("--- Change Password ---\n");
    printf("Current password: ");
    get_masked_password(oldpass, sizeof oldpass);

    if (strcmp(oldpass, session->password) != 0) {
        printf("\n[!] Incorrect password.\nPress Enter to return...");
        getchar();
        return;
    }

    printf("New password: ");
    get_masked_password(newpass, sizeof newpass);

    if (is_invalid_input(newpass)) {
        printf("\n[!] Invalid password.\nPress Enter to return...");
        getchar();
        return;
    }

    /* update registry file */
    FILE *f = fopen(REGISTRY_FILE, "r");
    FILE *tmp = fopen("data/tmp.txt", "w");
    char file_user[50], file_pass[50];

    while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
        if (strcmp(file_user, session->username) == 0) {
            toggle_encryption(newpass);
            fprintf(tmp, "%s|%s\n", file_user, newpass);
            toggle_encryption(newpass); /* restore session copy */
            strcpy(session->password, newpass);
        } else {
            fprintf(tmp, "%s|%s\n", file_user, file_pass);
        }
    }
    fclose(f);
    fclose(tmp);
    remove(REGISTRY_FILE);
    rename("data/tmp.txt", REGISTRY_FILE);

    printf("\n[+] Password changed successfully.\nPress Enter to continue...");
    getchar();
}
