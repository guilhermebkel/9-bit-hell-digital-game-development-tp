#pragma once
#include "Scene.h"
#include <vector>

class UIStatWidget;
class UIButtonComponent;
class Player;

class UpgradeScene : public Scene
{
public:
    explicit UpgradeScene(class Game* game);
    ~UpgradeScene() override;

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;

private:
    void SelectNextButton();
    void SelectPreviousButton();
    void ClickSelectedButton();
    void UpdateButtonSelection();

    UIStatWidget* mCoinWidget;
    std::vector<UIButtonComponent*> mButtons;

    std::vector<class Actor*> mSceneActors;

    size_t mSelectedButtonIndex;
    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
};