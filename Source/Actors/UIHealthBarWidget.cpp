#include "UIHealthBarWidget.h"
#include "../Game.h"
#include "Actor.h"
#include "HUD.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/StaticSpriteComponent.h"
#include "CorruptionOverlay.h"

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

    // Ensure health bar UI is drawn above the corruption overlay
    int overlayOrder = static_cast<int>(CorruptionOverlay::DRAW_ORDER);
    int frameOrder = overlayOrder + 10; // frame on top
    // draw order from top (foreground) -> bottom (background): frame, top edge, top fill, bottom fill, side edges, backfills, background
    int topEdgeOrder = frameOrder - 1;        // top edge just below frame
    int topFillOrder = frameOrder - 2;        // top fill drawn under top edge but above bottom fill
    int bottomFillOrder = frameOrder - 3;     // bottom fill drawn below top fill
    int edgeOrder = frameOrder - 4;           // side edges drawn below fills
    int backFillOrder = frameOrder - 5;       // underlapping fills (below edges)
    int backgroundOrder = frameOrder - 6;     // background drawn first

    mFrameActor = new Actor(game);
    mFrameActor->SetPosition(mPosition);
    new StaticSpriteComponent(mFrameActor, Game::ResolvePath("Assets/HealthBarEmpty.png"), mSize.x, mSize.y, frameOrder);

    float fullFillWidth = mSize.x - 8.0f;
    float fullFillHeight = mSize.y - 8.0f;


    // Colors: base fill and a slightly lighter top color
    Vector4 baseColor = mFillColor;
    Vector4 topColor = Vector4(
        Math::Min(baseColor.x * 1.15f, 1.0f),
        Math::Min(baseColor.y * 1.15f, 1.0f),
        Math::Min(baseColor.z * 1.15f, 1.0f),
        baseColor.w);

    // Side edges darker than base
    Vector4 edgeColor = Vector4(baseColor.x * 0.6f, baseColor.y * 0.6f, baseColor.z * 0.6f, baseColor.w);
    // Top edge slightly lighter than topColor
    Vector4 topEdgeColor = Vector4(
        Math::Min(topColor.x * 1.08f, 1.0f),
        Math::Min(topColor.y * 1.08f, 1.0f),
        Math::Min(topColor.z * 1.08f, 1.0f),
        topColor.w);

    // Full background rect so there's always a base behind scaled fills
    Vector4 backgroundColor = Vector4(baseColor.x * 0.85f, baseColor.y * 0.85f, baseColor.z * 0.85f, baseColor.w);
    mBackgroundActor = new Actor(game);
    mBackgroundActor->SetPosition(mPosition);
    mBackgroundRect = new RectComponent(mBackgroundActor, static_cast<int>(fullFillWidth), static_cast<int>(fullFillHeight), RendererMode::TRIANGLES, backgroundOrder);
    mBackgroundRect->SetColor(backgroundColor);

    // Heights for bottom (40%) and top (60%) parts (kept for offsets if needed)
    float bottomHeight = fullFillHeight * 0.4f;
    float topHeight = fullFillHeight - bottomHeight;


    // Bottom fill actor (draw only bottomHeight so top can overlay)
    mFillBottomActor = new Actor(game);
    mFillBottomActor->SetPosition(Vector2(mPosition.x, mPosition.y + (topHeight / 2.0f)));
    mFillBottomRect = new RectComponent(mFillBottomActor, static_cast<int>(fullFillWidth), static_cast<int>(bottomHeight), RendererMode::TRIANGLES, bottomFillOrder);
    mFillBottomRect->SetColor(baseColor);

    // Back/underlapping bottom fill to prevent visual gaps when scaled small
    mFillBottomBackActor = new Actor(game);
    mFillBottomBackActor->SetPosition(Vector2(mPosition.x, mPosition.y));
    mFillBottomBackRect = new RectComponent(mFillBottomBackActor, static_cast<int>(fullFillWidth), static_cast<int>(fullFillHeight) + 1, RendererMode::TRIANGLES, backFillOrder);
    mFillBottomBackRect->SetColor(baseColor);

    // Top fill actor (draw only topHeight, positioned at top portion)
    mFillTopActor = new Actor(game);
    mFillTopActor->SetPosition(Vector2(mPosition.x, mPosition.y - (bottomHeight / 2.0f)));
    mFillTopRect = new RectComponent(mFillTopActor, static_cast<int>(fullFillWidth), static_cast<int>(topHeight), RendererMode::TRIANGLES, topFillOrder);
    mFillTopRect->SetColor(topColor);

    // Back/underlapping top fill to prevent visual gaps when scaled small
    mFillTopBackActor = new Actor(game);
    mFillTopBackActor->SetPosition(Vector2(mPosition.x, mPosition.y));
    mFillTopBackRect = new RectComponent(mFillTopBackActor, static_cast<int>(fullFillWidth), static_cast<int>(fullFillHeight) + 1, RendererMode::TRIANGLES, backFillOrder);
    mFillTopBackRect->SetColor(topColor);

    // Side edges (thin vertical lines) positioned relative to fill; position will be updated in Update()
    mEdgeLeftActor = new Actor(game);
    mEdgeLeftRect = new RectComponent(mEdgeLeftActor, 4, static_cast<int>(fullFillHeight), RendererMode::TRIANGLES, edgeOrder);
    mEdgeLeftRect->SetColor(edgeColor);

    mEdgeRightActor = new Actor(game);
    mEdgeRightRect = new RectComponent(mEdgeRightActor, 4, static_cast<int>(fullFillHeight), RendererMode::TRIANGLES, edgeOrder);
    mEdgeRightRect->SetColor(edgeColor);

    // Top edge (thin horizontal line) slightly lighter
    mTopEdgeActor = new Actor(game);
    mTopEdgeRect = new RectComponent(mTopEdgeActor, static_cast<int>(fullFillWidth), 3, RendererMode::TRIANGLES, topEdgeOrder);
    mTopEdgeRect->SetColor(topEdgeColor);

    mBottomEdgeActor = new Actor(game);
    Vector4 bottomEdgeColor = Vector4(
        Math::Min(edgeColor.x * 1.12f, 1.0f),
        Math::Min(edgeColor.y * 1.12f, 1.0f),
        Math::Min(edgeColor.z * 1.12f, 1.0f),
        edgeColor.w);
    mBottomEdgeRect = new RectComponent(mBottomEdgeActor, static_cast<int>(fullFillWidth), 3, RendererMode::TRIANGLES, topEdgeOrder);
    mBottomEdgeRect->SetColor(bottomEdgeColor);
}

