#pragma once
#include "Actor.h"

class SlimeProjectile : public Actor
{
public:
    static constexpr float SPEED = 400.0f;
    static constexpr float DAMAGE = 15.0f;
    static constexpr float SIZE = 24.0f;

    explicit SlimeProjectile(class Game* game, const Vector2& direction);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    void HandleCollision(class AABBColliderComponent* other);
    class AABBColliderComponent* mCollider;
    Vector2 mVelocity;
};