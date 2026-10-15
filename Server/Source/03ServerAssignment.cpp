// ***********************************************************
// Server側課題 前回課題と重複するところはコピーしてください。
#include "03ServerAssignment.h"

#include <algorithm>	// std::find用
#include <cstring>		// memcpy, strlen用

// サーバ側コマンド入力スレッド
// cin は入力があるまで止まるので、メインループとは別スレッドで待つ
void ServerAssignment03::Exit()
{
	while (loop) {
		std::string input;
		std::cin >> input;
		if (input == "exit")
		{
			loop = false; // メインループを抜けさせる
		}
	}
}

// データ受信したときに呼ばれる関数
void ServerAssignment03::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// Accept で紐づけておいた this を取り出す
	ServerAssignment03* self = static_cast<ServerAssignment03*>(connection_data);

	switch (static_cast<NetworkTag>(payload_type))
	{
	case NetworkTag::Attack:
	{
		if (payload_len < sizeof(PlayerInput)) { return;}

		// 受信データを PlayerInput に戻す(クライアントと同じ要領)
		PlayerInput input;
		memcpy(&input, payload, sizeof(PlayerInput));

		// サーバのログに出力
		std::cout << "recv data attack" << std::endl;
		std::cout << "x:" << input.clickPosition.x
				  << "y:" << input.clickPosition.y
				  << "z:" << input.clickPosition.z << std::endl;

		// 送ってきたクライアントへ、そのまま送り返す。
		// クライアントは自分の「MyPlayer」を動かすので、全員に送らず送信元だけに返す
		mrs_write_record(
			connection,
			MrsRecordOption::MRS_RECORD_OPTION_NONE,
			static_cast<uint16>(NetworkTag::Attack),
			&input,
			sizeof(PlayerInput));

		break;
	}
	case NetworkTag::Message:
	{
		//受信用バッファ（256バイト）に入りきらないサイズなら読まない
		char text[256]{};
		if (payload_len > sizeof(text) - 1) { return; }

		//受信データを文字列として取り出す
		mrs::Buffer buffer;
		buffer.Write(payload, payload_len);
		// バッファデータからpayLoadStrのアドレスにbufferに保存されたデータをコピー
		if (!buffer.Read(text, payload_len)) {
			std::cout << "Read Error" << std::endl;
			return;
		}

		// 受信データを表示
		std::cout << "recv data message" << std::endl;
		std::cout << text << std::endl;

		//メッセージは接続中の全クライアントへ送り返す（チャットなので全員に見せる）
		for (MrsConnection c : self->connection)
		{
			mrs_write_record(
				c,                                          // 送信先コネクション
				MrsRecordOption::MRS_RECORD_OPTION_NONE,    // 通信オプション(なし)
				static_cast<uint16>(NetworkTag::Message),                                          // データコマンド
				text,                                       // 送信データ
				static_cast<uint32>(strlen(text)) + 1);     // 終端の\0まで含めたサイズ
		}
		break;
	}
	case NetworkTag::Move:
	{
		if (payload_len < sizeof(PlayerInput)) { return; }

		PlayerInput input;
		// 受信データを PlayerInput に戻す(クライアントと同じ要領)
		memcpy(&input, payload, sizeof(PlayerInput));

		// サーバのログに出力
		std::cout << "recv data move" << std::endl;
		std::cout << "x:" << input.clickPosition.x
				  << "y:" << input.clickPosition.y
				  << "z:" << input.clickPosition.z << std::endl;

		// 送ってきたクライアントへ、そのまま送り返す。
		// クライアントは自分の「MyPlayer」を動かすので、全員に送らず送信元だけに返す
		mrs_write_record(
			connection,
			MrsRecordOption::MRS_RECORD_OPTION_NONE,
			static_cast<uint16>(NetworkTag::Move),
			&input,
			sizeof(PlayerInput));
		break;
	}
	default:
		break;
	}

}
//クライアントが切断したときに呼ばれる
void ServerAssignment03::Disconnect(MrsConnection connection, void* connection_data)
{
	ServerAssignment03* self = static_cast<ServerAssignment03*>(connection_data);
	//接続リストから削除（残すと、閉じたコネクションへ送信しようとしてしまう）
	auto it = std::find(self->connection.begin(), self->connection.end(), connection);
	if (it != self->connection.end())
	{
		self->connection.erase(it);
	}
	std::cout << "Disconnected" << std::endl;
}

// クライアントが接続してきたときに呼ばれる関数
void ServerAssignment03::Accept(MrsServer server, void* server_data, MrsConnection connection)
{
	// Execute で mrs_server_set_data に渡した this を取り出す
	ServerAssignment03* self = static_cast<ServerAssignment03*>(server_data);
	mrs_connection_set_data(connection, self);
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	mrs_set_disconnect_callback(connection, Disconnect);
	// 送り返す対象として、接続リストに追加
	//メッセージを全員に返せるよう、接続リストに追加
	self->connection.push_back(connection);
	std::cout << "New Connected" << std::endl;

}

void ServerAssignment03::Execute()
{
	//ライブラリの初期化
	if (!mrs_initialize())
	{
		return;
	}
	//0.0.0.0 = どのIPからの接続も受け付ける
	char address[40] = { "0.0.0.0" };
	uint16 port = 7000;		//クライアントと同じポート番号
	uint32 backlog = 10;	//接続待ちキューのサイズ

	//サーバオブジェクトを作り、接続の受付を開始する
	mrsServer = mrs_server_create(MRS_CONNECTION_TYPE_TCP, address, port, backlog);
	if (mrsServer == nullptr) { return; }

	//サーバにthisを紐づける(Acceptで取り出して使う）
	mrs_server_set_data(mrsServer, this);
	//新しい接続が来た時に呼ばれる関数を登録
	mrs_server_set_new_connection_callback(mrsServer, Accept);

	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意し、開始
	std::thread th(&ServerAssignment03::Exit, this);

	// exitが入力されるまで、送受信とコールバック呼び出しを回し続ける
	do {
		// スタックされているデータの送信、受信を行う。
		mrs_update();
	} while (loop);
	th.join();

	// mrs終了処理
	mrs_close(mrsServer);
	mrs_update();
	mrs_finalize();

}