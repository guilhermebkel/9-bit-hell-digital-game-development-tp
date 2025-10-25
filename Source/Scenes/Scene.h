#pragma once
#include <SDL_stdinc.h>

class Scene
{
public:
    explicit Scene(class Game* game) : mGame(game) {}
    virtual ~Scene() = default;

    virtual void Load() = 0;
    
    virtual void Unload() = 0;

    virtual void Update(float deltaTime) = 0;

    virtual void ProcessInput(const uint8_t* keyState) = 0;

protected:
    class Game* GetGame() { return mGame; }

private:
    class Game* mGame;
};