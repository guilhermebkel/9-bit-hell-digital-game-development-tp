//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Player.h"
#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Player::Player(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mRigidBodyComponent(nullptr)
{
    AnimatorComponent* anim = new AnimatorComponent(
        this,
        "../Assets/Sprites/Player/Player.png",
        "../Assets/Sprites/Player/Player.json",
        64,
        64
    );

    anim->AddAnimation("idle", {2, 1});
    anim->AddAnimation("run", {4, 12, 15, 11, 6, 13, 16, 14, 7});
    anim->AddAnimation("damage", {8, 9, 10, 5});
    anim->AddAnimation("being-hit", {3, 0});

    anim->SetAnimation("idle");
    anim->SetAnimFPS(8.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, Player::MASS, Player::FRICTION);

    new AABBColliderComponent(this, 0, 0, 32, 32, ColliderLayer::Player);
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

    mRigidBodyComponent->ApplyForce(force);
}

void Player::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    ManageAnimations();
}

void Player::ManageAnimations()
{
    AnimatorComponent* anim = GetComponent<AnimatorComponent>();
    if (!anim || mIsDead) return;

    if (mIsRunning)
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
        // Kill();
    }
}

void Player::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        // Goomba* goomba = dynamic_cast<Goomba*>(other->GetOwner());
        // if (goomba)
        // {
        //     goomba->Kill();
        //
        //     Vector2 vel = mRigidBodyComponent->GetVelocity();
        //     vel.y = -mJumpSpeed * 0.5f;
        //     mRigidBodyComponent->SetVelocity(vel);
        // }
    }
}
