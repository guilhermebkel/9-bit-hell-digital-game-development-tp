#include "Purifier.h"
#include "Player.h"
#include "../Game.h"
#include "../Math.h"
#include "../Random.h"
#include "../Audio/AudioSystem.h"
#include <SDL2/SDL.h>
#include <cmath>

Purifier::Purifier(class Game* game)
    : Collectable(game, "../Assets/Sprites/Collectables/PowerUp.png", Purifier::SPRITE_WIDTH, Purifier::SPRITE_HEIGHT)
    , mPulseTimer(Random::GetFloatRange(0.0f, 6.28f))
{
}

void Purifier::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);
    
    mPulseTimer += deltaTime * 1.33f;
    
    float pulseScale = 1.0f + (std::sin(mPulseTimer) * 0.17f);
    SetScale(Vector2(pulseScale, pulseScale));
}

void Purifier::OnCollect(Player* player)
{
    player->GetGame()->ReduceCorruption(0.25f);
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/pickup-cross.wav");
}