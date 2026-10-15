#include "05ServerAssignment.h"



// サーバ側コマンド入力スレッド
void ServerAssignment05::Exit()
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

void ServerAssignment05::Execute()
{
	// WinsockAPIを初期化
	WSADATA wsaData;


	// サーバの受付設定
	struct sockaddr_in addr;


	// ソケットの作成
	SOCKET sock;




	// ソケットと受付情報を紐づける




	// 受付を開始する




	// クライアントからの受付処理



	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment05::Exit, this);

	do {

	} while (loop);
	th.join();

	// クライアントのソケット切断



	// 受付ソケットの切断



	// WSA終了



}