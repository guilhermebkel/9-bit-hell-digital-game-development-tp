#pragma once
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>
#include "../Math.h"

class Font
{
public:
    explicit Font(class Game* game);
    ~Font();

    bool Load(const std::string& fileName);
    void Unload();

    class Texture* RenderText(const std::string& text, const Vector3& color = Color::White, int pointSize = 24);

private:
    class Game* mGame;
    std::unordered_map<int, TTF_Font*> mFontData;
};