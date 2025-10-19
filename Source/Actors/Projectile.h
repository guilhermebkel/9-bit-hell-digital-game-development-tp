#pragma once
#include "Actor.h"

class Projectile : public Actor
{
public:
    static constexpr float SPEED = 600.0f;

    static constexpr float SPRITE_WIDTH = 24.0f;
    static constexpr float SPRITE_HEIGHT = 24.0f;
    static constexpr float PHYSICS_WIDTH = SPRITE_WIDTH * 1.0f;
    static constexpr float PHYSICS_HEIGHT = SPRITE_HEIGHT * 2.0f;

    explicit Projectile(class Game* game);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void HandleCollision(AABBColliderComponent* other);
};