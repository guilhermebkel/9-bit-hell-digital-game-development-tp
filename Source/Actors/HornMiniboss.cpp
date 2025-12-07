#include "HornMiniboss.h"
#include "Player.h"
#include "FallingHand.h"
#include "GroundSpike.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

HornMiniboss::HornMiniboss(Game* game)
    : Miniboss(game, MAX_HP)
    , mState(BossState::Moving)
    , mNextAttack(AttackType::HandDrop)
    , mStateTimer(0.0f)
{
    mAnimator = new AnimatorComponent(
        this, 
        "../Assets/Sprites/HornMiniboss/HornMiniboss.png", 
        "../Assets/Sprites/HornMiniboss/HornMiniboss.json", 
        SPRITE_WIDTH,
        SPRITE_HEIGHT
    );
    
    mAnimator->AddAnimation("attack", {0, 1});
    mAnimator->AddAnimation("special", {3, 4});
    mAnimator->AddAnimation("being-hit", {2});
    mAnimator->AddAnimation("idle", {5, 6});
    mAnimator->AddAnimation("walk", {8, 7});
    mAnimator->AddAnimation("dead", {5});
    
    mAnimator->SetAnimation("idle");
    mAnimator->SetAnimFPS(5.0f);

    mRigidBody = new RigidBodyComponent(this, 8.0f, 0.0f);
    
    mCollider = new AABBColliderComponent(this, 0, 0, PHYSICS_WIDTH, PHYSICS_HEIGHT, ColliderLayer::Enemy);
    
    CreateHealthBar(Vector2(100.0f, 15.0f));
}

void HornMiniboss::OnUpdate(float deltaTime)
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
            mAnimator->SetAnimation("special");
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
        
        if (mForcedAttackIntervalTimer <= 0.0f && mForcedAttackCount < 3)
        {
            PerformForcedAttack();
            mForcedAttackCount++;
            mForcedAttackIntervalTimer = 0.2f;
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

void HornMiniboss::UpdateAI(float deltaTime)
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
        mRigidBody->SetVelocity(direction * WALK_SPEED);
        
        SetScale(Vector2(direction.x < 0 ? -1.0f : 1.0f, 1.0f));
        mAnimator->SetAnimation("walk");

        mStateTimer += deltaTime;
        if (mStateTimer >= 2.0f && distance < 1200.0f)
        {
            mState = BossState::WindUp;
            mStateTimer = ATTACK_WINDUP;
            mRigidBody->SetVelocity(Vector2::Zero);
            
            if (Random::GetFloat() > 0.5f)
            {
                mNextAttack = AttackType::GroundSpikes;
                mAnimator->SetAnimation("special");
            }
            else
            {
                mNextAttack = AttackType::HandDrop;
                mAnimator->SetAnimation("attack");
                GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/horn-special-launch.wav");
            }
        }
        break;
    }

    case BossState::WindUp:
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Attacking;
            
            if (mNextAttack == AttackType::GroundSpikes)
            {
                PerformSpikeAttack();
            }
            else
            {
                PerformHandAttack();
            }
            
            mStateTimer = 0.8f;
        }
        break;

    case BossState::Attacking:
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Cooldown;
            mStateTimer = ATTACK_COOLDOWN;
            mAnimator->SetAnimation("idle");
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

void HornMiniboss::TakeDamage(float amount)
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

    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/horn-hurt.wav");
}

void HornMiniboss::PerformForcedAttack()
{
    PerformSpikeAttack();
}

void HornMiniboss::PerformSpikeAttack()
{
    const Player* player = GetGame()->GetPlayer();
    if (!player) return;

    Vector2 targetPos = player->GetPosition();
    float spacing = 50.0f;

    for (int i = -1; i <= 1; ++i)
    {
        GroundSpike* spike = new GroundSpike(GetGame());
        spike->SetPosition(Vector2(targetPos.x + (i * spacing), targetPos.y));
    }
    
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/horn-attack.wav");
}

void HornMiniboss::PerformHandAttack()
{
    const Player* player = GetGame()->GetPlayer();
    if (!player) return;

    Vector2 targetPos = player->GetPosition();
    
    // Left hand at left edge of player, 200px above
    // Reduced offset by 1/4 for closer targeting
    FallingHand* h1 = new FallingHand(GetGame());
    float leftOffset = -(Player::SPRITE_WIDTH / 2.0f) * 0.75f;
    h1->SetPosition(Vector2(targetPos.x + leftOffset, targetPos.y - 200.0f));

    // Right hand at right edge of player, 200px above
    FallingHand* h2 = new FallingHand(GetGame());
    float rightOffset = (Player::SPRITE_WIDTH / 2.0f) * 0.75f;
    h2->SetPosition(Vector2(targetPos.x + rightOffset, targetPos.y - 200.0f));
    
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/horn-special-land.wav");
}