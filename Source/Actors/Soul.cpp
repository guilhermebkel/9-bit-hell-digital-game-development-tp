#include "Soul.h"
#include "Player.h"
#include "../Game.h"
#include "../Math.h"
#include "../Random.h"
#include "../Audio/AudioSystem.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include <cmath>

Soul::Soul(class Game* game, Soul::SoulType type)
    : Collectable(game, Soul::SPRITE_WIDTH, Soul::SPRITE_HEIGHT)
    , mFloatTimer(Random::GetFloatRange(0.0f, 6.28f))
    , mInitialPosition(Vector2::Zero)
    , mType(type)
    , mValue(1)
{
    std::string spritePath = Game::ResolvePath("Assets/Sprites/Collectables/Soul.png");

    if (mType == Soul::SoulType::Purple)
    {
        spritePath = Game::ResolvePath("Assets/Sprites/Collectables/PurpleSoul.png");
        mValue = 10;
    }
    else if (mType == Soul::SoulType::Golden)
    {
        spritePath = Game::ResolvePath("Assets/Sprites/Collectables/GoldenSoul.png");
        mValue = 50;
    }

    new StaticSpriteComponent(
        this,
        spritePath,
        Soul::SPRITE_WIDTH,
        Soul::SPRITE_HEIGHT,
        90
    );
}

void Soul::OnUpdate(float deltaTime)
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

void Soul::OnCollect(Player* player)
{
    GetGame()->AddSoul(mValue);
    
    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/pickup-soul.wav"));
}
