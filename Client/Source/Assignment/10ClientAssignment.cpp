#include "10ClientAssignment.h"



ClientAssignment10::ClientAssignment10()
{

}

ClientAssignment10::~ClientAssignment10()
{
}

void ClientAssignment10::Initialize()
{
	SceneBase::Initialize();
}

void ClientAssignment10::Finalize()
{
	int wsacleanup = WSACleanup();
}

void ClientAssignment10::Render()
{
	// 基底クラスのレンダー呼び出し
	SceneBase::Render();
	// imgui
	Graphics& graphics = Graphics::Instance();
	float screenWidth = static_cast<float>(graphics.GetScreenWidth());
	ImGui::SetNextWindowPos(ImVec2(screenWidth / 2 - 150, 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(300, 100), ImGuiCond_Once);
	if (ImGui::Begin("Download", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button("Start"))
		{
			FileDownload();
		}
		if (ImGui::Button(u8"タイトルへ"))
		{
			SceneManager::Instance().ChangeScene(new SceneTitle());
		}

	}
	ImGui::End();
}

void ClientAssignment10::FileDownload()
{
	// WinsockAPI初期化


	// 接続先情報設定
	struct sockaddr_in addr;



	// ソケット作成



	// サーバへ接続

	std::cout << "Connect Success" << std::endl;

	// URL指定
	std::string path;
	std::string filename;

	// リクエストヘッダ作成
	std::string request;

	// リクエストヘッダを送信




	std::cout << "Server Response." << std::endl;

	// 受信したデータを下記の変数に入れる
	std::vector<char> data;

	// 必要に応じて変数を用意してください。

	// データ受信完了までループ
	while (1) {
		char buffer[1024] = {};
		int size = 0;

		// データ受信






	}
	std::cout << std::endl;
	// ******************************************
	// ファイル書き出し
	std::ofstream writingFile{};
	writingFile.open(filename, std::ios::binary);
	writingFile.write(&data.at(0), data.size());
	writingFile.close();
	// 書き出し終了ログ出力(\aに意味はなし。システム音鳴らすだけ)
	std::cout << filename << "書き出しました。\a" << std::endl;



	// ソケット終了処理



	// WSA終了処理


}