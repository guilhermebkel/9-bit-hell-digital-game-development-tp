#include "UIButtonComponent.h"

#include "StaticSpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "UITextComponent.h"

UIButtonComponent::UIButtonComponent(Actor* owner, const std::string& text, Vector2 size, std::function<void()> onClick, int drawOrder, int textPointSize, const std::string& iconPath, Vector2 iconSize)
    : DrawComponent(owner, drawOrder - 1)
    , mOnClick(std::move(onClick))
    , mSize(size)
    , mSelected(false)
    , mIconSprite(nullptr)
    , mIconActor(nullptr)
    , mIconSize(iconSize)
    , mTextPointSize(textPointSize)
    , mHighlightSize(size)
{
    mTextActor = new Actor(owner->GetGame());
    mTextComponent = new UITextComponent(mTextActor, drawOrder + 1);
    mTextComponent->SetText(text, Color::White, mTextPointSize);

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

float UIButtonComponent::GetTextWidth() const
{
    if (mTextComponent)
    {
        if (mTextComponent->GetTexture())
        {
            return static_cast<float>(mTextComponent->GetTexture()->GetWidth());
        }
        return mTextComponent->GetText().length() * (static_cast<float>(mTextPointSize) * 0.5f);
    }
    return 0.0f;
}

void UIButtonComponent::Draw(class Renderer* renderer)
{
    UpdatePositions();

    if (mSelected)
    {
        Vector4 highlightColor(1.0f, 0.0f, 0.0f, 0.25f);
        renderer->DrawRect(mOwner->GetPosition(), mHighlightSize, 0.0f, highlightColor, Vector2::Zero, RendererMode::TRIANGLES);
    }
}

void UIButtonComponent::SetText(const std::string& text)
{
    mTextComponent->SetText(text, Color::White, mTextPointSize);
}

void UIButtonComponent::UpdatePositions()
{
    Vector2 center = mOwner->GetPosition();

    float textWidth = GetTextWidth();

    if (mIconSprite)
    {
        float spacing = 24.0f;
        float totalWidth = mIconSize.x + spacing + textWidth;
        float highlightWidth = Math::Max(mSize.x, totalWidth + 32.0f);
        float highlightHeight = Math::Max(mSize.y, static_cast<float>(mTextPointSize) + 24.0f);
        mHighlightSize = Vector2(highlightWidth, highlightHeight);

        float startX = center.x - (totalWidth / 2.0f);

        mIconActor->SetPosition(Vector2(startX + (mIconSize.x / 2.0f), center.y));

        mTextActor->SetPosition(Vector2(startX + mIconSize.x + spacing + (textWidth / 2.0f), center.y));
    }
    else
    {
        float highlightWidth = Math::Max(mSize.x, textWidth + 32.0f);
        float highlightHeight = Math::Max(mSize.y, static_cast<float>(mTextPointSize) + 24.0f);
        mHighlightSize = Vector2(highlightWidth, highlightHeight);

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