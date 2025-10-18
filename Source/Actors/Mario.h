//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

class Mario : public Actor
{
public:
    static constexpr float FORWARD_SPEED = 2500.0f;
    static constexpr float JUMP_SPEED = 850.0f;
    static constexpr float MASS = 1.0f;
    static constexpr float FRICTION = 10.0f;

    explicit Mario(Game* game, float forwardSpeed = 1500.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class ParticleSystemComponent* mFireBalls;

    float mFireBallCooldown = 1.0f;
};