#pragma once
#include <mrs.hpp>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"

class ClientAssignment01 : public SceneBase
{
public:
	ClientAssignment01();
	~ClientAssignment01();

	void Initialize();
	void Finalize();

	void Render();

	// ネットワーク処理更新関数
	void NetworkUpdate();

	// 必要に応じてコールバック関数を追加
	
	//エラー時
	static void Error(MrsConnection connection, void* connection_data, MrsConnectionError status);
	// サーバ接続時
	static void Connect(MrsConnection connection, void* connection_data);
	// サーバ切断時
	static void Disconnect(MrsConnection connection, void* connection_data);
	// データ受信時
	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);

private:
	std::vector<std::string> messages;
	char input[32];
	MrsConnection mrsConnection;
};