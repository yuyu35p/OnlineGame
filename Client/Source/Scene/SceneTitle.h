#pragma once

#include "Assignment\00ClientSample.h"
#include "Assignment\01ClientAssignment.h"
#include "Assignment\02ClientAssignment.h"
#include "Assignment\03ClientAssignment.h"
#include "Assignment\04ClientAssignment.h"
#include "Assignment\05ClientAssignment.h"
#include "Assignment\06ClientAssignment.h"
#include "Assignment\07ClientAssignment.h"
#include "Assignment\08ClientAssignment.h"
#include "Assignment\09ClientAssignment.h"
#include "Assignment\10ClientAssignment.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Graphics/Graphics.h"
#include "Graphics/Sprite.h"
#include "Input\Input.h"




// コンフィグシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {}

	// 初期化
	void Initialize() override;

	void Update(float elapsedTime) {};

	// 終了化
	void Finalize() override;

	void Render()override;
private:
	std::unique_ptr<Sprite> logo = nullptr;
	std::unique_ptr<Sprite> background = nullptr;
};
