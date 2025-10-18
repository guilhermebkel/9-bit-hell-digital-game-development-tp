//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Player.h"
#include "Enemy.h"
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
                printf("\nENTROU\n");
                Enemy* enemy = new Enemy(GetGame());

                enemy->SetPosition(player->GetPosition());

                mState = ActorState::Destroy;
        }
}