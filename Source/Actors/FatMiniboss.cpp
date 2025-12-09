#include "FatMiniboss.h"
#include "Soul.h"
#include "Player.h"
#include "SlimeProjectile.h"
#include "SlimePuddle.h"
#include "../Game.h"
#include "../Random.h"
#include "../Math.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

FatMiniboss::FatMiniboss(Game* game)
    : Miniboss(game, MAX_HP)
    , mState(BossState::Moving)
    , mStateTimer(0.0f)
    , mPuddleTimer(0.0f)
{
    mAnimator = new AnimatorComponent(
        this,
        Game::ResolvePath("Assets/Sprites/FatMiniboss/FatMiniboss.png"),
        Game::ResolvePath("Assets/Sprites/FatMiniboss/FatMiniboss.json"),
        SPRITE_WIDTH,
        SPRITE_HEIGHT
    );

    mAnimator->AddAnimation("idle", {3, 4});
    mAnimator->AddAnimation("walk", {7, 6});
    mAnimator->AddAnimation("attack", {0, 1});
    mAnimator->AddAnimation("special", {5});
    mAnimator->AddAnimation("being-hit", {2});
    mAnimator->AddAnimation("dead", {3});

    mAnimator->SetAnimation("idle");
    mAnimator->SetAnimFPS(4.0f);

    mRigidBody = new RigidBodyComponent(this, 10.0f, 0.0f);

    mCollider = new AABBColliderComponent(this, 0, 0, PHYSICS_WIDTH, PHYSICS_HEIGHT, ColliderLayer::Enemy);
    
    CreateHealthBar(Vector2(100.0f, 15.0f), Vector4(101.0f / 255.0f, 125.0f / 255.0f, 49.0f / 255.0f, 1.0f));
}

void FatMiniboss::OnUpdate(float deltaTime)
{
    Miniboss::OnUpdate(deltaTime);

    if (mIsDead) return;

    if (mState == BossState::BeingHit)
    {
        mBeingHitTimer -= deltaTime;
        mStateTimer -= deltaTime;
        
        // Após 0.5s de being-hit, transiciona para ForcedAttack
        if (mBeingHitTimer <= 0.0f && mState == BossState::BeingHit)
        {
            mState = BossState::ForcedAttack;
            mStateTimer = 0.8f;
            mAnimator->SetAnimation("attack");
            mForcedAttackCount = 0;
            mForcedAttackIntervalTimer = 0.0f;
        }
        // Não chamar UpdateAI enquanto estiver em BeingHit
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
            mForcedAttackExecuted = false;
            if (mAnimator)
            {
                mAnimator->SetAnimation("idle");
            }
        }
        // Não chamar UpdateAI durante ForcedAttack
        return;
    }

    UpdateAI(deltaTime);

    if (mAnimator)
    {
        mAnimator->SetDrawOrder(100 + static_cast<int>(GetPosition().y));
    }
}

