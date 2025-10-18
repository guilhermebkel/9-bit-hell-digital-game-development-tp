// File Path: ./Source/Actors/Goomba.cpp

#include "Enemy.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Random.h" // Precisamos para a velocidade aleatória

Enemy::Enemy(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
        , mDyingTimer(deathTime)
        , mRigidBodyComponent(nullptr)
        , mColliderComponent(nullptr)
        , mDrawComponent(nullptr)
{
    mDrawComponent = new AnimatorComponent(
        this,
        "../Assets/Sprites/Goomba/Goomba.png",
        "../Assets/Sprites/Goomba/Goomba.json",
        Enemy::WIDTH,
        Enemy::HEIGHT
    );

    mDrawComponent->AddAnimation("walk", {1, 2});
    mDrawComponent->AddAnimation("dead", {0});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(5.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, Enemy::WIDTH, Enemy::HEIGHT, ColliderLayer::Enemy);

    Vector2 initialVelocity = Vector2::Zero;
    while (initialVelocity.Length() < 1.0f)
    {
        initialVelocity = Random::GetVector(Vector2(-mForwardSpeed, -mForwardSpeed), Vector2(mForwardSpeed, mForwardSpeed));
    }
    mRigidBodyComponent->SetVelocity(initialVelocity);
}

void Enemy::Kill()
{
    if (mIsDying) return;
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Enemy::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f)
        {
            mState = ActorState::Destroy;
        }
        return;
    }

    Vector2 pos = GetPosition();
    Vector2 vel = mRigidBodyComponent->GetVelocity();

    const float halfWidth = Enemy::WIDTH / 2.0f;
    const float halfHeight = Enemy::HEIGHT / 2.0f;

    if ((pos.x <= halfWidth && vel.x < 0.0f) || (pos.x >= Game::WINDOW_WIDTH - halfWidth && vel.x > 0.0f))
    {
        vel.x *= -1.0f;
    }

    if ((pos.y <= GetGame()->GetUpperBoundary() + halfHeight && vel.y < 0.0f) || (pos.y >= Game::WINDOW_HEIGHT - halfHeight && vel.y > 0.0f))
    {
        vel.y *= -1.0f;
    }

    mRigidBodyComponent->SetVelocity(vel);
}

void Enemy::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
}

void Enemy::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
}