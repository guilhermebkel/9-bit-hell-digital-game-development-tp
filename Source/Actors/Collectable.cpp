// File Path: ./Source/Actors/Collectable.cpp

#include "Collectable.h"
#include "../Game.h"
#include "Player.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Collectable::Collectable(Game* game, const std::string& texturePath, int width, int height)
    : Actor(game)
{
    new StaticSpriteComponent(this, texturePath, width, height, 90);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Collectable, true);
}

void Collectable::HandleCollision(AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        Player* player = dynamic_cast<Player*>(other->GetOwner());
        if (player)
        {
            OnCollect(player);
            mState = ActorState::Destroy;
        }
    }
}

void Collectable::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}

void Collectable::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}