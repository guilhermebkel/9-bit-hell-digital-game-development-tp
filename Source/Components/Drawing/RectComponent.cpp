#include "RectComponent.h"
#include "../../Renderer/Renderer.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

RectComponent::RectComponent(class Actor* owner, int width, int height, RendererMode mode, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mMode(mode)
    , mWidth(width)
    , mHeight(height)
{

}

RectComponent::~RectComponent()
{

}

void RectComponent::Draw(class Renderer *renderer)
{
    if(mIsVisible)
    {
        renderer->DrawRect(mOwner->GetPosition(), Vector2(mWidth, mHeight), mOwner->GetRotation(),
                           Vector4(mColor.x, mColor.y, mColor.z, 1.0f), GetGame()->GetCameraPos(), mMode);
    }
}