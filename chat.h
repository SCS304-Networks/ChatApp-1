#ifndef CHAT_H
#define CHAT_H

#include "models.h"

// Displays filtered chat history between two users
void display_chat_history(User me, User partner);

// Inbox and messaging functions
void render_inbox(User me);
void transmit_message(User me, User partner, char text[]);

// Partner validation and chat engine
int validate_partner(const char *username);
void display_chat_header(User me, User partner);
void live_chat_engine(User me, User partner);

#endif

