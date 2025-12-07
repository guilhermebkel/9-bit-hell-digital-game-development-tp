#include "Spawner.h"
#include "../Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Coin.h"
#include "Purifier.h"
#include "Healer.h"
#include "FatMiniboss.h"
#include "HornMiniboss.h"
#include "EyeMiniboss.h"
#include "../Random.h"
#include <algorithm>
#include <array>

Spawner::Spawner(Game* game, SpawnType type, int totalCount, int waveSize, int keepPopulation, bool waitForClear)
    : Actor(game)
    , mSpawnType(type)
    , mTotalCount(totalCount)
    , mRemainingCount(totalCount)
    , mWaveSize(waveSize)
    , mKeepPopulation(keepPopulation)
    , mWaitForClear(waitForClear)
    , mInitialSpawnDelay(0.0f)
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

    if (mWaitForClear)
    {
        bool areThereEnemySpawners = false;
        for (auto actor : GetGame()->GetActors())
        {
            if (actor == this || actor->GetState() != ActorState::Active) continue;

            auto otherSpawner = dynamic_cast<Spawner*>(actor);
            if (otherSpawner && !otherSpawner->mWaitForClear)
            {
                areThereEnemySpawners = true;
                break;
            }
        }

        if (activeEnemies == 0 && !areThereEnemySpawners)
        {
            SpawnOne();
            mRemainingCount--;
        }
        return;
    }

    if (activeEnemies <= mKeepPopulation)
    {
        int countToSpawn = std::min(mWaveSize, mRemainingCount);
        for (int i = 0; i < countToSpawn; ++i)
        {
            SpawnOne();
        }
        mRemainingCount -= countToSpawn;
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
    
    const float MIN_SPAWN_DISTANCE = 100.0f;
    bool validSpawn = false;
    int maxAttempts = 10;
    
    for (int attempt = 0; attempt < maxAttempts && !validSpawn; ++attempt)
    {
        spawnPos = Vector2(
            Random::GetFloatRange(100.0f, Game::WINDOW_WIDTH - 100.0f),
            Random::GetFloatRange(GetGame()->GetUpperBoundary() + 100.0f, Game::WINDOW_HEIGHT - 100.0f)
        );
        
        if (!player || Vector2::Distance(spawnPos, player->GetPosition()) >= MIN_SPAWN_DISTANCE)
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
            std::array<Enemy::EnemyType, 3> enemyTypes = {
                Enemy::EnemyType::Eye,
                Enemy::EnemyType::Horn,
                Enemy::EnemyType::Fat
            };
            int randomIndex = Random::GetIntRange(0, static_cast<int>(enemyTypes.size()) - 1);
            Enemy* enemy = new Enemy(GetGame(), enemyTypes[randomIndex]);
            enemy->SetPosition(spawnPos);
            break;
        }
        case SpawnType::Coin:
        {
            Coin* coin = new Coin(GetGame());
            coin->SetPosition(spawnPos);
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