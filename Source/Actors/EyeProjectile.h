#pragma once
#include "Actor.h"

class EyeProjectile : public Actor
{
public:
    static constexpr float SPEED = 350.0f;
    static constexpr float DAMAGE = 10.0f;
    static constexpr float SIZE = 24.0f;

    explicit EyeProjectile(class Game* game, const Vector2& direction);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    void HandleCollision(class AABBColliderComponent* other);
    class AABBColliderComponent* mCollider;
};