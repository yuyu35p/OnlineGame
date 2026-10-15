#include "Character/Actor.h"
#include "Graphics/Graphics.h"


Actor::Actor()
{
	model = nullptr;
	position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	angle = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	targetPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	scale = angle = DirectX::XMFLOAT3(0.2f, 0.2f, 0.2f);

}

Actor::~Actor()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
}

void Actor::Update(float elapsedTime)
{
	// オブジェクト行列を更新
	UpdateTransform();

	// アニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

void Actor::Render(ID3D11DeviceContext* context, Shader* shader)
{
	shader->Draw(context, model);
}


// 目的地点へ移動
void Actor::MoveToTarget(float elapsedTime, float vx , float vz ,float speedRate)
{
	// 移動処理
	Move(vx, vz, moveSpeed * speedRate);
	Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}
void Actor::Move(float vx, float vz, float speed)
{
	// 移動方向ベクトルを設定
	float mx = vx * speed;
	float mz = vz * speed;
	position.x += mx;
	position.z += mz;
}
// 旋回処理
void Actor::Turn(float elapsedTime, float vx, float vz, float speed)
{
	speed *= elapsedTime;

	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	float length = sqrtf(vx * vx + vz * vz);
	if (length <= 0.001f)
	{
		return;
	}
	// 進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	// 自身の回転値から前方向を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	// 回転角を求めるため、2つの単位ベクトルの内積を計算する
	float dot = frontX * vx + frontZ * vz;

	// 内積値は-1.0～1.0で表現されており、2つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;

	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontX * vz) - (frontZ * vx);

	// 2Dの外積値が生の場合か負の場合かによって左右判定が行える
	// 左右判定を行うことによって左右回転を選択する
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}
void Actor::LookAt(float vx, float vz)
{
	vx = vx - position.x;
	vz = vz - position.z;
	// 生成したベクトルの角度を算出
	float radian = atan2f(vx, vz);
	angle.y = radian;
}

void Actor::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);

	DirectX::XMMATRIX R = Y * X * Z;

	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

void Actor::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	// 衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));

}

