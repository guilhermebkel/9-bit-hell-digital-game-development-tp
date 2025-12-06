#include "Spawner.h"
#include "../Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Coin.h"
#include "Purifier.h"
#include "../Random.h"
#include <cmath>

Spawner::Spawner(Game* game, SpawnType type, int count, bool spawnOnStart)
    : Actor(game)
    , mSpawnType(type)
    , mSpawnCount(count)
    , mHasSpawned(!spawnOnStart)
{}

void Spawner::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    if (mHasSpawned)
    {
        mState = ActorState::Destroy;
        return;
    }

    const class Player* player = GetGame()->GetPlayer();
    if (player)
    {
        for (int i = 0; i < mSpawnCount; ++i)
        {
            Vector2 spawnPos = Vector2(
                Random::GetFloatRange(100.0f, Game::WINDOW_WIDTH - 100.0f),
                Random::GetFloatRange(GetGame()->GetUpperBoundary() + 100.0f, Game::WINDOW_HEIGHT - 100.0f)
            );

            switch (mSpawnType)
            {
                case SpawnType::Enemy:
                {
                    std::array<Enemy::EnemyType, 3> enemyTypes = {
                        Enemy::EnemyType::Eye,
                        Enemy::EnemyType::Horn,
                        Enemy::EnemyType::Fat
                    };

                    // Escolhe um tipo de inimigo aleatoriamente
                    auto randomIndex = Random::GetIntRange(0, enemyTypes.size() - 1);
                    auto enemyType = enemyTypes[randomIndex];
                    Enemy* enemy = new Enemy(GetGame(), enemyType);
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
            }
        }

        mHasSpawned = true;
    }
}