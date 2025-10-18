//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Player.h"
#include "Enemy.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Player::Player(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mIsAttacking(false)
        , mAttackTimer(0.0f)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mRigidBodyComponent(nullptr)
{
    AnimatorComponent* anim = new AnimatorComponent(
        this,
        "../Assets/Sprites/Player/Player.png",
        "../Assets/Sprites/Player/Player.json",
        Player::SPRITE_WIDTH,
        Player::SPRITE_HEIGHT
    );

    anim->AddAnimation("idle", {1, 2});
    anim->AddAnimation("run", {8, 9, 10, 11, 12, 13, 14, 15, 16});
    anim->AddAnimation("attack", {3, 4, 5, 6});
    anim->AddAnimation("being-hit", {7, 0});

    anim->SetAnimation("idle");
    anim->SetAnimFPS(8.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, Player::MASS, Player::FRICTION);

    // Align collider base with sprite base
    const int dy = (int)((SPRITE_HEIGHT / 2.0f) - (PHYSICS_HEIGHT / 2.0f));
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

    if (state[SDL_SCANCODE_SPACE])
    {
        mIsAttacking = true;
        mAttackTimer = ATTACK_ANIMATION_DURATION;
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

    if (mIsAttacking)
    {
        mAttackTimer -= deltaTime;
        if (mAttackTimer <= 0.0f)
        {
            mIsAttacking = false;
        }
    }

    ManageAnimations();
}

void Player::ManageAnimations()
{
    AnimatorComponent* anim = GetComponent<AnimatorComponent>();
    if (!anim || mIsDead) return;

    if (mIsAttacking)
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
        if (mIsAttacking)
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
        if (mIsAttacking)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(other->GetOwner());
            if (enemy)
            {
                enemy->Kill();
            }
        }
    }
}
