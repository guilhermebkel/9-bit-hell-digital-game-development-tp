#pragma once
#include "Miniboss.h"

class HornMiniboss : public Miniboss
{
public:
    static constexpr float SPRITE_WIDTH = 84.0f;
    static constexpr float SPRITE_HEIGHT = 84.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 0.60f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 0.60f;
    
    static constexpr float MAX_HP = 400.0f;
    static constexpr float WALK_SPEED = 120.0f;
    
    static constexpr float ATTACK_COOLDOWN = 2.0f;
    static constexpr float ATTACK_WINDUP = 0.6f;

    enum class BossState { Moving, WindUp, Attacking, Cooldown, Dead };
    enum class AttackType { HandDrop, GroundSpikes };

    explicit HornMiniboss(class Game* game);

    void OnUpdate(float deltaTime) override;
    void TakeDamage(float amount) override;

private:
    void UpdateAI(float deltaTime);
    void PerformHandAttack();
    void PerformSpikeAttack();

    BossState mState;
    AttackType mNextAttack;
    float mStateTimer;
};