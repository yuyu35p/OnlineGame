#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"

// 更新処理
void CameraController::Update(float elapsedTime)
{
	// キーボードからマウスに変更
	//GamePad& gamePad = Input::Instance().GetGamePad();
	//float ax = gamePad.GetAxisRX();
	//float ay = gamePad.GetAxisRY();
	Mouse& mouse = Input::Instance().GetMouse();
	float ax = static_cast<float>(mouse.GetPositionX() - mouse.GetOldPositionX());
	float ay = static_cast<float>(mouse.GetPositionY() - mouse.GetOldPositionY());
	if (mouse.GetButtonDown() & mouse.BTN_MIDDLE)
	{
		drag = true;
	}
	// カメラの回転速度
	float speed = rollSpeed * elapsedTime;

	if (drag)
	{
		// マウス移動に合わせてX軸とY軸を回転
		angle.x += ay * speed;
		angle.y += ax * speed;
		// X軸のカメラの回転を制御
		if (angle.x > maxAngle.x)
		{
			angle.x = maxAngle.x;
		}
		if (angle.x < minAngle.x)
		{
			angle.x = minAngle.x;
		}
		if (angle.y > maxAngle.y)
		{
			angle.y = 0;
		}
		if (angle.y < minAngle.y)
		{
			angle.y = 0;
		}
	}
	if (mouse.GetButtonUp() & mouse.BTN_MIDDLE)
	{
		drag = false;
	}
	static int a = 0;
	range -= mouse.GetWheel();
	if (range < 10.0f)
	{
		range = 10.f;
	}
	if (range > 70.0f)
	{
		range = 70.f;
	}
	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	
	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];// 行列3行目（Z軸成分）
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);// XMVECTORからXMFLOAT3に変換

	// 注視点から後ろベクトル方向に一定距離を離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * range;
	eye.y = target.y - front.y * range;
	eye.z = target.z - front.z * range;

	// カメラの視点と注視点に設定
	Camera::Instance().SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}