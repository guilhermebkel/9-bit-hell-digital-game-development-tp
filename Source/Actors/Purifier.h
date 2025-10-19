#pragma once
#include "Collectable.h"

class Purifier : public Collectable
{
public:
    explicit Purifier(class Game* game);

protected:
    void OnCollect(class Player* player) override;
};