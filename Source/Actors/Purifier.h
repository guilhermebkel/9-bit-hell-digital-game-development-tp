#pragma once
#include "Collectable.h"

class Purifier : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 32.0f;
    static constexpr int SPRITE_HEIGHT = 32.0f;

    explicit Purifier(class Game* game);

protected:
    void OnCollect(class Player* player) override;
};