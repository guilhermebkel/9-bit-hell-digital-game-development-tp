#pragma once

#include "Actor.h"

class Enemy : public Actor
{
public:
    enum class AIState
    {
        Moving,
        WindUp,
        Attacking,
        Cooldown,
        Chasing
    };

    static constexpr float SPRITE_WIDTH = 48.0f;
    static constexpr float SPRITE_HEIGHT = 48.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.50f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.25f;
    static constexpr float AGGRO_AREA_SIZE = 256.0f;

    explicit Enemy(Game* game, float forwardSpeed = 100.0f, float deathTime = 0.5f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

private:
    void UpdateAI(float deltaTime);
    void PerformAttack();
    AIState mAIState;
    float mStateTimer;

    const float mAttackWindUpTime = 0.5f;
    const float mAttackDuration = 0.5f;
    const float mAttackCooldown = 1.0f;
    const float mAttackDamage = 10.0f;
    AABBColliderComponent* mAggroCollider;

    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};