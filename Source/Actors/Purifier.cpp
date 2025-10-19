// File Path: ./Source/Actors/Mushroom.cpp

#include "Purifier.h"
#include "Player.h"
#include <SDL2/SDL.h>

Purifier::Purifier(class Game* game)
    : Collectable(game, "../Assets/Sprites/Collectables/Mushroom.png", Purifier::SPRITE_WIDTH, Purifier::SPRITE_HEIGHT)
{
}

void Purifier::OnCollect(Player* player)
{
    SDL_Log("Item de purificacao coletado!");
}