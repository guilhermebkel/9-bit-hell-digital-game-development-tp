#pragma once
#include "Miniboss.h"

class EyeMiniboss : public Miniboss
{
public:
    static constexpr float SPRITE_WIDTH = 24.0f * 4.0f;
    static constexpr float SPRITE_HEIGHT = 24.0f * 4.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.60f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.60f;
    
    static constexpr float MAX_HP = 100.0f;
    static constexpr float MOVE_SPEED = 100.0f;
    
    static constexpr float ATTACK_COOLDOWN = 2.0f;
    static constexpr float WINDUP_TIME = 0.5f;
    static constexpr float BURST_INTERVAL = 0.4f;

    enum class BossState { Moving, WindUp, Attacking, Cooldown, BeingHit, Counter, ForcedAttack, Dead };

    explicit EyeMiniboss(class Game* game);

    void OnUpdate(float deltaTime) override;
    void TakeDamage(float amount) override;
    void Kill() override;
    void SpawnSoulsOnDeath() override;

private:
    void UpdateAI(float deltaTime);
    void PerformSpreadShot();
    void PerformForcedAttack() override;

    BossState mState;
    float mStateTimer;
    
    int mBurstShotsLeft;
};