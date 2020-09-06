#include "stdafx.h"

int cfileexists(const char * filename) 
{
	/* try to open file to read */
	FILE *file;
	if (file = fopen(filename, "r"))
	{
		fclose(file);
		return 1;
	}
	return 0;
}

VOID DecTodirect(int DecNum, char* Buff)
{
	int count = 0;
	for (unsigned q = 31; ~q; --q)
	{
		Buff[count] = ('0' + !!(DecNum&(1U << q)));
		count += 1;
	}

}
VOID DecToBinnary(int DecNum, char* Buff)
{
	int count = 0;
	while (DecNum) 
	{
		if (DecNum & 1)
			Buff[count] = 0x31;
		else
			Buff[count] = 0x30;
		count += 1;
		DecNum >>= 1;
	}
}
CHAR* MsgProc(PCLIENT_INFO pClient, CHAR* Message, size_t MsgLen)
{
	if (MsgLen < 4)
	{
		//return ERROR;byh7gbhubhu
	}
	switch (*(DWORD*)Message)
	{
		case REGISTER_USER:
		{
			unsigned char digest[16] = { 0 };
			char FinalPath[MAX_PATH + 6] = { 0 };
			char TempBuf[MAX_PATH] = { 0 };
			//Если клиент уже регистрируется
		/*	if (pClient->ClientCondition == Loginned)
			{
				send(pClient->comm_fd, (LPSTR)"Error, already registered", (int)strlen("Error, already registered"), 0);
				break;
			}*/
			//изменяем состояние клиента
		//	pClient->ClientCondition = Registred;
			//Сообщение по протоколу о введении логина
			//send(pClient->comm_fd, (LPSTR)"Enter login", (int)strlen("Enter login"), 0);
			//Сообщение по протоколу о введении логина

			AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));

			sprintf(FinalPath, "users\\%s", TempBuf);
			
			if (cfileexists(FinalPath))
			{
				AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));

				send(pClient->comm_fd, (LPSTR)"ERRN3-WRUNAME", (int)strlen("ERRN3-WRUNAME"), 0);//имя занято переделать!
				break;
			}
			FILE *Login;
			Login = fopen(FinalPath, "w");
			/*if (Login == NULL)
			{
				MessageBox(NULL, "Error", "Error", MB_OK);
				break;
			}*/
			memset(TempBuf, 0, MAX_PATH);

			//send(pClient->comm_fd, (LPSTR)"Enter password", (int)strlen("Enter password"), 0);

			int count = AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));

			md5((unsigned char*)TempBuf , count, digest);

			fwrite(digest, 1, sizeof(digest), Login);

			fclose(Login);

			//изменяем состояние клиента*/
			pClient->ClientCondition = Connected;
			send(pClient->comm_fd, (LPSTR)"GOOD", (int)strlen("GOOD"), 0);
			break;
		}

		case LOGIN:
		{
			unsigned char digest[16] = { 0 };
			char FinalPath[MAX_PATH + 6] = { 0 };
			char TempBuf[MAX_PATH] = { 0 };
			char FileData[16] = { 0 };

			AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
			sprintf(FinalPath, "users\\%s", TempBuf);

			FILE *Login;

			Login = fopen(FinalPath, "r");

			if (Login == NULL)
			{
				AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
				send(pClient->comm_fd, (LPSTR)"ERRN2-WRLGOPSWD", (int)strlen("ERRN2-WRLGOPSWD"), 0);
				//сообщение об ошибке
				break;
			}
			memset(TempBuf, 0, MAX_PATH);

			int count = AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));

			md5((unsigned char*)TempBuf, count, digest);

			fread(FileData, 1, sizeof(FileData), Login);

			if (memcmp(FileData, digest, sizeof(FileData) != 0))
			{
				//сообщение об ошибке
				send(pClient->comm_fd, (LPSTR)"ERRN5-UNF", (int)strlen("ERRN5-UNF"), 0);
				break;
			}

			pClient->ClientCondition = Loginned;
			send(pClient->comm_fd, (LPSTR)"GOOD", (int)strlen("GOOD"), 0);

			break;
		}

		case NUMBER:
		{
			int Num = 0;
			char FinalNum[MAX_PATH] = { 0 };
			char TempBuf[MAX_PATH] = { 0 };
			char FinalNumDir[MAX_PATH] = { 0 };
			if (pClient->ClientCondition != Loginned)
			{
				send(pClient->comm_fd, (LPSTR)"ERRN6-PERDE", (int)strlen("ERRN6-PERDE"), 0);
				break;
			}

			AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
			Num = atoi(TempBuf);
			DecToBinnary(Num, FinalNum);
			DecTodirect(Num, FinalNumDir);
			send(pClient->comm_fd, (LPSTR)FinalNum, (int)strlen(FinalNum), 0);
			send(pClient->comm_fd, (LPSTR)FinalNumDir, (int)strlen(FinalNum), 0);
			break;
		}

		case HEX:
		{
			char FinalNum[MAX_PATH] = { 0 };
			int Num = 0;
			char TempBuf[MAX_PATH] = { 0 };
			if (pClient->ClientCondition != WaitNum)
			{
				AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
				//ошибка
				break;
			}

			AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
			Num = atoi(TempBuf);
			sprintf(FinalNum, "%x", Num);

			send(pClient->comm_fd, (LPSTR)FinalNum, (int)strlen(FinalNum), 0);
			pClient->ClientCondition = Loginned;
			break;
		}

		case OCT:
		{
			int Num = 0;
			char FinalNum[MAX_PATH] = { 0 };
			char TempBuf[MAX_PATH] = { 0 };
			if (pClient->ClientCondition != WaitNum)
			{
				AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
				//ошибка
				break;
			}

			AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
			Num = atoi(TempBuf);
			sprintf(FinalNum, "%o", Num);
			send(pClient->comm_fd, (LPSTR)FinalNum, (int)strlen(FinalNum), 0);
			pClient->ClientCondition = Loginned;
			break;
		}

		case DOUBLE:
		{
			int Num = 0;
			char FinalNum[MAX_PATH] = { 0 };
			char FinalDirectNum[MAX_PATH] = { 0 };
			char TempBuf[MAX_PATH] = { 0 };
			if (pClient->ClientCondition != WaitNum)
			{
				AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
				//ошибка
				break;
			}

			AsyncRecieve(pClient->comm_fd, TempBuf, sizeof(TempBuf));
			Num = atoi(TempBuf);
			DecToBinnary(Num, FinalNum);
			DecTodirect(Num, FinalDirectNum);
			send(pClient->comm_fd, (LPSTR)FinalNum, (int)strlen(FinalNum), 0);
			pClient->ClientCondition = Loginned;
			break;
		}

		default:
		{
			int i = 0;
			break;
		}
		
	}
	return NULL;
}