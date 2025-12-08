#pragma once
#include "Scene.h"
#include <vector>

// Forward declarations
class UIImageButtonComponent;
class UIButtonComponent;
class SolidShapeComponent;

class LevelSelectionScene : public Scene
{
public:
    explicit LevelSelectionScene(class Game* game);
    ~LevelSelectionScene() override;

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;

private:
    void SelectNextButton();
    void SelectPreviousButton();
    void ClickSelectedButton();
    void UpdateButtonSelection();
    void UpdateDifficultySelection();

    std::vector<UIImageButtonComponent*> mLevelButtons;
    std::vector<class Actor*> mHighlightActors;

    std::vector<UIImageButtonComponent*> mDifficultyButtons;
    std::vector<class UITextComponent*> mDifficultyLabels;
    std::vector<class Actor*> mDifficultyHighlightActors;
    std::vector<class Actor*> mDifficultyBackgroundActors;

    float mDifficultyBaseScale = 3.0f;
    float mDifficultyButtonWidth = 220.0f;
    int mDifficultySelectedIndex = 0;
    bool mDifficultyFocus = false;

    UIButtonComponent* mBackButton;
    class Actor* mBackButtonActor;

    int mTotalButtons;
    size_t mSelectedButtonIndex;
    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
    bool mLeftPressed = false;
    bool mRightPressed = false;
};