// ***********************************************************
// Client側課題 前回課題と重複するところはコピーしてください。
#include "06ClientAssignment.h"



ClientAssignment06::ClientAssignment06()
{
	input[0] = '\0';
}

ClientAssignment06::~ClientAssignment06()
{
}

void ClientAssignment06::Initialize()
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

void ClientAssignment06::Finalize()
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

void ClientAssignment06::Render()
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

void ClientAssignment06::recvThread()
{
	do {
		char buffer[2048]{};







	} while (loop);

}
