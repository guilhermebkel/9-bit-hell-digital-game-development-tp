#include "Healer.h"
#include "Player.h"
#include "../Game.h"
#include "../Audio/AudioSystem.h"
#include "../Components/Drawing/AnimatorComponent.h"

Healer::Healer(class Game* game)
    : Collectable(game, Healer::SPRITE_WIDTH, Healer::SPRITE_HEIGHT)
{
    class AnimatorComponent* mDrawComponent = new AnimatorComponent(
        this,
        Game::ResolvePath("Assets/Sprites/Collectables/HealthOrb/HealthOrb.png"),
        Game::ResolvePath("Assets/Sprites/Collectables/HealthOrb/HealthOrb.json"),
        Healer::SPRITE_WIDTH,
        Healer::SPRITE_HEIGHT
    );
    mDrawComponent->AddAnimation("idle", {0, 1, 2, 3, 2, 1});
    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(6.0f);
}

void Healer::OnUpdate(float deltaTime)
{
    Actor::OnUpdate(deltaTime);
}

void Healer::OnCollect(Player* player)
{
    player->Heal(10);
    GetGame()->GetAudioSystem()->PlaySound(Game::ResolvePath("Assets/Sounds/pickup-health.wav"));
}