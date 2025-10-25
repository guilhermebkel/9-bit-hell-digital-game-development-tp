#include "MainMenuScene.h"
#include "../Game.h"
#include "../Actors/Background.h"
#include "../Actors/MenuScreen.h"

MainMenuScene::MainMenuScene(Game* game) : Scene(game) {}

void MainMenuScene::Load()
{
    new Background(GetGame(), "../Assets/MenuBackground.png");
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