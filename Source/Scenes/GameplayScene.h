#pragma once
#include "Scene.h"

class GameplayScene : public Scene
{
public:
    explicit GameplayScene(class Game* game);

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;
};