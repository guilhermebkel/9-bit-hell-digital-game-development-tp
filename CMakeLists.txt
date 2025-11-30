set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "9-bit-hell")
project(${PROJECT_NAME})

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
        Source/Scenes/UpgradeScene.cpp
        Source/Scenes/UpgradeScene.h
        Source/Scenes/GameOverScene.cpp
        Source/Scenes/GameOverScene.h
        Source/Audio/AudioSystem.cpp
        Source/Audio/AudioSystem.h
)

if(WIN32)
    set(SDL2_PATH "C:/Program Files/SDL2")
    set(SDL2_IMAGE_PATH "C:/Program Files/SDL2_image")
    set(SDL2_TTF_PATH "C:/Program Files/SDL2_ttf")
    set(SDL2_MIXER_PATH "C:/Program Files/SDL2_mixer")
    set(GLEW_PATH "C:/Program Files/GLEW")

    set(SDL2_INCLUDE_DIR "${SDL2_PATH}/include")
    set(SDL2_IMAGE_INCLUDE_DIR "${SDL2_IMAGE_PATH}/include/SDL2")
    set(SDL2_TTF_INCLUDE_DIR "${SDL2_TTF_PATH}/include/SDL2")
    set(SDL2_MIXER_INCLUDE_DIR "${SDL2_MIXER_PATH}/include/SDL2")
    set(GLEW_INCLUDE_DIR "${GLEW_PATH}/include")

    set(SDL2_LIBRARY "${SDL2_PATH}/lib/x64/SDL2.lib")
    set(SDL2MAIN_LIBRARY "${SDL2_PATH}/lib/x64/SDL2main.lib")
    set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_PATH}/lib/x64/SDL2_image.lib")
    set(SDL2_TTF_LIBRARY "${SDL2_TTF_PATH}/lib/x64/SDL2_ttf.lib")
    set(SDL2_MIXER_LIBRARY "${SDL2_MIXER_PATH}/lib/x64/SDL2_mixer.lib")
    set(GLEW_LIBRARY "${GLEW_PATH}/lib/Release/x64/glew32.lib")

    target_include_directories(${PROJECT_NAME} PRIVATE
            ${SDL2_INCLUDE_DIR}
            ${SDL2_IMAGE_INCLUDE_DIR}
            ${SDL2_TTF_INCLUDE_DIR}
            ${GLEW_INCLUDE_DIR}
            ${SDL2_MIXER_INCLUDE_DIR}
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${SDL2_LIBRARY}
            ${SDL2MAIN_LIBRARY}
            ${SDL2_IMAGE_LIBRARY}
            ${SDL2_TTF_LIBRARY}
            ${SDL2_MIXER_LIBRARY}
            ${GLEW_LIBRARY}
            opengl32
    )

elseif(UNIX)
    find_package(SDL2 EXACT REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_library(SDL2_MIXER_LIBRARY SDL2_mixer)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_MIXER_LIBRARY})
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)


    target_include_directories(${PROJECT_NAME} PRIVATE
            ${SDL2_TTF_INCLUDE_DIR}
            ${SDL2_INCLUDE_DIR}
            ${SDL2_IMAGE_INCLUDE_DIR}
            ${SDL2_MIXER_INCLUDE_DIR}
            ${GLEW_INCLUDE_DIR}

    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
            GLEW::GLEW
            SDL2::SDL2
            ${SDL2_IMAGE_LIBRARIES}
            ${SDL2_TTF_LIBRARIES}
            ${SDL2_MIXER_LIBRARIES}
            OpenGL::GL

    )
endif()