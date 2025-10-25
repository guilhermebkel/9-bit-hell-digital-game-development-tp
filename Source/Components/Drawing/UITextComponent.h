#pragma once
#include "DrawComponent.h"
#include <string>
#include "../../Math.h"

class UITextComponent : public DrawComponent
{
public:
    UITextComponent(class Actor* owner, int drawOrder = 200);
    ~UITextComponent() override;

    void Draw(class Renderer* renderer) override;

    void SetText(const std::string& text, const Vector3& color = Color::White, int pointSize = 24);
    std::string GetText() { return mText; };

    class Texture* GetTexture() const { return mTexture; }

protected:
    void UpdateTexture();

    std::string mText;
    class Font* mFont;
    class Texture* mTexture;
    Vector3 mColor;
    int mPointSize;
};