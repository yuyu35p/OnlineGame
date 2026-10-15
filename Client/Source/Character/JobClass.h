#pragma once
#include "Character/Player.h"


class JobClass
{
public:
	JobClass() { this->owner = nullptr; }
	JobClass(Player* player) :owner(player) {}
	~JobClass() {}

	virtual void Update(float elapsedTime);
	void ChangeState(Player::State state);
	
	// 待機と移動は全員共通。
	// 待機State
	virtual void TransitionIdleState();
	virtual void UpdateIdleState(float elapsedTime);

	// 移動State
	virtual void TransitionMoveState();
	virtual void UpdateMoveState(float elapsedTime);

	// 攻撃とスキルはジョブによって異なるため、純粋仮想関数指定。派生先で実装する。
	// 攻撃ステート
	virtual void TransitionAttackState();
	virtual void UpdateAttackState(float elapsedTime);

	// スキルステート
	virtual void TransitionSkillState() = 0;
	virtual void UpdateSkillState(float elapsedTime);

protected:
	Player* owner;
};

class ArcherClass : public JobClass
{
public:
	ArcherClass(Player* player) :JobClass(player) {}

	// 攻撃ステート
	void TransitionAttackState()override;
	void UpdateAttackState(float elapsedTime)override;

	// スキルステート
	void TransitionSkillState()override;
	void UpdateSkillState(float elapsedTime)override;
};
class MageClass : public JobClass
{
public:
	MageClass(Player* player) :JobClass(player) {}

	// 攻撃ステート
	void TransitionAttackState()override;
	void UpdateAttackState(float elapsedTime)override;

	// スキルステート
	void TransitionSkillState()override;
	void UpdateSkillState(float elapsedTime)override;
};
class RogueClass : public JobClass
{
public:

	RogueClass(Player* player) :JobClass(player) {}

	// 攻撃ステート
	void TransitionAttackState()override;
	void UpdateAttackState(float elapsedTime)override;

	// スキルステート
	void TransitionSkillState()override;
	void UpdateSkillState(float elapsedTime)override;
};
class TankClass : public JobClass
{
public:

	TankClass(Player* player) :JobClass(player) {}

	// 攻撃ステート
	void TransitionAttackState()override;
	void UpdateAttackState(float elapsedTime)override;

	// スキルステート
	void TransitionSkillState()override;
	void UpdateSkillState(float elapsedTime)override;
};