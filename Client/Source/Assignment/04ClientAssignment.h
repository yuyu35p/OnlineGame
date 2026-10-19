#pragma once
#include <mrs.hpp>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"


class ClientAssignment04 : public SceneBase
{
public:
	ClientAssignment04();
	~ClientAssignment04();

	void Initialize();
	void Finalize();

	void Render();

	// ネットワーク処理更新関数
	void NetworkUpdate();

	// 必要に応じてコールバック関数を追加
	static void Connect(MrsConnection connection, void* connectionData);
	static void Disconnect(MrsConnection connection, void* connection_data);
	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);

	// 送信データタグ
	enum class NetworkTag : uint16
	{
		Message,		// メッセージ
		Move,			// 移動
		Attack,			// 攻撃
		Login,			// login
		Logout,			// logout
		Sync			// 同期
	};

	// 送信データ用構造体
	struct PlayerInput
	{
		int id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 clickPosition;
	};

	struct PlayerLogin
	{
		int id;
	};
	struct PlayerLogout
	{
		int id;
	};
	struct PlayerSync
	{
		int id;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 targetPosition;
		Player::State state;
	};
private:
	MrsConnection mrsConnection;
	bool isLogin = false;

	int timer = 0;
};