#include "ShapeComponent.h"
#include "RectComponent.h"
#include "../../Actors/Actor.h"

ShapeComponent::ShapeComponent(Actor* owner, int drawOrder)
    : DrawComponent(owner, drawOrder)
{
}

ShapeComponent::~ShapeComponent()
{
    for (auto actor : mLineActors)
    {
        actor->SetState(ActorState::Destroy);
    }
    mLineActors.clear();
}

void ShapeComponent::Draw(Renderer* renderer)
{
}

void ShapeComponent::AddLine(const Vector2& start, const Vector2& end, float thickness)
{
    Vector2 diff = end - start;
    float length = diff.Length();

    if (length < 0.1f) return;

    float angle = Math::Atan2(diff.y, diff.x);
    Vector2 center = start + (diff * 0.5f);

    Actor* lineActor = new Actor(mOwner->GetGame());
    lineActor->SetPosition(center);
    lineActor->SetRotation(angle);

    auto* rect = new RectComponent(
        lineActor,
        static_cast<int>(length),
        static_cast<int>(thickness),
        RendererMode::TRIANGLES,
        mDrawOrder
    );

    rect->SetColor(Vector4(mColor.x, mColor.y, mColor.z, 1.0f));

    mLineActors.push_back(lineActor);
}

void ShapeComponent::SetColor(const Vector3& color)
{
    mColor = color;
    Vector4 colorV4(color.x, color.y, color.z, 1.0f);

    for (auto actor : mLineActors)
    {
        auto* rect = actor->GetComponent<RectComponent>();
        if (rect)
        {
            rect->SetColor(colorV4);
        }
    }
}