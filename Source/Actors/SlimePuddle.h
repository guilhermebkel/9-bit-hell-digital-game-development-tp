#pragma once
#include "Actor.h"

class SlimePuddle : public Actor
{
public:
    static constexpr float DAMAGE = 5.0f;
    static constexpr float LIFETIME = 5.0f; // Dura 5 segundos
    static constexpr float SIZE = 32.0f;

    explicit SlimePuddle(class Game* game);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    float mLifeTimer;
    class AABBColliderComponent* mCollider;
};