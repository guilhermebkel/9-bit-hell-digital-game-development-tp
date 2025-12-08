#include "UIImageButtonComponent.h"
#include "../../Components/Drawing/StaticSpriteComponent.h"

UIImageButtonComponent::UIImageButtonComponent(Game* game, const std::string& texturePath, Vector2 position, std::function<void()> onClick, int width, int height, int drawOrder)
    : Actor(game)
    , mOnClick(std::move(onClick))
    , mOriginalScale(1.0f)
{
    SetPosition(position);

    mSprite = new StaticSpriteComponent(this, texturePath, width, height, drawOrder);
}

void UIImageButtonComponent::SetSelected(bool selected)
{
    if (selected)
    {
        SetScale(Vector2(4.8f, 4.8f));
    }
    else
    {
        SetScale(Vector2(4.0f, 4.0f));
    }
}

void UIImageButtonComponent::Click()
{
    if (mOnClick)
    {
        mOnClick();
    }
}