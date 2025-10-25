#pragma once
#include <memory>
#include <SDL.h>
#include <vector>
#include "Renderer/Renderer.h"
#include "Scenes/Scene.h"

class Game
{
public:
    enum class GameScene
    {
        MainMenu,
        Gameplay
    };

    enum class SceneState
    {
        Running,
        FadingOut,
        FadingIn
    };

    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    void SetScene(GameScene scene, float transitionTime = 0.5f);

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    // Renderer
    class Renderer* GetRenderer() { return mRenderer; }

    static const int WINDOW_WIDTH   = 1024;
    static const int WINDOW_HEIGHT  = 768;
    static const int FPS = 60;

    // Draw functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);
    void RequestDrawSort() { mDrawSortRequested = true; }
    std::vector<class DrawComponent*>& GetDrawables() { return mDrawables; }

    // Collider functions
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(class AABBColliderComponent* collider);
    std::vector<class AABBColliderComponent*>& GetColliders() { return mColliders; }

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Game specific
    const class Player* GetPlayer() { return mPlayer; }
    void SetPlayer(class Player* player) { mPlayer = player; }

    // Corruption specific
    void AddCorruption(float amount) {
        mCorruptionLevel += amount;
        mCorruptionLevel = Math::Clamp(mCorruptionLevel, 0.0f, 1.0f);
    }
    float GetCorruptionRate() const { return mCorruptionRate; }

    // Limitation in Y-axis for movement inside Scenario
    float GetUpperBoundary() const { return mUpperBoundaryY; }
    void SetUpperBoundary(float y) { mUpperBoundaryY = y; }

    // Corruption management
    void ReduceCorruption(float amount);
    float GetCorruptionLevel() const { return mCorruptionLevel; }

    void TogglePause();
    bool IsPaused() const { return mIsPaused; }

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void UpdateCamera();
    void GenerateOutput();

    void UpdateSceneManager(float deltaTime);
    void ChangeScene();
    void UnloadAllActors();

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // Camera
    Vector2 mCameraPos;

    // All the draw components
    std::vector<class DrawComponent*> mDrawables;
    bool mDrawSortRequested;

    // All the collision components
    std::vector<class AABBColliderComponent*> mColliders;

    // SDL stuff
    SDL_Window* mWindow;
    class Renderer* mRenderer;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;

    // Game-specific
    class Player *mPlayer;

    // Limitation in Y-axis for movement inside Scenario
    float mUpperBoundaryY;

    // Corruption management
    float mCorruptionLevel;
    float mCorruptionRate;

    std::unique_ptr<Scene> mCurrentScene;
    GameScene mCurrentSceneEnum = GameScene::MainMenu;
    GameScene mNextScene = GameScene::MainMenu;
    SceneState mSceneState = SceneState::Running;
    float mTransitionTimer = 0.0f;
    float mTransitionTotalTime = 0.0f;

    bool mIsPaused = false;
    class Actor* mPauseScreen = nullptr;
};
