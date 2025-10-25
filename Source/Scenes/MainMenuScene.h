#pragma once
#include "Scene.h"

class MainMenuScene : public Scene
{
public:
    explicit MainMenuScene(class Game* game);

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;
};