#pragma once
#include "Scene.h"
#include <vector>

class GameOverScene : public Scene
{
public:
    explicit GameOverScene(class Game* game);
    ~GameOverScene() override;

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;

private:
    void SelectNextButton();
    void SelectPreviousButton();
    void ClickSelectedButton();
    void UpdateButtonSelection();

    std::vector<class UIButtonComponent*> mButtons;
    std::vector<class Actor*> mSceneActors;

    size_t mSelectedButtonIndex;
    bool mUpPressed = false;
    bool mDownPressed = false;
    bool mEnterPressed = false;
};