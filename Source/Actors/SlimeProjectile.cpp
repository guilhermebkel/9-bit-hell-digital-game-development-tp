#include "SlimeProjectile.h"
#include "../Game.h"
#include "Player.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

SlimeProjectile::SlimeProjectile(Game* game, const Vector2& direction)
    : Actor(game)
{
    new StaticSpriteComponent(this, "../Assets/SlimeProjectile.png", SIZE, SIZE);

    auto rb = new RigidBodyComponent(this, 1.0f, 0.0f);
    mVelocity = direction * SPEED;
    rb->SetVelocity(mVelocity);

    mCollider = new AABBColliderComponent(this, 0, 0, SIZE, SIZE, ColliderLayer::Enemy);
    
    float angle = Math::Atan2(direction.y, direction.x);
    SetRotation(angle);
}

void SlimeProjectile::OnUpdate(float deltaTime)
{
    Vector2 pos = GetPosition();
    if (pos.x < 0 || pos.x > Game::WINDOW_WIDTH || 
        pos.y < 0 || pos.y > Game::WINDOW_HEIGHT)
    {
        SetState(ActorState::Destroy);
    }
}

void SlimeProjectile::HandleCollision(AABBColliderComponent* other)
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

void SlimeProjectile::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}

void SlimeProjectile::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    HandleCollision(other);
}