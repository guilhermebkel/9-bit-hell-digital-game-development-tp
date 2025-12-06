#include "Collectable.h"
#include "Enemy.h"
#include "../Game.h"
#include "Player.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Collectable::Collectable(Game* game, int width, int height)
    : Actor(game)
{
    float physicsWidth = width * 0.50f;
    float physicsHeight = height * 0.25f;
    // Align collider base with sprite base
    const int dy = (int)((physicsHeight / 2.0f) - (physicsHeight / 2.0f));

    mColliderComponent = new AABBColliderComponent(this, 0, dy, physicsWidth, physicsHeight, ColliderLayer::Collectable, true);
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