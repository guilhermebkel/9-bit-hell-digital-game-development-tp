#include "HUD.h"
#include "../Game.h"
#include "Player.h"
#include "../Components/Drawing/UITextComponent.h"

HUD::HUD(Game* game)
    : Actor(game)
    , mCoinText(nullptr)
    , mCorruptionText(nullptr)
{

    Actor* coinActor = new Actor(game);
    mCoinText = new UITextComponent(coinActor, HUD::DRAW_ORDER);
    mCoinText->SetText("Moedas: 0", Color::White, 24);

    Actor* corruptionActor = new Actor(game);
    mCorruptionText = new UITextComponent(corruptionActor, HUD::DRAW_ORDER);
    mCorruptionText->SetText("Corrupcao: 0%", Color::White, 24);
}

void HUD::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    const Player* player = GetGame()->GetPlayer();
    if (player && mCoinText)
    {
        std::string coinStr = "Moedas: " + std::to_string(player->GetCoinCount());
        mCoinText->SetText(coinStr, Color::White, 24);

        Vector2 position = mCoinText->GetOwner()->GetPosition();
        position.x = 0.0f + (mCoinText->GetTexture()->GetWidth() / 2.0f) + 16.0f;
        position.y = 0.0f + (mCoinText->GetTexture()->GetHeight() / 2.0f) + 16.0f;
        mCoinText->GetOwner()->SetPosition(position);
    }

    if (mCorruptionText)
    {
        int corruptionPercent = static_cast<int>(GetGame()->GetCorruptionLevel() * 100);
        std::string corruptionStr = "Corrupcao: " + std::to_string(corruptionPercent) + "%";
        mCorruptionText->SetText(corruptionStr, Color::White, 24);

        Vector2 position = mCorruptionText->GetOwner()->GetPosition();
        position.x = Game::WINDOW_WIDTH - (mCoinText->GetTexture()->GetWidth() / 2.0f) - 48.0f;
        position.y = 0.0f + (mCoinText->GetTexture()->GetHeight() / 2.0f) + 16.0f;
        mCorruptionText->GetOwner()->SetPosition(position);
    }
}