#include "UITextComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Font.h"
#include "../../Renderer/Texture.h"
#include <SDL_log.h>

UITextComponent::UITextComponent(Actor* owner, int drawOrder)
    : DrawComponent(owner, drawOrder)
    , mFont(nullptr)
    , mTexture(nullptr)
    , mColor(Color::White)
    , mPointSize(24)
{
    mFont = new Font(GetGame());
    SetFont("../Assets/Fonts/Jersey10-Regular.ttf");
}

UITextComponent::~UITextComponent()
{
    if (mFont)
    {
        mFont->Unload();
        delete mFont;
    }
    if (mTexture)
    {
        delete mTexture;
    }
}

bool UITextComponent::SetFont(const std::string& fontPath)
{
    if (!mFont)
    {
        mFont = new Font(GetGame());
    }

    mFont->Unload();

    if (!mFont->Load(fontPath))
    {
        SDL_Log("Failed to load font %s", fontPath.c_str());
        return false;
    }

    UpdateTexture();
    return true;
}

void UITextComponent::SetText(const std::string& text, const Vector3& color, int pointSize)
{
    mText = text;
    mColor = color;
    mPointSize = pointSize;
    UpdateTexture();
}

void UITextComponent::SetColor(const Vector3& color)
{
    mColor = color;
    UpdateTexture();
}

void UITextComponent::UpdateTexture()
{
    if (mTexture)
    {
        delete mTexture;
        mTexture = nullptr;
    }

    if (mFont && !mText.empty())
    {
        mTexture = mFont->RenderText(mText, mColor, mPointSize);
    }
}

void UITextComponent::Draw(Renderer* renderer)
{
    if (mIsVisible && mTexture)
    {
        renderer->DrawTexture(
            mOwner->GetPosition(),
            Vector2(static_cast<float>(mTexture->GetWidth()), static_cast<float>(mTexture->GetHeight())),
            mOwner->GetRotation(),
            Vector3(1.0f, 1.0f, 1.0f),
            mTexture
        );
    }
}