#include "Scene.h"
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Camera/Camera.h"
#include "Character/JobClass.h"




void SceneBase::Initialize()
{
	font = debug_new Font("Data/Font/MS Gothic.fnt", 1024 * 16);

	stage = debug_new Stage();

	// カメラ初期設定
	Graphics& graphics = Graphics::Instance();
	Camera& camera = Camera::Instance();
	camera.SetLookAt(
		DirectX::XMFLOAT3(-20, 50, 0),
		DirectX::XMFLOAT3(DirectX::XMFLOAT3(-70.0f, 0.5f, -45.0f)),
		DirectX::XMFLOAT3(0, 1, 0)
	);

	camera.SetPerspecticeFov(
		DirectX::XMConvertToRadians(45),
		graphics.GetScreenWidth() / graphics.GetScreenHeight(),
		0.1f,
		1000.0f
	);

	// カメラコントローラー初期化
	cameraController = debug_new CameraController();
	DirectX::XMFLOAT3 target = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	cameraController->SetTarget(target);
	cameraController->SetAngle(DirectX::XMFLOAT3(DirectX::XMConvertToRadians(70.0f), DirectX::XMConvertToRadians(180.0f), 0.0f));
	cameraController->SetRange(50.0f);
	cameraController->Update(1.0f);

}

void SceneBase::Finalize()
{
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	// カメラコントローラー終了
	if (cameraController != nullptr)
	{
		delete cameraController;
		cameraController = nullptr;
	}
	if (font != nullptr)
	{
		delete font;
		font = nullptr;
	}
	if (playerManager != nullptr)
	{
		for (Player* player : playerManager->GetPlayers())
		{
			if (player != nullptr)
			{
				delete player;
				player = nullptr;
			}
		}
		delete playerManager;
		playerManager = nullptr;
	}

}

void SceneBase::Update(float elapsedTime)
{

	NetworkUpdate();

	// プレイヤー更新
	if (playerManager != nullptr)
	{
		for (Player* player : playerManager->GetPlayers())
		{
			if (player->GetReady())
			{
				player->Update(elapsedTime);
			}
		}
	}

	stage->Update(elapsedTime);

	if (playerManager != nullptr && playerManager->GetMyPlayer() != nullptr)
	{
		// カメラコントローラー更新処理
		DirectX::XMFLOAT3 target = playerManager->GetMyPlayer()->GetPosition();
		target.y += 0.5f;
		cameraController->SetTarget(target);

		HWND hWnd;
		hWnd = GetActiveWindow();
		if (hWnd != NULL)
		{
			// カメラ更新
			cameraController->Update(elapsedTime);
		}
	}
	if (playerManager != nullptr)
	{
		// 削除用配列に入っているプレイヤーを削除
		playerManager->DeletePlayer();
	}
}

void SceneBase::Render()
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	ID3D11RenderTargetView* rtv = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* dsv = graphics.GetDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	// 描画処理
	RenderContext rc;
	rc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };	// ライト方向（下方向）

	// カメラパラメータ設定
	Camera& camera = Camera::Instance();
	rc.view = camera.GetView();
	rc.projection = camera.GetProjection();

	Shader* shader = graphics.GetShader();
	shader->Begin(dc, rc);

	// ステージ描画
	stage->Render(dc, shader);

	// プレイヤー描画
	if (playerManager != nullptr)
	{
		for (Player* player : playerManager->GetPlayers())
		{
			if (player->GetReady())
			{
				player->Render(dc, shader);
			}
		}
	}
	shader->End(dc);

	// ラインレンダラ描画実行
	graphics.GetLineRenderer()->Render(dc, rc.view, rc.projection);

	// デバッグレンダラ描画実行
	graphics.GetDebugRenderer()->Render(dc, rc.view, rc.projection);
}

// クリック座標取得
bool SceneBase::GetClickPosition(DirectX::XMFLOAT3 screenPosition, HitResult& hit)
{
	Graphics& graphics = Graphics::Instance();
	ID3D11DeviceContext* dc = graphics.GetDeviceContext();
	// カメラパラメータ設定
	Camera& camera = Camera::Instance();

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&camera.GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&camera.GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	DirectX::XMVECTOR ScreenPosition, WorldPosition;

	// レイの視点を算出
	ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
	WorldPosition = DirectX::XMVector3Unproject(
		ScreenPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);
	DirectX::XMFLOAT3 rayStart;
	DirectX::XMStoreFloat3(&rayStart, WorldPosition);

	// レイの終点を算出
	screenPosition.z = 1.0f;
	ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);
	WorldPosition = DirectX::XMVector3Unproject(
		ScreenPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);
	DirectX::XMFLOAT3 rayEnd;
	DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

	// レイキャスト
	return Stage::Instance().RayCast(rayStart, rayEnd, hit);
}