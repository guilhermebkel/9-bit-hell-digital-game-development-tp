#include "GroundSpike.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Audio/AudioSystem.h"

GroundSpike::GroundSpike(Game* game)
    : Actor(game)
    , mTimer(0.0f)
{
    // Desenha na frente do chão, atrás do player
    new StaticSpriteComponent(this, "../Assets/Spike.png", SPRITE_WIDTH, SPRITE_HEIGHT, 20);
    
    // O espinho começa "enterrado" (Escala Y = 0) e com pivô na base
    // Nota: Como o sistema de escala padrão é pelo centro, faremos um efeito visual simples
    // ou aceitaremos que ele cresce do meio. Para simplificar: cresce do meio.
    SetScale(Vector2(1.0f, 0.0f));

    mCollider = new AABBColliderComponent(this, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, ColliderLayer::Enemy, true);
    mCollider->SetEnabled(false); // Começa desativado para dar tempo de reação
}

void GroundSpike::OnUpdate(float deltaTime)
{
    mTimer += deltaTime;

    // Fase 1: Crescendo (Aviso)
    if (mTimer < RISE_TIME)
    {
        float pct = mTimer / RISE_TIME;
        SetScale(Vector2(1.0f, pct)); // Cresce verticalmente
    }
    // Fase 2: Ativo e Letal
    else if (mTimer < LIFE_TIME)
    {
        SetScale(Vector2(1.0f, 1.0f));
        if (!mCollider->IsEnabled())
        {
            mCollider->SetEnabled(true);
            GetGame()->GetAudioSystem()->PlaySound("../Assets/Sounds/spike-rise.wav"); // Som hipotético
        }
    }
    // Fim da vida
    else
    {
        SetState(ActorState::Destroy);
    }
}

void GroundSpike::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (player) player->TakeDamage(DAMAGE);
    }
}

void GroundSpike::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        auto player = dynamic_cast<Player*>(other->GetOwner());
        if (player) player->TakeDamage(DAMAGE);
    }
}