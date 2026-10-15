// ***********************************************************
// Client側課題 前回課題と重複するところはコピーしてください。

#include "04ClientAssignment.h"



ClientAssignment04::ClientAssignment04()
{
	mrsConnection = nullptr;
}

ClientAssignment04::~ClientAssignment04()
{

}

void ClientAssignment04::Initialize()
{
	SceneBase::Initialize();

	//MRSライブラリ初期化
	if (!mrs_initialize()) { std::cout << "mrs_initialize not faild" << std::endl; }
	else { std::cout << "mrs_initialize success" << std::endl; }

	//接続開始
	char address[40] = { "127.0.0.1" };
	//char address[40] = { "192.168.137.1" };
	//char address[40] = { "10.108.0.249" };
	uint16 port = 7000;
	uint32 timeout = 5000;
	mrsConnection = mrs_connect(MRS_CONNECTION_TYPE_UDP, address, port, timeout);

	//コネクションにthisを紐づける
	mrs_connection_set_data(mrsConnection, this);

	//接続に成功したときに呼ばれる関数を登録
	mrs_set_connect_callback(mrsConnection, Connect);
	//切断時に呼ばれる関数を登録
	mrs_set_disconnect_callback(mrsConnection, Disconnect);

	playerManager = new PlayerManager();
}

void ClientAssignment04::Finalize()
{
	//終了処理
	if (mrsConnection != nullptr)
	{
		// mrs_closeの時点で解放されず、次のmrs_updateで解放される
		mrs_close(mrsConnection);
		mrsConnection = nullptr;
		mrs_update();
	}
	//ライブラリ全体を解放
	mrs_finalize();
}

void ClientAssignment04::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();

	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_Once);
	if (ImGui::Begin("Title", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		if (ImGui::Button(u8"タイトルへ"))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}

	}
	ImGui::End();
	ImGui::SetNextWindowPos(ImVec2(900, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);
	if (ImGui::Begin("Player Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		int i = 0;
		for (Player* player : playerManager->GetPlayers())
		{
			std::string text = "Player ID : " + std::to_string(player->GetPlayerID());
			ImGui::Text(text.c_str());
			DirectX::XMFLOAT3 position = player->GetPosition();
			text = "Position" + std::to_string(player->GetPlayerID());
			ImGui::InputFloat3(text.c_str(), &position.x);
			player->SetPosition(position);
			// 回転
			DirectX::XMFLOAT3 angle = player->GetAngle();
			DirectX::XMFLOAT3 a;
			a.x = DirectX::XMConvertToDegrees(angle.x);
			a.y = DirectX::XMConvertToDegrees(angle.y);
			a.z = DirectX::XMConvertToDegrees(angle.z);
			text = "Angle" + std::to_string(player->GetPlayerID());
			ImGui::InputFloat3(text.c_str(), &a.x);
			angle.x = DirectX::XMConvertToRadians(a.x);
			angle.y = DirectX::XMConvertToRadians(a.y);
			angle.z = DirectX::XMConvertToRadians(a.z);
			player->SetAngle(angle);
			// スケール
			DirectX::XMFLOAT3 scale = player->GetScale();
			text = "Scale" + std::to_string(player->GetPlayerID());
			ImGui::InputFloat3(text.c_str(), &scale.x);
			player->SetScale(scale);
			// ターゲット座標
			DirectX::XMFLOAT3 targetPosition = player->GetTargetPosition();
			text = "TargetPosition" + std::to_string(player->GetPlayerID());
			ImGui::InputFloat3(text.c_str(), &targetPosition.x);
			player->SetTargetPosition(targetPosition);
		}
	}
	ImGui::End();
}

void ClientAssignment04::NetworkUpdate()
{
	//送信の実行・受信・コールバック呼び出しを行う。毎フレーム最初に呼ぶ
	mrs_update();

	//2フレームに1回、自分の情報をサーバへ送る
	timer++;
	if (timer >= 2)
	{
		timer = 0;

		Player* myPlayer = playerManager->GetMyPlayer();
		if (myPlayer != nullptr)
		{
			PlayerSync sync;
			sync.id = myPlayer->GetPlayerID();
			sync.position = myPlayer->GetPosition();
			sync.targetPosition = myPlayer->GetTargetPosition();
			sync.state = myPlayer->GetState();

			mrs_write_record(
				mrsConnection,
				MrsRecordOption::MRS_RECORD_OPTION_NONE,
				static_cast<uint16>(NetworkTag::Sync),
				&sync,
				sizeof(PlayerSync)
			);
		}
	}



	//自分のIDが決まる前は無視
	if (!isLogin) { return; }
	
	// handle window
	HWND hWnd;
	// windowがアクティブか判断(非アクティブのときNULLが返ってくる)
	hWnd = GetActiveWindow();
	if (hWnd != NULL)
	{
		if (!ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))
		{
			if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			{
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

					// IMGUIとの当たり判定チェック
					HitResult hit;
					// レイキャストで3D空間のクリック座標を取得
					if (GetClickPosition(clickPosition, hit))
					{
						//送るデータ（クリックした３Ｄ座標）
						Player* myPlayer = playerManager->GetMyPlayer();
						PlayerInput playerInput;
						playerInput.id = myPlayer->GetPlayerID();
						playerInput.position = myPlayer->GetPosition();
						playerInput.clickPosition = hit.position;

						mrs::Buffer send;
						send.Write(&playerInput, sizeof(playerInput));

						//クリックしたボタンによってタグを決める
						//左クリックー＞攻撃　右クリックー＞移動
						NetworkTag tag{};
						if (mouse.GetButtonDown() & Mouse::BTN_LEFT) { tag = NetworkTag::Attack; }
						else if (mouse.GetButtonDown() & Mouse::BTN_RIGHT) { tag = NetworkTag::Move; }

						//サーバに送信
						mrs_write_record(
							mrsConnection,
							MrsRecordOption::MRS_RECORD_OPTION_NONE,
							static_cast<uint16>(tag),
							send.GetData(),
							send.GetDataLen()
						);
					}
				}
			}
		}		
	}
}

