#pragma once
#include "../Math.h"

class UIHealthBarWidget
{
public:
    UIHealthBarWidget(class Game* game, const Vector2& position, const Vector2& size, int drawOrder = 200,
        const Vector4& fillColor = Vector4(109.0f / 255.0f, 21.0f / 255.0f, 21.0f / 255.0f, 1.0f));
    ~UIHealthBarWidget();

    void Update(float currentValue, float maxValue);
    void SetPosition(const Vector2& position);
    void SetVisible(bool visible);
    void SetFillColor(const Vector4& color);

private:
    class Actor* mFillTopActor;
    class Actor* mFillBottomActor;
    class Actor* mFrameActor;
    class Actor* mEdgeLeftActor;
    class Actor* mEdgeRightActor;
    class Actor* mTopEdgeActor;
    class Actor* mBottomEdgeActor;
        // Full background bar drawn at full size so there's always a base behind scaled fills
        class Actor* mBackgroundActor;

    // Back/underlapping fills to avoid visual gaps when scaled small
    class Actor* mFillTopBackActor;
    class Actor* mFillBottomBackActor;

    class RectComponent* mFillTopRect;
    class RectComponent* mFillBottomRect;
    class RectComponent* mEdgeLeftRect;
    class RectComponent* mEdgeRightRect;
    class RectComponent* mTopEdgeRect;
    class RectComponent* mBottomEdgeRect;
        class RectComponent* mBackgroundRect;

    class RectComponent* mFillTopBackRect;
    class RectComponent* mFillBottomBackRect;

    Vector2 mPosition;
    Vector2 mSize;
    Vector4 mFillColor;
};