#include "Player.h"
#include "Enemy.h"
#include "Projectile.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Player::Player(Game* game, const float forwardSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mIsMeleeAttacking(false)
        , mMeleeAttackAnimationTimer(0.0f)
        , mIsRangedAttacking(false)
        , mRangedAttackAnimationTimer(0.0f)
        , mRangedAttackCooldownTimer(0.0f)
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

    mDrawComponent->AddAnimation("idle", {1, 2});
    mDrawComponent->AddAnimation("run", {8, 9, 10, 11, 12, 13, 14, 15, 16});
    mDrawComponent->AddAnimation("attack", {3, 4, 5, 6});
    mDrawComponent->AddAnimation("being-hit", {7, 0});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(8.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, Player::MASS, Player::FRICTION);

    // Align collider base with sprite base
    const int dy = (int)((Player::SPRITE_HEIGHT / 2.0f) - (Player::PHYSICS_HEIGHT / 2.0f));
    new AABBColliderComponent(this, 0, dy, Player::PHYSICS_WIDTH, Player::PHYSICS_HEIGHT, ColliderLayer::Player);
}

void Player::OnProcessInput(const uint8_t* state)
{
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

    if (state[SDL_SCANCODE_J])
    {
        mIsMeleeAttacking = true;
        mMeleeAttackAnimationTimer = Player::MELEE_ATTACK_ANIMATION_DURATION;
    }

    if (state[SDL_SCANCODE_K] && mRangedAttackCooldownTimer <= 0.0f)
    {
        mRangedAttackCooldownTimer = GetGame()->GetPlayerFireRate();

        mIsRangedAttacking = true;
        mRangedAttackAnimationTimer = Player::RANGED_ATTACK_ANIMATION_DURATION;

        float direction = GetScale().x;
        Vector2 startPosition = GetPosition() + Vector2(direction * 20.0f, 0.0f);

        Projectile* projectile = new Projectile(GetGame());
        projectile->SetPosition(startPosition);
        projectile->GetComponent<RigidBodyComponent>()->SetVelocity(Vector2(direction * Projectile::SPEED, 0.0f));
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

    ManageAnimations();
}

void Player::ManageAnimations()
{
    AnimatorComponent* anim = GetComponent<AnimatorComponent>();
    if (!anim || mIsDead) return;

    if (mIsMeleeAttacking || mIsRangedAttacking)
    {
        anim->SetAnimation("attack");
    }
    else if (mIsRunning)
    {
        anim->SetAnimation("run");
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
        if (mIsMeleeAttacking)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy)
            {
                enemy->Kill();
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
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy)
            {
                enemy->Kill();
            }
        }
    }
}

void Player::TakeDamage(float amount)
{
    if (mIsDead) return;

    mHealth -= amount;
    if (mHealth <= 0.0f)
    {
        mHealth = 0.0f;
        Kill();
    }
}
