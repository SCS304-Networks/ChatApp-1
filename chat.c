#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chat.h"
#include "models.h"
#include "utils.h"
#include "auth.h"

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
#endif

/**
 * Filter and render the conversation log between the session user and a chosen partner.
 * It interrogates the messages.txt file and displays only relevant exchanges.
 */
void display_chat_history(User me, User partner) {
    FILE *file = fopen("data/messages.txt", "r");
    Message msg;

    // Gracefully exit if the message database has not been initialized yet
    if (file == NULL) return; 

    // Scan the file line-by-line using the pipe-delimited schema: id|from|to|text|time
    while (fscanf(file, "%d|%49[^|]|%49[^|]|%255[^|]|%19[^\n]\n",
                  &msg.id, msg.from, msg.to, msg.message, msg.timestamp) == 5) {
        
        // Logical AND/OR check to select messages where the user is either sender or receiver
        if ((strcmp(msg.from, me.username) == 0 && strcmp(msg.to, partner.username) == 0) ||
            (strcmp(msg.from, partner.username) == 0 && strcmp(msg.to, me.username) == 0)) {
            
            // Output formatted to match the bracketed UI requirement: < Name : [Time] > Content
            printf("< %s : [%s] > %s\n", msg.from, msg.timestamp, msg.message);
        }
    }
    fclose(file);
}

/**
 * Scan message history to identify and display a list of unique past conversation partners.
 * This acts as the "Inbox" view for the user dashboard.
 */
void render_inbox(User me) {
    clear_screen();
    FILE *file = fopen("data/messages.txt", "r");
    char partners[100][50]; // Temporary memory to track unique usernames
    int count = 0;
    Message msg;

    printf("\n========================================\n");
    printf("           YOUR CONVERSATIONS           \n");
    printf("========================================\n");

    if (file != NULL) {
        // Iterate through the global store to find existing threads involving the current user
        while (fscanf(file, "%d|%49[^|]|%49[^|]|%255[^|]|%19[^\n]\n",
                      &msg.id, msg.from, msg.to, msg.message, msg.timestamp) == 5) {
            
            char *target = NULL;
            // Identify if the other party is the sender or receiver
            if (strcmp(msg.from, me.username) == 0) target = msg.to;
            else if (strcmp(msg.to, me.username) == 0) target = msg.from;

            if (target) {
                // Deduplication logic to ensure names are only listed once in the UI
                int exists = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(partners[i], target) == 0) { exists = 1; break; }
                }
                // If unique, add to the tracking array and print to screen
                if (!exists && count < 100) {
                    strncpy(partners[count++], target, 49);
                    printf(" - %s\n", target);
                }
            }
        }
        fclose(file);
    }

    // Feedback for new accounts with no history
    if (count == 0) printf(" (No recent chats. Start a new one!)\n");
    printf("----------------------------------------\n");
}

/**
 * Persist a new message to the flat-file database using a thread-safe locking mechanism.
 * This handles the synchronization of Message IDs and timestamps.
 */
void transmit_message(User me, User partner, char text[]) {
    // Semaphore Pattern: Prevent race conditions by checking for messages.lock
    acquire_lock(); 

    // Open file in append mode to add new record at the end
    FILE *file = fopen("data/messages.txt", "a+");
    if (file == NULL) {
        release_lock();
        return;
    }

    // ID Synchronization: Traverse the file to find the highest current ID and increment
    int nextID = 1;
    char line[600];
    rewind(file);
    while (fgets(line, sizeof(line), file)) {
        int currentID;
        if (sscanf(line, "%d|", &currentID) == 1) {
            if (currentID >= nextID) nextID = currentID + 1;
        }
    }

    // Generate current system time for the message metadata
    char ts[20];
    generate_timestamp(ts); 

    // Disk Commitment: Write serialized data in the pipe-delimited global format
    fprintf(file, "%d|%s|%s|%s|%s\n", nextID, me.username, partner.username, text, ts);
    
    // Resource Management: Close file handle and delete the .lock file to allow other users access
    fclose(file);
    release_lock(); 
}

/**
 * validate_partner - Checks if a target username exists in the registry
 * @username: The username to validate
 *
 * Return: 1 if found, 0 if not found
 */
int validate_partner(const char *username) {
    FILE *f = fopen(REGISTRY_FILE, "r");
    if (!f) return 0;

    char file_user[50], file_pass[50];
    while (fscanf(f, " %[^|]|%s ", file_user, file_pass) == 2) {
        if (strcmp(file_user, username) == 0) {
            fclose(f);
            return 1; // Found
        }
    }
    fclose(f);
    return 0; // Not found
}

/**
 * display_chat_header - Renders the chat session header
 * @me: The current user
 * @partner: The chat partner
 */
void display_chat_header(User me, User partner) {
    printf("========================================\n");
    printf("   CHAT SESSION: %s <--> %s        \n", me.username, partner.username);
    printf("========================================\n\n");
}

/**
 * live_chat_engine - The core real-time chat loop for the active session
 * @me: The current session user
 * @partner: The selected chat partner
 *
 * Description: Implements the Smart Refresh system using file byte-count
 * comparison to detect new messages. Redraws the screen only when new data
 * is detected. Uses non-blocking input detection to allow the user to type
 * while the refresh loop runs. Exits when user types 'back'.
 */
void live_chat_engine(User me, User partner) {
    long lastFileSize = -1;
    long currentFileSize;
    char msgBuffer[256];
    int running = 1;

    // Initial screen draw
    clear_screen();
    display_chat_header(me, partner);
    display_chat_history(me, partner);
    printf("\n----------------------------------------\n");
    printf("You (type 'back' to return): ");
    fflush(stdout);

    while (running) {
        // Phase 2: Smart Refresh - Check file size for changes
        currentFileSize = fetch_byte_count("data/messages.txt");

        // Only redraw if file size changed (new message received)
        if (currentFileSize != lastFileSize && lastFileSize != -1) {
            // Concurrency check before reading
            acquire_lock();
            
            clear_screen();
            display_chat_header(me, partner);
            display_chat_history(me, partner);
            printf("\n----------------------------------------\n");
            printf("You (type 'back' to return): ");
            fflush(stdout);
            
            release_lock();
        }
        lastFileSize = currentFileSize;

        // Non-blocking input check
        if (input_available()) {
            // User started typing - capture full input
            if (fgets(msgBuffer, sizeof(msgBuffer), stdin) != NULL) {
                sanitize_input(msgBuffer);

                // Check for exit command
                if (strcmp(msgBuffer, "back") == 0) {
                    running = 0;
                    break;
                }

                // Don't send empty messages
                if (strlen(msgBuffer) > 0) {
                    // Phase 5: Sending a new message
                    transmit_message(me, partner, msgBuffer);

                    // Force refresh by resetting lastFileSize
                    lastFileSize = -1;

                    // Redraw immediately to show own message
                    clear_screen();
                    display_chat_header(me, partner);
                    display_chat_history(me, partner);
                    printf("\n----------------------------------------\n");
                    printf("You (type 'back' to return): ");
                    fflush(stdout);
                }
            }
        }

        // Polling interval: 200ms pause to prevent CPU spinning
#ifdef _WIN32
        Sleep(200);
#else
        usleep(200000);
#endif
    }
}
