//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mRigidBodyComponent(nullptr)
{
    AnimatorComponent* anim = new AnimatorComponent(
        this,
        "../Assets/Sprites/Mario/Mario.png",
        "../Assets/Sprites/Mario/Mario.json",
        Game::TILE_SIZE,
        Game::TILE_SIZE
    );

    anim->AddAnimation("idle", {1});
    anim->AddAnimation("run", {3, 4, 5});
    anim->AddAnimation("jump", {2});
    anim->AddAnimation("dead", {0});

    anim->SetAnimation("idle");
    anim->SetAnimFPS(10.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, Mario::MASS, Mario::FRICTION);

    new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Player);
}

void Mario::OnProcessInput(const uint8_t* state)
{
    mIsRunning = false;

    if (state[SDL_SCANCODE_D])
    {
        mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
        SetScale(Vector2(1.0f, 1.0f));
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_A])
    {
        mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
        SetScale(Vector2(-1.0f, 1.0f));
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_SPACE] && mIsOnGround)
    {
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = -mJumpSpeed;
        mRigidBodyComponent->SetVelocity(vel);
        mIsOnGround = false;
    }
}

void Mario::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    if (!Math::NearlyZero(mRigidBodyComponent->GetVelocity().y))
    {
        mIsOnGround = false;
    }

    if (GetPosition().y > Game::WINDOW_HEIGHT + 100)
    {
        Kill();
    }

    Vector2 pos = GetPosition();
    const Vector2& cameraPos = GetGame()->GetCameraPos();
    if (pos.x < cameraPos.x)
    {
        pos.x = cameraPos.x;
        SetPosition(pos);
    }

    ManageAnimations();
}

void Mario::ManageAnimations()
{
    AnimatorComponent* anim = GetComponent<AnimatorComponent>();
    if (!anim || mIsDead) return;

    if (!IsOnGround())
    {
        anim->SetAnimation("jump");
    }
    else
    {
        if (mIsRunning)
        {
            anim->SetAnimation("run");
        }
        else
        {
            anim->SetAnimation("idle");
        }
    }
}

void Mario::Kill()
{
    if (mIsDead) return;

    mIsDead = true;

    GetComponent<AnimatorComponent>()->SetAnimation("dead");

    mRigidBodyComponent->SetEnabled(false);
    GetComponent<AABBColliderComponent>()->SetEnabled(false);
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Kill();
    }
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Goomba* goomba = dynamic_cast<Goomba*>(other->GetOwner());
        if (goomba)
        {
            goomba->Kill();

            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = -mJumpSpeed * 0.5f;
            mRigidBodyComponent->SetVelocity(vel);
        }
    }
}