UIHealthBarWidget::~UIHealthBarWidget()
{
    if (mFillBottomActor) mFillBottomActor->SetState(ActorState::Destroy);
    if (mFillBottomBackActor) mFillBottomBackActor->SetState(ActorState::Destroy);
    if (mFillTopActor) mFillTopActor->SetState(ActorState::Destroy);
    if (mFillTopBackActor) mFillTopBackActor->SetState(ActorState::Destroy);
    if (mEdgeLeftActor) mEdgeLeftActor->SetState(ActorState::Destroy);
    if (mEdgeRightActor) mEdgeRightActor->SetState(ActorState::Destroy);
    if (mTopEdgeActor) mTopEdgeActor->SetState(ActorState::Destroy);
    if (mBottomEdgeActor) mBottomEdgeActor->SetState(ActorState::Destroy);
    if (mBackgroundActor) mBackgroundActor->SetState(ActorState::Destroy);
    if (mFrameActor) mFrameActor->SetState(ActorState::Destroy);
}

void UIHealthBarWidget::Update(float currentValue, float maxValue)
{
    float percent = (maxValue > 0) ? (currentValue / maxValue) : 0.0f;
    percent = Math::Clamp(percent, 0.0f, 1.0f);

    float fillWidth = (mSize.x - 8.0f) * percent;
    float fullFillWidth = (mSize.x - 8.0f);
    float fullFillHeight = (mSize.y - 8.0f);
    float bottomHeight = fullFillHeight * 0.4f;
    float topHeight = fullFillHeight - bottomHeight;

    // Center X for scaled fill (so left edge aligns correctly)
    float newX = mPosition.x - (fullFillWidth - fillWidth) / 2.0f;

    // Update top and bottom fills: scale in X, position centers
    if (mFillBottomActor)
    {
        mFillBottomActor->SetScale(Vector2(percent, 1.0f));
        mFillBottomActor->SetPosition(Vector2(newX, mPosition.y + (topHeight / 2.0f)));
    }

    if (mFillBottomBackActor)
    {
        mFillBottomBackActor->SetScale(Vector2(percent, 1.0f));
        mFillBottomBackActor->SetPosition(Vector2(newX, mPosition.y));
    }

    if (mFillTopActor)
    {
        mFillTopActor->SetScale(Vector2(percent, 1.0f));
        mFillTopActor->SetPosition(Vector2(newX, mPosition.y - (bottomHeight / 2.0f)));
    }

    if (mFillTopBackActor)
    {
        mFillTopBackActor->SetScale(Vector2(percent, 1.0f));
        mFillTopBackActor->SetPosition(Vector2(newX, mPosition.y));
    }

    // Update side edges positions based on left/right of current fill
    float leftX = newX - (fillWidth / 2.0f);
    float rightX = newX + (fillWidth / 2.0f);

    const int edgeThickness = 4;
    if (mEdgeLeftActor)
    {
        mEdgeLeftActor->SetPosition(Vector2(leftX + (edgeThickness / 2.0f), mPosition.y));
    }
    if (mEdgeRightActor)
    {
        mEdgeRightActor->SetPosition(Vector2(rightX - (edgeThickness / 2.0f), mPosition.y));
    }

    if (mTopEdgeActor)
    {
        float topY = mPosition.y - (fullFillHeight / 2.0f) + 1.0f;
        mTopEdgeActor->SetScale(Vector2(percent, 1.0f));
        mTopEdgeActor->SetPosition(Vector2(newX, topY));
    }

    if (mBottomEdgeActor)
    {
        float bottomY = mPosition.y + (fullFillHeight / 2.0f) - 1.0f;
        mBottomEdgeActor->SetScale(Vector2(percent, 1.0f));
        mBottomEdgeActor->SetPosition(Vector2(newX, bottomY));
    }

    if (mBackgroundActor)
    {
        mBackgroundActor->SetScale(Vector2(percent, 1.0f));
        mBackgroundActor->SetPosition(Vector2(newX, mPosition.y));
    }
}

