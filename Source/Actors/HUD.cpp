#include "HUD.h"
#include "../Game.h"
#include "Player.h"
#include "../Components/Drawing/UITextComponent.h"

HUD::HUD(Game* game)
    : Actor(game)
    , mCoinWidget(nullptr)
    , mCorruptionWidget(nullptr)
{
    int pointSize = 24;

    mCoinWidget = new UIStatWidget(game, "COINS", pointSize);
    mCoinWidget->SetOutline(true);
    mCoinWidget->SetPosition(Vector2(20.0f, 32.0f), HAlign::Left);

    mCorruptionWidget = new UIStatWidget(game, "CORRUPTION", pointSize);
    mCorruptionWidget->SetOutline(true);
    mCorruptionWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, 32.0f), HAlign::Right);
}

void HUD::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    const Player* player = GetGame()->GetPlayer();
    if (player)
    {
        mCoinWidget->SetValue(std::to_string(player->GetCoinCount()));
    }

    int corruptionPercent = static_cast<int>(GetGame()->GetCorruptionLevel() * 100);
    mCorruptionWidget->SetValue(std::to_string(corruptionPercent) + "%");
}