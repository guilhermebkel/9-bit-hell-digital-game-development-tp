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

    mCoinWidget = new UIStatWidget(game, "SOULS", pointSize, HUD::DRAW_ORDER);
    mCoinWidget->SetOutline(true);
    mCoinWidget->SetPosition(Vector2(60.0f, 56.0f), HAlign::Left);
    mCoinWidget->SetValue(std::to_string(GetGame()->GetCoinCount()));

    mCorruptionWidget = new UIStatWidget(game, GetCurrentLevelTitle(game->GetCurrentLevelID()), pointSize, HUD::DRAW_ORDER);
    mCorruptionWidget->SetOutline(true);
    mCorruptionWidget->SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f + 48.0f, 56.0f * 2.0f), HAlign::Center);

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

std::string HUD::GetCurrentLevelTitle (LevelID levelId)
{
    std::string levelTitle;

    switch (levelId)
    {
        case LevelID::Tutorial: levelTitle = "TUTORIAL"; break;
        case LevelID::Level1: levelTitle = "LEVEL 1"; break;
        case LevelID::Level2: levelTitle = "LEVEL 2"; break;
        case LevelID::Level3: levelTitle = "LEVEL 3"; break;
        case LevelID::Level4: levelTitle = "LEVEL 4"; break;
        case LevelID::Level5: levelTitle = "LEVEL 5"; break;
        case LevelID::Level6: levelTitle = "LEVEL 6"; break;
        case LevelID::Level7: levelTitle = "LEVEL 7"; break;
        case LevelID::Level8: levelTitle = "LEVEL 8"; break;
        case LevelID::Level9: levelTitle = "LEVEL 9"; break;
    }

    return levelTitle;
}