#include "Scene/SceneTitle.h"
#include "Scene/SceneManager.h"

#include <chrono>
#include <timeapi.h>
#include <memory>
#include <sstream>
#include "Graphics/Graphics.h"
#include "Input/Input.h"

#pragma comment(lib,"winmm.lib")

#include "System/Framework.h"

//目標フレームレート
static const double targetFps = 60.0;
// 垂直同期間隔設定
static const int syncInterval = 1;
// コンストラクタ
Framework::Framework(HWND hWnd)
	: hWnd(hWnd)
	, input(hWnd)
	, graphics(hWnd)
{
#if _DEBUG
	if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
		// エクスプローラから起動した場合は新規にコンソールを割り当てる
		AllocConsole();
	}
	FILE* stream;
	freopen_s(&stream, "CONIN$", "r", stdin);
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	std::cout << "クライアントログ(std::coutでこちらの画面に出力されます)" << std::endl;

#endif
	
	// タイトル画面
	SceneManager::Instance().ChangeScene(new SceneTitle);
}

// デストラクタ
Framework::~Framework()
{
	SceneManager::Instance().Clear();

#if _DEBUG
	FreeConsole();
#endif
}

// 更新処理
void Framework::Update(float elapsedTime/*Elapsed seconds from last frame*/)
{
	// 入力更新処理
	input.Update();

	// シーン更新処理
	SceneManager::Instance().Update(elapsedTime);
}

// 描画処理
void Framework::Render(float elapsedTime/*Elapsed seconds from last frame*/)
{
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();

	// IMGUIフレーム開始処理
	graphics.GetImGuiRenderer()->NewFrame();

	// シーン描画処理
	SceneManager::Instance().Render();

	// IMGUI描画
	graphics.GetImGuiRenderer()->Render(dc);

	// バックバッファに描画した画を画面に表示する。
	graphics.GetSwapChain()->Present(syncInterval, 0);
}

// フレームレート計算
void Framework::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.
	static int frames = 0;
	static float time_tlapsed = 0.0f;

	frames++;

	// Compute averages over one second period.
	if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
	{
		float fps = static_cast<float>(frames); // fps = frameCnt / 1
		float mspf = 1000.0f / fps;
		std::ostringstream outs;
		outs.precision(6);
		outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
		SetWindowTextA(hWnd, outs.str().c_str());

		// Reset for next average.
		frames = 0;
		time_tlapsed += 1.0f;
	}
}

// アプリケーションループ
int Framework::Run()
{
	MSG msg = {};

	// Sleep の精度を1ミリ秒にする。
	// 標準だと約15ミリ秒単位で、60FPS(約16.7ミリ秒)の管理には粗すぎる
	timeBeginPeriod(1);

	// 1フレームの長さ(1/60秒)と、次のフレームを始める予定時刻
	using Clock = std::chrono::steady_clock;
	const Clock::duration frameDuration =
		std::chrono::duration_cast<Clock::duration>(std::chrono::duration<double>(1.0 / targetFps));
	Clock::time_point nextFrameTime = Clock::now();

	// メッセージループの前に呼ぶ。最初のフレームの経過時間が大きくなりすぎないように
	timer.Reset();

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 次のフレームの時刻になるまで、更新も描画もせずに待つ
			Clock::time_point now = Clock::now();
			if (now < nextFrameTime)
			{
				// 残りが2ミリ秒以上ならSleepでCPUを休ませ、それ未満は空回しで待つ
				if (nextFrameTime - now > std::chrono::milliseconds(2)) { Sleep(1); }
				continue;
			}

			// 次の予定時刻を1フレーム分進める
			nextFrameTime += frameDuration;
			// 大きく遅れたときは、遅れを取り戻そうとせず、今から数え直す
			if (now > nextFrameTime) { nextFrameTime = now + frameDuration; }

			// ここから先は今までどおり。
			// FPS表示が正しくなるよう、待ちの判定より後ろに置く
			timer.Tick();
			CalculateFrameStats();

			float elapsedTime = syncInterval == 0
				? timer.TimeInterval()
				: syncInterval / 60.0f
				;
			Update(elapsedTime);
			Render(elapsedTime);
		}
	}

	timeEndPeriod(1);
	return static_cast<int>(msg.wParam);
}
// メッセージハンドラ
LRESULT CALLBACK Framework::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (Graphics::Instance().GetImGuiRenderer()->HandleMessage(hWnd, msg, wParam, lParam))
		return true;
	Mouse& mouse = Input::Instance().GetMouse();
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case WM_MOUSEWHEEL:
		mouse.SetWheel(static_cast<int>((float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA));
		break;
	case WM_ENTERSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		timer.Stop();
		break;
	case WM_EXITSIZEMOVE:
		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
		timer.Start();
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}
