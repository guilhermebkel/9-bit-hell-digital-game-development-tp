#pragma once
#include "Collectable.h"

class Coin : public Collectable
{
public:
    explicit Coin(class Game* game);

protected:
    void OnCollect(class Player* player) override;
};