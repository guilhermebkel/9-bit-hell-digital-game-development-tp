#include "Background.h"
#include "../Game.h"
#include "../Components/Drawing/StaticSpriteComponent.h"

Background::Background(Game* game, const std::string& backgroundPath)
    : Actor(game)
{
    SetPosition(Vector2(Game::WINDOW_WIDTH / 2.0f, Game::WINDOW_HEIGHT / 2.0f));

    new StaticSpriteComponent(
        this,
        backgroundPath,
        Game::WINDOW_WIDTH,
        Game::WINDOW_HEIGHT,
        10
    );
}