#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "Mario.h"

Goomba::Goomba(Game* game, float forwardSpeed, float deathTime)
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
        32,
        32
    );

    mDrawComponent->AddAnimation("walk", {1, 2});
    mDrawComponent->AddAnimation("dead", {0});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(5.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f);
    
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 32, 32, ColliderLayer::Enemy);
    
    SetScale(Vector2(-1.0f, 1.0f));
    Vector2 initialVelocity(-mForwardSpeed, 0.0f);
    mRigidBodyComponent->SetVelocity(initialVelocity);
}

void Goomba::Kill()
{
    if (mIsDying) return;

    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Goomba::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f)
        {
            mState = ActorState::Destroy;
        }
    }

    if (GetPosition().y > Game::WINDOW_HEIGHT + 100)
    {
        mState = ActorState::Destroy;
    }
}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

    if (other->GetLayer() == ColliderLayer::Player)
    {
        Mario* mario = dynamic_cast<Mario*>(other->GetOwner());
        if (mario) {
            mario->Kill();
        }
        return;
    }

    if (other->GetLayer() == ColliderLayer::Blocks || other->GetLayer() == ColliderLayer::Enemy)
    {
        Vector2 currentScale = GetScale();
        currentScale.x *= -1.0f;
        SetScale(currentScale);

        Vector2 newVelocity(mForwardSpeed * currentScale.x, mRigidBodyComponent->GetVelocity().y);
        mRigidBodyComponent->SetVelocity(newVelocity);
    }
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
}