#include "09ServerAssignment.h"



// サーバ側コマンド入力スレッド
void ServerAssignment09::Exit()
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


void ServerAssignment09::EraseClient(Client* client)
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

void ServerAssignment09::Execute()
{
	// WinsockAPIを初期化
	WSADATA wsaData;




	// サーバの受付設定
	struct sockaddr_in addr;




	// ソケットの作成
	SOCKET sock;




	// ソケットと受付情報を紐づける



	// ノンブロッキングの設定
	u_long mode;




	// ******応用課題*******************
	// ******select関数準備*************




	// 受付を開始する






	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment09::Exit, this);



	// クライアントからの受付処理
	int size = sizeof(struct sockaddr_in);
	Client* newClient = new Client();
	Client* tempClient = newClient;
	do {
		// 接続要求を処理する
		{





		}	
		// クライアント全員からの受信処理
		{
			for (Client* client : clients)
			{
				char buffer[2048];
				// 受信処理





			}
		}
	} while (loop);
	


	th.join();
	// クライアントのソケット削除
	for (int i = 0; i < clients.size(); ++i)
	{


	}
	// 受付ソケットの切断



	// WSA終了



}