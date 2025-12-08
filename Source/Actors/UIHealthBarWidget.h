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
    class Actor* mFillActor;
    class Actor* mFrameActor;

    class RectComponent* mFillRect;

    Vector2 mPosition;
    Vector2 mSize;
    Vector4 mFillColor;
};