#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define BUFFER_LEN 1000

#include <iostream>
#include <winsock2.h>
#include <string>

using namespace std;

#define BOARDSIZE 8

void InitMap(int(*board)[BOARDSIZE])
{
    for (int j = 0; j < BOARDSIZE; j++)
    {
        for (int i = 0; i < BOARDSIZE; i++)
        {
            board[j][i] = 0;
        }
    }
}

void DrawMap(int(*board)[BOARDSIZE])
{
    for (int j = 0; j < BOARDSIZE; j++)
    {
        for (int i = 0; i < BOARDSIZE; i++)
        {
            if (board[j][i])
                cout << "��";
            else
                cout << "�Q";
        }
        cout << '\n';
    }
}

int main()
{
    char buffer[BUFFER_LEN];
    cout << "�|�[�g�ԍ��F";
    cin >> buffer;
    unsigned short portNum = atoi(buffer);

    char serverIP[40];
    cout << "�T�[�o�[��IP�A�h���X�F";
    cin >> serverIP;

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (result)
    {
        cout << "WSAStartup�̎��s�B\n";
        return -1;
    }
    cout << "WSAStartup�̐����B\n";

    SOCKET soc = socket(AF_INET, SOCK_STREAM, 0);
    if (soc < 0)
    {
        cout << "�\�P�b�g�I�[�v���G���[\n";
        WSACleanup();
        return -1;
    }

    HOSTENT* lpHost = gethostbyname(serverIP);
    if (lpHost == NULL)
    {
        unsigned int addr = inet_addr(serverIP);
        lpHost = gethostbyaddr((char*)&addr, 4, AF_INET);
    }
    if (lpHost == NULL)
    {
        cout << "gethostbyaddr�̃G���[\n";
        closesocket(soc);
        WSACleanup();
        return -1;
    }

    SOCKADDR_IN saddr;
    ZeroMemory(&saddr, sizeof(SOCKADDR_IN));
    saddr.sin_family = lpHost->h_addrtype;
    saddr.sin_port = htons(portNum);
    saddr.sin_addr.s_addr = *((u_long*)lpHost->h_addr);
    if (connect(soc, (SOCKADDR*)&saddr, sizeof(saddr)) == SOCKET_ERROR)
    {
        cout << "connect�̃G���[\n";
        closesocket(soc);
        WSACleanup();
        return -1;
    }
    else
        cout << "connect����\n";

    int board[BOARDSIZE][BOARDSIZE];
    InitMap(board);
    DrawMap(board);

    while (1)
    {
        cout << "�΂�u���ʒu����͂��Ă��������i��: A1�j�F";
        string input;
        int inputR, inputC;
        cin >> input;
        buffer[0] = input[0] - 'A';
        buffer[1] = input[1] - '0';
        buffer[2] = '\0';

        send(soc, buffer, (int)strlen(buffer), 0);

        int rcv = recv(soc, buffer, sizeof(buffer) - 1, 0);
        if (rcv == SOCKET_ERROR)
        {
            cout << "�G���[�ł��B\n";
            break;
        }
        cout << "rcv=" << rcv << '\n';
        buffer[rcv] = '\0';

        inputR = buffer[0];
        inputC = buffer[1];
        board[inputR][inputC] = 1;
        DrawMap(board);
    }

    shutdown(soc, SD_BOTH);
    closesocket(soc);
    cout << "�I��\n";

    result = WSACleanup();
    if (result)
    {
        cout << "WSACleanup�̎��s�B\n";
        return -1;
    }
    cout << "WSACleanup�̐����B\n";
}
