// ***********************************************************
// Server側課題 前回課題と重複するところはコピーしてください。
#include "04ServerAssignment.h"

#include <algorithm>	// std::find用
#include <cstring>		// memcpy, strlen用


ServerAssignment04::ServerAssignment04()
{

}
ServerAssignment04::~ServerAssignment04()
{

}
// サーバ側コマンド入力スレッド
void ServerAssignment04::Exit()
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
void ServerAssignment04::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// Accept で紐づけておいた this を取り出す
	ServerAssignment04* self = static_cast<ServerAssignment04*>(connection_data);
	
	switch (static_cast<NetworkTag>(payload_type))
	{
	case NetworkTag::Move:
	{
		if (payload_len < sizeof(PlayerInput)) { return; }

		//受信データをPlayerInputに戻す
		PlayerInput input;
		memcpy(&input, payload, sizeof(PlayerInput));

		//サーバのログに出力
		std::cout << "recv data Move" << std::endl;
		std::cout << "x:" << input.clickPosition.x
			<< "y:" << input.clickPosition.y
			<< "z:" << input.clickPosition.z << std::endl;

		//サーバが持っているそのプレイヤーのデータを更新する
		for (Client c : self->clients)
		{
			if (c.player->id == input.id)
			{
				c.player->position = input.position;
				c.player->targetPosition = input.clickPosition;
				c.player->state = Player::State::Move;
				break;
			}
		}

		//受け取ったデータを、同じタグのまま、送信者を含む全クライアントへ送る
		for (Client& c : self->clients)
		{
			mrs_write_record(
				c.mrsConnection,
				MrsRecordOption::MRS_RECORD_OPTION_NONE,
				static_cast<uint16>(NetworkTag::Move),
				&input,
				sizeof(PlayerInput)
			);
		}

		break;
	}
	case NetworkTag::Attack:
	{
		if (payload_len < sizeof(PlayerInput)) { return; }

		//受信データをPlayerInputに戻す
		PlayerInput input;
		memcpy(&input, payload, sizeof(PlayerInput));

		//サーバのログに出力
		std::cout << "recv data Attack" << std::endl;
		std::cout << "x:" << input.clickPosition.x
			<< "y:" << input.clickPosition.y
			<< "z:" << input.clickPosition.z << std::endl;

		for (Client& c : self->clients)
		{
			mrs_write_record(
				c.mrsConnection,
				MrsRecordOption::MRS_RECORD_OPTION_NONE,
				static_cast<uint16>(NetworkTag::Attack),
				&input,
				sizeof(PlayerInput)
			);
		}
		break;
	}
	case NetworkTag::Logout:
	{
		int senderId = -1;
		for (Client& c : self->clients)
		{
			if (c.mrsConnection == connection) { senderId = c.player->id; break; }
		}

		if (senderId < 0) { return; }
		//送信者以外の全員に送信者のIDを知らせる
		PlayerLogout logout;
		logout.id = senderId;

		for (Client& c : self->clients)
		{
			if (c.mrsConnection == connection) { continue; }

			mrs_write_record(
				c.mrsConnection,
				MrsRecordOption::MRS_RECORD_OPTION_NONE,
				static_cast<uint16>(NetworkTag::Logout),
				&logout,
				sizeof(PlayerLogout)
			);
		}
		//サーバからも削除
		self->EraseClient(connection);
		break;
	}
	case NetworkTag::Sync:
	{
		if (payload_len < sizeof(PlayerSync)) { return; }

		PlayerSync sync;
		memcpy(&sync, payload, sizeof(PlayerSync));
		// 送ってきたコネクションのクライアントを探して、そのデータだけ更新する。
		// sync.id ではなく connection で探すのは、他人のIDを名乗ったデータで
		// 別のプレイヤーを書き換えられないようにするため
		for (Client& c : self->clients)
		{
			if (c.mrsConnection != connection) { continue; }

			// サーバのデータを更新
			c.player->position = sync.position;
			c.player->targetPosition = sync.targetPosition;
			c.player->state = sync.state;

			// 更新したデータを、送信者以外の全クライアントへ送る。
			// IDは、受け取ったデータではなく、サーバが持っているIDを使う
			PlayerSync relay;
			relay.id = c.player->id;
			relay.position = c.player->position;
			relay.targetPosition = c.player->targetPosition;
			relay.state = c.player->state;

			for (Client& other : self->clients)
			{
				if (other.mrsConnection == connection) { continue; }	// 送信者は除く
				mrs_write_record(
					other.mrsConnection,
					MrsRecordOption::MRS_RECORD_OPTION_NONE,
					static_cast<uint16>(NetworkTag::Sync),
					&relay,
					sizeof(PlayerSync));
			}
			break;
		}
		break;
	}
	default:
		break;

	}
}
void ServerAssignment04::Disconnect(MrsConnection connection, void* connection_data)
{
	ServerAssignment04* self = static_cast<ServerAssignment04*>(connection_data);
	// 切断されたクライアントのIDを探す
	int logoutId = -1;
	for (Client& c : self->clients)
	{
		if (c.mrsConnection == connection) { logoutId = c.player->id; break; }
	}
	// 見つからない = すでに Logout で削除済み。何もしない
	if (logoutId < 0) { return; }

	// 先にサーバから削除する(切れたコネクションへ送らないため)
	self->EraseClient(connection);

	// 残っている全クライアントにIDを知らせる
	PlayerLogout logout;
	logout.id = logoutId;
	for (Client& c : self->clients)
	{
		mrs_write_record(
			c.mrsConnection,
			MrsRecordOption::MRS_RECORD_OPTION_NONE,
			static_cast<uint16>(NetworkTag::Logout),
			&logout,
			sizeof(PlayerLogout));
	}	//サーバからユーザーデータを削除する
	std::cout << "Disconnected PlayerId:" << self->id << std::endl;
}

