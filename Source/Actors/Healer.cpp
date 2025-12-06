#include "Healer.h"
#include "Player.h"
#include "../Game.h"
#include "../Math.h"
#include "../Random.h"
#include "../Audio/AudioSystem.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include <cmath>

Healer::Healer(class Game* game)
    : Collectable(game, Healer::SPRITE_WIDTH, Healer::SPRITE_HEIGHT)
    , mPulseTimer(Random::GetFloatRange(0.0f, 6.28f))
{
    new StaticSpriteComponent(
        this,
        "../Assets/Sprites/Collectables/Healer.png",
        Healer::SPRITE_WIDTH,
        Healer::SPRITE_HEIGHT,
        90
    );
}

void Healer::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);

    mPulseTimer += deltaTime * 1.33f;

    float pulseScale = 1.0f + (std::sin(mPulseTimer) * 0.1f);
    SetScale(Vector2(pulseScale, pulseScale));
}

void Healer::OnCollect(Player* player)
{
    player->Heal(10);
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/pickup-cross.wav");
}