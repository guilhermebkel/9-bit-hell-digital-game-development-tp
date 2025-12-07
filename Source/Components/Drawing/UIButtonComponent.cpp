#include "UIButtonComponent.h"

#include "StaticSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "UITextComponent.h"

UIButtonComponent::UIButtonComponent(Actor* owner, const std::string& text, Vector2 size, std::function<void()> onClick, int drawOrder, const std::string& iconPath, Vector2 iconSize)
    : DrawComponent(owner, drawOrder - 1)
    , mOnClick(std::move(onClick))
    , mSize(size)
    , mSelected(false)
    , mIconSprite(nullptr)
    , mIconActor(nullptr)
    , mIconSize(iconSize)
{
    mTextActor = new Actor(owner->GetGame());
    mTextComponent = new UITextComponent(mTextActor, drawOrder + 1);
    mTextComponent->SetText(text, Color::White, 24);

    if (!iconPath.empty())
    {
        mIconActor = new Actor(owner->GetGame());
        mIconSprite = new StaticSpriteComponent(mIconActor, iconPath, static_cast<int>(iconSize.x), static_cast<int>(iconSize.y), drawOrder + 1);
        mIconSprite->SetColor(Vector3(0.5f, 0.0f, 0.0f));
    }
}

UIButtonComponent::~UIButtonComponent()
{
    mTextActor->SetState(ActorState::Destroy);
}

void UIButtonComponent::Draw(class Renderer* renderer)
{
    if (mSelected)
    {
        Vector4 highlightColor(1.0f, 0.0f, 0.0f, 0.25f);
        renderer->DrawRect(mOwner->GetPosition(), mSize, 0.0f, highlightColor, Vector2::Zero, RendererMode::TRIANGLES);
    }

    UpdatePositions();
}

void UIButtonComponent::SetText(const std::string& text)
{
    mTextComponent->SetText(text, Color::White, 24);
}

void UIButtonComponent::UpdatePositions()
{
    Vector2 center = mOwner->GetPosition();

    if (mIconSprite)
    {
        float textWidth = 0.0f;
        if (mTextComponent->GetTexture())
        {
            textWidth = static_cast<float>(mTextComponent->GetTexture()->GetWidth());
        }
        else
        {
            textWidth = mTextComponent->GetText().length() * 12.0f;
        }

        float spacing = 24.0f;
        float totalWidth = mIconSize.x + spacing + textWidth;

        float startX = center.x - (totalWidth / 2.0f);

        mIconActor->SetPosition(Vector2(startX + (mIconSize.x / 2.0f), center.y));

        mTextActor->SetPosition(Vector2(startX + mIconSize.x + spacing + (textWidth / 2.0f), center.y));
    }
    else
    {
        mTextActor->SetPosition(center);
    }
}

void UIButtonComponent::Click()
{
    if (mOnClick)
    {
        mOnClick();
    }
}