// クライアントが接続してきたときに呼ばれる関数
void ServerAssignment04::Accept(MrsServer server, void* server_data, MrsConnection connection)
{
	// Execute で mrs_server_set_data に渡した this を取り出す
	ServerAssignment04* self = static_cast<ServerAssignment04*>(server_data);
	mrs_connection_set_data(connection, self);
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// ユーザが切断したときに呼ばれるCallBack関数の設定
	mrs_set_disconnect_callback(connection, Disconnect);

	std::cout << "New Connected" << std::endl;

	//サーバー側にプレイヤーを生成
	Player* player = new Player();
	player->id = self->id;
	player->position = DirectX::XMFLOAT3(0, 0, 0);
	player->angle = DirectX::XMFLOAT3(0, 0, 0);
	player->targetPosition = DirectX::XMFLOAT3(0, 0, 0);
	player->state = Player::State::Idle;

	Client client;
	client.mrsConnection = connection;
	client.player = player;

	self->AddID();//次の人のためにIDを進める
	self->clients.emplace_back(client);

	//新しいIDを接続者を含む全クライアントに送る（Login）
	PlayerLogin login;
	login.id = player->id;
	for (Client& c : self->clients)
	{
		mrs_write_record(
			c.mrsConnection,
			MrsRecordOption::MRS_RECORD_OPTION_NONE,
			static_cast<uint16>(NetworkTag::Login),
			&login,
			sizeof(PlayerLogin)
		);
	}

	//新しく来た人にだけ、すでにいるほかのクライアントの情報を送信
	for (Client& c : self->clients)
	{
		if (c.mrsConnection == connection) { continue; }

		PlayerSync sync;
		sync.id = c.player->id;
		sync.position = c.player->position;
		sync.targetPosition = c.player->targetPosition;
		//sync.position = c.player->position;
		//sync.targetPosition = c.player->targetPosition;
		sync.state = c.player->state;

		mrs_write_record(
			connection,
			MrsRecordOption::MRS_RECORD_OPTION_NONE,
			static_cast<uint16>(NetworkTag::Sync),
			&sync,
			sizeof(PlayerSync)
		);
	}
}

void ServerAssignment04::Execute()
{
	// Server課題 サーバ情報設定
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
	mrsServer = mrs_server_create(MRS_CONNECTION_TYPE_UDP, address, port, backlog);
	if (mrsServer == nullptr) { return; }

	//サーバにthisを紐づける(Acceptで取り出して使う）
	mrs_server_set_data(mrsServer, this);
	//新しい接続が来た時に呼ばれる関数を登録
	mrs_server_set_new_connection_callback(mrsServer, Accept);

	// サーバ側からコマンド入力で終了されるまでループする。
	// キーボードでexitを入力するとループを抜けるための別スレッドを用意
	std::thread th(&ServerAssignment04::Exit, this);

	do {
		// スタックされているデータの送信、受信を行う。
		mrs_update();
	} while (loop);
	th.join();

	// クライアント全削除
	for (Client client : clients)
	{
		mrs_close(client.mrsConnection);
		client.mrsConnection = nullptr;
		delete client.player;
		client.player = nullptr;
	}
	clients.clear();

	// mrs終了処理
	mrs_close(mrsServer);
	mrs_update();
	mrs_finalize();

}

void ServerAssignment04::EraseClient(MrsConnection connection) {
	int i = 0;
	for (Client client : clients)
	{
		if (client.mrsConnection == connection)
		{
			mrs_close(client.mrsConnection);
			delete client.player;
			client.player = nullptr;
			break;
		}
		++i;
	}
	clients.erase(clients.begin() + i);
}