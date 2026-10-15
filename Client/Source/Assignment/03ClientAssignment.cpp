// ***********************************************************
// Client側課題 前回課題と重複するところはコピーしてください。
#include "03ClientAssignment.h"

ClientAssignment03::ClientAssignment03()
{
	//接続前なので空にしておく
	mrsConnection = nullptr;
}

ClientAssignment03::~ClientAssignment03()
{

}

void ClientAssignment03::Initialize()
{
	SceneBase::Initialize();

	//MRSライブラリ初期化
	if (!mrs_initialize()) { std::cout << "mrs_initialize not faild" << std::endl; }
	else { std::cout << "mrs_initialize success" << std::endl; }

	//接続開始
	char address[40] = { "127.0.0.1" }; //接続先IP（自分のPC)
	//char address[40] = { "10.200.5.151" }; //接続先IP
	uint16 port = 7000;					//サーバと同じポート番号
	uint32 timeout = 5000;				//接続のタイムアウト（ミリ秒）5000 = 5秒
	mrsConnection = mrs_connect(MRS_CONNECTION_TYPE_TCP, address, port, timeout);

	//コネクションにthisを紐づける
	//コールバックはstatic関数でメンバ変数に触れないため、
	//引数のconnection_dataとしてthisを上取れるようにしておく
	mrs_connection_set_data(mrsConnection, this);

	//接続に成功したときに呼ばれる関数を登録
	mrs_set_connect_callback(mrsConnection, Connect);
	//切断時に呼ばれる関数を登録（Connect内でも登録しているので、どちらか片方でもいい）
	mrs_set_disconnect_callback(mrsConnection, Disconnect);

	// === Plaer設定 ======
	//プレイヤーを管理するオブジェクトを作り、「自分が操作するプレイヤーのID」を０にする
	playerManager = new PlayerManager();
	playerManager->SetMyPlayerID(0);
	//キャラクター本体を作る
	Player* player = new Player();

	// 各プレイヤーのキャラクターに合わせた設定を行う
	// JobClassはステートマシン
	player->SetPlayerID(0);									  // ID 0 = [自分のプレイヤー]
	player->SetPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)); // 初期位置（原点）
	player->SetScale(DirectX::XMFLOAT3(0.02f, 0.02f, 0.02f)); // モデルの拡大率
	player->SetModel();										  // ３Dモデルの設定
	//管理オブジェクトに登録する
	playerManager->AddPlayer(player);
	//準備完了フラグ
	playerManager->GetMyPlayer()->SetReady(true);

}

void ClientAssignment03::Finalize()
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

void ClientAssignment03::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();

	// imgui
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 620), ImGuiCond_Once);
 
	if (ImGui::Begin("Chat", nullptr, ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoResize))
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

				//送信用バッファに、入力した文字列を書き込む
				mrs::Buffer send;
				send.Write(&input, sizeof(input));

				//第3引数にタグを指定する
				//引数はuint16なのでenum classをstatic_castで変換して渡す
				mrs_write_record(
					mrsConnection,
					MrsRecordOption::MRS_RECORD_OPTION_NONE,
					static_cast<uint16>(NetworkTag::Message),
					send.GetData(),
					send.GetDataLen()
				);
			}
			//送信後は入力欄を空にする
			input[0] = '\0';
		}
		if (ImGui::Button(u8"タイトルへ"))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}
	}
	ImGui::End();

	// ========= プレイヤー情報ウィンドウ ===========
	ImGui::SetNextWindowPos(ImVec2(900, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 180), ImGuiCond_Once);
	if (ImGui::Begin("Player Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		//自分のプレイヤーを取得して、値を表示・編集できるようにする
		Player* player = playerManager->GetMyPlayer();

		//位置
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
	}
	ImGui::End();
}

