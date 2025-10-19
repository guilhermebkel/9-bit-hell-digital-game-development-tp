// File Path: ./Source/Actors/Coin.cpp

#include "Coin.h"
#include "Player.h"
#include <SDL2/SDL.h>

Coin::Coin(class Game* game)
    : Collectable(game, "../Assets/Sprites/Collectables/Coin.png", Coin::SPRITE_WIDTH, Coin::SPRITE_HEIGHT)
{
}

void Coin::OnCollect(Player* player)
{
    player->AddCoin();
    SDL_Log("Moeda coletada! Total: %d", player->GetCoinCount());
}