#include "Spawner.h"
#include "../Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Soul.h"
#include "Purifier.h"
#include "Healer.h"
#include "FatMiniboss.h"
#include "HornMiniboss.h"
#include "EyeMiniboss.h"
#include "../Random.h"
#include <algorithm>
#include <array>

Spawner::Spawner(Game* game, SpawnType type, int totalCount, int waveSize, int keepPopulation, bool waitForClear, float spawnInterval)
    : Actor(game)
    , mSpawnType(type)
    , mTotalCount(totalCount)
    , mRemainingCount(totalCount)
    , mWaveSize(waveSize)
    , mKeepPopulation(keepPopulation)
    , mWaitForClear(waitForClear)
    , mInitialSpawnDelay(0.0f)
    , mSpawnInterval(spawnInterval)
    , mSpawnTimer(0.0f)
{
    if (mWaveSize <= 0) mWaveSize = totalCount;
    if (mKeepPopulation < 0) mKeepPopulation = 0;
}

void Spawner::OnUpdate(float deltaTime)
{
    if (mInitialSpawnDelay > 0.0f)
    {
        mInitialSpawnDelay -= deltaTime;
        return;
    }

    if (mRemainingCount <= 0)
    {
        SetState(ActorState::Destroy);
        return;
    }

    int activeEnemies = CountActiveEnemies();
    // Update spawn timer
    if (mSpawnTimer > 0.0f)
    {
        mSpawnTimer -= deltaTime;
        if (mSpawnTimer < 0.0f) mSpawnTimer = 0.0f;
    }

    // If this is a collectable spawner and there are no enemies/minibosses
    // or non-collectable spawners left, flush remaining collectables immediately
    bool isCollectable = (mSpawnType == SpawnType::Soul || mSpawnType == SpawnType::PurpleSoul || mSpawnType == SpawnType::Purifier || mSpawnType == SpawnType::Healer);
    if (isCollectable)
    {
        bool anyEnemyLike = false;
        for (auto actor : GetGame()->GetActors())
        {
            if (actor == this || actor->GetState() != ActorState::Active) continue;

            if (dynamic_cast<Enemy*>(actor) || dynamic_cast<Miniboss*>(actor))
            {
                anyEnemyLike = true;
                break;
            }

            auto otherSpawner = dynamic_cast<Spawner*>(actor);
            if (otherSpawner)
            {
                // if other spawner will spawn enemies (i.e., not a collectable), consider it enemy-like
                if (!(otherSpawner->mSpawnType == SpawnType::Soul || otherSpawner->mSpawnType == SpawnType::PurpleSoul || otherSpawner->mSpawnType == SpawnType::Purifier || otherSpawner->mSpawnType == SpawnType::Healer))
                {
                    anyEnemyLike = true;
                    break;
                }
            }
        }

        if (!anyEnemyLike)
        {
            // spawn all remaining collectables immediately so level can complete
            while (mRemainingCount > 0)
            {
                SpawnOne();
                --mRemainingCount;
            }
            SetState(ActorState::Destroy);
            return;
        }
    }

    if (mWaitForClear)
    {
        bool areThereEnemySpawners = false;
        for (auto actor : GetGame()->GetActors())
        {
            if (actor == this || actor->GetState() != ActorState::Active) continue;

            auto otherSpawner = dynamic_cast<Spawner*>(actor);
            if (otherSpawner)
            {
                // Consider it an enemy spawner only if it will spawn enemies (i.e., not a collectable)
                bool otherIsCollectable = (otherSpawner->mSpawnType == SpawnType::Soul || otherSpawner->mSpawnType == SpawnType::PurpleSoul || otherSpawner->mSpawnType == SpawnType::Purifier || otherSpawner->mSpawnType == SpawnType::Healer);
                if (!otherIsCollectable && !otherSpawner->mWaitForClear)
                {
                    areThereEnemySpawners = true;
                    break;
                }
            }
        }

        if (activeEnemies == 0 && !areThereEnemySpawners)
        {
            if (mSpawnInterval > 0.0f)
            {
                if (mSpawnTimer > 0.0f) return;
                SpawnOne();
                mRemainingCount--;
                mSpawnTimer = mSpawnInterval;
            }
            else
            {
                SpawnOne();
                mRemainingCount--;
            }
        }
        return;
    }

    if (activeEnemies <= mKeepPopulation)
    {
        int countToSpawn = std::min(mWaveSize, mRemainingCount);
        if (mSpawnInterval > 0.0f)
        {
            // Spawn one at a time respecting the interval
            if (mSpawnTimer <= 0.0f)
            {
                SpawnOne();
                mRemainingCount -= 1;
                mSpawnTimer = mSpawnInterval;
            }
        }
        else
        {
            for (int i = 0; i < countToSpawn; ++i)
            {
                SpawnOne();
            }
            mRemainingCount -= countToSpawn;
        }
    }
}

