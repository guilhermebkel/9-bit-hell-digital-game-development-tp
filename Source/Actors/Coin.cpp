#include "Coin.h"
#include "Player.h"
#include "../Game.h"

Coin::Coin(class Game* game)
    : Collectable(game, "../Assets/Sprites/Collectables/Coin.png", Coin::SPRITE_WIDTH, Coin::SPRITE_HEIGHT)
{
}

void Coin::OnCollect(Player* player)
{
    GetGame()->AddCoin();
}