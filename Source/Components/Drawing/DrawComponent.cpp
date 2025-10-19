#include "DrawComponent.h"
#include "../../Game.h"
#include "../../Actors/Actor.h"

DrawComponent::DrawComponent(class Actor *owner, int drawOrder)
        : Component(owner)
        , mDrawOrder(drawOrder)
        , mIsVisible(true)
        , mColor(Vector3::One)
{
    mOwner->GetGame()->AddDrawable(this);
}

DrawComponent::~DrawComponent()
{
    mOwner->GetGame()->RemoveDrawable(this);
}

void DrawComponent::Draw(Renderer *renderer)
{

}

void DrawComponent::SetDrawOrder(int drawOrder)
{
    mDrawOrder = drawOrder;
    mOwner->GetGame()->RequestDrawSort();
}
