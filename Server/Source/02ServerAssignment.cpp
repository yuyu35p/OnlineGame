// ***********************************************************
// Server側課題 前回課題と重複するところはコピーしてください。
#include "02ServerAssignment.h"

#include <algorithm>
#include <cstring>



// サーバ側コマンド入力スレッド
void ServerAssignment02::Exit()
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
void ServerAssignment02::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// 受信データを PlayerInput に戻す(クライアントと同じ要領)
	PlayerInput input;
	memcpy(&input, payload, sizeof(PlayerInput));

	// サーバのログに出力
	std::cout << "recv data" << std::endl;
	std::cout << "x:" << input.clickPosition.x
		<< "y:" << input.clickPosition.y
		<< "z:" << input.clickPosition.z << std::endl;

	// 送ってきたクライアントへ、そのまま送り返す。
	// クライアントは自分の「MyPlayer」を動かすので、全員に送らず送信元だけに返す
	mrs_write_record(
		connection,
		MrsRecordOption::MRS_RECORD_OPTION_NONE,
		0,
		&input,
		sizeof(PlayerInput));

}
void ServerAssignment02::Disconnect(MrsConnection connection, void* connection_data)
{
	ServerAssignment02* self = static_cast<ServerAssignment02*>(connection_data);
	auto it = std::find(self->connection.begin(), self->connection.end(), connection);
	if (it != self->connection.end())
	{
		self->connection.erase(it);
	}
	std::cout << "Disconnected" << std::endl;
}

// クライアントが接続してきたときに呼ばれる関数
void ServerAssignment02::Accept(MrsServer server, void* server_data, MrsConnection connection)
{
	// Execute で mrs_server_set_data に渡した this を取り出す
	ServerAssignment02* self = static_cast<ServerAssignment02*>(server_data);
	mrs_connection_set_data(connection, self);
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	mrs_set_disconnect_callback(connection, Disconnect);
	// 送り返す対象として、接続リストに追加
	self->connection.push_back(connection);
	std::cout << "New Connected" << std::endl;
}

void ServerAssignment02::Execute()
{
	if (!mrs_initialize())
	{
		return;
	}

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
	std::thread th(&ServerAssignment02::Exit, this);

	do {
		// スタックされているデータの送信、受信を行う。
		mrs_update();
	} while (loop);
	th.join();

	mrs_close(mrsServer);
	mrs_update();
	// mrs終了処理
	mrs_finalize();

}