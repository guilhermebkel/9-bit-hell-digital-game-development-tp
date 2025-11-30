#pragma once
#include "Collectable.h"

class Coin : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 16;
    static constexpr int SPRITE_HEIGHT = 16;

    explicit Coin(class Game* game);

protected:
    void OnCollect(class Player* player) override;
};