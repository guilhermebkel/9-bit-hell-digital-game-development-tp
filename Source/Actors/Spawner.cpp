//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"
#include <cmath>

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{}

void Spawner::OnUpdate(float deltaTime)
{
        Actor::OnUpdate(deltaTime);

        const class Mario* mario = GetGame()->GetPlayer();

        if (mario)
        {
                float dist = std::abs(GetPosition().x - mario->GetPosition().x);

                if (dist < mSpawnDistance)
                {
                        Goomba* goomba = new Goomba(GetGame());

                        goomba->SetPosition(this->GetPosition());

                        mState = ActorState::Destroy;
                }
        }
}