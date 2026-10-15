#pragma once
// Winsock2.h‚ğæ‚É“Ç‚İ‚ß‚ÎƒGƒ‰[‚Ío‚È‚¢
#include <Winsock2.h>
#include <Windows.h>

#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"



class ClientAssignment05 : public SceneBase
{
public:
	ClientAssignment05();
	~ClientAssignment05();

	void Initialize();
	void Finalize();

	void Render();

private:
	std::vector<std::string> messages;
	char input[32];
	WSADATA wsaData{};
	SOCKET sock{};
};