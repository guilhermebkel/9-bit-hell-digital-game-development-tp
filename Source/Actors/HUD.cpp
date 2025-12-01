#include "HUD.h"
#include "../Game.h"
#include "Player.h"
#include "UIHealthBarWidget.h"
#include "../Components/Drawing/UITextComponent.h"

HUD::HUD(Game* game)
    : Actor(game)
    , mCoinWidget(nullptr)
    , mCorruptionWidget(nullptr)
{
    int pointSize = 36;

    mCoinWidget = new UIStatWidget(game, "COINS", pointSize, HUD::DRAW_ORDER);
    mCoinWidget->SetOutline(true);
    mCoinWidget->SetPosition(Vector2(20.0f, 56.0f), HAlign::Left);
    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));

    mCorruptionWidget = new UIStatWidget(game, "CORRUPTION", pointSize, HUD::DRAW_ORDER);
    mCorruptionWidget->SetOutline(true);
    mCorruptionWidget->SetPosition(Vector2(Game::WINDOW_WIDTH - 20.0f, 56.0f), HAlign::Right);
    int corruptionPercent = static_cast<int>(GetGame()->GetCorruptionLevel() * 100);
    mCorruptionWidget->SetValue(std::to_string(corruptionPercent) + "%");

    mHealthBarWidget = new UIHealthBarWidget(game, Vector2(Game::WINDOW_WIDTH / 2.0f, 56.0f), Vector2(350.0f, 35.0f), HUD::DRAW_ORDER);
}

void HUD::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    const Player* player = GetGame()->GetPlayer();

    if (!player)
    {
        return;
    }

    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));

    int corruptionPercent = static_cast<int>(GetGame()->GetCorruptionLevel() * 100);
    mCorruptionWidget->SetValue(std::to_string(corruptionPercent) + "%");

    mHealthBarWidget->Update(player->GetHealth(), player->GetMaxHealth());
}