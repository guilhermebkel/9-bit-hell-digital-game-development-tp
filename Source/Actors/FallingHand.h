#pragma once
#include "Actor.h"

class FallingHand : public Actor
{
public:
    static constexpr float DAMAGE = 20.0f;
    static constexpr float FALL_SPEED = 600.0f;
    static constexpr float SPRITE_WIDTH = 8.0f * 4.0f;
    static constexpr float SPRITE_HEIGHT = 12.0f * 4.0f;

    explicit FallingHand(class Game* game);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    class RigidBodyComponent* mRigidBody;
};