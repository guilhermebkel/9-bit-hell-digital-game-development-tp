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
    , mPriceTextComponent(nullptr)
    , mPriceTextActor(nullptr)
    , mPriceIconSprite(nullptr)
    , mPriceIconActor(nullptr)
    , mPriceIconSize(Vector2::Zero)
    , mTextPointSize(textPointSize)
    , mHighlightSize(size)
    , mDrawOrder(drawOrder)
    , mPriceSpacing(8.0f)
    , mPriceMargin(16.0f)
    , mMainContentLeftAligned(false)
    , mAffordable(true)
    , mDisabledColor(Vector3(0.6f, 0.6f, 0.6f))
    , mTextColor(Color::White)
    , mIconBaseColor(Vector3::One)
    , mPriceIconBaseColor(Vector3::One)
{
    mTextActor = new Actor(owner->GetGame());
    mTextComponent = new UITextComponent(mTextActor, drawOrder + 1);
    mTextComponent->SetText(text, Color::White, mTextPointSize);

    if (!iconPath.empty())
    {
        mIconActor = new Actor(owner->GetGame());
        mIconSprite = new StaticSpriteComponent(mIconActor, iconPath, static_cast<int>(iconSize.x), static_cast<int>(iconSize.y), drawOrder + 1);
        Vector3 iconColor(0.5f, 0.0f, 0.0f);
        mIconSprite->SetColor(iconColor);
        mIconBaseColor = iconColor;
    }
    UpdateAffordabilityAppearance();
}

UIButtonComponent::~UIButtonComponent()
{
    mTextActor->SetState(ActorState::Destroy);
    if (mPriceTextActor)
    {
        mPriceTextActor->SetState(ActorState::Destroy);
    }
    if (mPriceIconActor)
    {
        mPriceIconActor->SetState(ActorState::Destroy);
    }
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
    mTextComponent->SetText(text, GetTextColor(), mTextPointSize);
}

void UIButtonComponent::SetPrice(int value)
{
    if (!mPriceTextActor)
    {
        mPriceTextActor = new Actor(mOwner->GetGame());
        mPriceTextComponent = new UITextComponent(mPriceTextActor, mDrawOrder + 1);
    }

    mPriceTextComponent->SetText(std::to_string(value), GetTextColor(), mTextPointSize);
    UpdatePositions();
}

void UIButtonComponent::SetPriceIcon(const std::string& iconPath, Vector2 size)
{
    if (mPriceIconActor)
    {
        mPriceIconActor->SetState(ActorState::Destroy);
        mPriceIconActor = nullptr;
        mPriceIconSprite = nullptr;
    }

    mPriceIconActor = new Actor(mOwner->GetGame());
    mPriceIconSprite = new StaticSpriteComponent(mPriceIconActor, iconPath, static_cast<int>(size.x), static_cast<int>(size.y), mDrawOrder + 1);
    mPriceIconSize = size;
    Vector3 priceIconColor = Vector3::One;
    mPriceIconSprite->SetColor(priceIconColor);
    mPriceIconBaseColor = priceIconColor;
    UpdatePositions();
    UpdateAffordabilityAppearance();
}

void UIButtonComponent::SetMainContentLeftAligned(bool leftAligned)
{
    if (mMainContentLeftAligned != leftAligned)
    {
        mMainContentLeftAligned = leftAligned;
        UpdatePositions();
    }
}

