set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "9-bit-hell")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/Drawing/DrawComponent.cpp
        Source/Components/Drawing/DrawComponent.h
        Source/Components/Physics/RigidBodyComponent.cpp
        Source/Components/Physics/RigidBodyComponent.h
        Source/Components/Physics/AABBColliderComponent.cpp
        Source/Components/Physics/AABBColliderComponent.h
        Source/Components/ParticleSystemComponent.cpp
        Source/Components/ParticleSystemComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Enemy.cpp
        Source/Actors/Enemy.h
        Source/Actors/Spawner.cpp
        Source/Actors/Spawner.h
        Source/Actors/Player.cpp
        Source/Actors/Player.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/Actors/Background.cpp
        Source/Actors/Background.h
        Source/Components/Drawing/StaticSpriteComponent.cpp
        Source/Components/Drawing/StaticSpriteComponent.h
        Source/Actors/Collectable.cpp
        Source/Actors/Collectable.h
        Source/Actors/Coin.cpp
        Source/Actors/Coin.h
        Source/Actors/Purifier.cpp
        Source/Actors/Purifier.h
        Source/Actors/CorruptionOverlay.cpp
        Source/Actors/CorruptionOverlay.h
        Source/Actors/Projectile.cpp
        Source/Actors/Projectile.h
        Source/Renderer/Font.cpp
        Source/Renderer/Font.h
        Source/Components/Drawing/UITextComponent.cpp
        Source/Components/Drawing/UITextComponent.h
        Source/Actors/HUD.cpp
        Source/Actors/HUD.h
        Source/Components/Drawing/UIButtonComponent.cpp
        Source/Components/Drawing/UIButtonComponent.h
        Source/Actors/MenuScreen.cpp
        Source/Actors/MenuScreen.h
        Source/Actors/PauseScreen.cpp
        Source/Actors/PauseScreen.h
        Source/Scenes/Scene.cpp
        Source/Scenes/Scene.h
        Source/Scenes/MainMenuScene.cpp
        Source/Scenes/MainMenuScene.h
        Source/Scenes/GameplayScene.cpp
        Source/Scenes/GameplayScene.h
        Source/Actors/UIStatWidget.cpp
        Source/Actors/UIStatWidget.h
        Source/Actors/UIHealthBarWidget.cpp
        Source/Actors/UIHealthBarWidget.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW SDL2::SDL2 ${SDL2_IMAGE_LIBRARIES} ${SDL2_TTF_LIBRARIES} OpenGL::GL)