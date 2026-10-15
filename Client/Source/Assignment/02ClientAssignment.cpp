// ***********************************************************
// Client側課題 前回課題と重複するところはコピーしてください。

#include "02ClientAssignment.h"



ClientAssignment02::ClientAssignment02()
{
	mrsConnection = nullptr;
}

ClientAssignment02::~ClientAssignment02()
{
}

void ClientAssignment02::Initialize()
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
	playerManager->GetMyPlayer()->SetReady(true);

}

void ClientAssignment02::Finalize()
{
	if (mrsConnection != nullptr)
	{
		mrs_close(mrsConnection);
		mrsConnection = nullptr;
		mrs_update();
	}
	mrs_finalize();
}

void ClientAssignment02::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();
	// imgui
	ImGui::SetNextWindowPos(ImVec2(900, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);
	if (ImGui::Begin("Player Info", nullptr, ImGuiWindowFlags_None))
	{
		Player* player = playerManager->GetMyPlayer();
		DirectX::XMFLOAT3 position = player->GetPosition();
		ImGui::InputFloat3("Position", &position.x);
		player->SetPosition(position);
		// 回転
		DirectX::XMFLOAT3 angle = player->GetAngle();
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		player->SetAngle(angle);
		// スケール
		DirectX::XMFLOAT3 scale = player->GetScale();
		ImGui::InputFloat3("Scale", &scale.x);
		player->SetScale(scale);
		// ターゲット座標
		DirectX::XMFLOAT3 targetPosition = player->GetTargetPosition();
		ImGui::InputFloat3("TargetPosition", &targetPosition.x);
		player->SetTargetPosition(targetPosition);

		if (ImGui::Button(u8"タイトルへ"))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}
	}
	ImGui::End();
}

void ClientAssignment02::NetworkUpdate()
{
	mrs_update();
	// handle window
	HWND hWnd;
	// windowがアクティブか判断(非アクティブのときNULLが返ってくる)
	hWnd = GetActiveWindow();
	if (hWnd != NULL)
	{
		if (ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))return;
		if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))return;
		// マウスインプット情報取得
		Mouse& mouse = Input::Instance().GetMouse();
		// マウスボタンが押されている時
		if (mouse.GetButtonDown() & Mouse::BTN_LEFT || mouse.GetButtonDown() & Mouse::BTN_RIGHT)
		{
			// マウス座標取得
			DirectX::XMFLOAT3 clickPosition;
			clickPosition.x = static_cast<float>(mouse.GetPositionX());
			clickPosition.y = static_cast<float>(mouse.GetPositionY());
			clickPosition.z = 0;
			HitResult hit;
			// レイキャストで3D空間のクリック座標を取得
			if (GetClickPosition(clickPosition, hit))
			{
				PlayerInput input;
				input.clickPosition = hit.position;
				// 送信処理
				mrs::Buffer send;
				send.Write(&input, sizeof(input));
				//サーバに送信
				mrs_write_record(
					mrsConnection,
					MrsRecordOption::MRS_RECORD_OPTION_NONE,
					0,
					send.GetData(),
					send.GetDataLen()
				);
			}
		}
	}
}

void ClientAssignment02::Error(MrsConnection connection, void* connection_data, MrsConnectionError status)
{
	std::cout << mrs_get_connection_error_string(status) << std::endl;
}

void ClientAssignment02::Connect(MrsConnection connection, void* connectionData)
{
	std::cout << "Success Connection" << std::endl;
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	mrs_set_read_record_callback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	mrs_set_disconnect_callback(connection, Disconnect);
}

void ClientAssignment02::Disconnect(MrsConnection connection, void* connection_data)
{
	std::cout << "DisConnected" << std::endl;
}

void ClientAssignment02::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{
	// 紐づけておいたthisをClientAssignment02*に戻す
	//　資料のポインタ変数はこのselfのこと
	ClientAssignment02* self = static_cast<ClientAssignment02*>(connection_data);

	if (payload_len < sizeof(PlayerInput)) { return; }

	//受信データ(void*)を、PlayerInput型の変数へコピーして元の型に戻す
	//payloadはconst void* でキャストしづらいので、memcpyで変数に移す
	PlayerInput input;
	memcpy(&input, payload, sizeof(PlayerInput));

	//ログ画面に出力
	std::cout << "recv data" << std::endl;
	std::cout << "x:" << input.clickPosition.x
		<< "y:" << input.clickPosition.y
		<< "z:" << input.clickPosition.z << std::endl;
	
	//キャラクターを動かす
	Player* player = self->playerManager->GetMyPlayer();
	player->GetJobClass()->ChangeState(Player::State::Move);
	player->SetTargetPosition(input.clickPosition);

	// messagesに追加
	//mrs::Buffer buffer;


	//buffer.Clear();
}
