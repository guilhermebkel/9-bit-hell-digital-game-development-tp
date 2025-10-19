//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Player.h"
#include "Enemy.h"
#include "../Random.h"
#include <cmath>

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{}

void Spawner::OnUpdate(float deltaTime)
{
        Actor::OnUpdate(deltaTime);

        for (int i = 0; i < 10; ++i)
        {
                Enemy* enemy = new Enemy(GetGame());

                enemy->SetPosition(Vector2(
                    Random::GetFloatRange(100.0f, Game::WINDOW_WIDTH - 100.0f),
                    Random::GetFloatRange(GetGame()->GetUpperBoundary() + 100.0f, Game::WINDOW_HEIGHT - 100.0f)
                ));
        }

        mState = ActorState::Destroy;
}