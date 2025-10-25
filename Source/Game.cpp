#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Player.h"
#include <SDL_ttf.h>
#include "Actors/PauseScreen.h"
#include "Components/Drawing/UIButtonComponent.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/GameplayScene.h"
#include "Scenes/Scene.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mDrawSortRequested(false)
        ,mUpperBoundaryY(0.0f)
        ,mCorruptionLevel(0.0f)
        ,mCorruptionRate(0.008f)
        ,mPlayer(nullptr)
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

    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("9-bit Hell", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mNextScene = GameScene::MainMenu;
    ChangeScene();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::UnloadAllActors()
{
    while (!mActors.empty())
    {
        delete mActors.back();
    }
    mPendingActors.clear();

    mDrawables.clear();
    mColliders.clear();

    mPlayer = nullptr;
}

void Game::ChangeScene()
{
    if (mCurrentScene)
    {
        mCurrentScene->Unload();
    }
    UnloadAllActors();

    mCurrentScene = nullptr;

    switch (mNextScene)
    {
    case GameScene::MainMenu:
        mCurrentScene = std::make_unique<MainMenuScene>(this);
        break;
    case GameScene::Gameplay:
        mCurrentScene = std::make_unique<GameplayScene>(this, LevelID::Tutorial);
        break;
    }

    if (mCurrentScene)
    {
        mCurrentScene->Load();
    }

    mCurrentSceneEnum = mNextScene;
}

void Game::SetScene(Game::GameScene scene, float transitionTime)
{
    if (mSceneState == SceneState::Running)
    {
        mNextScene = scene;
        mSceneState = SceneState::FadingOut;
        mTransitionTimer = transitionTime;
        mTransitionTotalTime = transitionTime;
    }
}

void Game::UpdateSceneManager(float deltaTime)
{
    switch (mSceneState)
    {
    case SceneState::FadingOut:
        mTransitionTimer -= deltaTime;
        if (mTransitionTimer <= 0.0f)
        {
            ChangeScene();
            mSceneState = SceneState::FadingIn;
            mTransitionTimer = mTransitionTotalTime;
        }
        break;

    case SceneState::FadingIn:
        mTransitionTimer -= deltaTime;
        if (mTransitionTimer <= 0.0f)
        {
            mSceneState = SceneState::Running;
        }
        break;

    case SceneState::Running:
        break;
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

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
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE && !event.key.repeat)
            {
                if (mCurrentSceneEnum == GameScene::Gameplay && mSceneState == SceneState::Running)
                {
                    TogglePause();
                }
            }
            break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (mSceneState == SceneState::Running)
    {
        if (!mIsPaused)
        {
            if (mCurrentScene)
            {
                mCurrentScene->ProcessInput(state);
            }

            mUpdatingActors = true;
            for (auto actor : mActors)
            {
                actor->ProcessInput(state);
            }
            mUpdatingActors = false;
        }
        else if (mPauseScreen)
        {
            mPauseScreen->ProcessInput(state);
        }
    }
}

void Game::TogglePause()
{
    mIsPaused = !mIsPaused;

    if (mIsPaused)
    {
        mPauseScreen = new PauseScreen(this);
    }
    else
    {
        if (mPauseScreen)
        {
            mPauseScreen->SetState(ActorState::Destroy);
            mPauseScreen = nullptr;
        }
    }
}

void Game::UpdateGame(float deltaTime)
{
    UpdateSceneManager(deltaTime);

    if (mSceneState == SceneState::Running && !mIsPaused)
    {
        UpdateActors(deltaTime);
        UpdateCamera();

        if (mPlayer)
        {
            mCorruptionLevel += mCorruptionRate * deltaTime;
            mCorruptionLevel = Math::Clamp(mCorruptionLevel, 0.0f, 1.0f);
        }
    }
}

void Game::ReduceCorruption(float amount)
{
    mCorruptionLevel -= amount;
    mCorruptionLevel = Math::Clamp(mCorruptionLevel, 0.0f, 1.0f);
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
    mCameraPos = Vector2::Zero;
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
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
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
    if(iter != mDrawables.end())
    {
        mDrawables.erase(iter);
    }
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    if(iter != mColliders.end())
    {
        mColliders.erase(iter);
    }
}

void Game::GenerateOutput()
{
    if (mDrawSortRequested)
    {
        std::sort(mDrawables.begin(), mDrawables.end(), [](DrawComponent* a, DrawComponent* b) {
            return a->GetDrawOrder() < b->GetDrawOrder();
        });

        mDrawSortRequested = false;
    }

    mRenderer->Clear();

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);
    }

    if (mSceneState != SceneState::Running)
    {
        float alpha = 0.0f;
        if (mSceneState == SceneState::FadingOut)
        {
            alpha = 1.0f - mTransitionTimer / mTransitionTotalTime;
        }
        else
        {
            alpha = mTransitionTimer / mTransitionTotalTime;
        }

        Vector4 black(0.0f, 0.0f, 0.0f, alpha);
        mRenderer->DrawRect(Vector2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f),
                            Vector2(WINDOW_WIDTH, WINDOW_HEIGHT), 0.0f, black, Vector2::Zero,
                            RendererMode::TRIANGLES);
    }

    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty()) {
        delete mActors.back();
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);

    TTF_Quit();
    SDL_Quit();
}