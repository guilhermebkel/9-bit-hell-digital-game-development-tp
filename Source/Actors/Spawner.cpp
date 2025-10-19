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

        const class Player* player = GetGame()->GetPlayer();

        if (player)
        {
                Enemy* enemy = new Enemy(GetGame());

                enemy->SetPosition(Vector2(
                        Random::GetIntRange(100, Game::WINDOW_WIDTH - 100),
                        Random::GetIntRange(GetGame()->GetUpperBoundary() + 100, Game::WINDOW_HEIGHT - 100)
                ));

                mState = ActorState::Destroy;
        }
}