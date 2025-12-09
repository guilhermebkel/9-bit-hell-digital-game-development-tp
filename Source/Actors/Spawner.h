#pragma once
#include "Actor.h"

enum class SpawnType
{
    Enemy,
    Soul,
    PurpleSoul,
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
                     int waveSize = 0, int keepPopulation = 0, bool waitForClear = false, float spawnInterval = 0.0f);

    void OnUpdate(float deltaTime) override;

private:
    SpawnType mSpawnType;
    int mTotalCount;      // Total a ser spawnado no total
    int mRemainingCount;  // Quanto falta spawnar
    int mWaveSize;        // Tamanho do lote
    int mKeepPopulation;  // Gatilho para respawn
    bool mWaitForClear;   // Comportamento de Boss
    float mInitialSpawnDelay;  // Delay antes do primeiro spawn
    // Intervalo entre spawns (em segundos). Se > 0, só gera novos lotes a cada intervalo.
    float mSpawnInterval;
    // Timer usado para contar até o próximo spawn quando mSpawnInterval > 0
    float mSpawnTimer;

    int CountActiveEnemies();
    void SpawnOne();
};