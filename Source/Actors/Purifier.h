#pragma once
#include "Collectable.h"

class Purifier : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 34.0f;
    static constexpr int SPRITE_HEIGHT = 34.0f;

    explicit Purifier(class Game* game);

    void OnUpdate(float deltaTime) override;

protected:
    void OnCollect(class Player* player) override;

private:
    float mPulseTimer;
};