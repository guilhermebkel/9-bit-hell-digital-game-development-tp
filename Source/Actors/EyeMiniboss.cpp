#include "EyeMiniboss.h"
#include "Soul.h"
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
        Game::ResolvePath("Assets/Sprites/EyeMiniboss/EyeMiniboss.png"), 
        Game::ResolvePath("Assets/Sprites/EyeMiniboss/EyeMiniboss.json"), 
        SPRITE_WIDTH,
        SPRITE_HEIGHT
    );
    
    mAnimator->AddAnimation("attack", {0, 1});
    mAnimator->AddAnimation("being-hit", {2});
    mAnimator->AddAnimation("idle", {3, 4});
    mAnimator->AddAnimation("walk", {7, 5, 6});
    mAnimator->AddAnimation("dead", {3});
    
    mAnimator->SetAnimation("idle");
    mAnimator->SetAnimFPS(6.0f);

    mRigidBody = new RigidBodyComponent(this, 2.0f, 0.0f);
    
    mCollider = new AABBColliderComponent(this, 0, 0, PHYSICS_WIDTH, PHYSICS_HEIGHT, ColliderLayer::Enemy);
    
    CreateHealthBar(Vector2(100.0f, 15.0f), Vector4(140.0f / 255.0f, 67.0f / 255.0f, 66.0f / 255.0f, 1.0f));
}

void EyeMiniboss::OnUpdate(float deltaTime)
{
    Miniboss::OnUpdate(deltaTime);

    if (mIsDead) return;

    if (mState == BossState::BeingHit)
    {
        mBeingHitTimer -= deltaTime;
        if (mBeingHitTimer <= 0.0f)
        {
            mState = BossState::ForcedAttack;
            mStateTimer = 0.8f;
            mAnimator->SetAnimation("attack");
            mForcedAttackCount = 0;
            mForcedAttackIntervalTimer = 0.0f;
        }
        return;
    }

    if (mState == BossState::ForcedAttack)
    {
        mStateTimer -= deltaTime;
        mForcedAttackIntervalTimer -= deltaTime;
        
        if (mAnimator)
        {
            mAnimator->ForceFinalFrame();
        }
        
        if (mForcedAttackIntervalTimer <= 0.0f && mForcedAttackCount < 6)
        {
            PerformForcedAttack();
            mForcedAttackCount++;
            mForcedAttackIntervalTimer = 0.15f;
        }
        
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Moving;
            mStateTimer = 0.0f;
            mForcedAttackCount = 0;
            mForcedAttackIntervalTimer = 0.0f;
            if (mAnimator)
            {
                mAnimator->SetAnimation("idle");
            }
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
    
    mJustBecameVulnerable = false;

    switch (mState)
    {
    case BossState::Moving:
    {
        Vector2 direction = playerPos - myPos;
        direction.Normalize();
        
        if (distance < 200.0f)
        {
            mRigidBody->SetVelocity(direction * (-MOVE_SPEED * mDifficultyMultiplier));
        }
        else
        {
            mRigidBody->SetVelocity(direction * (MOVE_SPEED * mDifficultyMultiplier));
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
    if (IsInvulnerable() || mIsDead) return;

    Miniboss::TakeDamage(amount);

    if (!mIsDead && mAnimator)
    {
        mState = BossState::BeingHit;
        mAnimator->SetAnimation("being-hit");
        mBeingHitTimer = 0.5f;
        mRigidBody->SetVelocity(Vector2::Zero);
    }

    if (!mIsDead)
    {
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/eye-hurt.wav"));
    }
}

void EyeMiniboss::PerformForcedAttack()
{
    PerformSpreadShot();
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

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/eye-attack.wav"));
}

void EyeMiniboss::SpawnSoulsOnDeath()
{
    float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
    float radius = Random::GetFloatRange(8.0f, 32.0f);
    Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);

    Soul* s = new Soul(GetGame(), Soul::SoulType::Purple);
    Vector2 spawnPos = GetPosition() + offset;
    const float margin = 16.0f;
    spawnPos.x = Math::Clamp(spawnPos.x, margin, static_cast<float>(Game::WINDOW_WIDTH) - margin);
    float minY = GetGame()->GetUpperBoundary() + margin;
    spawnPos.y = Math::Clamp(spawnPos.y, minY, static_cast<float>(Game::WINDOW_HEIGHT) - margin);
    s->SetPosition(spawnPos);
}

void EyeMiniboss::Kill()
{
    Miniboss::Kill();

    if (GetGame() && GetGame()->GetAudioSystem())
    {
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/eye-defeated.ogg"));
    }
}