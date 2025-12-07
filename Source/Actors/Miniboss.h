#pragma once
#include "Actor.h"

class Miniboss : public Actor
{
public:
    static constexpr float HIT_FLASH_DURATION = 0.1f;
    static constexpr float BODY_DAMAGE = 20.0f; // Dano ao encostar no boss
    static constexpr float INVULNERABILITY_DURATION = 1.5f; // Invencibilidade após dano - tempo para counter-attack

    Miniboss(class Game* game, float health);

    void OnUpdate(float deltaTime) override;
    
    virtual void TakeDamage(float amount);
    void Kill() override;
    bool IsDead() const { return mIsDead; }
    bool IsInvulnerable() const { return mInvulnerabilityTimer > 0.0f; }
    void CreateHealthBar(const Vector2& barSize = Vector2(100.0f, 15.0f));
    virtual void PerformForcedAttack() {} // Para ser sobrescrito por cada miniboss

    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

protected:
    float mHealth;
    float mMaxHealth;
    bool mIsDead;
    bool mJustBecameVulnerable; // Flag para ativar counter-attack
    float mBeingHitTimer; // Timer para controlar transição de being-hit para forced attack
    bool mForcedAttackExecuted; // Flag para garantir que forced attack execute apenas uma vez
    int mForcedAttackCount; // Contador de quantos ataques já foram feitos no ForcedAttack
    float mForcedAttackIntervalTimer; // Timer para controlar intervalo entre ataques
    class UIHealthBarWidget* mHealthBar;
    Vector2 mHealthBarOffset;

    class AnimatorComponent* mAnimator;
    class RigidBodyComponent* mRigidBody;
    class AABBColliderComponent* mCollider;

private:
    bool mIsFlashing;
    float mFlashTimer;
    Vector3 mOriginalColor;
    float mInvulnerabilityTimer;
};