#pragma once
#include <mrs.hpp>
#include "Scene\Scene.h"

class ClientSample00 : public SceneBase
{
public:
	ClientSample00();
	~ClientSample00();

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

private:
	std::vector<std::string> messages;
	char input[32];
	MrsConnection mrsConnection;
};