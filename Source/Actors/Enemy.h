#pragma once

#include "Actor.h"

class Enemy : public Actor
{
public:
    enum class EnemyType
    {
        Eye,
        Horn
    };

    enum class AIState
    {
        Moving,
        WindUp,
        Attacking,
        Cooldown,
        Chasing
    };

    static constexpr float SPRITE_WIDTH = 60.0f;
    static constexpr float SPRITE_HEIGHT = 60.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.50f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.25f;
    static constexpr float AGGRO_AREA_SIZE = 800.0f;

    static constexpr float ATTACK_DISTANCE = 50.0f;
    static constexpr float ATTACK_WIND_UP_TIME = 0.5f;
    static constexpr float ATTACK_DURATION = 0.5f;
    static constexpr float ATTACK_COOLDOWN = 1.0f;
    static constexpr float ATTACK_DAMAGE = 10.0f;

    explicit Enemy(Game* game, EnemyType type, float forwardSpeed = 100.0f, float deathTime = 0.5f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

private:
    void UpdateAI(float deltaTime);
    void PerformAttack();
    AIState mAIState;
    float mStateTimer;

    AABBColliderComponent* mAggroCollider;

    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};