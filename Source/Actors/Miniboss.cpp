#include "Miniboss.h"
#include "Player.h"
#include "UIHealthBarWidget.h"
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
    , mJustBecameVulnerable(false)
    , mBeingHitTimer(0.0f)
    , mForcedAttackExecuted(false)
    , mForcedAttackCount(0)
    , mForcedAttackIntervalTimer(0.0f)
    , mHealthBar(nullptr)
    , mHealthBarOffset(Vector2(0.0f, -75.0f))
    , mAnimator(nullptr)
    , mRigidBody(nullptr)
    , mCollider(nullptr)
    , mIsFlashing(false)
    , mFlashTimer(0.0f)
    , mOriginalColor(Vector3::One)
    , mInvulnerabilityTimer(0.0f)
{
}

void Miniboss::OnUpdate(float deltaTime)
{
    bool wasInvulnerable = mInvulnerabilityTimer > 0.0f;
    
    mInvulnerabilityTimer -= deltaTime;
    if (mInvulnerabilityTimer < 0.0f)
    {
        mInvulnerabilityTimer = 0.0f;
    }

    // Detecta quando a invencibilidade termina
    if (wasInvulnerable && !IsInvulnerable())
    {
        mJustBecameVulnerable = true;
    }

    if (mHealthBar)
    {
        Vector2 barPosition = GetPosition() + mHealthBarOffset;
        mHealthBar->SetPosition(barPosition);
        mHealthBar->Update(mHealth, mMaxHealth);
    }

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

    // Clamp posição dentro dos limites da tela (assim como o Player faz)
    Vector2 clampedPosition = GetPosition();
    clampedPosition.x = Math::Clamp(clampedPosition.x, halfWidth, Game::WINDOW_WIDTH - halfWidth);
    clampedPosition.y = Math::Clamp(clampedPosition.y, GetGame()->GetUpperBoundary() + halfHeight, Game::WINDOW_HEIGHT - halfHeight);
    SetPosition(clampedPosition);

    if (mAnimator)
    {
        mAnimator->SetDrawOrder(100 + static_cast<int>(GetPosition().y));
    }
}

void Miniboss::TakeDamage(float amount)
{
    if (mIsDead || IsInvulnerable()) return;

    mHealth -= amount;
    mInvulnerabilityTimer = INVULNERABILITY_DURATION;
    
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

void Miniboss::CreateHealthBar(const Vector2& barSize)
{
    if (!mHealthBar)
    {
        Vector2 barPosition = GetPosition() + Vector2(0.0f, -100.0f);
        mHealthBar = new UIHealthBarWidget(GetGame(), barPosition, barSize, 200);
    }
}