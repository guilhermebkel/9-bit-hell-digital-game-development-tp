#include "UIStatWidget.h"
#include "../Game.h"
#include "Actor.h"
#include "HUD.h"
#include "../Components/Drawing/UITextComponent.h"
#include "../Components/Drawing/StaticSpriteComponent.h"

UIStatWidget::UIStatWidget(Game* game, const std::string& label, int pointSize, int drawOrder)
    : mGame(game)
    , mPosition(Vector2::Zero)
    , mAlignment(HAlign::Left)
    , mPointSize(pointSize)
    , mIconActor(nullptr)
    , mIconSprite(nullptr)
    , mIconSize(Vector2::Zero)
    , mDrawOrder(drawOrder)
    , mIconValueLeftAligned(false)
    , mValueOffsetX(0.0f)
{
    mLabelActor = new Actor(mGame);
    mLabelText = new UITextComponent(mLabelActor, drawOrder);
    mLabelText->SetText(label, Color::White, mPointSize);

    mValueActor = new Actor(mGame);
    mValueText = new UITextComponent(mValueActor, drawOrder);
    mValueText->SetText("", Color::White, mPointSize);
}

UIStatWidget::~UIStatWidget()
{
    mLabelActor->SetState(ActorState::Destroy);
    mValueActor->SetState(ActorState::Destroy);
    if (mIconActor)
    {
        mIconActor->SetState(ActorState::Destroy);
    }
}

void UIStatWidget::SetPosition(const Vector2& position, HAlign align)
{
    mPosition = position;
    mAlignment = align;
    UpdatePositions();
}

void UIStatWidget::SetValue(const std::string& value)
{
    if (mValueText->GetText() != value)
    {
        mValueText->SetText(value, Color::White, mPointSize);

        UpdatePositions();
    }
}

void UIStatWidget::SetOutline(bool enabled, const Vector3& color, float size)
{
}

void UIStatWidget::UpdatePositions()
{
    float labelWidth = mLabelText->GetTexture() ? mLabelText->GetTexture()->GetWidth() : 0.0f;
    float valueWidth = mValueText->GetTexture() ? mValueText->GetTexture()->GetWidth() : 0.0f;
    float labelHeight = mLabelText->GetTexture() ? mLabelText->GetTexture()->GetHeight() : 0.0f;

    Vector2 labelPos;
    switch (mAlignment)
    {
        case HAlign::Left:
            labelPos = Vector2(mPosition.x, mPosition.y);
            break;
        case HAlign::Right:
        case HAlign::Center:
            labelPos = Vector2(mPosition.x - (labelWidth / 2.0f), mPosition.y);
            break;
    }

    mLabelActor->SetPosition(labelPos);

    if (!mIconValueLeftAligned)
    {
        Vector2 valuePos;
        switch (mAlignment)
        {
            case HAlign::Left:
                valuePos = Vector2(mPosition.x, mPosition.y + labelHeight + mVerticalSpacing);
                break;
            case HAlign::Right:
            case HAlign::Center:
                valuePos = Vector2(mPosition.x - (valueWidth / 2.0f), mPosition.y + labelHeight + mVerticalSpacing);
                break;
        }

        valuePos.x += mValueOffsetX;
        mValueActor->SetPosition(valuePos);

        if (mIconActor && mIconSprite)
        {
            Vector2 iconPos = valuePos;
            iconPos.x -= (valueWidth / 2.0f + (mIconSize.x / 2.0f) + mIconSpacing);
            iconPos.x += mValueOffsetX;
            mIconActor->SetPosition(iconPos);
            mIconActor->SetScale(Vector2(1.0f, 1.0f));
        }
    }
    else
    {
        float groupWidth = valueWidth;
        float iconGroupWidth = (mIconSprite ? (mIconSize.x + mIconSpacing) : 0.0f);
        groupWidth += iconGroupWidth;

        float groupStartX = mPosition.x;
        switch (mAlignment)
        {
            case HAlign::Left:
                groupStartX = mPosition.x;
                break;
            case HAlign::Center:
                groupStartX = mPosition.x - (groupWidth / 2.0f);
                break;
            case HAlign::Right:
                groupStartX = mPosition.x - groupWidth;
                break;
        }

        Vector2 valuePos = Vector2(groupStartX + iconGroupWidth + (valueWidth / 2.0f) + mValueOffsetX, mPosition.y + labelHeight + mVerticalSpacing);
        mValueActor->SetPosition(valuePos);

        if (mIconActor && mIconSprite)
        {
            float iconCenterX = groupStartX + (mIconSize.x / 2.0f);
            iconCenterX += mValueOffsetX;
            mIconActor->SetPosition(Vector2(iconCenterX, valuePos.y));
            mIconActor->SetScale(Vector2(1.0f, 1.0f));
        }
    }
}

void UIStatWidget::SetIcon(const std::string& texturePath, const Vector2& size)
{
    if (mIconActor)
    {
        mIconActor->SetState(ActorState::Destroy);
        mIconActor = nullptr;
        mIconSprite = nullptr;
    }

    mIconActor = new Actor(mGame);
    mIconSprite = new StaticSpriteComponent(mIconActor, texturePath, static_cast<int>(size.x), static_cast<int>(size.y), mDrawOrder);
    mIconSize = size;

    UpdatePositions();
}

void UIStatWidget::SetValueOffsetX(float offset)
{
    if (mValueOffsetX != offset)
    {
        mValueOffsetX = offset;
        UpdatePositions();
    }
}

void UIStatWidget::SetIconValueLeftAligned(bool leftAligned)
{
    if (mIconValueLeftAligned != leftAligned)
    {
        mIconValueLeftAligned = leftAligned;
        UpdatePositions();
    }
}