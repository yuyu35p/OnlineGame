// ***********************************************************
// Client側課題 前回課題と重複するところはコピーしてください。

#include "08ClientAssignment.h"



ClientAssignment08::ClientAssignment08()
{
	input[0] = '\0';
}

ClientAssignment08::~ClientAssignment08()
{
}

void ClientAssignment08::Initialize()
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

}

void ClientAssignment08::Finalize()
{
	// マルチスレッドのループフラグを下ろす
	loop = false;

	// recvはブロック関数なのでrecv関数を終わらせるためにLogoutを送信
	PlayerLogout logout{};
	logout.cmd = NetworkTag::Logout;
	logout.id = playerManager->GetMyPlayerID();
	int s = send(sock, reinterpret_cast<char*>(&logout), sizeof(PlayerLogout), 0);

	// スレッドの終了まで待機
	th.join();

	// ソケット終了処理



	// WSA終了処理



}

void ClientAssignment08::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();
	// imgui
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
			text = "Angle" + std::to_string(player->GetPlayerID());
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

void ClientAssignment08::NetworkUpdate()
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

// 受信関数
void ClientAssignment08::recvThread()
{
	do {
		char buffer[2048]{};







	} while (loop);
}
