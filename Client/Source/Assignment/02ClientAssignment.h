#pragma once
#include <mrs.hpp>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"


class ClientAssignment02 : public SceneBase
{
public:
	ClientAssignment02();
	~ClientAssignment02();

	void Initialize();
	void Finalize();

	void Render();

	// ネットワーク処理更新関数
	void NetworkUpdate();

	// 必要に応じてコールバック関数を追加
	static void Error(MrsConnection connection, void* connection_data, MrsConnectionError status);
	static void Connect(MrsConnection connection, void* connectionData);
	static void Disconnect(MrsConnection connection, void* connection_data);
	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);

	// 通信データ用構造体
	struct PlayerInput
	{
		DirectX::XMFLOAT3 clickPosition;
	};

private:
	MrsConnection mrsConnection;
};