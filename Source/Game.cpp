// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
//
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "Game.h"
#include "CSV.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Goomba.h"
#include "Actors/Spawner.h"
#include "Actors/Mario.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mMario(nullptr)
        ,mLevelData(nullptr)
{

}

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("TP3: Super Mario Bros", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::InitializeActors()
{
    mLevelData = LoadLevel("../Assets/Levels/Level1-1/Level1-1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);

    if(mLevelData)
    {
        BuildLevel(mLevelData, LEVEL_WIDTH, LEVEL_HEIGHT);
    }
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    int** levelData = new int*[height];
    for (int i = 0; i < height; ++i) {
        levelData[i] = new int[width];
    }

    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Failed to open level file: %s", fileName.c_str());
        for (int i = 0; i < height; ++i) delete[] levelData[i];
        delete[] levelData;
        return nullptr;
    }

    std::string line;
    int currentRow = 0;

    while (std::getline(file, line))
    {
        if (currentRow < height)
        {
            std::vector<int> row = CSVHelper::Split(line, ',');

            for (int j = 0; j < width; ++j)
            {
                if (j < row.size()) {
                    levelData[currentRow][j] = row[j];
                } else {
                    levelData[currentRow][j] = -1; // Valor padrão para células vazias
                }
            }
        }
        currentRow++;
    }

    for (int i = 0; i < height; ++i) {
        std::string rowStr = "";
        for (int j = 0; j < width; ++j) {
            rowStr += std::to_string(levelData[i][j]) + "\t";
        }
        SDL_Log("%s", rowStr.c_str());
    }

    return levelData;
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (levelData[i][j] == static_cast<int>(TileID::MarioStart))
            {
                if (!mMario) {
                    mMario = new Mario(this, Mario::FORWARD_SPEED, Mario::JUMP_SPEED);

                    Vector2 pos(j * TILE_SIZE + TILE_SIZE / 2.0f, i * TILE_SIZE + TILE_SIZE / 2.0f);
                    mMario->SetPosition(pos);

                    levelData[i][j] = -1;

                    break;
                }
            }
        }

        if(mMario) break;
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            Vector2 pos(j * TILE_SIZE + TILE_SIZE / 2.0f, i * TILE_SIZE + TILE_SIZE / 2.0f);
            int tileID = levelData[i][j];
            Actor* actor = nullptr;

            switch(tileID)
            {
                case static_cast<int>(TileID::Ground):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockA.png");
                    break;
                case static_cast<int>(TileID::StairBlock):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockD.png");
                    break;
                case static_cast<int>(TileID::QuestionBlock):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockC.png");
                    break;
                case static_cast<int>(TileID::Brick):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockB.png");
                    break;
                case static_cast<int>(TileID::PipeTopLeft):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockF.png");
                    break;
                case static_cast<int>(TileID::PipeTopRight):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockG.png");
                    break;
                case static_cast<int>(TileID::PipeBodyLeft):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockH.png");
                    break;
                case static_cast<int>(TileID::PipeBodyRight):
                    actor = new Block(this, "../Assets/Sprites/Blocks/BlockI.png");
                    break;
                case static_cast<int>(TileID::GoombaSpawn):
                    actor = new Spawner(this, Spawner::SPAWN_DISTANCE);
                    break;
            }

            if (actor)
            {
                actor->SetPosition(pos);
            }
        }
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::UpdateCamera()
{
    if (mMario)
    {
        // A posição x da câmera é a posição do Mario menos metade da largura da tela,
        // para que o Mario fique no centro.
        mCameraPos.x = mMario->GetPosition().x - (WINDOW_WIDTH / 2.0f);

        // A posição y da câmera permanece fixa por enquanto.
        mCameraPos.y = 0.0f;

        // Limita a câmera para não passar da borda esquerda do nível (posição 0).
        mCameraPos.x = std::max(0.0f, mCameraPos.x);

        // Limita a câmera para não passar da borda direita do nível.
        // A largura total do nível é LEVEL_WIDTH * TILE_SIZE.
        // A câmera não pode ir além de (largura do nível - largura da tela).
        float maxCameraX = (LEVEL_WIDTH * TILE_SIZE) - WINDOW_WIDTH;
        mCameraPos.x = std::min(maxCameraX, mCameraPos.x);
    }
}

void Game::AddActor(Actor* actor)
{
    if (mUpdatingActors)
    {
        mPendingActors.emplace_back(actor);
    }
    else
    {
        mActors.emplace_back(actor);
    }
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if(mIsDebugging)
        {
           // Call draw for actor components
              for (auto comp : drawable->GetOwner()->GetComponents())
              {
                comp->DebugDraw(mRenderer);
              }
        }
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete level data
    if (mLevelData) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}