#include "auth.h"
#include "chat.h"
#include "models.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Global application state */
User sessionUser;     /* currently logged-in user */
int is_logged_in = 0; /* 0 = not logged in, 1 = logged in */
int app_running = 1;  /* main loop flag */

/* Forward declarations */
void display_main_menu(void);
void display_dashboard(void);
void search_user_flow(char *current_user);

/**
 * main - Entry point for the Chat-Connect application
 *
 * Return: Always 0 (Success)
 */
int main(void) {
  /* initialize volatile state */
  memset(&sessionUser, 0, sizeof(User));

  while (app_running) {
    if (!is_logged_in) {
      display_main_menu();
    } else {
      display_dashboard();
    }
  }

  printf("Exiting application. Goodbye.\n");
  return (0);
}

/**
 * search_user_flow - Scans the registry for a specific peer
 * @current_user: The username of the person currently logged in
 *
 * Description: Implements systematic interrogation of users.txt.
 * Return: void
 */
void search_user_flow(char *current_user) {
  char target[50], file_user[50], file_pass[50];
  int user_found = 0;
  FILE *f;

  /* 1. UI Initialization & Header */
  clear_screen();
  printf("========================================\n");
  printf("             USER SEARCH                \n");
  printf("========================================\n\n");

  /* 2. Search Parameter Capture & Sanitization */
  printf("Enter Target Username: ");
  fgets(target, 50, stdin);
  sanitize_input(target); /* */

  /* 3. Identity Verification (Self-Search Check) */
  if (strcmp(target, current_user) == 0) {
    printf("\n[!] NOTE: That's you! You are already online.\n");
  } else {
    /* 4. Database Interrogation */
    f = fopen(REGISTRY_FILE, "r");
    if (!f) {
      printf("\n[!] ERROR: Directory unavailable.\n");
    } else {
      /* 5. Comparison Logic (fscanf with pipe delimiter) */
      while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
        if (strcmp(file_user, target) == 0) {
          user_found = 1;
          break;
        }
      }
      fclose(f); /* 6. Resource Management */

      /* 7. Result Branching */
      if (user_found)
        printf("\n[+] SUCCESS: User '%s' is registered and available.\n",
               target);
      else
        printf("\n[!] ERROR: User '%s' not found in the directory.\n", target);
    }
  }

  /* 8. User Acknowledgment */
  printf("\n----------------------------------------\n");
  printf("Press Enter to return to the Dashboard...");
  getchar();
}

/**
 * display_main_menu - Shows the public state options
 *
 * Return: void
 */
void display_main_menu(void) {
  int choice = 0;

  clear_screen();
  printf("========================================\n");
  printf("       WELCOME TO CHAT-CONNECT          \n");
  printf("========================================\n\n");
  printf("    1. Login to Account\n");
  printf("    2. Register New Account\n");
  printf("    3. Exit Application\n");
  printf("\n    Choose an option: ");

  if (scanf(" %d", &choice) != 1) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
      ;
    return;
  }
  getchar();

  switch (choice) {
  case 1:
    if (authenticate_user(&sessionUser))
      is_logged_in = 1;
    break;
  case 2:
    register_user();
    break;
  case 3:
    app_running = 0;
    break;
  default:
    printf("Invalid option. Press Enter to continue...");
    getchar();
    break;
  }
}

/**
 * display_dashboard - Shows the authenticated state options
 *
 * Return: void
 */
void display_dashboard(void) {
  int choice = 0;

  clear_screen();
  printf("========================================\n");
  printf("           USER DASHBOARD               \n");
  printf("========================================\n");
  printf("Welcome back, %s!\n",
         sessionUser.username[0] ? sessionUser.username : "User");
  printf("Status: Online\n\n");
  printf("    1. Open Chat with Partner\n");
  printf("    2. Search for a User\n");
  printf("    3. Deregister (Delete Account)\n");
  printf("    4. Logout\n\n");
  printf("----------------------------------------\n");
  printf("    Choose an option: ");

  if (scanf(" %d", &choice) != 1) {
    int c;

    while ((c = getchar()) != '\n' && c != EOF)
      ;
    return;
  }
  getchar();

  switch (choice) {
  case 1:
    printf("Open Chat selected. (Module coming soon)\n");
    printf("Press Enter to continue...");
    getchar();
    break;
  case 2:
    /* Systematic Search implementation */
    search_user_flow(sessionUser.username);
    break;
  case 3:
    execute_account_wipe(&sessionUser);
    is_logged_in = 0;
    break;
  case 4:
    terminate_session(&sessionUser);
    is_logged_in = 0;
    break;
  default:
    printf("Invalid option. Press Enter to continue...");
    getchar();
    break;
  }
}