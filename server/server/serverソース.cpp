#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define BUFFER_LEN 1000
char buffer[BUFFER_LEN];
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
                cout << "○";
            else
                cout << "＿";
        }
        cout << '\n';
    }
}

int main()
{
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(1, 1), &wsaData);
    if (result)
    {
        cout << "WSAStartupの失敗。\n";
        return -1;
    }
    cout << "WSAStartupの成功。\n";

    SOCKET listenSoc = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSoc < 0)
    {
        cout << "ソケットオープンエラー\n";
        WSACleanup();
        return -1;
    }
    cout << "リスンソケットオープン完了。\n";

    SOCKADDR_IN saddr;
    ZeroMemory(&saddr, sizeof(SOCKADDR_IN));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(55555);
    saddr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenSoc, (struct sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR)
    {
        cout << "bindのエラー\n";
        closesocket(listenSoc);
        WSACleanup();
        return -1;
    }
    else
        cout << "bind成功\n";

    if (listen(listenSoc, 0) == SOCKET_ERROR)
    {
        cout << "listen error.\n";
        closesocket(listenSoc);
        WSACleanup();
        return -1;
    }
    else
        cout << "listen成功\n";

    SOCKADDR_IN from;
    int fromlen = sizeof(from);
    SOCKET soc = accept(listenSoc, (SOCKADDR*)&from, &fromlen);
    if (soc == INVALID_SOCKET)
    {
        cout << "accept error.\n";
        closesocket(listenSoc);
        WSACleanup();
        return -1;
    }
    else
        cout << inet_ntoa(from.sin_addr) << "が接続してきました。\n";

    closesocket(listenSoc);
    cout << "リスンソケットクローズ完了。\n";

    int board[BOARDSIZE][BOARDSIZE];
    InitMap(board);
    DrawMap(board);

    while (1)
    {
        string input;
        int inputR, inputC;
        cout << "石を置く位置を入力してください（例: A1）：";
        cin >> input;
        buffer[0] = input[0] - 'A';
        buffer[1] = input[1] - '0';
        buffer[2] = '\0';

        send(soc, buffer, int(strlen(buffer)), 0);

        int rcv = recv(soc, buffer, sizeof(buffer) - 1, 0);
        if (rcv == SOCKET_ERROR)
        {
            cout << "エラーです。\n";
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
    cout << "本体終了\n";

    result = WSACleanup();
    if (result)
    {
        cout << "WSACleanupの失敗。\n";
        return -1;
    }
    cout << "WSACleanupの成功。\n";
}
