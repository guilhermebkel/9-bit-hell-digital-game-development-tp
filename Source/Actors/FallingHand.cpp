#include "FallingHand.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

FallingHand::FallingHand(Game* game)
    : Actor(game)
{
    new StaticSpriteComponent(this, "../Assets/HandProjectile.png", SPRITE_WIDTH, SPRITE_HEIGHT, 150);

    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
    mRigidBody->SetVelocity(Vector2(0.0f, FALL_SPEED));

    new AABBColliderComponent(this, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, ColliderLayer::Enemy);
}

void FallingHand::OnUpdate(float deltaTime)
{
    if (GetPosition().y > Game::WINDOW_HEIGHT)
    {
        SetState(ActorState::Destroy);
    }
}

void FallingHand::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (player)
        {
            player->TakeDamage(DAMAGE);
            SetState(ActorState::Destroy);
        }
    }
}

void FallingHand::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (player)
        {
            player->TakeDamage(DAMAGE);
            SetState(ActorState::Destroy);
        }
    }
}