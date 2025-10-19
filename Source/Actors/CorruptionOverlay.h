#pragma once
#include "Actor.h"

class CorruptionOverlay : public Actor
{
public:
    explicit CorruptionOverlay(class Game* game);

    void OnUpdate(float deltaTime) override;

private:
    const float MAX_SCALE = 4.0f;
    const float MIN_SCALE = 0.8f;
};