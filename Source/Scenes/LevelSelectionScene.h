#pragma once
#include "Scene.h"
#include <vector>

// Forward declarations
class UIImageButtonComponent;
class UIButtonComponent;

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

    std::vector<UIImageButtonComponent*> mLevelButtons;

    UIButtonComponent* mBackButton;
    class Actor* mBackButtonActor;

    int mTotalButtons;
    size_t mSelectedButtonIndex;
    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
};