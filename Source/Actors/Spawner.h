#pragma once
#include "Actor.h"

enum class SpawnType
{
    Enemy,
    Coin,
    Purifier,
    Healer,
    FatMiniboss
};

class Spawner : public Actor
{
public:
    explicit Spawner(Game* game, SpawnType type, int count, bool spawnOnStart = true);

    void OnUpdate(float deltaTime) override;
private:
    SpawnType mSpawnType;
    int mSpawnCount;
    bool mHasSpawned;
};