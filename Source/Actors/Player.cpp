#include "Player.h"
#include "Enemy.h"
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
        , mHasKilledEnemyInCurrentAttack(false)
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

    // Align collider base with sprite base
    const int dy = (int)((Player::SPRITE_HEIGHT / 2.0f) - (Player::PHYSICS_HEIGHT / 2.0f));
    new AABBColliderComponent(this, 0, dy, Player::PHYSICS_WIDTH, Player::PHYSICS_HEIGHT, ColliderLayer::Player);
}

void Player::OnProcessInput(const uint8_t* state)
{
    // Bloqueia input durante o congelamento por dano
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

    if (state[SDL_SCANCODE_J] && !mIsMeleeAttacking)
    {
        mIsMeleeAttacking = true;
        mMeleeAttackAnimationTimer = Player::MELEE_ATTACK_ANIMATION_DURATION;
        mHasKilledEnemyInCurrentAttack = false;
        
        // Tocar aleatoriamente um dos 4 sons de ataque
        int soundIndex = Random::GetIntRange(1, 4);
        std::string soundFile = "../Assets/Sounds/sword-attack-" + std::to_string(soundIndex) + ".wav";
        GetGame()->GetAudioSystem()->PlaySound(soundFile);
    }

    if (state[SDL_SCANCODE_K] && mRangedAttackCooldownTimer <= 0.0f)
    {
        mRangedAttackCooldownTimer = GetGame()->GetPlayerFireRate();

        mIsRangedAttacking = true;
        mRangedAttackAnimationTimer = Player::RANGED_ATTACK_ANIMATION_DURATION;

        float direction = GetScale().x;

        Vector2 startPosition = GetPosition() + Vector2(direction * 20.0f, 20.0f);

        Projectile* projectile = new Projectile(GetGame());
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
        
        // Pisca a cada X segundos
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
}

void Player::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        if (mIsMeleeAttacking && !mHasKilledEnemyInCurrentAttack)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy)
            {
                enemy->Kill();
                mHasKilledEnemyInCurrentAttack = true; // Marca que já matou 1 inimigo
            }
        }
    }
}

void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        if (mIsMeleeAttacking && !mHasKilledEnemyInCurrentAttack)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy)
            {
                enemy->Kill();
                mHasKilledEnemyInCurrentAttack = true; // Marca que já matou 1 inimigo
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
    int soundIndex = Random::GetIntRange(1, 3);
    std::string soundFile = "../Assets/Sounds/player-hurt-" + std::to_string(soundIndex) + ".wav";
    GetGame()->GetAudioSystem()->PlaySound(soundFile);
    
    if (mHealth <= 0.0f)
    {
        mHealth = 0.0f;
        Kill();
    }
}

void Player::Heal(float amount)
{
    if (mIsDead || mIsInvincible || mHitFreezeTimer > 0.0f) return;

    mHealth += amount;
}
