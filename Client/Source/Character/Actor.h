#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Collision/Collision.h"
#include "Stage/Stage.h"


class Actor
{
public:
	// アニメーション
	enum class Animation
	{
		Idle,
		Walk,
		Run,
		Pickup,
		Revive,
		ImpDefeat,
		Fall,
		EmoteCheer,
		EmoteDance,
		EmoteWave,
		EmoteSitStart,
		EmoteSitLoop,
		EmoteSitEnd,
		BossChargeAttackStart,
		BossChargeAttackLoop,
		BossChargeAttackEnd,
		Stunned,
		BossDefeatIdle,
		FallPose,
		RunStart,
		CarryItemPickup,
		CarryItemPlace,
		CarryItemIdle,
		CarryItemWalk,
		Attack1,
		Attack2,
		SkillHeal,
		SkillShieldBufLoop,
		SkillShieldBufStart,
		SkillPowershotEnd,
		SkillPowershotLoop,
		SkillPowershotStart,
		SkillSneakPoof,
		SkillDashAttackStart,
		SkillDashAttackLoop,
		SkillDashAttackEnd,
		HitreactCritical,
		End
	};

	Actor();
	~Actor();

	virtual void Update(float elapsedTime);

	// 描画
	virtual void Render(ID3D11DeviceContext* context, Shader* shader);

	// デバッグ描画
	virtual void DrawDebugPrimitive();


	// model取得
	Model* GetModel() { return model; }

	// 座標取得
	DirectX::XMFLOAT3 GetPosition() { return position; }

	// 座標設定
	void SetPosition(DirectX::XMFLOAT3 setPosition) { position = setPosition; }

	// 角度取得
	DirectX::XMFLOAT3 GetAngle() { return angle; }

	// 角度設定
	void SetAngle(DirectX::XMFLOAT3 setAngle) { angle = setAngle; }

	// 倍率取得
	DirectX::XMFLOAT3 GetScale() { return scale; }

	// 倍率設定
	void SetScale(DirectX::XMFLOAT3 setScale) { scale = setScale; }

	// 移動先取得
	DirectX::XMFLOAT3 GetTargetPosition() { return targetPosition; }

	// 移動先設定
	void SetTargetPosition(DirectX::XMFLOAT3 setTargetPosition) { targetPosition = setTargetPosition; }

	// 生死判断
	void SetAlived(bool alived) { this->alived = alived; }
	bool GetAlived() { return alived; }
	// 体力関連

	void SetHealth(int health) { this->health = health; }
	int GetHealth() { return health; }
	float GetTurnSpeed(){ return turnSpeed; }
	// 移動先取得
	// ターゲットまで移動
	void MoveToTarget(float elapsedTime, float vx, float vz, float speedRate);

	// 移動処理
	void Move(float vx, float vz, float speed);

	// 回転
	void Turn(float elapsedTime, float vx, float vz, float speed);
	void LookAt(float vx ,float vz);
	// Transform更新
	void UpdateTransform();


protected:
	DirectX::XMFLOAT3 position{ 0,0,0 };
	DirectX::XMFLOAT3 angle{ 0,0,0 };
	DirectX::XMFLOAT3 scale;
	bool alived = true;
	int health = 10;
	DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };
	Model* model = nullptr;

	float attackRange = 2.5;
	float range = 10.0f;
	float radius = 2.5f;
	float height = 4.0f;
	float maxMoveSpeed = 0.5f;
	float moveSpeed = 0.2f;
	float turnSpeed = DirectX::XMConvertToRadians(360);

	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float	stepOffset = 1.0f;
};