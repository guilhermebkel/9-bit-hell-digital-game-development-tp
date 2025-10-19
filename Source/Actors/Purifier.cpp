// File Path: ./Source/Actors/Mushroom.cpp

#include "Purifier.h"
#include "Player.h"
#include <SDL2/SDL.h>

Purifier::Purifier(class Game* game)
    : Collectable(game, "../Assets/Sprites/Collectables/Mushroom.png", 32, 32)
{
}

void Purifier::OnCollect(Player* player)
{
    SDL_Log("Item de purificacao coletado!");
}