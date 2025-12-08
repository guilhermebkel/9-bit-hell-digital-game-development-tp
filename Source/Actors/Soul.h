#pragma once
#include "Collectable.h"

class Soul : public Collectable
{
public:
    static constexpr int SPRITE_WIDTH = 34.0f;
    static constexpr int SPRITE_HEIGHT = 34.0f;

    enum class SoulType
    {
        Normal,
        Purple,
        Golden
    };

    explicit Soul(class Game* game, SoulType type = SoulType::Normal);

    void OnUpdate(float deltaTime) override;

protected:
    void OnCollect(class Player* player) override;

private:
    float mFloatTimer;
    Vector2 mInitialPosition;
    SoulType mType;
    int mValue;
};
