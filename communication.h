#include <sys/socket.h>
#include <sys/types.h>
#include "config.h"
#ifndef communication_h
#define communication_h

#define ARRAY_SIZE(arr) (int)(sizeof(arr)/sizeof((arr)[0]))

//struct to store data about the socket, and its file descriptor
struct com_SocketInfo {
	int socket;
	struct sockaddr_storage addr;
};

//accept and handle all communication with clients
int com_acceptClients(struct com_SocketInfo* sockAddr);

//start server socket based on configuration
int com_startServerSocket(struct fig_ConfigData* data, struct com_SocketInfo* sockAddr, int forceIPv4);

#endif