void FatMiniboss::UpdateAI(float deltaTime)
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
        mRigidBody->SetVelocity(direction * (WALK_SPEED * mDifficultyMultiplier));

        SetScale(Vector2(direction.x < 0 ? -1.0f : 1.0f, 1.0f));
        mAnimator->SetAnimation("walk");

        mPuddleTimer -= deltaTime;
        if (mPuddleTimer <= 0.0f)
        {
            SpawnPuddle();
            mPuddleTimer = PUDDLE_SPAWN_INTERVAL;
        }

        mStateTimer += deltaTime;
        if (mStateTimer >= 3.0f && distance < 900.0f)
        {
            mState = BossState::WindUp;
            mStateTimer = ATTACK_WINDUP;
            mRigidBody->SetVelocity(Vector2::Zero);

            mAnimator->SetAnimation("special");
        }
        break;
    }

    case BossState::WindUp:
        mStateTimer -= deltaTime;
        if (mStateTimer <= 0.0f)
        {
            mState = BossState::Attacking;
            mAnimator->SetAnimation("attack");
            ShootSlime();
            mStateTimer = 0.5f;
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

void FatMiniboss::PerformForcedAttack()
{
    const Player* player = GetGame()->GetPlayer();
    if (!player) return;

    Vector2 dir = player->GetPosition() - GetPosition();
    dir.Normalize();
    
    // Adicionar pequena variação de ângulo (±10 graus)
    float angle = Math::Atan2(dir.y, dir.x);
    float angleVariation = Math::ToRadians(Random::GetFloatRange(-10.0f, 10.0f));
    angle += angleVariation;
    
    Vector2 variedDir(Math::Cos(angle), Math::Sin(angle));
    
    SlimeProjectile* proj = new SlimeProjectile(GetGame(), variedDir);
    proj->SetPosition(GetPosition() + Vector2(0.0f, -10.0f));

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/fat-attack.wav"));
}

void FatMiniboss::TakeDamage(float amount)
{
    if (IsInvulnerable() || mIsDead) return;

    Miniboss::TakeDamage(amount);

    if (!mIsDead && mAnimator)
    {
        mState = BossState::BeingHit;
        mAnimator->SetAnimation("being-hit");
        mStateTimer = 0.5f;
        mBeingHitTimer = 0.5f; // Timer para transição para ForcedAttack
        mRigidBody->SetVelocity(Vector2::Zero);
    }

    if (!mIsDead)
    {
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/fat-hurt.wav"));
    }
}

void FatMiniboss::SpawnPuddle()
{
    SlimePuddle* puddle = new SlimePuddle(GetGame());
    puddle->SetPosition(GetPosition() + Vector2(0.0f, SPRITE_HEIGHT/2.0f - 15.0f));
}

void FatMiniboss::ShootSlime()
{
    const Player* player = GetGame()->GetPlayer();
    if (!player) return;

    Vector2 dir = player->GetPosition() - GetPosition();
    dir.Normalize();

    float baseAngle = Math::Atan2(dir.y, dir.x);
    float spread = Math::ToRadians(10.0f);
    float angle1 = baseAngle - (spread / 2.0f);
    float angle2 = baseAngle + (spread / 2.0f);

    Vector2 dir1(Math::Cos(angle1), Math::Sin(angle1));
    Vector2 dir2(Math::Cos(angle2), Math::Sin(angle2));

    SlimeProjectile* p1 = new SlimeProjectile(GetGame(), dir1);
    p1->SetPosition(GetPosition() + Vector2(0.0f, -10.0f));

    SlimeProjectile* p2 = new SlimeProjectile(GetGame(), dir2);
    p2->SetPosition(GetPosition() + Vector2(0.0f, -10.0f));

    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/fat-attack.wav"));
}

void FatMiniboss::SpawnSoulsOnDeath()
{
    const int count = 3;
    for (int i = 0; i < count; ++i)
    {
        float angle = Random::GetFloatRange(0.0f, Math::TwoPi);
        float radius = Random::GetFloatRange(8.0f, 48.0f);
        Vector2 offset(Math::Cos(angle) * radius, Math::Sin(angle) * radius);

        Soul* s = new Soul(GetGame(), Soul::SoulType::Purple);
        Vector2 spawnPos = GetPosition() + offset;
        const float margin = 16.0f;
        spawnPos.x = Math::Clamp(spawnPos.x, margin, static_cast<float>(Game::WINDOW_WIDTH) - margin);
        float minY = GetGame()->GetUpperBoundary() + margin;
        spawnPos.y = Math::Clamp(spawnPos.y, minY, static_cast<float>(Game::WINDOW_HEIGHT) - margin);
        s->SetPosition(spawnPos);
    }
}

void FatMiniboss::Kill()
{
    Miniboss::Kill();

    if (GetGame() && GetGame()->GetAudioSystem())
    {
        GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/fat-defeated.wav"));
    }
}