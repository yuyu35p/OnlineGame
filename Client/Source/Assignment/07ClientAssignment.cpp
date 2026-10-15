// ***********************************************************
// Client側課題 前回課題と重複するところはコピーしてください。
#include "07ClientAssignment.h"



ClientAssignment07::ClientAssignment07()
{
	input[0] = '\0';
}

ClientAssignment07::~ClientAssignment07()
{
}

void ClientAssignment07::Initialize()
{
	SceneBase::Initialize();

	// WinsockAPI初期化


	// 接続先情報設定
	struct sockaddr_in addr;



	// ソケット作成



	// サーバへ接続




	// 受信スレッド実装




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

void ClientAssignment07::Finalize()
{
	// マルチスレッドのループフラグを下ろす
	loop = false;

	// recvはブロック関数なのでrecv関数を終わらせるために-1を1バイトで送信
	char end = -1;
	int s = send(sock, &end, sizeof(end), 0);

	// スレッドの終了まで待機
	th.join();

	// ソケット終了処理



	// WSA終了処理
}

void ClientAssignment07::Render()
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

void ClientAssignment07::NetworkUpdate()
{
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
						PlayerInput playerInput{};
						// 送信処理




					}
				}
			}
		}
	}
}

void ClientAssignment07::recvThread()
{
	do {
		char buffer[2048]{};
		






	} while (loop);
}