int Spawner::CountActiveEnemies()
{
    int count = 0;
    for (auto actor : GetGame()->GetActors())
    {
        if (actor->GetState() == ActorState::Active)
        {
            if (dynamic_cast<Enemy*>(actor) || dynamic_cast<Miniboss*>(actor))
            {
                count++;
            }
        }
    }
    return count;
}

void Spawner::SpawnOne()
{
    const Player* player = GetGame()->GetPlayer();
    Vector2 spawnPos;
    
    const float MIN_SPAWN_DISTANCE = 68.0f;
    bool validSpawn = false;
    int maxAttempts = 10;
    
    bool isCollectable = (mSpawnType == SpawnType::Soul || mSpawnType == SpawnType::PurpleSoul || mSpawnType == SpawnType::Purifier || mSpawnType == SpawnType::Healer);
    
    for (int attempt = 0; attempt < maxAttempts && !validSpawn; ++attempt)
    {
        spawnPos = Vector2(
            Random::GetFloatRange(100.0f, Game::WINDOW_WIDTH - 100.0f),
            Random::GetFloatRange(GetGame()->GetUpperBoundary() + 100.0f, Game::WINDOW_HEIGHT - 100.0f)
        );
        
        if (isCollectable || !player || Vector2::Distance(spawnPos, player->GetPosition()) >= MIN_SPAWN_DISTANCE)
        {
            validSpawn = true;
        }
    }
    
    if (!validSpawn)
    {
        spawnPos = Vector2(
            Random::GetFloatRange(100.0f, Game::WINDOW_WIDTH - 100.0f),
            Random::GetFloatRange(GetGame()->GetUpperBoundary() + 100.0f, Game::WINDOW_HEIGHT - 100.0f)
        );
    }

    switch (mSpawnType)
    {
        case SpawnType::Enemy:
        {
            // Choose enemy types depending on current level
            auto levelID = GetGame()->GetCurrentLevelID();

            std::vector<Enemy::EnemyType> enemyTypes;

            // Levels 1-3: only Eye
            if (levelID == LevelID::Tutorial || levelID == LevelID::Level1 || levelID == LevelID::Level2 || levelID == LevelID::Level3)
            {
                enemyTypes.push_back(Enemy::EnemyType::Eye);
            }
            // Levels 4-6: Eye and Fat
            else if (levelID == LevelID::Level4 || levelID == LevelID::Level5 || levelID == LevelID::Level6)
            {
                enemyTypes.push_back(Enemy::EnemyType::Eye);
                enemyTypes.push_back(Enemy::EnemyType::Fat);
            }
            // Levels 7-9: all three types
            else
            {
                enemyTypes.push_back(Enemy::EnemyType::Eye);
                enemyTypes.push_back(Enemy::EnemyType::Horn);
                enemyTypes.push_back(Enemy::EnemyType::Fat);
            }

            int randomIndex = Random::GetIntRange(0, static_cast<int>(enemyTypes.size()) - 1);
            Enemy* enemy = new Enemy(GetGame(), enemyTypes[randomIndex]);
            enemy->SetPosition(spawnPos);
            break;
        }
        case SpawnType::Soul:
        {
            Soul* soul = new Soul(GetGame());
            soul->SetPosition(spawnPos);
            break;
        }
        case SpawnType::PurpleSoul:
        {
            Soul* soul = new Soul(GetGame(), Soul::SoulType::Purple);
            soul->SetPosition(spawnPos);
            break;
        }
        case SpawnType::Purifier:
        {
            Purifier* purifier = new Purifier(GetGame());
            purifier->SetPosition(spawnPos);
            break;
        }
        case SpawnType::Healer:
        {
            Healer* healer = new Healer(GetGame());
            healer->SetPosition(spawnPos);
            break;
        }
        case SpawnType::FatMiniboss:
        {
            FatMiniboss* fatMiniboss = new FatMiniboss(GetGame());
            fatMiniboss->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, spawnPos.y));
            break;
        }
        case SpawnType::HornMiniboss:
        {
            HornMiniboss* hornMiniboss = new HornMiniboss(GetGame());
            hornMiniboss->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, spawnPos.y));
            break;
        }
        case SpawnType::EyeMiniboss:
        {
            EyeMiniboss* eyeMiniboss = new EyeMiniboss(GetGame());
            eyeMiniboss->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, spawnPos.y));
            break;
        }
    }
}