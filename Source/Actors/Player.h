#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
    static constexpr float MASS = 1.0f;
    static constexpr float FRICTION = 10.0f;

    static constexpr float SPRITE_WIDTH = 80.0f;
    static constexpr float SPRITE_HEIGHT = 80.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.50f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.25f;

    static constexpr float MELEE_ATTACK_ANIMATION_DURATION = 0.3f;
    static constexpr float RANGED_ATTACK_ANIMATION_DURATION = 0.3f;
    static constexpr float RANGED_ATTACK_COOLDOWN = 0.3f;

    explicit Player(Game* game, float forwardSpeed = 2500.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void AddCoin(int amount = 1);
    int GetCoinCount() const;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void TakeDamage(float amount);
    float GetHealth() const { return mHealth; }
    float GetMaxHealth() const { return mMaxHealth; }

private:
    void ManageAnimations();

    float mForwardSpeed;
    bool mIsRunning;
    bool mIsDead;

    bool mIsMeleeAttacking;
    float mMeleeAttackAnimationTimer;

    bool mIsRangedAttacking;
    float mRangedAttackAnimationTimer;
    float mRangedAttackCooldownTimer;

    int mCoinCount;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    float mFireBallCooldown = 1.0f;

    float mHealth;
    float mMaxHealth;
};