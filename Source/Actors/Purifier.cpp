#include "Purifier.h"
#include "Player.h"
#include "../Game.h"
#include "../Random.h"
#include "../Audio/AudioSystem.h"
#include "../Components/Drawing/AnimatorComponent.h"

Purifier::Purifier(class Game* game)
    : Collectable(game, Purifier::SPRITE_WIDTH, Purifier::SPRITE_HEIGHT)
    , mPulseTimer(Random::GetFloatRange(0.0f, 6.28f))
{
    class AnimatorComponent* mDrawComponent = new AnimatorComponent(
        this,
        "../Assets/Sprites/Collectables/Light/Light.png",
        "../Assets/Sprites/Collectables/Light/Light.json",
        Purifier::SPRITE_WIDTH,
        Purifier::SPRITE_HEIGHT
    );
    mDrawComponent->AddAnimation("idle", {0, 1, 2});
    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(4.0f);
}

void Purifier::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);
}

void Purifier::OnCollect(Player* player)
{
    player->GetGame()->ReduceCorruption(0.25f);
    GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/pickup-cross.wav");
}