void ClientAssignment03::NetworkUpdate()
{
	//送信の実行・受信・コールバック呼び出しを行う。毎フレーム最初に呼ぶ
	mrs_update();
	// handle window
	HWND hWnd;
	// windowがアクティブか判断(非アクティブのときNULLが返ってくる)
	hWnd = GetActiveWindow();
	if (hWnd != NULL)
	{
		// ImGuiのウィンドウ上でのクリックは、ゲーム内のクリックとして扱わない
		if (!ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow))
		{
			if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
			{
				// マウスインプット情報取得
				Mouse& mouse = Input::Instance().GetMouse();
				// マウスボタンが押されている時
				if (mouse.GetButtonDown() & Mouse::BTN_LEFT || mouse.GetButtonDown() & Mouse::BTN_RIGHT)
				{
					// マウス座標取得（２Ｄ）
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
						PlayerInput playerInput;
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

void ClientAssignment03::Connect(MrsConnection connection, void* connectionData)
{
	std::cout << "Success Connection" << std::endl;
	// レコードが読み込み可能になった際に呼ばれるCallBack関数の設定
	//これがないとサーバからの返信を受け取れない
	mrs_set_read_record_callback(connection, ReadRecord);
	// 切断されたときのCallBack関数
	mrs_set_disconnect_callback(connection, Disconnect);
}

void ClientAssignment03::Disconnect(MrsConnection connection, void* connection_data)
{
	std::cout << "DisConnected" << std::endl;
}

void ClientAssignment03::ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len)
{

	// 紐づけておいたthisをClientAssignment02*に戻す
	//　資料のポインタ変数はこのselfのこと
	ClientAssignment03* self = static_cast<ClientAssignment03*>(connection_data);

	//　自分のプレイヤー（移動・攻撃のときに使う）
	Player* player = self->playerManager->GetMyPlayer();

	switch (static_cast<NetworkTag>(payload_type))
	{
	case NetworkTag::Attack:
	{
		//座標データとして足りないサイズなら読まない（範囲外アクセス防止）
		if (payload_len < sizeof(PlayerInput)) { return; }

		//受信データ(void*)をPlayerInputに戻す
		PlayerInput playerInput;
		memcpy(&playerInput, payload, sizeof(PlayerInput));

		std::cout << "recv data" << std::endl;
		std::cout << "x:" << playerInput.clickPosition.x
				  << "y:" << playerInput.clickPosition.y
				  << "z:" << playerInput.clickPosition.z << std::endl;
		std::cout << "Tag:" << "Attack" << std::endl;

		player->SetTargetPosition(playerInput.clickPosition);
		player->GetJobClass()->ChangeState(Player::State::Attack);
		break;
	}
	case NetworkTag::Message:
	{
		//受信バッファ（256バイト）に入りきらないサイズなら読まない
		char payloadData[256]{};
		if (payload_len > sizeof(payloadData) - 1) { return; }

		//受信データをバッファ経由で文字列として取り出す
		mrs::Buffer buffer;
		buffer.Write(payload, payload_len);

		// バッファデータからpayLoadStrにデータ読み込み
		if (!buffer.Read(payloadData, payload_len)) {
			std::cout << "read error" << std::endl;
		}
		std::cout << payloadData << std::endl;
		std::cout << "Tag:" << "Message" << std::endl;
		// コネクションに紐づいているメンバ変数messagesに登録
		self->messages.emplace_back(payloadData);
		buffer.Clear();
		break;
	}
	case NetworkTag::Move:
	{
		if (payload_len < sizeof(PlayerInput)) { return; }
		
		PlayerInput playerInput;
		memcpy(&playerInput, payload, sizeof(PlayerInput));

		std::cout << "recv data" << std::endl;
		std::cout << "x:" << playerInput.clickPosition.x
				  << "y:" << playerInput.clickPosition.y
				  << "z:" << playerInput.clickPosition.z
				  << std::endl;
		std::cout << "Tag:" << "Move" << std::endl;

		player->GetJobClass()->ChangeState(Player::State::Move);
		player->SetTargetPosition(playerInput.clickPosition);
		break;
	}
	default:
		break;
	}
}
