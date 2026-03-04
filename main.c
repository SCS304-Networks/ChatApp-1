#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * search_user - Executes the systematic data flow for finding a peer
 * @current_user: The username of the person currently logged in
 *
 * Description: Scans users.txt for a target, prevents self-searching,
 * and manages file resources efficiently.
 * Return: void
 */
void search_user(char *current_user) {
  char target[50], file_user[50], file_pass[50];
  int user_found = 0;
  FILE *f;

  /* 1. UI Initialization */
  clear_screen();
  printf("========================================\n");
  printf("             SEARCH FOR A USER          \n");
  printf("========================================\n\n");

  /* 2. Search Parameter Capture & Sanitization */
  printf("Enter Target Username: ");
  fgets(target, 50, stdin);
  sanitize_input(target);

  /* 3. Identity Verification (Self-Search Check) */
  if (strcmp(target, current_user) == 0) {
    printf("\n[!] NOTE: That's you! You are already online.\n");
  } else {
    /* 4. Database Interrogation */
    f = fopen(REGISTRY_FILE, "r");
    if (!f) {
      printf("\n[!] ERROR: Directory unavailable.\n");
    } else {
      /* 5. Comparison Logic */
      while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
        if (strcmp(file_user, target) == 0) {
          user_found = 1;
          break;
        }
      }
      fclose(f); /* 6. Resource Management */

      /* 7. Result Branching */
      if (user_found)
        printf("\n[+] SUCCESS: User '%s' is registered.\n", target);
      else
        printf("\n[!] ERROR: User '%s' not found.\n", target);
    }
  }

  /* 8. User Acknowledgment */
  printf("\n----------------------------------------\n");
  printf("Press Enter to return to the Dashboard...");
  getchar();
}

/**
 * main - Entry point for the Chat-Connect application
 *
 * Return: Always 0 (Success)
 */
int main(void) {
  User currentUser;
  int choice;

  /* Initialize RAM state */
  strcpy(currentUser.status, "OFFLINE");
  memset(currentUser.username, 0, 50);

  while (1) {
    clear_screen();
    if (strcmp(currentUser.status, "OFFLINE") == 0) {
      /* A. Main Menu (Public State) */
      printf("========================================\n");
      printf("       WELCOME TO CHAT-CONNECT          \n");
      printf("========================================\n\n");
      printf("    1. Login to Account\n");
      printf("    2. Register New Account\n");
      printf("    3. Exit Application\n\n");
      printf("----------------------------------------\n");
      printf("Selection > ");
      if (scanf("%d", &choice) != 1)
        choice = 0;
      getchar(); /* Consume newline */

      if (choice == 1)
        authenticate_user(&currentUser);
      else if (choice == 2)
        register_user();
      else if (choice == 3)
        break;
    } else {
      /* D. User Dashboard (Authenticated) */
      printf("========================================\n");
      printf("           USER DASHBOARD               \n");
      printf("========================================\n");
      printf("Welcome back, %s!\n", currentUser.username);
      printf("Status: Online\n\n");
      printf("    1. Open Chat with Partner\n");
      printf("    2. Search for a User\n");
      printf("    3. Deregister (Delete Account)\n");
      printf("    4. Logout\n\n");
      printf("----------------------------------------\n");
      printf("Selection > ");
      if (scanf("%d", &choice) != 1)
        choice = 0;
      getchar();

      if (choice == 1)
        printf("\n[!] Chat Module coming soon...\n");
      else if (choice == 2)
        search_user(currentUser.username); /* Fixed parameter */
      else if (choice == 3)
        execute_account_wipe(&currentUser);
      else if (choice == 4)
        terminate_session(&currentUser);
    }
  }
  return (0);
}