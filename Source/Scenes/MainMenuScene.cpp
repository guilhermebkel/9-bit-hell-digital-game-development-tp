#include "MainMenuScene.h"
#include "../Game.h"
#include "../Actors/MenuBackground.h"
#include "../Actors/MenuScreen.h"

MainMenuScene::MainMenuScene(Game* game) : Scene(game) {}

void MainMenuScene::Load()
{
    new MenuBackground(GetGame());
    new MenuScreen(GetGame());
}

void MainMenuScene::Unload()
{
}

void MainMenuScene::Update(float deltaTime)
{
}

void MainMenuScene::ProcessInput(const uint8_t* keyState)
{
}