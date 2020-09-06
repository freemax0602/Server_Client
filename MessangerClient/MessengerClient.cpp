#include "stdafx.h"
#pragma comment(lib, "Ws2_32.lib")

DWORD WINAPI RecvThread(SOCKET sockfd)
{
	int recieved = 0;
	char recvline[100];

	while ("")
	{
		memset(recvline, 0, sizeof(recvline));
		recieved = recv((SOCKET)sockfd, recvline, sizeof(recvline), 0);

		if (recieved == -1 || recieved == 0)
		{
			system("pause");
			ExitProcess(0);
		}

		printf("%s\r\n", recvline);
	}

	return 0;
}

int main(int argc, char **argv)
{
	WSADATA wsaData;

	//Подключение клиента к серверу
	if (WSAStartup(WINSOCK_VERSION, &wsaData))
	{
		printf("winsock not be initialized !\r\n");
		WSACleanup();
	}

	SOCKET sockfd;
	char sendline[100];

	struct sockaddr_in servaddr;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&servaddr, 0, sizeof servaddr);

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(59999);

	inet_pton(AF_INET, "127.0.0.1", &(servaddr.sin_addr));

	printf("Start connecting\r\n");

	connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	//Создание потока обработки от сервера
	HANDLE thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecvThread, (LPVOID)sockfd, 0, NULL);

	printf("Connected. Error %d\r\n", GetLastError());

	 
	while (1)
	{
		memset(sendline, 0, 100);
		//Ожидание ввода сообщения от пользователя
		//для пробелов
		//scanf_s("%[^\n]", sendline, (unsigned)_countof(sendline));
		scanf_s("%s", sendline, (unsigned)_countof(sendline));
		//И отправка его серверу
		send(sockfd, sendline, (int)strlen(sendline), 0);
	}

	system("pause");
}