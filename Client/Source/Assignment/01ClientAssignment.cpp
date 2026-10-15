// ***********************************************************
// Client課題 00Sampleを参考に実装
#include "01ClientAssignment.h"



ClientAssignment01::ClientAssignment01()
{
	mrsConnection = nullptr;
	input[0] = '\0';
}

ClientAssignment01::~ClientAssignment01()
{
}

void ClientAssignment01::Initialize()
{
	SceneBase::Initialize();

	//MRSライブラリ初期化
	if (!mrs_initialize()) { std::cout << "mrs_initialize not faild" << std::endl; }
	else { std::cout << "mrs_initialize success" << std::endl; }
	
	//接続開始
	char address[40] = { "127.0.0.1" };
	uint16 port = 7000;
	uint32 timeout = 5000;
	mrsConnection = mrs_connect(MRS_CONNECTION_TYPE_TCP, address, port, timeout);

	//コネクションにthisを紐づける
	mrs_connection_set_data(mrsConnection, this);

	//エラー時に呼ばれる関数を登録
	mrs_set_error_callback(mrsConnection, Error);
	//接続に成功したときに呼ばれる関数を登録
	mrs_set_connect_callback(mrsConnection, Connect);
	//切断時に呼ばれる関数を登録
	mrs_set_disconnect_callback(mrsConnection, Disconnect);


	// Player設定
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
	playerManager->GetMyPlayer()->SetReady(true);

}

void ClientAssignment01::Finalize()
{
	if (mrsConnection != nullptr)
	{
		mrs_close(mrsConnection);
		mrsConnection = nullptr;
		mrs_update();
	}
	mrs_finalize();
}

void ClientAssignment01::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();
	// imgui
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 620), ImGuiCond_Once);
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
				mrs::Buffer send;
				send.Write(input, sizeof(input));

				mrs_write_record(
					mrsConnection,
					MrsRecordOption::MRS_RECORD_OPTION_NONE,
					0,
					send.GetData(),
					send.GetDataLen()
				);


			}
			input[0] = '\0';
		}
		if (ImGui::Button(u8"タイトルへ"))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}

	}
	ImGui::End();
}

void ClientAssignment01::NetworkUpdate()
{
	mrs_update();
}

void ClientAssignment01::Error(MrsConnection connection, void* connection_data, MrsConnectionError status)
{
	std::cout << mrs_get_connection_error_string(status) << std::endl;
}

void ClientAssignment01::Connect(MrsConnection connection, void* connectionData)
{
	std::cout << "Success Connection" << std::endl;
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	mrs_set_disconnect_callback(connection, Disconnect);

}

void ClientAssignment01::Disconnect(MrsConnection connection, void* connection_data)
{
	std::cout << "DisConnected" << std::endl;
}

void ClientAssignment01::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// connectionに紐づいているポインタからメンバ変数を参照する
	ClientAssignment01* self = static_cast<ClientAssignment01*>(mrs_connection_get_data(connection));
	// messagesに追加
	mrs::Buffer buffer;
	buffer.Write(payload, payload_len);

	char payloadData[256]{};

	// バッファデータからpayLoadStrにデータ読み込み
	if (!buffer.Read(payloadData, payload_len)) {
		std::cout << "read error" << std::endl;
	}

	std::cout << payloadData << std::endl;
	// コネクションに紐づいているメンバ変数messagesに登録
	self->messages.emplace_back(payloadData);
	buffer.Clear();
}
