#include "UIStatWidget.h"
#include "../Game.h"
#include "Actor.h"
#include "HUD.h"
#include "../Components/Drawing/UITextComponent.h"

UIStatWidget::UIStatWidget(Game* game, const std::string& label, int pointSize)
    : mGame(game)
    , mPosition(Vector2::Zero)
    , mAlignment(HAlign::Left)
    , mPointSize(pointSize)
{
    mLabelActor = new Actor(mGame);
    mLabelText = new UITextComponent(mLabelActor, HUD::DRAW_ORDER);
    mLabelText->SetText(label, Color::White, mPointSize);

    mValueActor = new Actor(mGame);
    mValueText = new UITextComponent(mValueActor, HUD::DRAW_ORDER);
    mValueText->SetText("", Color::White, mPointSize);
}

UIStatWidget::~UIStatWidget()
{
    mLabelActor->SetState(ActorState::Destroy);
    mValueActor->SetState(ActorState::Destroy);
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

        if (mAlignment == HAlign::Right)
        {
            UpdatePositions();
        }
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

    Vector2 labelPos, valuePos;

    switch (mAlignment)
    {
        case HAlign::Left:
            labelPos = Vector2(mPosition.x + (labelWidth / 2.0f), mPosition.y);
            valuePos = Vector2(mPosition.x + (valueWidth / 2.0f) + 6.0f, mPosition.y + labelHeight + mVerticalSpacing);
            break;
        case HAlign::Right:
            labelPos = Vector2(mPosition.x - (labelWidth / 2.0f), mPosition.y);
            valuePos = Vector2(mPosition.x - (valueWidth / 2.0f), mPosition.y + labelHeight + mVerticalSpacing);
            break;
        case HAlign::Center:
            labelPos = Vector2(mPosition.x - (labelWidth / 2.0f), mPosition.y);
            valuePos = Vector2(mPosition.x - (valueWidth / 2.0f), mPosition.y + labelHeight + mVerticalSpacing);
            break;
    }
    
    mLabelActor->SetPosition(labelPos);
    mValueActor->SetPosition(valuePos);
}