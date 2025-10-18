#include "Background.h"
#include "../Game.h"
#include "../Components/Drawing/StaticSpriteComponent.h"

Background::Background(Game* game)
    : Actor(game)
{
    SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f));

    new StaticSpriteComponent(
        this,
        "../Assets/Levels/Level1/Background.png",
        Game::WINDOW_WIDTH,
        Game::WINDOW_HEIGHT,
        10
    );

    GetGame()->SetUpperBoundary(423.f);
}