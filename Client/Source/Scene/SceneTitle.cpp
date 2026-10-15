#include "SceneTitle.h"

// 初期化
void SceneTitle::Initialize()
{
	logo = std::make_unique<Sprite>("Data/Sprite/UI/ui_title_logo.png");
	background = std::make_unique<Sprite>("Data/Sprite/background.png");
}

// 終了化
void SceneTitle::Finalize()
{

}

// 描画処理
void SceneTitle::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	float screenHeight = static_cast<float>(graphics.GetScreenHeight());

	// 背景表示
	background;
	float textureWidth = static_cast<float>(background->GetTextureWidth());
	float textureHeight = static_cast<float>(background->GetTextureHeight());
	// タイトルスプライト描画
	background->Render(dc,
		0, 0, screenWidth, screenHeight,
		0, 0, textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	// ロゴ表示
	textureWidth = static_cast<float>(logo->GetTextureWidth());
	textureHeight = static_cast<float>(logo->GetTextureHeight());
	logo->Render(dc,
		(screenWidth / 2) - (screenWidth / 4), 0, screenWidth / 2, screenHeight / 2,
		0, 0, textureWidth, textureHeight,
		0,
		1, 1, 1, 1);

	// imgui
	ImGui::SetNextWindowPos(ImVec2(screenWidth / 2 - 150, 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 370), ImGuiCond_Once);

	// サーバーと接続
	if (ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button(u8"Sample00:MRS サンプル"))
		{
			SceneManager::Instance().ChangeScene(new ClientSample00);
		}
		if (ImGui::Button(u8"Assignment01:MRS サーバと通信"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment01);
		}
		if (ImGui::Button(u8"Assignment02:MRS キャラクター移動"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment02);
		}
		if (ImGui::Button(u8"Assignment03:MRS データの判別"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment03);
		}
		if (ImGui::Button(u8"Assignment04:MRS 複数ユーザとの通信"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment04);
		}
		if (ImGui::Button(u8"Assignment05:WinSock サーバと接続"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment05);
		}
		if (ImGui::Button(u8"Assignment06:WinSock サーバと通信"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment06);
		}
		if (ImGui::Button(u8"Assignment07:WinSock データの判別"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment07);
		}
		if (ImGui::Button(u8"Assignment08:WinSock 複数ユーザとの通信"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment08);
		}
		if (ImGui::Button(u8"Assignment09:WinSock ノンブロッキング処理"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment09);
		}
		if (ImGui::Button(u8"Assignment10:WinSock ファイルダウンロード"))
		{
			SceneManager::Instance().ChangeScene(new ClientAssignment10);
		}
	}
	ImGui::End();
}
