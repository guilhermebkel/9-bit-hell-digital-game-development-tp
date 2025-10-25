#pragma once
#include "Actor.h"
#include "CorruptionOverlay.h"
#include "UIHealthBarWidget.h"
#include "UIStatWidget.h"

class HUD : public Actor
{
public:
    static constexpr float DRAW_ORDER = CorruptionOverlay::DRAW_ORDER + 1; // Higher than Corruption Overlay

    explicit HUD(class Game* game);

    void OnUpdate(float deltaTime) override;

private:
    UIStatWidget* mCoinWidget;
    UIStatWidget* mCorruptionWidget;
    UIHealthBarWidget* mHealthBarWidget;

    void OnUpdate();
};
