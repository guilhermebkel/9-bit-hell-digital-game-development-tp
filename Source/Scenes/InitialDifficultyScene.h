#pragma once
#include "Scene.h"
#include <vector>

class UIImageButtonComponent;
class UIButtonComponent;
class UITextComponent;

class InitialDifficultyScene : public Scene
{
public:
    explicit InitialDifficultyScene(class Game* game);
    ~InitialDifficultyScene() override = default;

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;

private:
    void UpdateDifficultySelection();

    std::vector<UIImageButtonComponent*> mDifficultyButtons;
    std::vector<UITextComponent*> mDifficultyLabels;
    std::vector<class Actor*> mDifficultyHighlightActors;

    int mSelectedIndex = 0;
    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;

    UIButtonComponent* mStartButton = nullptr;
    UIButtonComponent* mBackButton = nullptr;

    float mDifficultyBaseScale = 2.0f;
};
