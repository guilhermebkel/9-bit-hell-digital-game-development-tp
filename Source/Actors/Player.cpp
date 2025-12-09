#include "Player.h"
#include "Enemy.h"
#include "Miniboss.h"
#include "Projectile.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"
#include "../Audio/AudioSystem.h"
#include "../Random.h"

Player::Player(Game* game, const float forwardSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mIsMeleeAttacking(false)
        , mMeleeAttackAnimationTimer(0.0f)
        , mMeleeAttackCooldownTimer(0.0f)
        , mEnemiesHitInCurrentAttack(0)
        , mIsRangedAttacking(false)
        , mRangedAttackAnimationTimer(0.0f)
        , mRangedAttackCooldownTimer(0.0f)
        , mIsBeingHit(false)
        , mBeingHitAnimationTimer(0.0f)
        , mIsInvincible(false)
        , mInvincibilityTimer(0.0f)
        , mHitFreezeTimer(0.0f)
        , mBlinkTimer(0.0f)
        , mForwardSpeed(forwardSpeed)
        , mRigidBodyComponent(nullptr)
        , mHealth(100.0f)
        , mMaxHealth(100.0f)
{
    mDrawComponent = new AnimatorComponent(
        this,
        "../Assets/Sprites/Principal/Principal.png",
        "../Assets/Sprites/Principal/Principal.json",
        Player::SPRITE_WIDTH,
        Player::SPRITE_HEIGHT
    );

    mDrawComponent->SetColor(Vector3(1.0f, 1.0f, 1.0f));
    mDrawComponent->AddAnimation("idle", {8, 9});
    mDrawComponent->AddAnimation("walk", {10, 11, 12});
    mDrawComponent->AddAnimation("attack", {0, 1});
    mDrawComponent->AddAnimation("being-hit", {2, 3});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(8.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, Player::MASS, Player::FRICTION);

    const int dy = (int)((Player::SPRITE_HEIGHT / 2.0f) - (Player::PHYSICS_HEIGHT / 2.0f));
    new AABBColliderComponent(this, 0, dy, Player::PHYSICS_WIDTH, Player::PHYSICS_HEIGHT, ColliderLayer::Player);

    SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, 580.0f));
}

void Player::OnProcessInput(const uint8_t* state)
{
    if (mHitFreezeTimer > 0.0f)
    {
        return;
    }
    
    mIsRunning = false;
    Vector2 force = Vector2::Zero;

    if (state[SDL_SCANCODE_D])
    {
        force.x += mForwardSpeed;
        SetScale(Vector2(1.0f, 1.0f));
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A])
    {
        force.x -= mForwardSpeed;
        SetScale(Vector2(-1.0f, 1.0f));
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_W])
    {
        force.y -= mForwardSpeed;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_S])
    {
        force.y += mForwardSpeed;
        mIsRunning = true;
    }

    bool meleeRequested = state[SDL_SCANCODE_J];
    bool rangedRequested = state[SDL_SCANCODE_K];
    bool canStartMelee = meleeRequested && !mIsMeleeAttacking && !mIsRangedAttacking && mMeleeAttackCooldownTimer <= 0.0f;
    bool canStartRanged = rangedRequested && !mIsRangedAttacking && !mIsMeleeAttacking && mRangedAttackCooldownTimer <= 0.0f;

    if (canStartMelee)
    {
        mIsMeleeAttacking = true;
        mMeleeAttackAnimationTimer = Player::MELEE_ATTACK_ANIMATION_DURATION;
        mMeleeAttackCooldownTimer = Player::MELEE_ATTACK_COOLDOWN;
        mEnemiesHitInCurrentAttack = 0;
        
        int soundIndex = Random::GetIntRange(1, 4);
        std::string soundFile = "../Assets/Sounds/sword-attack-" + std::to_string(soundIndex) + ".wav";
        GetGame()->GetAudioSystem()->PlaySound(soundFile);
    }
    else if (canStartRanged)
    {
        mRangedAttackCooldownTimer = GetGame()->GetPlayerFireRate();

        mIsRangedAttacking = true;
        mRangedAttackAnimationTimer = Player::RANGED_ATTACK_ANIMATION_DURATION;

        float direction = GetScale().x;

        Vector2 startPosition = GetPosition() + Vector2(direction * 20.0f, 20.0f);

        Projectile* projectile = new Projectile(GetGame(), GetGame()->GetPlayerRangedDamage());
        projectile->SetPosition(startPosition);
        projectile->SetScale(Vector2(direction, 1.0f));
        projectile->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(direction * Projectile::SPEED, 0.0f));
        
        GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/sword-throw.wav");
    }

    mRigidBodyComponent->ApplyForce(force);
}

