#pragma once

#include <unordered_map>
#include "DrawComponent.h"

class Texture;

class AnimatorComponent : public DrawComponent {
public:
    // (Lower draw order corresponds with further back)
    AnimatorComponent(class Actor* owner, const std::string &texturePath, const std::string &dataPath,
            int width, int height, int drawOrder = 100);
    ~AnimatorComponent() override;

    void Draw(Renderer* renderer) override;
    void Update(float deltaTime) override;

    int GetWidth() { return mWidth; }
    int GetHeight() { return mHeight; }

    // Use to change the FPS of the animation
    void SetAnimFPS(float fps) { mAnimFPS = fps; }

    // Set the current active animation
    void SetAnimation(const std::string& name);

    // Use to pause/unpause the animation
    void SetIsPaused(bool pause) { mIsPaused = pause; }

    // Add an animation of the corresponding name to the animation map
    void AddAnimation(const std::string& name, const std::vector<int>& images);

    std::string GetAnimationName() { return mAnimName; }
    
    // Força a exibição do último frame da animação atual
    void ForceFinalFrame() { 
        if (!mAnimName.empty() && mAnimations.count(mAnimName) > 0)
        {
            const std::vector<int>& animFrames = mAnimations[mAnimName];
            int totalFrames = static_cast<int>(animFrames.size());
            if (totalFrames > 0)
            {
                mAnimTimer = (totalFrames - 1 + 0.5f) / mAnimFPS;
            }
        }
    }

private:
    bool LoadSpriteSheetData(const std::string& dataPath);

    // Sprite sheet texture
    class Texture* mSpriteTexture;

    // Vector of sprites
    std::vector<Vector4> mSpriteSheetData;

    // Map of animation name to vector of textures corresponding to the animation
    std::unordered_map<std::string, std::vector<int>> mAnimations;

    // Name of current animation
    std::string mAnimName;

    // Tracks current elapsed time in animation
    float mAnimTimer = 0.0f;

    // The frames per second the animation should run at
    float mAnimFPS = 10.0f;

    // Whether or not the animation is paused (defaults to false)
    bool mIsPaused = false;

    // Size
    int mWidth;
    int mHeight;

    float mTextureFactor;
    Texture* mTexture;
};

