#include "Stage.h"


static Stage* instance = nullptr;

// インスタンス取得
Stage& Stage::Instance()
{
	return *instance;
}
Stage::Stage()
{
	instance = this;
	model = new Model("Data/Model/Environment/BossRoom.mdl");
}

Stage::~Stage()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
void Stage::Update(float elapsedTime)
{
	// 今は特にやることはない
}

void Stage::Render(ID3D11DeviceContext* dc, Shader* shader)
{
	shader->Draw(dc,model);
}

// レイキャスト
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model,hit);
}