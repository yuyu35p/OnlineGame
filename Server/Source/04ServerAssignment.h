#pragma once
#include <mrs.hpp>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "Player.h"


class ServerAssignment04
{
public:
	ServerAssignment04();
	~ServerAssignment04();
	void Execute();
	void Exit();

	static void ReadRecord(MrsConnection connection, void* connection_data, uint32 seqnum, uint16 options, uint16 payload_type, const void* payload, uint32 payload_len);
	static void Disconnect(MrsConnection connection, void* connection_data);
	static void Accept(MrsServer server, void* server_data, MrsConnection connection);


	enum class NetworkTag : uint16
	{
		Message,
		Move,
		Attack,
		Login,
		Logout,
		Sync
	};

	struct Client {
		MrsConnection mrsConnection = nullptr;
		Player* player = nullptr;
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

	void AddID() { id++; }
	std::vector<Client> GetClients() { return clients; }
	void EraseClient(MrsConnection connection);

private:
	int id = 0;
	MrsServer mrsServer = nullptr;
	std::vector<Client> clients;
	bool loop = true;

};