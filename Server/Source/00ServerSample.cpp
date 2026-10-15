#include "00ServerSample.h"


// サーバ側コマンド入力スレッド
void ServerSample::Exit()
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

// データ受信したときに呼ばれる関数
// MrsConnection connection -> 送信データを送ってきたクライアントとのコネクション
// void* connection_data -> mrs_connection_set_data関数でコネクションに紐づけたオブジェクトのポインタ
// uint32 seqnum -> 送信データ番号
// uint16 options -> 通信オプション(暗号化など)
// uint16 payload_type -> 送信データの種類
// const void* payload -> 送信データ
// uint32 payload_len -> 送信データのサイズ
void ServerSample::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// mrs用のバッファ
	mrs::Buffer buffer;
	// 受信データをバッファに書き込み
	// payloadは汎用ポインタのため型が不明であり、
	// const修飾子が付いているためキャストが行えない。
	// そのため一度mrs::Bufferに保存する
	buffer.Write(payload, payload_len);
	char payloadStr[256];
	// バッファデータからpayLoadStrのアドレスにbufferに保存されたデータをコピー
	if (!buffer.Read(payloadStr, payload_len)) {
		std::cout << "Read Error" << std::endl;
		return;
	}
	// 受信データを表示
	std::cout << payloadStr << std::endl;
}

// クライアントが接続してきたときに呼ばれる関数
// MrsServer server -> 接続受付をしたサーバオブジェクト
// void* server_data -> mrs_server_set_data関数でサーバオブジェクトに紐づけられたオブジェクトのポインタ
// MrsConnection connection -> 接続しにきたクライアントとのコネクション
void ServerSample::Accept(MrsServer server, void* server_data, MrsConnection connection)
{
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	mrs_set_disconnect_callback(connection, Disconnect);
}

void ServerSample::Disconnect(MrsConnection connection, void* connection_data)
{
	// 切断されたとき
	std::cout << "Disconnected" << std::endl;
}

void ServerSample::Error(MrsConnection connection, void* connection_data, MrsConnectionError status)
{
	// エラー発生したとき
	std::cout << "Error Code:" << static_cast<int>(status) << std::endl;
}


void ServerSample::Execute()
{
	// mrsライブラリの初期化
	// trueなら正常に完了。falseの場合失敗。
	if (!mrs_initialize())
	{
		std::cout << "Server Initialize NG" << std::endl;
		return;
	}
	else
	{
		std::cout << "Server Initialize OK" << std::endl;
	}

	// Server課題 サーバ情報設定
	// IPアドレス、ポート番号、バックログ数を指定
	// サーバ側のIPアドレスは受付アドレス。
	// 指定したIPアドレスの端末から接続を許可をする。
	// 不特定からのアクセスを許可する場合はINADDR_ANYアドレスを指定する。
	// INADDR_ANYアドレスは開発環境によって異なるがC/C++の場合は0.0.0.0が該当する。
	char address[40] = { "0.0.0.0" };
	// Port番号の指定。
	uint16 port = 7000;
	// 接続依頼待機キューのサイズを指定。
	// 接続依頼のデータを一つずつ処理を行っていくが
	// 同時にアクセスされた場合どちらかしか処理を行えない。
	// そのため、接続依頼は専用のキューに一度保存される。
	// そのキューのサイズを指定する。
	uint32 backlog = 10;


	// サーバオブジェクト作成(accept実行)
	//	MRS_CONNECTION_TYPE_TCP -> 通信プロトコルの指定。TCPの場合は左記を指定
	//	接続受け入れIPアドレスの指定
	//	データ受け入れのポート番号を指定
	//	接続依頼キューのサイズを指定
	mrsServer = mrs_server_create(MRS_CONNECTION_TYPE_TCP, address, port, backlog);
	if (mrsServer == nullptr)
	{
		std::cout << "Server Accept NG" << std::endl;
		return;
	}
	
	// 接続されたときのコールバック関数設定
	//	mrs_server_createで作ったサーバオブジェクトの指定
	//	接続成功時に実行したい処理の関数ポインタを指定
	mrs_server_set_new_connection_callback(mrsServer, Accept);

	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerSample::Exit, this);

	do {
		// スタックされているデータの送信、受信を行う。
		mrs_update();
	} while (loop);
	th.join();

	// サーバオブジェクトの解放。
	// 実際にこのタイミングでは解放されずに
	// 次に実行されるmrs_update関数が呼び出された際に解放が実行されます。
	mrs_close(mrsServer);
	mrs_update();

	// mrs終了処理
	// 全ての接続を解放する
	mrs_finalize();
}