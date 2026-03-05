#include <stdio.h>
#include <string.h>
#include "chat.h"
#include "models.h"


// Reads messages.txt and displays only messages between me and partner
void display_chat_history(User me, User partner) {

    FILE *file;
    Message msg;

    file = fopen("data/messages.txt", "r");
    if (file == NULL) {
        printf("\nNo messages yet.\n");
        return;
    }

    // Read each line from messages.txt
    while (fscanf(file, "%d|%49[^|]|%49[^|]|%255[^|]|%19[^\n]\n",
                  &msg.id,
                  msg.from,
                  msg.to,
                  msg.message,
                  msg.timestamp) == 5) {

        // FILTER CONDITION
        if (
            (strcmp(msg.from, me.username) == 0 &&
             strcmp(msg.to, partner.username) == 0)

            ||

            (strcmp(msg.from, partner.username) == 0 &&
             strcmp(msg.to, me.username) == 0)
        ) {

            // DISPLAY FORMAT
            printf("< %s : [%s] > %s\n",
                   msg.from,
                   msg.timestamp,
                   msg.message);
        }
    }

    fclose(file);
}