void UIHealthBarWidget::SetPosition(const Vector2& position)
{
    mPosition = position;
    mFrameActor->SetPosition(mPosition);
    float fullFillHeight = (mSize.y - 8.0f);
    float bottomHeight = fullFillHeight * 0.4f;
    float topHeight = fullFillHeight - bottomHeight;
    if (mFillBottomActor) mFillBottomActor->SetPosition(Vector2(mPosition.x, mPosition.y + (topHeight / 2.0f)));
    if (mFillTopActor) mFillTopActor->SetPosition(Vector2(mPosition.x, mPosition.y - (bottomHeight / 2.0f)));
    if (mFillBottomBackActor) mFillBottomBackActor->SetPosition(Vector2(mPosition.x, mPosition.y));
    if (mFillTopBackActor) mFillTopBackActor->SetPosition(Vector2(mPosition.x, mPosition.y));
    if (mBackgroundActor) mBackgroundActor->SetPosition(Vector2(mPosition.x, mPosition.y));
    if (mEdgeLeftActor) mEdgeLeftActor->SetPosition(Vector2(mPosition.x - (mSize.x / 2.0f), mPosition.y));
    if (mEdgeRightActor) mEdgeRightActor->SetPosition(Vector2(mPosition.x + (mSize.x / 2.0f), mPosition.y));
    if (mTopEdgeActor) mTopEdgeActor->SetPosition(Vector2(mPosition.x, mPosition.y - (fullFillHeight / 2.0f) + 1.0f));
    if (mBottomEdgeActor) mBottomEdgeActor->SetPosition(Vector2(mPosition.x, mPosition.y + (fullFillHeight / 2.0f) - 1.0f));
}

void UIHealthBarWidget::SetVisible(bool visible)
{
    ActorState newState = visible ? ActorState::Active : ActorState::Paused;
    if (mFillBottomActor) mFillBottomActor->SetState(newState);
    if (mFillBottomBackActor) mFillBottomBackActor->SetState(newState);
    if (mFillTopActor) mFillTopActor->SetState(newState);
    if (mFillTopBackActor) mFillTopBackActor->SetState(newState);
    if (mBackgroundActor) mBackgroundActor->SetState(newState);
    if (mEdgeLeftActor) mEdgeLeftActor->SetState(newState);
    if (mEdgeRightActor) mEdgeRightActor->SetState(newState);
    if (mTopEdgeActor) mTopEdgeActor->SetState(newState);
    if (mBottomEdgeActor) mBottomEdgeActor->SetState(newState);
    if (mFrameActor) mFrameActor->SetState(newState);
}

void UIHealthBarWidget::SetFillColor(const Vector4& color)
{
    mFillColor = color;
    Vector4 baseColor = mFillColor;
    Vector4 topColor = Vector4(
        Math::Min(baseColor.x * 1.15f, 1.0f),
        Math::Min(baseColor.y * 1.15f, 1.0f),
        Math::Min(baseColor.z * 1.15f, 1.0f),
        baseColor.w);

    Vector4 edgeColor = Vector4(baseColor.x * 0.6f, baseColor.y * 0.6f, baseColor.z * 0.6f, baseColor.w);
    Vector4 topEdgeColor = Vector4(
        Math::Min(topColor.x * 1.08f, 1.0f),
        Math::Min(topColor.y * 1.08f, 1.0f),
        Math::Min(topColor.z * 1.08f, 1.0f),
        topColor.w);

    Vector4 bottomEdgeColor = Vector4(
        Math::Min(edgeColor.x * 1.12f, 1.0f),
        Math::Min(edgeColor.y * 1.12f, 1.0f),
        Math::Min(edgeColor.z * 1.12f, 1.0f),
        edgeColor.w);

    Vector4 backgroundColor = Vector4(baseColor.x * 0.85f, baseColor.y * 0.85f, baseColor.z * 0.85f, baseColor.w);

    if (mFillBottomRect) mFillBottomRect->SetColor(baseColor);
    if (mFillBottomBackRect) mFillBottomBackRect->SetColor(baseColor);
    if (mFillTopRect) mFillTopRect->SetColor(topColor);
    if (mFillTopBackRect) mFillTopBackRect->SetColor(topColor);
    if (mEdgeLeftRect) mEdgeLeftRect->SetColor(edgeColor);
    if (mEdgeRightRect) mEdgeRightRect->SetColor(edgeColor);
    if (mTopEdgeRect) mTopEdgeRect->SetColor(topEdgeColor);
    if (mBottomEdgeRect) mBottomEdgeRect->SetColor(bottomEdgeColor);
    if (mBackgroundRect) mBackgroundRect->SetColor(backgroundColor);
}
 