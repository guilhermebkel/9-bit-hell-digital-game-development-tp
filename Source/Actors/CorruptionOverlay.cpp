#include "CorruptionOverlay.h"
#include "../Game.h"
#include "Player.h"
#include "../Components/Drawing/StaticSpriteComponent.h"

CorruptionOverlay::CorruptionOverlay(Game* game)
    : Actor(game)
{
    int higherThanActorsDrawOrder = Game::WINDOW_HEIGHT * 2.0f;
    int initialVignetteSize = Math::Max(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT) * 2.5f;

    new StaticSpriteComponent(this, "../Assets/Vignette.png", initialVignetteSize, initialVignetteSize, higherThanActorsDrawOrder);
}

void CorruptionOverlay::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    const Player* player = GetGame()->GetPlayer();
    if (!player)
    {
        return;
    }

    SetPosition(player->GetPosition());

    float corruption = GetGame()->GetCorruptionLevel();

    float targetScale = Math::Lerp(MAX_SCALE, MIN_SCALE, corruption);

    SetScale(Vector2(targetScale, targetScale));
}