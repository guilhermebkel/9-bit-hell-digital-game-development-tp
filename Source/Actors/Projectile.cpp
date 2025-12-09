// File Path: ./Source/Actors/Projectile.cpp

#include "Projectile.h"
#include "../Game.h"
#include "Enemy.h"
#include "Miniboss.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

Projectile::Projectile(Game* game, int damage)
    : Actor(game)
    , mEnemiesHit(0)
    , mDamage(damage)
    , mGamePtr(game)
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

    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        int maxEnemiesHit = 1 + mGamePtr->GetPlayerPiercing();

        Actor* owner = other->GetOwner();
        if (!owner) return;

        // Se este projétil já acertou este ator, ignora (evita múltiplos hits no mesmo alvo)
        if (mHitActors.find(owner) != mHitActors.end())
        {
            return;
        }

        Enemy* enemy = dynamic_cast<Enemy*>(owner);
        if (enemy)
        {
            enemy->TakeDamage(mDamage);
            mHitActors.insert(owner);
            mEnemiesHit++;
        }

        Miniboss* miniboss = dynamic_cast<Miniboss*>(owner);
        if (miniboss)
        {
            miniboss->TakeDamage(mDamage);
            mHitActors.insert(owner);
            mEnemiesHit++;
        }

        // Destruir projétil apenas se atingiu o máximo de inimigos permitido
        if (mEnemiesHit >= maxEnemiesHit)
        {
            mState = ActorState::Destroy;
        }
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