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
        SetScale(Vector2(1.3f, 1.3f));
        mSprite->SetColor(Vector3(1.0f, 1.0f, 1.0f));
    }
    else
    {
        SetScale(Vector2(1.0f, 1.0f));
        mSprite->SetColor(Vector3(1.0f, 1.0f, 1.0f)); 
    }
}

void UIImageButtonComponent::Click()
{
    if (mOnClick)
    {
        mOnClick();
    }
}