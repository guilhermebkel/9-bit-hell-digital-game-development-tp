#include "UIHealthBarWidget.h"
#include "../Game.h"
#include "Actor.h"
#include "HUD.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/StaticSpriteComponent.h"

namespace
{
    constexpr float HealthBarSpriteWidth = 128.0f;
    constexpr float HealthBarSpriteHeight = 16.0f;
}

UIHealthBarWidget::UIHealthBarWidget(Game* game, const Vector2& position, const Vector2& size, int drawOrder,
    const Vector4& fillColor)
    : mPosition(position)
    , mSize(size)
    , mFillColor(fillColor)
{
    mSize.x = Math::Max(mSize.x, HealthBarSpriteWidth);
    mSize.y = Math::Max(mSize.y, HealthBarSpriteHeight);

    mFrameActor = new Actor(game);
    mFrameActor->SetPosition(mPosition);
    new StaticSpriteComponent(mFrameActor, "../Assets/HealthBarEmpty.png", mSize.x, mSize.y, drawOrder);

    mFillActor = new Actor(game);
    mFillActor->SetPosition(mPosition);
    mFillRect = new RectComponent(mFillActor, static_cast<int>(mSize.x - 8.0f), static_cast<int>(mSize.y - 8.0f), RendererMode::TRIANGLES, drawOrder + 1);
    mFillRect->SetColor(mFillColor);
}

UIHealthBarWidget::~UIHealthBarWidget()
{
    mFillActor->SetState(ActorState::Destroy);
    mFrameActor->SetState(ActorState::Destroy);
}

void UIHealthBarWidget::Update(float currentValue, float maxValue)
{
    float percent = (maxValue > 0) ? (currentValue / maxValue) : 0.0f;
    percent = Math::Clamp(percent, 0.0f, 1.0f);

    mFillActor->SetScale(Vector2(percent, 1.0f));

    float fillWidth = mSize.x * percent;
    float newX = mPosition.x - (mSize.x - fillWidth) / 2.0f;
    
    mFillActor->SetPosition(Vector2(newX, mPosition.y));
}

void UIHealthBarWidget::SetPosition(const Vector2& position)
{
    mPosition = position;
    mFrameActor->SetPosition(mPosition);
}

void UIHealthBarWidget::SetVisible(bool visible)
{
    ActorState newState = visible ? ActorState::Active : ActorState::Paused;
    mFillActor->SetState(newState);
    mFrameActor->SetState(newState);
}

void UIHealthBarWidget::SetFillColor(const Vector4& color)
{
    mFillColor = color;
    if (mFillRect)
    {
        mFillRect->SetColor(mFillColor);
    }
}