#pragma once
#include "Collectable.h"

class Coin : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 32;
    static constexpr int SPRITE_HEIGHT = 32;

    explicit Coin(class Game* game);

protected:
    void OnCollect(class Player* player) override;
};