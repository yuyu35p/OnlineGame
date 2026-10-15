// ***********************************************************
// Server側課題 前回課題と重複するところはコピーしてください。
#include "08ServerAssignment.h"



// サーバ側コマンド入力スレッド
void ServerAssignment08::Exit()
{
	while (loop) {
		std::string input;
		std::cin >> input;
		if (input == "exit")
		{
			loop = false;
		}
	}

}

// 受信関数
void ServerAssignment08::Recieve(Client* client)
{
	do {
		char buffer[2048];




	} while (loop);
}

// クライアント削除関数
void ServerAssignment08::EraseClient(Client* client)
{
	int i = 0;
	for (i = 0; i < clients.size();++i)
	{
		if (clients.at(i)->sock == client->sock)
		{
			int r = closesocket(client->sock);
			if (r != 0)
			{
				std::cout << "Close Socket Failed." << std::endl;
			}
			client->sock = INVALID_SOCKET;
			delete client->player;
			break;
		}
	}
	clients.erase(clients.begin() + i);
}

void ServerAssignment08::Execute()
{
	// WinsockAPIを初期化
	WSADATA wsaData;




	// サーバの受付設定
	struct sockaddr_in addr;




	// ソケットの作成
	SOCKET sock;




	// ソケットと受付情報を紐づける




	// 受付を開始する




	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment08::Exit, this);
	
	// クライアントからの受付処理
	int size = sizeof(struct sockaddr_in);
	do {
		Client* newClient = new Client();
		// 接続要求を処理する







	} while (loop);


	th.join();
	for (std::thread* thread : recvThreads)
	{
		thread->join();
	}

	// クライアントのソケット削除
	for (int i = 0; i < clients.size();++i)
	{


	}
	// 受付ソケットの切断



	// WSA終了



}