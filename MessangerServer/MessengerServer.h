#pragma once
typedef enum ClientState
{
	Disconnected,
	Connected,
	Registred,
	Login,
	Loginned,
	WaitNum
}ClientState;

typedef struct _CLIENT_INFO
{
	LIST_ENTRY pListEntry;
	SOCKET	comm_fd;
	ClientState ClientCondition;
}CLIENT_INFO, *PCLIENT_INFO;

typedef struct _SERVER_INFO
{
	LIST_ENTRY ClientsListHead;
	unsigned int NumOfClient;
}SERVER_INFO, *PSERVER_INFO;

DWORD  RecvThread(SOCKET comm_fd);

VOID
SendMessageToClient(char* message, size_t comm_fd);

int
AsyncRecieve(SOCKET comm_fd, char* message, int msg_len);