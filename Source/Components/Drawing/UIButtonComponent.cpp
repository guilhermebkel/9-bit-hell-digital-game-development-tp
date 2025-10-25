#include "UIButtonComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "UITextComponent.h"

UIButtonComponent::UIButtonComponent(Actor* owner, const std::string& text, Vector2 size, std::function<void()> onClick, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mOnClick(std::move(onClick))
    , mSize(size)
    , mSelected(false)
{
    mTextActor = new Actor(owner->GetGame());
    mTextComponent = new UITextComponent(mTextActor, drawOrder);
    mTextComponent->SetText(text, Color::White, 24);
}

UIButtonComponent::~UIButtonComponent()
{
    mTextActor->SetState(ActorState::Destroy);
}

void UIButtonComponent::Draw(class Renderer* renderer)
{
    if (mSelected)
    {
        Vector4 highlightColor(1.0f, 1.0f, 0.0f, 0.25f);
        renderer->DrawRect(mOwner->GetPosition(), mSize, 0.0f, highlightColor, Vector2::Zero, RendererMode::TRIANGLES);
    }

    mTextActor->SetPosition(mOwner->GetPosition());
}

void UIButtonComponent::Click()
{
    if (mOnClick)
    {
        mOnClick();
    }
}