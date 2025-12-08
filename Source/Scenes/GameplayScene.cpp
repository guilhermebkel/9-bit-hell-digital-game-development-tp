#include "GameplayScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Actors/Player.h"
#include "../Actors/CorruptionOverlay.h"
#include "../Actors/Enemy.h"
#include "../Actors/Miniboss.h"
#include "../Actors/Spawner.h"
#include "../Actors/Soul.h"
#include "../Actors/HUD.h"
#include "../Audio/AudioSystem.h"


GameplayScene::GameplayScene(Game* game, LevelID level)
    : Scene(game)
    , mLevelID(level)
{}

void GameplayScene::Load()
{
    GetGame()->LoadGame();
    GetGame()->ResetCorruptionLevel();
    GetGame()->SetPlayer(new Player(GetGame(), GetGame()->GetPlayerSpeed()));

    new CorruptionOverlay(GetGame());
    new HUD(GetGame());

    switch (mLevelID)
    {
        case LevelID::Tutorial: LoadTutorial(); break;
        case LevelID::Level1: LoadLevel1(); break;
        case LevelID::Level2: LoadLevel2(); break;
        case LevelID::Level3: LoadLevel3(); break;
        case LevelID::Level4: LoadLevel4(); break;
        case LevelID::Level5: LoadLevel5(); break;
        case LevelID::Level6: LoadLevel6(); break;
        case LevelID::Level7: LoadLevel7(); break;
        case LevelID::Level8: LoadLevel8(); break;
        case LevelID::Level9: LoadLevel9(); break;
    }

    GetGame()->GetAudioSystem()->SetMusicVolume(0.33f);
}

void GameplayScene::Unload()
{
}

void GameplayScene::Update(float deltaTime)
{
    // Toca a música quando a transição termina
    if (!mMusicPlayed && GetGame()->GetSceneState() == Game::SceneState::Running)
    {
        mMusicPlayed = true;
        switch (mLevelID)
        {
            case LevelID::Tutorial:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/gameplay-m1-stage5-synapse-retrogenesis.ogg");
                break;
            case LevelID::Level1:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_1_c2_misplaced_in_time.mp3");
                break;
            case LevelID::Level2:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_2_d5_the_way_ahead_feels_lonely.mp3");
                break;
            case LevelID::Level3:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_3_e2_and_heart_breaks.mp3");
                break;
            case LevelID::Level4:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_4_e6_sublime_beyond_loss.mp3");
                break;
            case LevelID::Level5:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_5_f7_libet_delay.mp3");
                break;
            case LevelID::Level6:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_6_g1_stage_4_post_awareness_confusions.mp3");
                break;
            case LevelID::Level7:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_7_l1_stage_5_advanced_plaque_entanglements.mp3");
                break;
            case LevelID::Level8:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_8_o1_stage_6_a_confusion_so_thick_you_forget_forgetting.mp3");
                break;
            case LevelID::Level9:
                GetGame()->GetAudioSystem()->PlayMusic("../Assets/Sounds/level_9_q1_stage_6_long_decline_is_over.mp3");
                break;
        }
    }

    if (GetGame()->GetPlayer())
    {
        GetGame()->AddCorruption(GetGame()->GetCorruptionRate() * deltaTime);
    }

    if (IsLevelComplete())
    {
        bool isFinalLevel = GetGame()->GetCurrentLevelID() == LevelID::Level9;

        if (isFinalLevel)
        {
            GetGame()->SetScene(Game::GameScene::Victory);
        } else
        {
            GetGame()->UnlockNextLevel(mNextLevelID);
            GetGame()->SetCurrentLevelID(mNextLevelID);
            GetGame()->SetScene(Game::GameScene::Store);
        }
    }

    bool playerDiedByEnemy = GetGame()->GetPlayer()->GetHealth() <= 0.0f;
    bool playerDiedByCorruption = GetGame()->GetCorruptionLevel() >= 1.0f;
    bool isGameOver = playerDiedByEnemy || playerDiedByCorruption;

    if (isGameOver)
    {
        GetGame()->SetScene(Game::GameScene::GameOver);

        if (playerDiedByCorruption)
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/game-over-by-corruption.ogg");
        } else
        {
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/game-over.mp3");
        }
    }
}

