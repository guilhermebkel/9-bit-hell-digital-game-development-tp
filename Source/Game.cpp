#include <algorithm>
#include <vector>
#include <map>
#include <fstream>
#include <cstdio>
#include "Game.h"
#include "Json.h"
#include "Components/Drawing/DrawComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Player.h"
#include <SDL_ttf.h>
#include "Actors/PauseScreen.h"
#include "Components/Drawing/UIButtonComponent.h"
#include "Scenes/GameOverScene.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/GameplayScene.h"
#include "Scenes/Scene.h"
#include "Scenes/InitialDifficultyScene.h"
#include "Audio/AudioSystem.h"
#include "Scenes/LevelSelectionScene.h"
#include "Scenes/StoreScene.h"
#include "Scenes/VictoryScene.h"

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
        ,mCorruptionRate(Game::INITIAL_CORRUPTION_RATE)
        ,mPlayer(nullptr)
        ,mAudioSystem(nullptr)
        ,mDifficulty(Game::Difficulty::Medium)
{

}

const std::string Game::SAVE_FILE = "../9-bit-hell-save.json";

bool Game::Initialize()
{
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() == -1)
    {
        SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
        return false;
    }

    mWindow = SDL_CreateWindow("9-bit Hell", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mAudioSystem = new AudioSystem();
    if (!mAudioSystem->Initialize())
    {
        SDL_Log("Failed to initialize Audio System");
        return false;
    }

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
        case GameScene::InitialDifficulty:
            mCurrentScene = std::make_unique<class InitialDifficultyScene>(this);
            break;
        case GameScene::Gameplay:
            mCurrentScene = std::make_unique<GameplayScene>(this, mCurrentLevelID);
            break;
        case GameScene::Store:
            mCurrentScene = std::make_unique<StoreScene>(this);
            break;
        case GameScene::GameOver:
            mCurrentScene = std::make_unique<GameOverScene>(this);
            break;
        case GameScene::Victory:
            mCurrentScene = std::make_unique<VictoryScene>(this);
            break;
        case GameScene::LevelSelection:
            mCurrentScene = std::make_unique<LevelSelectionScene>(this);
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
            // Muda de cena no meio da transição
            ChangeScene();
            mSceneJustChanged = true;
            mSceneState = SceneState::FadingIn;
            mTransitionTimer = mTransitionTotalTime / 2.0f; // Fade in é metade do tempo
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
        mSceneJustChanged = false;
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
        else if (mIsPaused && mPauseScreen)
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
            // Destruir os atores filhos do PauseScreen
            mPauseScreen->DestroyChildActors();

            
            // Remover da lista de atores e deletar
            RemoveActor(mPauseScreen);
            delete mPauseScreen;
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
        mCurrentScene->Update(deltaTime);
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
        // Skip rendering if actor doesn't have valid position yet
        if (drawable->GetOwner() && !drawable->GetOwner()->HasValidPosition())
        {
            continue;
        }
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

    mAudioSystem->Shutdown();
    delete mAudioSystem;

    SDL_DestroyWindow(mWindow);

    TTF_Quit();
    SDL_Quit();
}

void Game::UpgradeFireRate()
{
    mPlayerUpgrades.fireRate *= 0.90f;

    if (mPlayerUpgrades.fireRate < 0.1f)
    {
        mPlayerUpgrades.fireRate = 0.1f;
    }
    
    // Increase price by 25% for next upgrade
    mPlayerUpgrades.fireRatePrice = static_cast<int>(mPlayerUpgrades.fireRatePrice * 1.25f);
}

void Game::UpgradeMeleeDamage()
{
    mPlayerUpgrades.meleeDamage += 5;
    
    // Increase price by 25% for next upgrade
    mPlayerUpgrades.damagePriceBase = static_cast<int>(mPlayerUpgrades.damagePriceBase * 1.25f);
}

void Game::UpgradeRangedDamage()
{
    mPlayerUpgrades.rangedDamage += 3;
    
    // Increase price by 25% for next upgrade
    mPlayerUpgrades.damagePriceBase = static_cast<int>(mPlayerUpgrades.damagePriceBase * 1.25f);
}

void Game::UpgradeSpeed()
{
    mPlayerUpgrades.playerSpeed *= 1.10f;
    
    // Increase price by 25% for next upgrade
    mPlayerUpgrades.speedPriceBase = static_cast<int>(mPlayerUpgrades.speedPriceBase * 1.25f);
}

void Game::UpgradePiercing()
{
    mPlayerUpgrades.piercing += 1;
    
    // Increase price by 25% for next upgrade
    mPlayerUpgrades.piercingPriceBase = static_cast<int>(mPlayerUpgrades.piercingPriceBase * 1.25f);
}

void Game::UnlockNextLevel(LevelID nextLevelID)
{
    mPlayerUpgrades.maxUnlockedLevel = Math::Max(mPlayerUpgrades.maxUnlockedLevel, static_cast<int>(nextLevelID));
}

void Game::AddCoin(int amount)
{
    mPlayerUpgrades.coins += amount;
}

