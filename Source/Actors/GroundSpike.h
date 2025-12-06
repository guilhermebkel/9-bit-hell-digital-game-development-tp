#pragma once
#include "Actor.h"

class GroundSpike : public Actor
{
public:
    static constexpr float DAMAGE = 15.0f;
    static constexpr float RISE_TIME = 0.3f;
    static constexpr float LIFE_TIME = 1.5f;
    static constexpr float SPRITE_WIDTH = 12.0f * 5.0f;
    static constexpr float SPRITE_HEIGHT = 6.0f * 5.0f;;

    explicit GroundSpike(class Game* game);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    float mTimer;
    class AABBColliderComponent* mCollider;
};