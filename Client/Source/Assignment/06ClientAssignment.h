#pragma once
// Winsock2.h‚ğæ‚É“Ç‚İ‚ß‚ÎƒGƒ‰[‚Ío‚È‚¢
#include <Winsock2.h>
#include <Windows.h>

#include <thread>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"



class ClientAssignment06 : public SceneBase
{
public:
	ClientAssignment06();
	~ClientAssignment06();

	void Initialize();
	void Finalize();

	void Render();

	void recvThread();

private:
	std::vector<std::string> messages;
	char input[32];
	WSADATA wsaData{};
	SOCKET sock{};
	std::thread th;
	bool loop = true;
};