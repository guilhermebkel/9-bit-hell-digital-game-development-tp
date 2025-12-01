// File Path: ./Source/Actors/Projectile.cpp

#include "Projectile.h"
#include "../Game.h"
#include "Enemy.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

Projectile::Projectile(Game* game)
    : Actor(game)
    , mHasHitEnemy(false)
{
    new StaticSpriteComponent(this, "../Assets/Projectile.png", Projectile::SPRITE_WIDTH, Projectile::SPRITE_HEIGHT);

    new RigidBodyComponent(this, 1.0f, 0.0f);

    new AABBColliderComponent(this, 0, Projectile::SPRITE_HEIGHT, Projectile::PHYSICS_WIDTH, Projectile::PHYSICS_HEIGHT, ColliderLayer::Projectile);
}

void Projectile::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    Vector2 pos = GetPosition();
    if (pos.x < 0.0f || pos.x > Game::WINDOW_WIDTH)
    {
        mState = ActorState::Destroy;
    }
}

void Projectile::HandleCollision(AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy && !mHasHitEnemy)
    {
        Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
        if (enemy)
        {
            enemy->Kill();
            mHasHitEnemy = true;
        }

        mState = ActorState::Destroy;
    }
}

void Projectile::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}

void Projectile::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}