void ClientAssignment04::Connect(MrsConnection connection, void* connectionData)
{
	std::cout << "Success Connection" << std::endl;
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	//これがないとサーバからの返信を受け取れない
	mrs_set_read_record_callback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	mrs_set_disconnect_callback(connection, Disconnect);
}

void ClientAssignment04::Disconnect(MrsConnection connection, void* connection_data)
{
	std::cout << "DisConnected" << std::endl;
}

void ClientAssignment04::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{

	// 紐づけておいたthisをClientAssignment04*に戻す
	//　資料のポインタ変数はこのselfのこと
	ClientAssignment04* self = static_cast<ClientAssignment04*>(connection_data);

	switch (static_cast<NetworkTag>(payload_type))
	{
	case NetworkTag::Move:
	{
		if (payload_len < sizeof(PlayerInput)) { return; }

		PlayerInput playerInput;
		memcpy(&playerInput, payload, sizeof(PlayerInput));

		//データの中のidで動かすプレイヤーを探す
		Player* player = self->playerManager->GetPlayer(playerInput.id);
		//まだ登録されていないIDなら何もしない（Loginより先に届いた場合の対策）
		if (player == nullptr) { return; }

		std::cout << "recv data id:" << playerInput.id << std::endl;
		std::cout << "x:" << playerInput.clickPosition.x
			<< "y:" << playerInput.clickPosition.y
			<< "z:" << playerInput.clickPosition.z
			<< std::endl;
		std::cout << "Tag:" << "Move" << std::endl;

		player->SetPosition(playerInput.position);
		player->SetTargetPosition(playerInput.clickPosition);
		player->GetJobClass()->ChangeState(Player::State::Move);
		break;
	}
	case NetworkTag::Attack:
	{
		//座標データとして足りないサイズなら読まない（範囲外アクセス防止）
		if (payload_len < sizeof(PlayerInput)) { return; }

		//受信データ(void*)をPlayerInputに戻す
		PlayerInput playerInput;
		memcpy(&playerInput, payload, sizeof(PlayerInput));

		Player* player = self->playerManager->GetPlayer(playerInput.id);
		if (player == nullptr) { return; }

		std::cout << "recv data id:" << playerInput.id << std::endl;
		std::cout << "x:" << playerInput.clickPosition.x
			<< "y:" << playerInput.clickPosition.y
			<< "z:" << playerInput.clickPosition.z << std::endl;
		std::cout << "Tag:" << "Attack" << std::endl;

		player->SetPosition(playerInput.position);
		player->SetTargetPosition(playerInput.clickPosition);
		player->GetJobClass()->ChangeState(Player::State::Attack);
		break;
	}
	case NetworkTag::Login:
	{
		if (payload_len < sizeof(PlayerLogin)) { return; }

		PlayerLogin playerLogin;
		memcpy(&playerLogin, payload, sizeof(PlayerLogin));
		//ここにログの出力処理
		std::cout << "recv data login id:" << playerLogin.id << std::endl;

		//最初の1回（自分のログインだけ）自分のIDとして登録する

		if (!self->isLogin)
		{
			self->playerManager->SetMyPlayerID(playerLogin.id);
			self->isLogin = true;
			std::cout << "my player login" << std::endl;
		}


		Player* player = new Player();
		player->SetPlayerID(playerLogin.id);
		player->SetPosition(DirectX::XMFLOAT3(0, 0, 0));
		player->SetTargetPosition(DirectX::XMFLOAT3(0, 0, 0));
		player->SetState(Player::State::Idle);
		player->SetModel();
		player->SetReady(true);
		self->playerManager->AddPlayer(player);
		player->GetJobClass()->ChangeState(Player::State::Idle);
		break;
	}
	case NetworkTag::Logout:
	{
		if (payload_len < sizeof(PlayerLogout)) { return; }

		PlayerLogout playerLogout;
		memcpy(&playerLogout, payload, sizeof(PlayerLogout));
		//ここにログの出力処理
		std::cout << "Delete Player ID:" << playerLogout.id << std::endl;
		self->playerManager->ErasePlayer(playerLogout.id);
		break;
	}
	case NetworkTag::Sync: //Syncとは同期の略で複数のデータやタイミングを合わせることを意味する
	{
		if (payload_len < sizeof(PlayerSync)) { return; }

		PlayerSync playerSync;
		memcpy(&playerSync, payload, sizeof(PlayerSync));

		std::cout << "recv data sync id:" << playerSync.id << std::endl;

		Player* player = self->playerManager->GetPlayer(playerSync.id);
		//IDが登録されているかで処理を分ける
		if (player != nullptr)
		{
			//登録済み
			player->SetPosition(playerSync.position);
			player->SetTargetPosition(playerSync.targetPosition);
			player->SetState(playerSync.state);
			player->GetJobClass()->ChangeState(playerSync.state);
		}
		else
		{
			//未登録
			player = new Player();
			player->SetPosition(playerSync.position);
			player->SetTargetPosition(playerSync.targetPosition);
			player->SetPlayerID(playerSync.id);
			player->SetState(playerSync.state);
			player->SetModel();
			player->SetReady(true);
			self->playerManager->AddPlayer(player);
			player->GetJobClass()->ChangeState(playerSync.state);

		}
	}
	default:
		break;
	}
}

//ここで定期的にサーバー側に最新情報を送信する
//void ClientAssignment04::Update(float elapsedTime)
//{
//	timer++;
//
//
//}
