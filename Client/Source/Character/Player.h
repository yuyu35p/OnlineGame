#pragma once
#include "Character/Actor.h"

const int MAX_CLIENT = 4;
class JobClass;

class Player:public Actor
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
	enum class JobType
	{
		ArcherF = 0,
		ArcherM,
		MageF,
		MageM,
		RogueF,
		RogueM,
		TankF,
		TankM,
		End
	};

	// コンストラクタ
	Player();
	Player(int id);

	// デストラクタ
	~Player();

	// 更新
	void Update(float elapsedTime);

	// 描画
	void Render(ID3D11DeviceContext* context, Shader* shader);

	// ID
	short GetPlayerID() const{ return playerID; }
	void SetPlayerID(int id) { playerID = id; }

	// ステート
	Player::State GetState() { return state; }
	void SetState(Player::State state) { this->state = state; }
	// ジョブ指定
	JobType GetJobType() { return jobType; }
	void SetJobType(JobType jobType);

	// Jobステートマシン
	JobClass* GetJobClass() { return jobClass; }
	void SetJobClass(JobClass* jobClass) { this->jobClass = jobClass; }

	// 準備フラグ
	bool GetReady() { return ready; }
	void SetReady(bool flg) { ready = flg; }

	// モデル読み込み
	void SetModel();

protected:
	short playerID;
	JobType jobType = JobType::ArcherF;
	JobClass* jobClass = nullptr;
	Player::State state = Player::State::End;
	bool ready = false;
};

class PlayerManager
{
private:
	std::vector<Player*> players;
	short myPlayerId = -1;
	std::vector<Player*> deleteList;
public:
	PlayerManager() {}
	~PlayerManager() {}

	static PlayerManager& Instance()
	{
		static PlayerManager instance;
		return instance;
	}

	void AddPlayer(Player* player) { players.emplace_back(player); };
	std::vector<Player*> GetPlayers() { return players; }
	Player* GetPlayer(int id);
	Player* GetMyPlayer();
	void SetMyPlayerID(int id) { myPlayerId = id; }
	short GetMyPlayerID() { return myPlayerId; }
	void ErasePlayer(int id);
	void DeletePlayer();
};