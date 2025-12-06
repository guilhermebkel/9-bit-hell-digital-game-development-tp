#pragma once
#include "Actor.h"

class Miniboss : public Actor
{
public:
    static constexpr float HIT_FLASH_DURATION = 0.1f;
    static constexpr float BODY_DAMAGE = 20.0f; // Dano ao encostar no boss

    Miniboss(class Game* game, float health);

    void OnUpdate(float deltaTime) override;
    
    virtual void TakeDamage(float amount);
    void Kill() override;
    bool IsDead() const { return mIsDead; }

    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

protected:
    float mHealth;
    float mMaxHealth;
    bool mIsDead;

    class AnimatorComponent* mAnimator;
    class RigidBodyComponent* mRigidBody;
    class AABBColliderComponent* mCollider;

private:
    bool mIsFlashing;
    float mFlashTimer;
    Vector3 mOriginalColor;
};