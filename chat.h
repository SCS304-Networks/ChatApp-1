#ifndef CHAT_H
#define CHAT_H

#include "models.h"

// Displays filtered chat history between two users
void display_chat_history(User me, User partner);


// 
void render_inbox(User me);
void transmit_message(User me, User partner, char text[]);


#endif

