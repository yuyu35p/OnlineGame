#pragma once
#include <Winsock2.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include "Player.h"

class ServerAssignment09
{
public:
	// クライアント情報
	struct Client {
		struct sockaddr_in addr {};
		SOCKET sock;
		Player* player = nullptr;
	};
	enum class NetworkTag : unsigned short
	{
		Message,
		Move,
		Attack,
		Login,
		Logout,
		Sync
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


	void Execute();
	void Exit();
	void EraseClient(Client* client);

private:
	int id = 0;
	SOCKET sock = INVALID_SOCKET;
	std::vector<Client*> clients;
	bool loop = true;
	fd_set svFds{};
	fd_set clFds{};
	struct timeval tv {};
};