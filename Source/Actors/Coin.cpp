#include "Coin.h"
#include "Player.h"
#include "../Game.h"
#include "../Math.h"
#include "../Random.h"
#include "../Audio/AudioSystem.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include <cmath>

Coin::Coin(class Game* game)
    : Collectable(game, Coin::SPRITE_WIDTH, Coin::SPRITE_HEIGHT)
    , mFloatTimer(Random::GetFloatRange(0.0f, 6.28f))
    , mInitialPosition(Vector2::Zero)
{
    new StaticSpriteComponent(
        this,
        "../Assets/Sprites/Collectables/Coin.png",
        Coin::SPRITE_WIDTH,
        Coin::SPRITE_HEIGHT,
        90
    );
}

void Coin::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);
    
    // Salva a posição inicial no primeiro frame
    if (mInitialPosition.x == 0.0f && mInitialPosition.y == 0.0f)
    {
        mInitialPosition = GetPosition();
    }
    
    mFloatTimer += deltaTime * 1.5f;
    
    float floatOffset = std::sin(mFloatTimer) * (SPRITE_HEIGHT / 6.0f);
    SetPosition(Vector2(mInitialPosition.x, mInitialPosition.y + floatOffset));
}

void Coin::OnCollect(Player* player)
{
    GetGame()->AddCoin();
    
    // Tocar som aleatório de coleta de moeda
    int randomSound = Random::GetIntRange(1, 2);
    std::string soundFile = "../Assets/Sounds/pickup-soul-" + std::to_string(randomSound) + ".wav";
    GetGame()->GetAudioSystem()->PlaySound(soundFile);
}