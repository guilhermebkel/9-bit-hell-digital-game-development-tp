//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
    static constexpr float MASS = 1.0f;
    static constexpr float FRICTION = 10.0f;
    static constexpr float WIDTH = 80.0f;
    static constexpr float HEIGHT = 80.0f;

    explicit Player(Game* game, float forwardSpeed = 2500.0f, float jumpSpeed = -750.0f);

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