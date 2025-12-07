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
    
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/pickup-soul-2.wav");
}