#include "GameplayScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Actors/Player.h"
#include "../Actors/CorruptionOverlay.h"
#include "../Actors/Enemy.h"
#include "../Actors/Spawner.h"
#include "../Actors/HUD.h"
#include "../Audio/AudioSystem.h"


GameplayScene::GameplayScene(Game* game, LevelID level)
    : Scene(game)
    , mLevelID(level)
{}

void GameplayScene::Load()
{
    GetGame()->SetPlayer(new Player(GetGame()));

    new CorruptionOverlay(GetGame());
    new HUD(GetGame());
    GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/gameplay_03-M1-Stage-5-Synapse-Retrogenesis.ogg");
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

    if (IsLevelComplete())
    {
        GetGame()->SetCurrentLevelID(mNextLevelID);
        GetGame()->SetScene(Game::GameScene::Upgrade);
    }

    bool playerDiedByEnemy = GetGame()->GetPlayer()->GetHealth() <= 0.0f;
    bool playerDiedByCorruption = GetGame()->GetCorruptionLevel() >= 1.0f;
    bool isGameOver = playerDiedByEnemy || playerDiedByCorruption;

    if (isGameOver)
    {
        GetGame()->SetScene(Game::GameScene::GameOver);
    }
}

void GameplayScene::ProcessInput(const uint8_t* keyState)
{
}

void GameplayScene::LoadTutorial()
{
    SetNextLevelID(LevelID::Tutorial);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 10);
    new Spawner(GetGame(), SpawnType::Coin, 12);
    new Spawner(GetGame(), SpawnType::Purifier, 2);
}

void GameplayScene::LoadLevel1()
{

}

bool GameplayScene::IsLevelComplete()
{
    for (auto actor : GetGame()->GetActors())
    {
        if (dynamic_cast<Enemy*>(actor))
        {
            return false;
        }
    }

    return true;
}