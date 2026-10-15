#pragma once
#include <DirectXMath.h>
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)

class Player
{
public:
	enum class State
	{
		Idle,
		Move,
		Attack,
		Skill,
		End
	};

	short id = 0;
	DirectX::XMFLOAT3 position{ 0,0,0 };
	DirectX::XMFLOAT3 angle{ 0,0,0 };
	DirectX::XMFLOAT3 targetPosition{ 0,0,0 };
	State state=State::Idle;
};