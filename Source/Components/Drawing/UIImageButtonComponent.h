#pragma once
#include "../../Actors/Actor.h"
#include <string>
#include <functional>
#include "../../Math.h"

class UIImageButtonComponent : public Actor
{
public:
    UIImageButtonComponent(class Game* game, const std::string& texturePath, Vector2 position, std::function<void()> onClick, int width, int height, int drawOrder = 100);

    void SetSelected(bool selected);
    void Click();

    class StaticSpriteComponent* GetSprite() const { return mSprite; }

private:
    std::function<void()> mOnClick;
    class StaticSpriteComponent* mSprite;
    float mOriginalScale;
};