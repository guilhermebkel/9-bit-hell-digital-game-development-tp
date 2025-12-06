#include "EyeMiniboss.h"
#include "Player.h"
#include "EyeProjectile.h"
#include "../Game.h"
#include "../Random.h"
#include "../Math.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

EyeMiniboss::EyeMiniboss(Game* game)
    : Miniboss(game, MAX_HP)
    , mState(BossState::Moving)
    , mStateTimer(0.0f)
    , mBurstShotsLeft(0)
{
    mAnimator = new AnimatorComponent(
        this, 
        "../Assets/Sprites/EyeMiniboss/EyeMiniboss.png", 
        "../Assets/Sprites/EyeMiniboss/EyeMiniboss.json", 
        SPRITE_WIDTH,
        SPRITE_HEIGHT
    );
    
    mAnimator->AddAnimation("attack", {2, 5});
    mAnimator->AddAnimation("being-hit", {0});
    mAnimator->AddAnimation("idle", {1, 3});
    mAnimator->AddAnimation("walk", {7, 4, 6});
    mAnimator->AddAnimation("dead", {1});
    
    mAnimator->SetAnimation("idle");
    mAnimator->SetAnimFPS(6.0f);

    mRigidBody = new RigidBodyComponent(this, 2.0f, 0.0f);
    
    mCollider = new AABBColliderComponent(this, 0, 0, PHYSICS_WIDTH, PHYSICS_HEIGHT, ColliderLayer::Enemy);
}

void EyeMiniboss::OnUpdate(float deltaTime)
{
    Miniboss::OnUpdate(deltaTime);

    if (mIsDead) return;

    if (mAnimator->GetAnimationName() == "being-hit")
    {
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Moving;
            mStateTimer = 0.0f;
        }
        return;
    }

    UpdateAI(deltaTime);

    if (mAnimator)
    {
        mAnimator->SetDrawOrder(100 + static_cast<int>(GetPosition().y));
    }
}

void EyeMiniboss::UpdateAI(float deltaTime)
{
    const Player* player = GetGame()->GetPlayer();
    if (!player) return;

    Vector2 playerPos = player->GetPosition();
    Vector2 myPos = GetPosition();
    float distance = Vector2::Distance(myPos, playerPos);

    switch (mState)
    {
    case BossState::Moving:
    {
        Vector2 direction = playerPos - myPos;
        direction.Normalize();
        
        if (distance < 200.0f)
        {
            mRigidBody->SetVelocity(direction * -MOVE_SPEED);
        }
        else
        {
            mRigidBody->SetVelocity(direction * MOVE_SPEED);
        }
        
        SetScale(Vector2(direction.x < 0 ? -1.0f : 1.0f, 1.0f));
        mAnimator->SetAnimation("walk");

        mStateTimer += deltaTime;
        if (mStateTimer >= 1.5f && distance < 600.0f)
        {
            mState = BossState::WindUp;
            mStateTimer = WINDUP_TIME;
            mRigidBody->SetVelocity(Vector2::Zero);
            mAnimator->SetAnimation("idle");
            
            mBurstShotsLeft = Random::GetIntRange(3, 6);
        }
        break;
    }

    case BossState::WindUp:
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Attacking;
            mStateTimer = 0.0f;
            mAnimator->SetAnimation("attack");
        }
        break;

    case BossState::Attacking:
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            PerformSpreadShot();
            mBurstShotsLeft--;
            mStateTimer = BURST_INTERVAL;

            if (mBurstShotsLeft <= 0)
            {
                mState = BossState::Cooldown;
                mStateTimer = ATTACK_COOLDOWN;
                mAnimator->SetAnimation("idle");
            }
        }
        break;

    case BossState::Cooldown:
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Moving;
            mStateTimer = 0.0f;
        }
        break;
        
    case BossState::Dead:
        mAnimator->SetAnimation("dead");
        break;
    }
}

void EyeMiniboss::TakeDamage(float amount)
{
    Miniboss::TakeDamage(amount);

    if (!mIsDead && mAnimator)
    {
        if (mState != BossState::Attacking && mState != BossState::WindUp)
        {
            mAnimator->SetAnimation("being-hit");
            mStateTimer = 0.2f;
        }
    }

    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/eye-hurt.wav");
}

void EyeMiniboss::PerformSpreadShot()
{
    const Player* player = GetGame()->GetPlayer();
    if (!player) return;

    Vector2 toPlayer = player->GetPosition() - GetPosition();
    float baseAngle = Math::Atan2(toPlayer.y, toPlayer.x);

    int numProjectiles = Random::GetIntRange(3, 5);
    float angleStep = 0.0f;

    if (numProjectiles == 3) angleStep = Math::ToRadians(60.0f);
    else if (numProjectiles == 4) angleStep = Math::ToRadians(45.0f);
    else angleStep = Math::ToRadians(36.0f);

    float totalSpread = angleStep * (numProjectiles - 1);
    float startAngle = baseAngle - (totalSpread / 2.0f);

    for (int i = 0; i < numProjectiles; ++i)
    {
        float currentAngle = startAngle + (angleStep * i);
        Vector2 direction(Math::Cos(currentAngle), Math::Sin(currentAngle));

        EyeProjectile* proj = new EyeProjectile(GetGame(), direction);
        proj->SetPosition(GetPosition());
    }

    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/eye-attack.wav");
}