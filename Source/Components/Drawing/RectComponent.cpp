#include "RectComponent.h"
#include "../../Renderer/Renderer.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

RectComponent::RectComponent(class Actor* owner, int width, int height, RendererMode mode, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mMode(mode)
    , mWidth(width)
    , mHeight(height)
    , mColor(1.0f, 1.0f, 1.0f, 1.0f)
{

}

RectComponent::~RectComponent()
{

}

void RectComponent::Draw(class Renderer *renderer)
{
    if(mIsVisible)
    {
        Vector2 finalSize(mWidth * mOwner->GetScale().x, mHeight * mOwner->GetScale().y);

        renderer->DrawRect(mOwner->GetPosition(), finalSize, mOwner->GetRotation(),
                           mColor, GetGame()->GetCameraPos(), mMode);
    }
}