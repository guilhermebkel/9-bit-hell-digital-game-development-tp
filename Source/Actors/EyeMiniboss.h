#pragma once
#include "Miniboss.h"

class EyeMiniboss : public Miniboss
{
public:
    static constexpr float SPRITE_W = 64.0f;
    static constexpr float SPRITE_H = 64.0f;
    static constexpr float PHYSICS_W = 40.0f;
    static constexpr float PHYSICS_H = 40.0f;
    
    static constexpr float MAX_HP = 300.0f; // Menos vida que os outros, mas spamma tiro
    static constexpr float MOVE_SPEED = 90.0f;
    
    // Tempos
    static constexpr float ATTACK_COOLDOWN = 2.0f;
    static constexpr float WINDUP_TIME = 0.5f;
    static constexpr float BURST_INTERVAL = 0.4f; // Tempo entre disparos na mesma rajada

    enum class BossState { Moving, WindUp, Attacking, Cooldown, Dead };

    explicit EyeMiniboss(class Game* game);

    void OnUpdate(float deltaTime) override;
    void TakeDamage(float amount) override;

private:
    void UpdateAI(float deltaTime);
    void PerformSpreadShot();

    BossState mState;
    float mStateTimer;
    
    // Controle da rajada de tiros
    int mBurstShotsLeft;
};