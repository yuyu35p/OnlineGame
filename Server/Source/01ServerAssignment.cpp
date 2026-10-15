// ***********************************************************
// Server課題 00Sampleを参考に実装
#include "01ServerAssignment.h"



// サーバ側コマンド入力スレッド
void ServerAssignment01::Exit()
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
void ServerAssignment01::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// Accept で紐づけておいた this を取り出す
	ServerAssignment01* self = static_cast<ServerAssignment01*>(connection_data);

	mrs::Buffer buffer;
	buffer.Write(payload, payload_len);
	char payloadStr[256];
	// バッファデータからpayLoadStrのアドレスにbufferに保存されたデータをコピー
	if (!buffer.Read(payloadStr, payload_len)) {
		std::cout << "Read Error" << std::endl;
		return;
	}
	// 受信データを表示
	std::cout << payloadStr << std::endl;

	//クライアントに送り返す
	for (MrsConnection c : self->connection)
	{
		mrs_write_record(
			c,                                          // 送信先コネクション
			MrsRecordOption::MRS_RECORD_OPTION_NONE,    // 通信オプション(なし)
			0,                                          // データコマンド
			payloadStr,                                       // 送信データ
			static_cast<uint32>(strlen(payloadStr)) + 1);     // 終端の\0まで含めたサイズ
	}
}
void ServerAssignment01::Disconnect(MrsConnection connection, void* connection_data)
{

	std::cout << "Disconnected" << std::endl;
}

// クライアントが接続してきたときに呼ばれる関数
void ServerAssignment01::Accept(MrsServer server, void* server_data, MrsConnection connection)
{
	// Execute で mrs_server_set_data に渡した this を取り出す
	ServerAssignment01* self = static_cast<ServerAssignment01*>(server_data);
	mrs_connection_set_data(connection, self);
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	mrs_set_disconnect_callback(connection, Disconnect);
	// 送り返す対象として、接続リストに追加
	self->connection.push_back(connection);
	std::cout << "New Connected" << std::endl;
}

void ServerAssignment01::Execute()
{
	if (!mrs_initialize())
	{
		return;
	}

	// IPアドレス、ポート番号、バックログ数を指定
	char address[40] = { "0.0.0.0" };
	uint16 port = 7000;
	uint32 backlog = 10;

	//サーバオブジェクトを作り、接続の受付を開始する
	mrsServer = mrs_server_create(MRS_CONNECTION_TYPE_TCP, address, port, backlog);
	if (mrsServer == nullptr) { return; }

	//サーバにthisを紐づける
	mrs_server_set_data(mrsServer, this);
	//新しい接続が来た時に呼ばれる関数を登録
	mrs_server_set_new_connection_callback(mrsServer, Accept);

	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment01::Exit, this);

	do {
		// スタックされているデータの送信、受信を行う。
		mrs_update();
	} while (loop);
	th.join();

	mrs_close(mrsServer);

	// mrs終了処理
	mrs_finalize();
}