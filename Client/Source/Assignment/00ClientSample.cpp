#include "00ClientSample.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"


ClientSample00::ClientSample00()
{
	mrsConnection = nullptr;
	input[0] = '\0';
}

ClientSample00::~ClientSample00()
{
}

void ClientSample00::Initialize()
{
	SceneBase::Initialize();

	// MRSライブラリ初期化
	if (!mrs_initialize())
	{
		std::cout << "faild mrs_initialize..." << std::endl;
	}
	else
	{
		std::cout << "Success mrs_initialize" << std::endl;
	}
	// 接続先サーバのIPアドレス指定
	char address[40] = { "127.0.0.1" };
	// 接続先サーバのポート番号
	uint16 port = 7000;
	// タイムアウト時間
	uint32 timeout = 5000;
	// Connection
	mrsConnection = mrs_connect(MRS_CONNECTION_TYPE_TCP, address, port, 5000);

	// コールバック関数はstatic関数なので
	// コールバック関数内から非静的メンバ変数にアクセスできない。
	// そのため、connectionにポインタを紐づけておく
	mrs_connection_set_data(mrsConnection, this);

	// Setup CallBack
	// エラーが発生したときに実行される関数
	mrs_set_error_callback(mrsConnection, Error);
	// 接続に成功したときに実行される関数
	mrs_set_connect_callback(mrsConnection, Connect);

	// Plaer設定
	playerManager = new PlayerManager();
	playerManager->SetMyPlayerID(0);

	Player* player = new Player();

	// 各プレイヤーのキャラクターに合わせた設定を行う
	// JobClassはステートマシン
	player->SetPlayerID(0);
	player->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	player->SetScale(DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f));
	player->SetModel();

	playerManager->AddPlayer(player);


}

void ClientSample00::Finalize()
{
	// server終了処理
	if (mrsConnection != nullptr)
	{
		// mrs_closeを実行したタイミングでは解放はされない。
		// 次にmrs_updateが実行されたタイミングで解放は行われる
		mrs_close(mrsConnection);
		mrsConnection = nullptr;
		mrs_update();
	}

	// ライブラリ解放
	mrs_finalize();
}

void ClientSample00::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();
	// imgui
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 600), ImGuiCond_Once);
	if (ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_None))
	{
		ImGui::Text("Message");
		ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(250, 470), ImGuiWindowFlags_NoTitleBar);
		for (std::string message : messages) {
			ImGui::Text(u8"%s", message.c_str());
		}
		ImGui::EndChild();
		ImGui::Spacing();

		ImGui::InputText("Message", input, sizeof(input));
		if (ImGui::Button("Send"))
		{
			if (strcmp(input, "") != 0)
			{
				// 送信処理
				// 送信用バッファにデータを書き込む
				mrs::Buffer send;
				send.Write(&input, sizeof(input));
				
				// 送信データを送信レコードに書き込む
				// 送信レコードは次にmrs_update関数が実行されたタイミングで送られる
				mrs_write_record(
					mrsConnection,								// 送信先コネクション
					MrsRecordOption::MRS_RECORD_OPTION_NONE,	// 通信オプション
					0,											// データコマンド
					send.GetData(),								// 送信データ
					send.GetDataLen()							// 送信サイズ
				);
			}
			input[0] = '\0';
		}
	}
	ImGui::End();
}

// ネットワーク更新
void ClientSample00::NetworkUpdate()
{
	mrs_update();
}

// エラーが発生したときに呼ばれる
void ClientSample00::Error(MrsConnection connection, void* connection_data, MrsConnectionError status)
{
	// エラー文字列出力
	std::cout << mrs_get_connection_error_string(status) << std::endl;
}

// 接続時に呼ばれる関数
void ClientSample00::Connect(MrsConnection connection, void* connectionData)
{
	std::cout << "Success Connection" << std::endl;
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	mrs_set_disconnect_callback(connection, Disconnect);

}

// 切断時に呼ばれる関数
void ClientSample00::Disconnect(MrsConnection connection, void* connection_data)
{
	std::cout << "DisConnected" << std::endl;
}

// レコードが読み込めるタイミングで呼ばれる関数
void ClientSample00::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientSample00* self = static_cast<ClientSample00*>(connection_data);
	// データ読み取るためにバッファオブジェクトを用意
	mrs::Buffer buffer;
	// バッファに受信データを書き込む
	buffer.Write(payload, payload_len);

	char payloadData[256]{};

	// バッファデータからpayLoadStrにデータ読み込み
	if (!buffer.Read(payloadData, payload_len)) {
		std::cout << "read error" << std::endl;
	}

	// 受信データを表示
	std::cout << payloadData << std::endl;
	// コネクションに紐づいているメンバ変数messagesに登録
	self->messages.emplace_back(payloadData);
	buffer.Clear();
}
