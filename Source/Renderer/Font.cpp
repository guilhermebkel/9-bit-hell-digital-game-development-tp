#include "../Renderer/Font.h"
#include "../Game.h"
#include <SDL_log.h>
#include <vector>

Font::Font(class Game* game)
    : mGame(game)
{
}

Font::~Font()
{
}

bool Font::Load(const std::string& fileName)
{
    mFontFile = fileName;
    std::vector<int> pointSizes = { 8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 48, 72 };

    for (auto& size : pointSizes)
    {
        if (!LoadSize(size))
        {
            return false;
        }
    }
    return true;
}

void Font::Unload()
{
    for (auto& font : mFontData)
    {
        TTF_CloseFont(font.second);
    }
    mFontData.clear();
    mFontFile.clear();
}

Texture* Font::RenderText(const std::string& text, const Vector3& color, int pointSize)
{
    Texture* texture = nullptr;

    SDL_Color sdlColor;
    sdlColor.r = static_cast<Uint8>(color.x * 255);
    sdlColor.g = static_cast<Uint8>(color.y * 255);
    sdlColor.b = static_cast<Uint8>(color.z * 255);
    sdlColor.a = 255;

    auto iter = mFontData.find(pointSize);
    if (iter == mFontData.end())
    {
        if (!LoadSize(pointSize))
        {
            return nullptr;
        }
        iter = mFontData.find(pointSize);
    }

    if (iter != mFontData.end())
    {
        TTF_Font* font = iter->second;
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, text.c_str(), sdlColor);
        if (surf != nullptr)
        {
            texture = new Texture();
            texture->CreateFromSurface(surf, false);
            SDL_FreeSurface(surf);
        }
    }

    return texture;
}

bool Font::LoadSize(int pointSize)
{
    if (mFontFile.empty())
    {
        SDL_Log("Font file not set before loading size %d", pointSize);
        return false;
    }

    if (mFontData.find(pointSize) != mFontData.end())
    {
        return true;
    }

    TTF_Font* font = TTF_OpenFont(mFontFile.c_str(), pointSize);
    if (font == nullptr)
    {
        SDL_Log("Failed to load font %s in size %d: %s", mFontFile.c_str(), pointSize, TTF_GetError());
        return false;
    }

    mFontData.emplace(pointSize, font);
    return true;
}