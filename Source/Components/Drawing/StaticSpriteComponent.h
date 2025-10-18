#pragma once
#include "DrawComponent.h"
#include <string>

class StaticSpriteComponent : public DrawComponent
{
public:
    StaticSpriteComponent(class Actor* owner, const std::string& texturePath, int width, int height, int drawOrder = 100);
    ~StaticSpriteComponent();

    void Draw(class Renderer* renderer) override;

protected:
    class Texture* mTexture;
    int mWidth;
    int mHeight;
};