void UIButtonComponent::UpdatePositions()
{
    Vector2 center = mOwner->GetPosition();

    float textWidth = GetTextWidth();
    float baseContentWidth = textWidth;
    float spacing = 24.0f;
    float iconContentWidth = 0.0f;

    if (mIconSprite)
    {
        iconContentWidth = mIconSize.x + spacing;
        baseContentWidth += iconContentWidth;
    }

    float priceTextWidth = 0.0f;
    if (mPriceTextComponent && !mPriceTextComponent->GetText().empty() && mPriceTextComponent->GetTexture())
    {
        priceTextWidth = static_cast<float>(mPriceTextComponent->GetTexture()->GetWidth());
    }

    float priceIconContent = 0.0f;
    if (mPriceIconSprite && mPriceIconSize.x > 0.0f)
    {
        priceIconContent = mPriceIconSize.x + (priceTextWidth > 0.0f ? mPriceSpacing : 0.0f);
    }

    float priceGroupWidth = priceTextWidth + priceIconContent;

    float highlightWidth = mSize.x;
    float highlightHeight = Math::Max(mSize.y, static_cast<float>(mTextPointSize) + 24.0f);
    mHighlightSize = Vector2(highlightWidth, highlightHeight);

    float startX = center.x - (highlightWidth / 2.0f);

    float mainAreaWidth;
    if (priceGroupWidth > 0.0f)
    {
        mainAreaWidth = highlightWidth - priceGroupWidth - mPriceMargin;
    }
    else
    {
        mainAreaWidth = highlightWidth;
    }

    if (mainAreaWidth < baseContentWidth)
    {
        mainAreaWidth = baseContentWidth;
    }

    float mainStartX = startX + (priceGroupWidth > 0.0f ? mPriceMargin : 0.0f);
    if (!mMainContentLeftAligned)
    {
        mainStartX += (mainAreaWidth - baseContentWidth) * 0.5f;
    }

    if (mIconSprite)
    {
        float iconCenterX = mainStartX + (mIconSize.x / 2.0f);
        mIconActor->SetPosition(Vector2(iconCenterX, center.y));

        float textCenterX = mainStartX + iconContentWidth + (textWidth / 2.0f);
        mTextActor->SetPosition(Vector2(textCenterX, center.y));
    }
    else
    {
        float textCenterX = mainStartX + (textWidth / 2.0f);
        mTextActor->SetPosition(Vector2(textCenterX, center.y));
    }

    if (priceGroupWidth > 0.0f && mPriceTextActor)
    {
        float priceCenterX = startX + highlightWidth - mPriceMargin - (priceGroupWidth / 2.0f);

        if (mPriceIconSprite)
        {
            float iconCenterX = priceCenterX - (priceGroupWidth / 2.0f) + (mPriceIconSize.x / 2.0f);
            mPriceIconActor->SetPosition(Vector2(iconCenterX, center.y));

            float textCenterX = iconCenterX + (mPriceIconSize.x / 2.0f) + mPriceSpacing + (priceTextWidth / 2.0f);
            mPriceTextActor->SetPosition(Vector2(textCenterX, center.y));
        }
        else
        {
            mPriceTextActor->SetPosition(Vector2(priceCenterX, center.y));
        }
    }
}

void UIButtonComponent::Click()
{
    if (mOnClick)
    {
        mOnClick();
    }
}

void UIButtonComponent::SetAffordable(bool affordable)
{
    if (mAffordable == affordable)
    {
        return;
    }

    mAffordable = affordable;
    UpdateAffordabilityAppearance();
}

void UIButtonComponent::UpdateAffordabilityAppearance()
{
    Vector3 textColor = GetTextColor();

    if (mTextComponent)
    {
        mTextComponent->SetColor(textColor);
    }

    if (mPriceTextComponent)
    {
        mPriceTextComponent->SetColor(textColor);
    }

    if (mIconSprite)
    {
        Vector3 iconColor = mAffordable ? mIconBaseColor : mDisabledColor;
        mIconSprite->SetColor(iconColor);
    }

    if (mPriceIconSprite)
    {
        Vector3 priceIconColor = mAffordable ? mPriceIconBaseColor : mDisabledColor;
        mPriceIconSprite->SetColor(priceIconColor);
    }
}

Vector3 UIButtonComponent::GetTextColor() const
{
    return mAffordable ? mTextColor : mDisabledColor;
}