#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "chat.h"
#include "utils.h"
#include "models.h"

/* Global application state */
User sessionUser;     /* currently logged-in user */
int is_logged_in = 0; /* 0 = not logged in, 1 = logged in */
int app_running = 1;  /* main loop flag */

/* Forward declarations */
void display_main_menu(void);
void display_dashboard(void);

/* Minimal main to demonstrate menus */
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
    return 0;
}

/* Display the main menu shown when the user is not logged in. */
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
        /* consume invalid input */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    /* Clear the leftover newline from scanf */
    getchar();

    switch (choice) {
        case 1:
            /* Attempt login via auth module (skeleton usage) */
            if (authenticate_user(&sessionUser)) {
                is_logged_in = 1;
            }
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
            getchar();
            break;
    }
}

/* Display the dashboard shown after login/registration. */
void display_dashboard(void) {
    int choice = 0;

    clear_screen();
    printf("========================================\n");
    printf("           USER DASHBOARD               \n");
    printf("========================================\n");
    printf("Welcome back, %s!\n", sessionUser.username[0] ? sessionUser.username : "User");
    printf("Status: Online\n\n");
    printf("    1. Open Chat with Partner\n");
    printf("    2. Search for a User\n");
    printf("    3. Deregister (Delete Account)\n");
    printf("    4. Logout\n\n");
    printf("----------------------------------------\n");
    printf("    Choose an option: ");

    if (scanf(" %d", &choice) != 1) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    /* Clear the leftover newline from scanf */
    getchar();

    switch (choice) {
        case 1:
            /* open chat (placeholder) */
            printf("Open Chat selected. (Not implemented in main.c)\n");
            printf("Press Enter to continue...");
            getchar(); getchar();
            break;
        case 2:
            /* search user (placeholder) */
            printf("Search User selected. (Not implemented in main.c)\n");
            printf("Press Enter to continue...");
            getchar(); getchar();
            break;
        case 3:
            /* deregister */
            execute_account_wipe(&sessionUser);
            is_logged_in = 0;
            break;
        case 4:
            terminate_session(&sessionUser);
            is_logged_in = 0;
            break;
        default:
            printf("Invalid option. Press Enter to continue...");
            getchar(); getchar();
            break;
    }
}
