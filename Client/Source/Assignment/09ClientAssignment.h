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
#include <mutex>


class ClientAssignment09 : public SceneBase
{
public:
	ClientAssignment09();
	~ClientAssignment09();

	void Initialize();
	void Finalize();

	void Render();

	// ネットワーク処理更新関数
	void NetworkUpdate();

	void recvThread();

	enum class NetworkTag : unsigned short
	{
		Message,		// チャットメッセージ
		Move,			// 移動
		Attack,			// 攻撃
		Login,			// ログイン
		Logout,			// ログアウト
		Sync			// 同期
	};

	struct PlayerInput
	{
		NetworkTag cmd;
		short id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 clickPosition;
	};
	struct PlayerLogin
	{
		NetworkTag cmd;
		short id;
	};
	struct PlayerLogout
	{
		NetworkTag cmd;
		short id;
	};
	struct PlayerSync
	{
		NetworkTag cmd;
		short id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 targetPosition;
		Player::State state;
	};

private:
	std::vector<std::string> messages;
	char input[32];
	WSADATA wsaData{};
	SOCKET sock{};
	std::thread th;
	bool loop = true;
	std::mutex mutex;
};