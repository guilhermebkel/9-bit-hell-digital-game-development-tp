#pragma once
#include "Scene.h"

enum class LevelID
{
    Tutorial,
    Level1,
};

class GameplayScene : public Scene
{
public:
    explicit GameplayScene(class Game* game, LevelID level);

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;

private:
    void LoadTutorial();
    void LoadLevel1();
    LevelID mLevelID;
};