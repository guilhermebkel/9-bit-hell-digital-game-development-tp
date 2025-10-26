#pragma once
#include "../Math.h"

class UIHealthBarWidget
{
public:
    UIHealthBarWidget(class Game* game, const Vector2& position, const Vector2& size, int drawOrder = 200);
    ~UIHealthBarWidget();

    void Update(float currentValue, float maxValue);
    void SetVisible(bool visible);

private:
    class Actor* mFillActor;
    class Actor* mFrameActor;

    class RectComponent* mFillRect;

    Vector2 mPosition;
    Vector2 mSize;
};