void GameplayScene::ProcessInput(const uint8_t* keyState)
{
}

void GameplayScene::LoadTutorial()
{
    SetNextLevelID(LevelID::Level1);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 5);
    new Spawner(GetGame(), SpawnType::Soul, 5);
    new Spawner(GetGame(), SpawnType::Purifier, 1);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel1()
{
    SetNextLevelID(LevelID::Level2);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 15, 5, 2);
    new Spawner(GetGame(), SpawnType::Soul, 10);
    new Spawner(GetGame(), SpawnType::Purifier, 2);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel2()
{
    SetNextLevelID(LevelID::Level3);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 25, 6, 3);
    new Spawner(GetGame(), SpawnType::Soul, 15);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel3()
{
    SetNextLevelID(LevelID::Level4);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 10, 5, 2);
    new Spawner(GetGame(), SpawnType::EyeMiniboss, 1, 1, 0, true);
    new Spawner(GetGame(), SpawnType::Soul, 15);
    new Spawner(GetGame(), SpawnType::Purifier, 5);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel4()
{
    SetNextLevelID(LevelID::Level5);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 35, 7, 3);
    new Spawner(GetGame(), SpawnType::Soul, 20);
    new Spawner(GetGame(), SpawnType::Purifier, 1);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel5()
{
    SetNextLevelID(LevelID::Level6);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 40, 8, 4);
    new Spawner(GetGame(), SpawnType::Soul, 15);
    new Spawner(GetGame(), SpawnType::Healer, 2);
}

void GameplayScene::LoadLevel6()
{
    SetNextLevelID(LevelID::Level7);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 12, 6, 0);
    new Spawner(GetGame(), SpawnType::FatMiniboss, 1, 1, 0, true);
    new Spawner(GetGame(), SpawnType::Purifier, 5);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel7()
{
    SetNextLevelID(LevelID::Level8);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 50, 10, 5);
    new Spawner(GetGame(), SpawnType::Soul, 15);
    new Spawner(GetGame(), SpawnType::Purifier, 1);
    new Spawner(GetGame(), SpawnType::Healer, 1);
}

void GameplayScene::LoadLevel8()
{
    SetNextLevelID(LevelID::Level9);

    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 60, 10, 5);
    new Spawner(GetGame(), SpawnType::Soul, 15);
    new Spawner(GetGame(), SpawnType::Healer, 3);
}

void GameplayScene::LoadLevel9()
{
    new Background(GetGame(), "../Assets/Levels/Level1/BackgroundGame.png");
    GetGame()->SetUpperBoundary(423.0f);

    new Spawner(GetGame(), SpawnType::Enemy, 20, 10, 0);
    new Spawner(GetGame(), SpawnType::HornMiniboss, 1, 1, 0, true);
    new Spawner(GetGame(), SpawnType::Soul, 15);
    new Spawner(GetGame(), SpawnType::Purifier, 5);
    new Spawner(GetGame(), SpawnType::Healer, 3);
}

bool GameplayScene::IsLevelComplete()
{
    bool anyEnemyLike = false;
    bool anySoul = false;

    for (auto actor : GetGame()->GetActors())
    {
        if (actor->GetState() != ActorState::Active)
        {
            continue;
        }

        if (dynamic_cast<Enemy*>(actor) || dynamic_cast<Miniboss*>(actor) || dynamic_cast<Spawner*>(actor))
        {
            // if there are still enemies/minibosses/spawners active, level not complete
            return false;
        }

        // If there are souls left in the level, wait until player collects them
        if (dynamic_cast<class Soul*>(actor))
        {
            anySoul = true;
        }
    }

    // No enemies/spawners/minibosses alive; only finish when there are no souls left
    if (anySoul)
    {
        return false;
    }

    return true;
}