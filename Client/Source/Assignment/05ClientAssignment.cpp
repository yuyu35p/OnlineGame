#include "05ClientAssignment.h"



ClientAssignment05::ClientAssignment05()
{
	input[0] = '\0';
}

ClientAssignment05::~ClientAssignment05()
{
}

void ClientAssignment05::Initialize()
{
	SceneBase::Initialize();

	// WinsockAPIを初期化



	// 接続先情報設定
	struct sockaddr_in addr;



	// ソケット作成



	// サーバへ接続






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

void ClientAssignment05::Finalize()
{
	// socket終了処理


	// wsa終了処理


}

void ClientAssignment05::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();
	// imgui
	ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_Once);
	if (ImGui::Begin("Title", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button(u8"タイトルへ"))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}

	}
	ImGui::End();
}
