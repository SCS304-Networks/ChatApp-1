#ifndef MODELS_H
#define MODELS_H

/**
 * struct User - Encapsulates user identity and session data
 * @username: The unique identifier for the account
 * @password: The secret key used for authentication
 * @status: Account status used to distinguish active and deregistered users
 *
 * Description: The primary data structure for the user registry.
 */
typedef struct User
{
	char username[50];
	char password[50];
	char status[20];
} User;

/**
 * struct Message - Defines the messaging schema for database records
 * @id: Unique incrementing identifier for the message
 * @from: Sender's username
 * @to: Intended recipient's username
 * @message: The text content of the message
 * @timestamp: Time the message was sent (Format: HH:MM)
 *
 * Description: Treats database rows as discrete objects for filtering.
 */
typedef struct Message
{
	int id;
	char from[50];
	char to[50];
	char message[256];
	char timestamp[20];
} Message;

#endif /* MODELS_H */