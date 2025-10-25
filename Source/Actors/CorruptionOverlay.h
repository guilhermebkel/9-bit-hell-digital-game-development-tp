#pragma once
#include "Actor.h"
#include "../Game.h"

class CorruptionOverlay : public Actor
{
public:
    static constexpr float DRAW_ORDER = Game::WINDOW_HEIGHT * 2.0f; // Higher than all Actors

    explicit CorruptionOverlay(class Game* game);

    void OnUpdate(float deltaTime) override;

private:
    const float MAX_SCALE = 4.0f;
    const float MIN_SCALE = 0.8f;
};