void Player::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    Vector2 limitedPosition = GetPosition();
    const float halfWidth = Player::SPRITE_WIDTH / 2.0f;
    const float halfHeight = Player::SPRITE_HEIGHT / 2.0f;

    limitedPosition.x = Math::Clamp(limitedPosition.x, halfWidth, Game::WINDOW_WIDTH - halfWidth);
    limitedPosition.y = Math::Clamp(limitedPosition.y, GetGame()->GetUpperBoundary() + halfHeight, Game::WINDOW_HEIGHT - halfHeight);

    SetPosition(limitedPosition);

    if (mDrawComponent)
    {
        mDrawComponent->SetDrawOrder(100 + static_cast<int>(GetPosition().y));
    }

    if (mIsMeleeAttacking)
    {
        mMeleeAttackAnimationTimer -= deltaTime;
        if (mMeleeAttackAnimationTimer <= 0.0f)
        {
            mIsMeleeAttacking = false;
        }
    }

    if (mMeleeAttackCooldownTimer > 0.0f)
    {
        mMeleeAttackCooldownTimer -= deltaTime;
    }

    if (mIsRangedAttacking)
    {
        mRangedAttackAnimationTimer -= deltaTime;
        if (mRangedAttackAnimationTimer <= 0.0f)
        {
            mIsRangedAttacking = false;
        }
    }

    if (mRangedAttackCooldownTimer > 0.0f)
    {
        mRangedAttackCooldownTimer -= deltaTime;
    }

    if (mIsBeingHit)
    {
        mBeingHitAnimationTimer -= deltaTime;
        if (mBeingHitAnimationTimer <= 0.0f && mHitFreezeTimer <= 0.0f)
        {
            mIsBeingHit = false;
        }
    }

    if (mIsInvincible)
    {
        mInvincibilityTimer -= deltaTime;
        mBlinkTimer += deltaTime;
        
        bool shouldShow = (static_cast<int>(mBlinkTimer * 10.0f) % 2) == 0;
        if (mDrawComponent)
        {
            mDrawComponent->SetVisible(shouldShow);
        }
        
        if (mInvincibilityTimer <= 0.0f)
        {
            mIsInvincible = false;
            if (mDrawComponent)
            {
                mDrawComponent->SetVisible(true);
            }
        }
    }

    ManageAnimations();
    
    // Gerencia congelamento por dano (após ManageAnimations)
    if (mHitFreezeTimer > 0.0f)
    {
        if (mDrawComponent)
        {
            mDrawComponent->SetAnimation("being-hit");
            mDrawComponent->SetIsPaused(true);
        }
        
        mHitFreezeTimer -= deltaTime;
        
        if (mHitFreezeTimer <= 0.0f)
        {
            mRigidBodyComponent->SetEnabled(true);
            
            if (mDrawComponent)
            {
                mDrawComponent->SetIsPaused(false);
            }
            
            // Inicia invencibilidade após o congelamento
            mIsInvincible = true;
            mInvincibilityTimer = Player::INVINCIBILITY_DURATION;
            mBlinkTimer = 0.0f;
        }
    }
}

void Player::ManageAnimations()
{
    AnimatorComponent* anim = GetComponent<AnimatorComponent>();
    if (!anim || mIsDead) return;

    if (mIsBeingHit)
    {
        anim->SetAnimation("being-hit");
    }
    else if (mIsMeleeAttacking || mIsRangedAttacking)
    {
        anim->SetAnimation("attack");
    }
    else if (mIsRunning)
    {
        anim->SetAnimation("walk");
    }
    else
    {
        anim->SetAnimation("idle");
    }
}

void Player::Kill()
{
    if (mIsDead) return;

    mIsDead = true;

    GetComponent<AnimatorComponent>()->SetAnimation("dead");

    mRigidBodyComponent->SetEnabled(false);
    GetComponent<AABBColliderComponent>()->SetEnabled(false);

    if (GetGame() && GetGame()->GetAudioSystem())
    {
        GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/player-death.mp3");
    }
}

void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        if (mIsMeleeAttacking)
        {
            int maxEnemiesHit = 1 + GetGame()->GetPlayerPiercing();
            
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy)
            {
                // Verifica se este inimigo já foi atingido neste ataque
                if (mEnemiesHitInCurrentAttack < maxEnemiesHit)
                {
                    enemy->TakeDamage(GetGame()->GetPlayerMeleeDamage());
                    mEnemiesHitInCurrentAttack++;
                }
            }

            Miniboss* miniboss = dynamic_cast<Miniboss*>(other->GetOwner());
            if (miniboss && mEnemiesHitInCurrentAttack < maxEnemiesHit)
            {
                miniboss->TakeDamage(GetGame()->GetPlayerMeleeDamage());
                mEnemiesHitInCurrentAttack++;
            }
        }
    }
}

void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        if (mIsMeleeAttacking)
        {
            int maxEnemiesHit = 1 + GetGame()->GetPlayerPiercing();
            
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy && mEnemiesHitInCurrentAttack < maxEnemiesHit)
            {
                enemy->TakeDamage(GetGame()->GetPlayerMeleeDamage());
                mEnemiesHitInCurrentAttack++;
            }

            Miniboss* miniboss = dynamic_cast<Miniboss*>(other->GetOwner());
            if (miniboss && mEnemiesHitInCurrentAttack < maxEnemiesHit)
            {
                miniboss->TakeDamage(GetGame()->GetPlayerMeleeDamage());
                mEnemiesHitInCurrentAttack++;
            }
        }
    }
}

void Player::TakeDamage(float amount)
{
    if (mIsDead || mIsInvincible || mHitFreezeTimer > 0.0f) return;

    mHealth -= amount;
    
    // Ativa animação de dano
    mIsBeingHit = true;
    mBeingHitAnimationTimer = Player::BEING_HIT_ANIMATION_DURATION;
    
    // Ativa congelamento por 1s (sprite e movimento congelados)
    mHitFreezeTimer = Player::HIT_FREEZE_DURATION;
    mRigidBodyComponent->SetVelocity(Vector2::Zero);
    mRigidBodyComponent->SetEnabled(false);
        
    // Tocar aleatoriamente um dos 3 sons de dano do player
    if (mHealth <= 0.0f)
    {
        mHealth = 0.0f;
        Kill();
    }
    else
    {
        // Tocar aleatoriamente um dos 3 sons de dano do player (somente se não morreu)
        int soundIndex = Random::GetIntRange(1, 3);
        std::string soundFile = "../Assets/Sounds/player-hurt-" + std::to_string(soundIndex) + ".wav";
        GetGame()->GetAudioSystem()->PlaySound(soundFile);
    }
}

void Player::Heal(float amount)
{
    if (mIsDead || mIsInvincible || mHitFreezeTimer > 0.0f) return;

    mHealth += amount;
}
