#pragma once
#include "Actor.h"
#include <string>

class Collectable : public Actor
{
public:
    Collectable(class Game* game, const std::string& texturePath, int width, int height);

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

protected:
    virtual void OnCollect(class Player* player) = 0;

    class AABBColliderComponent* mColliderComponent;

private:
    void HandleCollision(AABBColliderComponent* other);
};