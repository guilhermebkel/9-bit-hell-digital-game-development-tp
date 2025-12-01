#pragma once
#include "Collectable.h"

class Coin : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 34.0f;
    static constexpr int SPRITE_HEIGHT = 34.0f;

    explicit Coin(class Game* game);

    void OnUpdate(float deltaTime) override;

protected:
    void OnCollect(class Player* player) override;

private:
    float mFloatTimer;
    Vector2 mInitialPosition;
};