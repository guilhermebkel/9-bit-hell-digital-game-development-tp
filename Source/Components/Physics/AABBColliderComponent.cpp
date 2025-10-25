#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "RigidBodyComponent.h"
#include <algorithm>

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    Vector2 center = mOwner->GetPosition() + mOffset;
    float halfW = mWidth / 2.0f;
    float halfH = mHeight / 2.0f;
    return Vector2(center.x - halfW, center.y - halfH);
}

Vector2 AABBColliderComponent::GetMax() const
{
    Vector2 center = mOwner->GetPosition() + mOffset;
    float halfW = mWidth / 2.0f;
    float halfH = mHeight / 2.0f;
    return Vector2(center.x + halfW, center.y + halfH);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    Vector2 aMin = this->GetMin();
    Vector2 aMax = this->GetMax();
    Vector2 bMin = b.GetMin();
    Vector2 bMax = b.GetMax();

    bool noOverlap = aMax.x < bMin.x || bMax.x < aMin.x ||
                     aMax.y < bMin.y || bMax.y < aMin.y;

    return !noOverlap;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const
{
    Vector2 aMin = this->GetMin();
    Vector2 aMax = this->GetMax();
    Vector2 bMin = b->GetMin();
    Vector2 bMax = b->GetMax();

    float topOverlap = bMax.y - aMin.y;
    float bottomOverlap = bMin.y - aMax.y;

    if (std::abs(topOverlap) < std::abs(bottomOverlap))
    {
        return topOverlap;
    }
    return bottomOverlap;
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const
{
    Vector2 aMin = this->GetMin();
    Vector2 aMax = this->GetMax();
    Vector2 bMin = b->GetMin();
    Vector2 bMax = b->GetMax();

    float rightOverlap = bMax.x - aMin.x;
    float leftOverlap = bMin.x - aMax.x;

    if (std::abs(rightOverlap) < std::abs(leftOverlap))
    {
        return rightOverlap;
    }
    return leftOverlap;
}

void AABBColliderComponent::DetectCollision(RigidBodyComponent* rigidBody)
{
    if (mIsStatic) return;

    auto colliders = GetGame()->GetColliders();

    for (auto other : colliders)
    {
        if (other == this || !other->IsEnabled()) continue;

        if (this->Intersect(*other))
        {
            float minXOverlap = GetMinHorizontalOverlap(other);
            float minYOverlap = GetMinVerticalOverlap(other);

            bool playerCollidedWithEnemy = mLayer == ColliderLayer::Player && other->mLayer == ColliderLayer::Enemy;
            bool enemyCollidedWithPlayer = mLayer == ColliderLayer::Enemy && other->mLayer == ColliderLayer::Player;
            bool canDecelerateOnCollision = playerCollidedWithEnemy || enemyCollidedWithPlayer;

            if (std::abs(minXOverlap) < std::abs(minYOverlap))
            {
                if (canDecelerateOnCollision)
                {
                    ResolveHorizontalCollisions(rigidBody, minXOverlap);
                }

                mOwner->OnHorizontalCollision(minXOverlap, other);
                other->GetOwner()->OnHorizontalCollision(-minXOverlap, this);
            }
            else
            {
                if (canDecelerateOnCollision)
                {
                    ResolveVerticalCollisions(rigidBody, minYOverlap);
                }

                mOwner->OnVerticalCollision(minYOverlap, other);
                other->GetOwner()->OnVerticalCollision(-minYOverlap, this);
            }
        }
    }
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{
    Vector2 pos = mOwner->GetPosition();
    pos.x += minXOverlap;
    mOwner->SetPosition(pos);

    Vector2 vel = rigidBody->GetVelocity();
    vel.x = 0.0f;
    rigidBody->SetVelocity(vel);
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
    Vector2 pos = mOwner->GetPosition();
    pos.y += minYOverlap;
    mOwner->SetPosition(pos);

    Vector2 vel = rigidBody->GetVelocity();
    vel.y = 0.0f;
    rigidBody->SetVelocity(vel);
}

void AABBColliderComponent::DebugDraw(class Renderer *renderer)
{
    renderer->DrawRect(mOwner->GetPosition() + mOffset,Vector2((float)mWidth, (float)mHeight), mOwner->GetRotation(),
                       Vector4(Color::Green.x, Color::Green.y, Color::Green.z, 1.0f), mOwner->GetGame()->GetCameraPos(), RendererMode::LINES);
}