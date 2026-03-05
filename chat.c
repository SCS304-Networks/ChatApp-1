#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chat.h"
#include "models.h"
#include "utils.h"

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
