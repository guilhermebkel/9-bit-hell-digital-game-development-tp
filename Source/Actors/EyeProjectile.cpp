#include "EyeProjectile.h"
#include "Player.h"
#include "../Game.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

EyeProjectile::EyeProjectile(Game* game, const Vector2& direction)
    : Actor(game)
{
    new StaticSpriteComponent(this, "../Assets/EyeProjectile.png", SPRITE_WIDTH, SPRITE_HEIGHT);

    auto rb = new RigidBodyComponent(this, 1.0f, 0.0f);
    rb->SetVelocity(direction * SPEED);

    mCollider = new AABBColliderComponent(this, 0, 0, SPRITE_WIDTH, SPRITE_HEIGHT, ColliderLayer::Enemy);
    
    SetRotation(Math::Atan2(direction.y, direction.x));
}

void EyeProjectile::OnUpdate(float deltaTime)
{
    Vector2 pos = GetPosition();
    if (pos.x < -50 || pos.x > Game::WINDOW_WIDTH + 50 || 
        pos.y < -50 || pos.y > Game::WINDOW_HEIGHT + 50)
    {
        SetState(ActorState::Destroy);
    }
}

void EyeProjectile::HandleCollision(AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player)
    {
        Player* player = dynamic_cast<Player*>(other->GetOwner());
        if (player)
        {
            player->TakeDamage(DAMAGE);
            SetState(ActorState::Destroy);
        }
    }
}

void EyeProjectile::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}

void EyeProjectile::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}