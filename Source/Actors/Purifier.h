#pragma once
#include "Collectable.h"

class Purifier : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 16;
    static constexpr int SPRITE_HEIGHT = 16;

    explicit Purifier(class Game* game);

protected:
    void OnCollect(class Player* player) override;
};