#pragma once
#include <Winsock2.h>
#include <iostream>
#include <thread>
#include <string>
#include <vector>

class ServerAssignment06
{
public:
	// クライアント情報
	struct Client {
		struct sockaddr_in addr;
		SOCKET sock;
	};

	void Execute();
	void Exit();

private:
	Client client;
	bool loop = true;
};