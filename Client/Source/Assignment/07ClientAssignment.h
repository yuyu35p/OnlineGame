#pragma once
// Winsock2.hを先に読み込めばエラーは出ない
#include <Winsock2.h>
#include <Windows.h>

#include <thread>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"



class ClientAssignment07 : public SceneBase
{
public:
	ClientAssignment07();
	~ClientAssignment07();

	void Initialize();
	void Finalize();

	void Render();

	// ネットワーク処理更新関数
	void NetworkUpdate();

	void recvThread();

	enum class NetworkTag : unsigned short
	{
		Message,
		Move,
		Attack
	};

	struct PlayerInput
	{
		NetworkTag cmd;
		DirectX::XMFLOAT3 clickPosition;
	};

	struct PlayerMessage
	{
		NetworkTag cmd;
		char text[256];
	};

private:
	std::vector<std::string> messages;
	char input[32];
	WSADATA wsaData{};
	SOCKET sock{};
	std::thread th;
	bool loop = true;
};