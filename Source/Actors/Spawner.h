#pragma once
#include "Actor.h"

enum class SpawnType
{
    Enemy,
    Coin,
    Purifier,
    Healer,
    FatMiniboss,
    HornMiniboss,
    EyeMiniboss
};

class Spawner : public Actor
{
public:
    // waveSize: Quantos inimigos spawnam por vez
    // keepPopulation: Spawna nova horda quando a quantidade de inimigos vivos cair abaixo desse número
    // waitForClear: Se true, só spawna quando NÃO houver nenhum inimigo na tela (para Bosses)
    explicit Spawner(class Game* game, SpawnType type, int totalCount,
                     int waveSize = 0, int keepPopulation = 0, bool waitForClear = false);

    void OnUpdate(float deltaTime) override;

private:
    SpawnType mSpawnType;
    int mTotalCount;      // Total a ser spawnado no total
    int mRemainingCount;  // Quanto falta spawnar
    int mWaveSize;        // Tamanho do lote
    int mKeepPopulation;  // Gatilho para respawn
    bool mWaitForClear;   // Comportamento de Boss
    float mInitialSpawnDelay;  // Delay antes do primeiro spawn

    int CountActiveEnemies();
    void SpawnOne();
};