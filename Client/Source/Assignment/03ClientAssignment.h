#pragma once
#include <mrs.hpp>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"


class ClientAssignment03 : public SceneBase
{
public:
	ClientAssignment03();
	~ClientAssignment03();

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
		Message,
		Move,
		Attack
	};

	// 送信データ用構造体
	struct PlayerInput
	{
		DirectX::XMFLOAT3 clickPosition;
	};

private:
	std::vector<std::string> messages;
	char input[32]={};
	MrsConnection mrsConnection;
};