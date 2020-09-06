// EchoServerThreads.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#pragma comment(lib, "Ws2_32.lib")

SERVER_INFO g_server_info;

//Получение данных о клиенте по номеру сокета
PCLIENT_INFO
GetClientByComm(SOCKET comm_fd)
{
	//Получение первого клиента
	PCLIENT_INFO pCurrentClientInfo = (PCLIENT_INFO)g_server_info.ClientsListHead.Flink;
	//Прохождение по всем клиентам
	while (g_server_info.ClientsListHead.Flink != pCurrentClientInfo->pListEntry.Flink)
	{
		printf("Current Client %llx\r\n", pCurrentClientInfo->comm_fd);

		//Если нужный клиент
		if (pCurrentClientInfo->comm_fd == comm_fd)
		{
			return pCurrentClientInfo;
		}

		//Переход к следущему клиенту
		pCurrentClientInfo = (PCLIENT_INFO)pCurrentClientInfo->pListEntry.Flink;
	}

	return NULL;
}

//Клиент отключился
void
ClientExit(SOCKET comm_fd)
{

	PCLIENT_INFO pClientInfo = GetClientByComm(comm_fd);

	printf("Client %zd removed\r\n", pClientInfo->comm_fd);
	//Удаление клиента из списка
	RemoveEntryList(&pClientInfo->pListEntry);
	g_server_info.NumOfClient -= 1;
	free(pClientInfo);
}

//Асинхронный прием
int
AsyncRecieve(SOCKET comm_fd, char* message, int msg_len)
{
	int recieved = 0;

	while ("Some shit")
	{
		recieved = recv(comm_fd, message, msg_len, 0);

		if (recieved == -1)
		{
			if (WSAGetLastError() != WSAETIMEDOUT)
				break;
			else
				continue;
		}
		else
		{
			break;
		}

	}

	return recieved;

}

//Отправка сообщения клиенту
VOID
SendMessageToClient(char* message, size_t comm_fd)
{
	PCLIENT_INFO pCurrentClientInfo = (PCLIENT_INFO)g_server_info.ClientsListHead.Flink;

	//Проходим по списку клиентов
	while (g_server_info.ClientsListHead.Flink != pCurrentClientInfo->pListEntry.Flink)
	{
		//Если найден клиент с нужным сокетом
		if (pCurrentClientInfo->comm_fd != comm_fd)
		{
			//отправляем ему сообщение
			send(pCurrentClientInfo->comm_fd, (LPSTR)message, (int)strlen(message), 0);
		}

		//берем следущего клиента
		pCurrentClientInfo = (PCLIENT_INFO)pCurrentClientInfo->pListEntry.Flink;
	}

	return;
}

//Вывод информации о всех клиентах
VOID
PrintAllClients()
{
	PCLIENT_INFO pCurrentClientInfo = (PCLIENT_INFO)g_server_info.ClientsListHead.Flink;

	while (g_server_info.ClientsListHead.Flink != pCurrentClientInfo->pListEntry.Flink)
	{
		printf("Current Client %zd\r\n", pCurrentClientInfo->comm_fd);
		pCurrentClientInfo = (PCLIENT_INFO)pCurrentClientInfo->pListEntry.Flink;
	}
}

int main()
{

	WSADATA wsaData;

	if (WSAStartup(WINSOCK_VERSION, &wsaData))
	{
		printf("winsock not bi initialized !\n");
		WSACleanup();
	}

	InitializeListHead(&g_server_info.ClientsListHead);

	SOCKET listen_fd, comm_fd;

	struct sockaddr_in servaddr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));


	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(59999);

	bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	while (1)
	{
		listen(listen_fd, 10);

		comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);

		if (g_server_info.NumOfClient == 3)
		{
			send(comm_fd, (LPSTR)"ERRN1-TMU", (int)strlen("ERRN1-TMU"), 0);
			closesocket(comm_fd);
			continue;
		}
		g_server_info.NumOfClient += 1;
		int timeout = 500;  // user timeout in milliseconds [ms]
		setsockopt(comm_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

		PCLIENT_INFO pClientInfo = (PCLIENT_INFO)malloc(sizeof(CLIENT_INFO));

		pClientInfo->comm_fd = comm_fd;

		InsertTailList(&g_server_info.ClientsListHead, &pClientInfo->pListEntry);

		//Создание потока обработки сообщений от каждого клиента
		HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, (LPVOID)comm_fd, 0, NULL);
	




	}
}






DWORD  RecvThread(SOCKET comm_fd)
{
	char str[100];

	memset(str, 0, sizeof(str));
	//Получаем данные о клиенте по номеру сокета
	PCLIENT_INFO pClientInfo = GetClientByComm(comm_fd);

	while (1)
	{
		memset(str, 0, 100);
		//Принимаем данные от клиента
		int recieved = AsyncRecieve(comm_fd, str, sizeof(str));

		if (recieved == -1 || recieved == 0)
		{
			printf("Client %zd fell off\r\n", comm_fd);
			//Клиент отключился, удаляем его из списка клиентов
			ClientExit(comm_fd);

			//PrintAllClients();

			return 0;
		}

		printf("Client %zd said: %s\r\n", comm_fd, str);

		//Обработка сообщений от клиентов
		MsgProc(pClientInfo, str, recieved);

		//SendMessageToClient(str, comm_fd);
	}
};