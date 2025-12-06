#pragma once
#include "Collectable.h"

class Healer : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 30.0f;
    static constexpr int SPRITE_HEIGHT = 30.0f;

    explicit Healer(class Game* game);

    void OnUpdate(float deltaTime) override;

protected:
    void OnCollect(class Player* player) override;

private:
    float mPulseTimer;
};