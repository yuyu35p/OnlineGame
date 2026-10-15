#pragma once
#include <mrs.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <DirectXMath.h>



class ServerAssignment03
{
public:
	void Execute();
	void Exit();

	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);
	static void Disconnect(MrsConnection connection, void* connection_data);
	static void Accept(MrsServer server, void* server_data, MrsConnection connection);

	enum class NetworkTag : uint16
	{
		Message,
		Move,
		Attack
	};

	// 通信データ用構造体
	struct PlayerInput
	{
		DirectX::XMFLOAT3 clickPosition;
	};

private:
	MrsServer mrsServer = nullptr;
	bool loop = true;
	std::vector<MrsConnection> connection; // 接続中クライアント
};