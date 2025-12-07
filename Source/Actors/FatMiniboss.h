#pragma once
#include "Miniboss.h"

class FatMiniboss : public Miniboss
{
public:
    static constexpr float SPRITE_WIDTH = 24.0f * 4.0f;
    static constexpr float SPRITE_HEIGHT = 24.0f * 4.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.60f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.60f;

    static constexpr float MAX_HP = 125.0f;
    static constexpr float WALK_SPEED = 80.0f;
    static constexpr float PUDDLE_SPAWN_INTERVAL = 0.5f;

    static constexpr float ATTACK_COOLDOWN = 2.5f;
    static constexpr float ATTACK_WINDUP = 0.5f;

    enum class BossState { Moving, WindUp, Attacking, Cooldown, BeingHit, Counter, ForcedAttack, Dead };

    explicit FatMiniboss(class Game* game);

    void OnUpdate(float deltaTime) override;
    void TakeDamage(float amount) override;

private:
    void UpdateAI(float deltaTime);
    void ShootSlime();
    void SpawnPuddle();
    void PerformForcedAttack() override;

    BossState mState;
    float mStateTimer;
    float mPuddleTimer;
};