void Game::SpendCoins(int amount)
{
    mPlayerUpgrades.coins -= amount;
}

int Game::GetDamagePrice() const
{
    return mPlayerUpgrades.damagePriceBase;
}

int Game::GetSpeedPrice() const
{
    return mPlayerUpgrades.speedPriceBase;
}

int Game::GetPiercingPrice() const
{
    return mPlayerUpgrades.piercingPriceBase;
}

int Game::GetPlayerHealth() const
{
    if (mPlayer)
    {
        return static_cast<int>(mPlayer->GetHealth());
    }
    return 0;
}

int Game::GetPlayerMaxHealth() const
{
    if (mPlayer)
    {
        return static_cast<int>(mPlayer->GetMaxHealth());
    }
    return 100;
}

std::string Game::GetSpeedDisplayValue() const
{
    // Velocidade padrão é 1725.0f
    // Calcular multiplicador baseado na velocidade padrão
    const float BASE_SPEED = 1725.0f;
    float multiplier = mPlayerUpgrades.playerSpeed / BASE_SPEED;
    
    // Formatar como multiplicador com 1 casa decimal
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "%.1fx", multiplier);
    return std::string(buffer);
}

void Game::SaveGame()
{
    nlohmann::json saveJson;

    saveJson["coins"] = mPlayerUpgrades.coins;
    saveJson["fireRate"] = mPlayerUpgrades.fireRate;
    saveJson["meleeDamage"] = mPlayerUpgrades.meleeDamage;
    saveJson["rangedDamage"] = mPlayerUpgrades.rangedDamage;
    saveJson["playerSpeed"] = mPlayerUpgrades.playerSpeed;
    saveJson["piercing"] = mPlayerUpgrades.piercing;
    saveJson["fireRatePrice"] = mPlayerUpgrades.fireRatePrice;
    saveJson["damagePriceBase"] = mPlayerUpgrades.damagePriceBase;
    saveJson["speedPriceBase"] = mPlayerUpgrades.speedPriceBase;
    saveJson["piercingPriceBase"] = mPlayerUpgrades.piercingPriceBase;
    saveJson["maxUnlockedLevel"] = mPlayerUpgrades.maxUnlockedLevel;
    saveJson["difficulty"] = static_cast<int>(mDifficulty);

    std::ofstream file(SAVE_FILE);

    if (file.is_open())
    {
        file << saveJson.dump(4);
        file.close();
        SDL_Log("Jogo salvo com sucesso!");
    }
    else
    {
        SDL_Log("Erro ao salvar o jogo.");
    }
}

void Game::LoadGame()
{
    std::ifstream file(SAVE_FILE);

    if (file.is_open())
    {
        try
        {
            nlohmann::json saveJson = nlohmann::json::parse(file);

            mPlayerUpgrades.coins = saveJson.value("coins", PlayerUpgrades().coins);
            mPlayerUpgrades.fireRate = saveJson.value("fireRate", PlayerUpgrades().fireRate);
            mPlayerUpgrades.meleeDamage = saveJson.value("meleeDamage", PlayerUpgrades().meleeDamage);
            mPlayerUpgrades.rangedDamage = saveJson.value("rangedDamage", PlayerUpgrades().rangedDamage);
            mPlayerUpgrades.playerSpeed = saveJson.value("playerSpeed", PlayerUpgrades().playerSpeed);
            mPlayerUpgrades.piercing = saveJson.value("piercing", PlayerUpgrades().piercing);
            mPlayerUpgrades.fireRatePrice = saveJson.value("fireRatePrice", PlayerUpgrades().fireRatePrice);
            mPlayerUpgrades.damagePriceBase = saveJson.value("damagePriceBase", PlayerUpgrades().damagePriceBase);
            mPlayerUpgrades.speedPriceBase = saveJson.value("speedPriceBase", PlayerUpgrades().speedPriceBase);
            mPlayerUpgrades.piercingPriceBase = saveJson.value("piercingPriceBase", PlayerUpgrades().piercingPriceBase);
            mPlayerUpgrades.maxUnlockedLevel = saveJson.value("maxUnlockedLevel", PlayerUpgrades().maxUnlockedLevel);

            int diffVal = saveJson.value("difficulty", static_cast<int>(Difficulty::Medium));
            if (diffVal < 0 || diffVal > 2) diffVal = static_cast<int>(Difficulty::Medium);
            mDifficulty = static_cast<Difficulty>(diffVal);

            SDL_Log("Jogo carregado!");
        }
        catch (const std::exception& e)
        {
            SDL_Log("Erro ao ler JSON de save: %s", e.what());
        }
    }
    else
    {
        SDL_Log("Nenhum save encontrado. Iniciando novo jogo.");
    }
}

bool Game::HasSaveFile() const
{
    std::ifstream file(SAVE_FILE);
    return file.is_open();
}

float Game::GetDifficultyMultiplier() const
{
    switch (mDifficulty)
    {
        case Difficulty::Easy: return 1.0f;
        case Difficulty::Medium: return 1.5f;
        case Difficulty::Hard: return 2.0f;
    }
    return 1.0f;
}