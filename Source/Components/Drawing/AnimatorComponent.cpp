#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"
#include <fstream>

AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mTexture(nullptr)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
    mTexture = owner->GetGame()->GetRenderer()->GetTexture(texPath);

    if (!dataPath.empty())
    {
        LoadSpriteSheetData(dataPath);
    }
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x)/textureWidth, static_cast<float>(y)/textureHeight,
                                      static_cast<float>(w)/textureWidth, static_cast<float>(h)/textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer)
{
    if (mIsVisible && mTexture)
    {
        Vector2 size(mWidth * std::abs(mOwner->GetScale().x), mHeight * std::abs(mOwner->GetScale().y));

        bool flip = mOwner->GetScale().x < 0.0f;

        Vector4 texRect(0.0f, 0.0f, 1.0f, 1.0f);

        if (!mAnimName.empty() && mAnimations.count(mAnimName))
        {
            const std::vector<int>& animFrames = mAnimations[mAnimName];
            if (!animFrames.empty())
            {
                int totalFrames = static_cast<int>(animFrames.size());
                int frameIndex = static_cast<int>(std::floor(mAnimTimer * mAnimFPS)) % totalFrames;

                int spriteSheetIndex = animFrames[frameIndex];

                // Correção: impede crash caso o index seja inválido
                if (spriteSheetIndex < 0 || spriteSheetIndex >= static_cast<int>(mSpriteSheetData.size()))
                {
                    SDL_Log("ERRO: spriteSheetIndex fora do range! index=%d size=%zu anim=%s frameIndex=%d totalFrames=%d",
                            spriteSheetIndex,
                            mSpriteSheetData.size(),
                            mAnimName.c_str(),
                            frameIndex,
                            totalFrames
                    );
                    return; // Evita crash
                }

                texRect = mSpriteSheetData[spriteSheetIndex];

            }
        }

        renderer->DrawTexture(
                mOwner->GetPosition(),
                size,
                mOwner->GetRotation(),
                mColor,
                mTexture,
                texRect,
                mOwner->GetGame()->GetCameraPos(),
                flip,
                mTextureFactor
        );
    }
}

void AnimatorComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.empty() || mAnimName.empty() || mAnimations.count(mAnimName) == 0)
    {
        return;
    }

    mAnimTimer += deltaTime;

    const std::vector<int>& animFrames = mAnimations[mAnimName];
    int totalFrames = static_cast<int>(animFrames.size());

    float animDuration = totalFrames / mAnimFPS;

    if (mAnimTimer >= animDuration)
    {
        mAnimTimer -= animDuration;
    }
}

void AnimatorComponent::SetAnimation(const std::string& name)
{
    mAnimName = name;
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}