#pragma once
#include <Winsock2.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <DirectXMath.h>

class ServerAssignment07
{
public:
	// クライアント情報
	struct Client {
		struct sockaddr_in addr;
		SOCKET sock;
	};
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

	void Execute();
	void Exit();

private:
	Client client;
	bool loop = true;
};