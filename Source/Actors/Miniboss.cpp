#include "Miniboss.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

Miniboss::Miniboss(Game* game, float health)
    : Actor(game)
    , mHealth(health)
    , mMaxHealth(health)
    , mIsDead(false)
    , mAnimator(nullptr)
    , mRigidBody(nullptr)
    , mCollider(nullptr)
    , mIsFlashing(false)
    , mFlashTimer(0.0f)
    , mOriginalColor(Vector3::One)
{
}

void Miniboss::OnUpdate(float deltaTime)
{
    if (mIsFlashing && mAnimator)
    {
        mFlashTimer -= deltaTime;
        if (mFlashTimer <= 0.0f)
        {
            mIsFlashing = false;
            mAnimator->SetColor(mOriginalColor);
        }
    }

    Vector2 pos = GetPosition();
    Vector2 vel = mRigidBody->GetVelocity();

    const float halfWidth = mAnimator->GetWidth() / 2.0f;
    const float halfHeight = mAnimator->GetHeight() / 2.0f;

    if ((pos.x <= halfWidth && vel.x < 0.0f) || (pos.x >= Game::WINDOW_WIDTH - halfWidth && vel.x > 0.0f))
    {
        vel.x *= -1.0f;
    }

    if ((pos.y <= GetGame()->GetUpperBoundary() + halfHeight && vel.y < 0.0f) || (pos.y >= Game::WINDOW_HEIGHT -
        halfHeight && vel.y > 0.0f))
    {
        vel.y *= -1.0f;
    }

    mRigidBody->SetVelocity(vel);

    if (vel.x < -1.0f)
    {
        SetScale(Vector2(-1.0f, 1.0f));
    }
    else if (vel.x > 1.0f)
    {
        SetScale(Vector2(1.0f, 1.0f));
    }

    if (mAnimator)
    {
        mAnimator->SetDrawOrder(100 + static_cast<int>(GetPosition().y));
    }
}

void Miniboss::TakeDamage(float amount)
{
    if (mIsDead) return;

    mHealth -= amount;
    
    if (mAnimator)
    {
        if (!mIsFlashing)
        {
            mOriginalColor = Vector3::One;
        }
        mIsFlashing = true;
        mFlashTimer = HIT_FLASH_DURATION;
        mAnimator->SetColor(Vector3(10.0f, 10.0f, 10.0f));
    }

    if (mHealth <= 0.0f)
    {
        Kill();
    }
}

void Miniboss::Kill()
{
    if (mIsDead) return;
    
    mIsDead = true;
    mHealth = 0.0f;

    if (mAnimator) mAnimator->SetAnimation("dead");
    if (mRigidBody) mRigidBody->SetEnabled(false);
    if (mCollider) mCollider->SetEnabled(false);
    
    SetState(ActorState::Destroy);
}

void Miniboss::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
}

void Miniboss::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
}