#include "GameplayScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Actors/Player.h"
#include "../Actors/CorruptionOverlay.h"
#include "../Actors/Spawner.h"
#include "../Actors/HUD.h"

GameplayScene::GameplayScene(Game* game, LevelID level)
    : Scene(game)
    , mLevelID(level)
{}

void GameplayScene::Load()
{
    GetGame()->SetPlayer(new Player(GetGame()));
    new CorruptionOverlay(GetGame());
    new HUD(GetGame());

    switch (mLevelID)
    {
        case LevelID::Tutorial:
            LoadTutorial();
            break;
        case LevelID::Level1:
            LoadLevel1();
            break;
    }
}

void GameplayScene::Unload()
{
}

void GameplayScene::Update(float deltaTime)
{
    if (GetGame()->GetPlayer())
    {
        GetGame()->AddCorruption(GetGame()->GetCorruptionRate() * deltaTime);
    }
}

void GameplayScene::ProcessInput(const uint8_t* keyState)
{
}

void GameplayScene::LoadTutorial()
{
    new Background(GetGame(), "../Assets/Levels/Level1/Background.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 5);
    new Spawner(GetGame(), SpawnType::Coin, 10);
    new Spawner(GetGame(), SpawnType::Purifier, 2);
}

void GameplayScene::LoadLevel1()
{

}