#include "Character/Player.h"
#include "Graphics/Graphics.h"
#include "Camera/Camera.h"
#include "Collision/Collision.h"
#include "Stage/Stage.h"
#include "Input/Input.h"
#include "Character/JobClass.h"

// メモリリーク調査用
#define debug_new new(_NORMAL_BLOCK,__FILE__,__LINE__)


Player::Player()
{
	playerID = 0;
	radius = 2.5f;
	height = 3.8f;
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	angle.x = 0.0f;
	angle.y = 0.0f;
	angle.z = 0.0f;
	jobType = Player::JobType::ArcherF;
	jobClass = new ArcherClass(this);
	ready = false;
	scale.x = 0.02f;
	scale.y = 0.02f;
	scale.z = 0.02f;
}

Player::Player(int id)
{
	playerID = id;
	radius = 2.5f;
	height = 3.8f;
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	angle.x = 0.0f;
	angle.y = 0.0f;
	angle.z = 0.0f;
	jobType = Player::JobType::ArcherF;
	jobClass = new ArcherClass(this);
	ready = false;
	scale.x = 0.02f;
	scale.y = 0.02f;
	scale.z = 0.02f;
}
Player::~Player()
{
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
	if (jobClass != nullptr)
	{
		delete jobClass;
		jobClass = nullptr;
	}
}
void Player::Update(float elapsedTime)
{
	// 受信したデータに合わせて更新
	jobClass->Update(elapsedTime);

	// オブジェクト行列を更新
	UpdateTransform();

	// アニメーション更新
	model->UpdateAnimation(elapsedTime);

	// モデル行列更新
	model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* context, Shader* shader)
{
	shader->Draw(context, model);
}


void Player::SetJobType(JobType jobType)
{
	this->jobType = jobType;
}

void Player::SetModel()
{
	if (jobType == Player::JobType::End)return;
	if (model != nullptr)
	{
		delete model;
		model = nullptr;
	}
	switch (this->jobType)
	{
	case JobType::ArcherF:
		model = debug_new Model("Data/Model/Character/ArcherGirl.mdl");
		break;
	case JobType::ArcherM:
		model = debug_new Model("Data/Model/Character/ArcherBoy.mdl");
		break;
	case JobType::MageF:
		model = debug_new Model("Data/Model/Character/MageGirl.mdl");
		break;
	case JobType::MageM:
		model = debug_new Model("Data/Model/Character/MageBoy.mdl");
		break;
	case JobType::RogueF:
		model = debug_new Model("Data/Model/Character/RogueGirl.mdl");
		break;
	case JobType::RogueM:
		model = debug_new Model("Data/Model/Character/RogueBoy.mdl");
		break;
	case JobType::TankF:
		model = debug_new Model("Data/Model/Character/TankGirl.mdl");
		break;
	case JobType::TankM:
		model = debug_new Model("Data/Model/Character/TankBoy.mdl");
		break;
	default:
		model = debug_new Model("Data/Model/Character/ArcherGirl.mdl");
		break;
	}
}

Player* PlayerManager::GetPlayer(int id)
{
	for (Player* player : players)
	{
		if (player->GetPlayerID() == id)
		{
			return player;
		}
	}
	return nullptr;
}

Player* PlayerManager::GetMyPlayer()
{
	for (Player* player : players)
	{
		if (player->GetPlayerID() == myPlayerId)
		{
			return player;
		}
	}
	return nullptr;
}
void PlayerManager::ErasePlayer(int id)
{
	int i = 0;
	for (Player* player : players)
	{
		if (player->GetPlayerID() == id)
		{
			player->SetReady(false);
			deleteList.emplace_back(player);
			break;
		}
		++i;
	}
}

void PlayerManager::DeletePlayer()
{
	for (Player* player : deleteList)
	{
		// std::vectorから要素を削除する場合はイテレーターで削除しなければならない
		std::vector<Player*>::iterator it = std::find(players.begin(), players.end(), player);
		if (it != players.end())
		{
			players.erase(it);
		}
		delete player;
	}
	deleteList.clear();
}
