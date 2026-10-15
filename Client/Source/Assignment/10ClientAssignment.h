#pragma once
// Winsock2.hを先に読み込めばエラーは出ない
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <fstream>
#include "Scene\SceneManager.h"
#include "Scene\Scene.h"
#include "Scene\SceneTitle.h"
#include "Graphics\Graphics.h"
#include "Input\Input.h"



class ClientAssignment10 : public SceneBase
{
public:
	ClientAssignment10();
	~ClientAssignment10();

	void Initialize();
	void Finalize();

	void Render();

	// ネットワーク処理更新関数
	void FileDownload();

private:

};