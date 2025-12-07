#pragma once
#include "Scene.h"

enum class LevelID
{
    Tutorial,
    Level1,
    Level2,
    Level3,
    Level4,
    Level5,
    Level6,
    Level7,
    Level8,
    Level9,
};

class GameplayScene : public Scene
{
public:
    explicit GameplayScene(class Game* game, LevelID level);

    void Load() override;
    void Unload() override;
    void Update(float deltaTime) override;
    void ProcessInput(const uint8_t* keyState) override;

    void SetNextLevelID(LevelID nextLevelID) { mNextLevelID = nextLevelID; }

private:
    void LoadTutorial();
    void LoadLevel1();
    void LoadLevel2();
    void LoadLevel3();
    void LoadLevel4();
    void LoadLevel5();
    void LoadLevel6();
    void LoadLevel7();
    void LoadLevel8();
    void LoadLevel9();
    bool IsLevelComplete();
    LevelID mLevelID;
    LevelID mNextLevelID;
    bool mMusicPlayed = false;
};