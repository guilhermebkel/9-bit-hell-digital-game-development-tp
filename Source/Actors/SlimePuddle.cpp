#include "SlimePuddle.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

SlimePuddle::SlimePuddle(Game* game)
    : Actor(game)
    , mLifeTimer(LIFETIME)
{
    new StaticSpriteComponent(this, "../Assets/SlimePuddle.png", SIZE, SIZE, 80);
    
    mCollider = new AABBColliderComponent(this, 0, 0, SIZE, SIZE, ColliderLayer::Enemy, true);
}

void SlimePuddle::OnUpdate(float deltaTime)
{
    mLifeTimer -= deltaTime;
    
    if (mLifeTimer <= 0.0f)
    {
        SetState(ActorState::Destroy);
    }
}

void SlimePuddle::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (player) player->TakeDamage(DAMAGE);
    }
}

void SlimePuddle::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (player) player->TakeDamage(DAMAGE);
    }
}