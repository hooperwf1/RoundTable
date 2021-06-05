#ifndef cmds_h
#define cmds_h

#include "chat.h"
#include "linkedlist.h"
#include "numerics.h"

#define NUMERIC_SIZE 15

struct chat_Message;
struct chat_Channel;

struct cmd_CommandList {
    struct link_List commands;	
    pthread_mutex_t commandMutex;
};

// permLevel describes the permissions needed for a user to run a command
// 0 = Unregistered
// 1 = Registered
// 2 = Server Admin
struct cmd_Command {
    char word[50];
    int minParams;
    char responses[5][50];
    int (*func)(struct chat_Message *, struct chat_Message *);
	int permLevel;
};

int init_commands();

// Will use parameters to construct a cmd_Command struct
// and adds it to the cmd_commandList struct
int cmd_addCommand(char *word, int minParams, int permLevel, int (*func)(struct chat_Message *, struct chat_Message *));

int cmd_runCommand(struct chat_Message *cmd);

/* Reply generators */
void rpl_notonchannel(struct chat_Message *msg, struct chat_Channel *chan, struct chat_UserData *user);

// Change a user's nickname
int cmd_nick(struct chat_Message *cmd, struct chat_Message *reply);

// Send to a message to user or channel
int cmd_privmsg(struct chat_Message *cmd, struct chat_Message *reply);

// Join a channel
int cmd_join(struct chat_Message *cmd, struct chat_Message *reply);

// Return a list of names inside a channel
int cmd_names(struct chat_Message *cmd, struct chat_Message *reply);

// Remove a user from a channel
int cmd_part(struct chat_Message *cmd, struct chat_Message *reply);

#endif
