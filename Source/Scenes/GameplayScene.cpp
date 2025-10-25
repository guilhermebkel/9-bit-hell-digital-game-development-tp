#include "GameplayScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Actors/Player.h"
#include "../Actors/CorruptionOverlay.h"
#include "../Actors/Spawner.h"
#include "../Actors/HUD.h"

GameplayScene::GameplayScene(Game* game) : Scene(game) {}

void GameplayScene::Load()
{
    new Background(GetGame());

    GetGame()->SetPlayer(new Player(GetGame()));

    new CorruptionOverlay(GetGame());

    new Spawner(GetGame(), SpawnType::Enemy, 5);
    new Spawner(GetGame(), SpawnType::Coin, 10);
    new Spawner(GetGame(), SpawnType::Purifier, 2);

    new HUD(GetGame());
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