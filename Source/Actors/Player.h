#pragma once
#include "Actor.h"

class Player : public Actor
{
public:
    static constexpr float MASS = 1.0f;
    static constexpr float FRICTION = 10.0f;

    static constexpr float SPRITE_WIDTH = 68.0f;
    static constexpr float SPRITE_HEIGHT = 68.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.50f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.25f;

    static constexpr float MELEE_ATTACK_ANIMATION_DURATION = 0.3f;
    static constexpr float RANGED_ATTACK_ANIMATION_DURATION = 0.3f;
    static constexpr float BEING_HIT_ANIMATION_DURATION = 0.3f;
    static constexpr float HIT_FREEZE_DURATION = 0.5f;
    static constexpr float INVINCIBILITY_DURATION = 1.0f;

    explicit Player(Game* game, float forwardSpeed = 2500.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

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
    bool mHasKilledEnemyInCurrentAttack;

    bool mIsRangedAttacking;
    float mRangedAttackAnimationTimer;
    float mRangedAttackCooldownTimer;

    bool mIsBeingHit;
    float mBeingHitAnimationTimer;
    
    bool mIsInvincible;
    float mInvincibilityTimer;
    float mHitFreezeTimer;
    float mBlinkTimer;

    int mCoinCount;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

    float mHealth;
    float mMaxHealth;
};