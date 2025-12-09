cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# Configurando o nome do projeto
set(PROJECT_NAME "9-bit-hell")
project(${PROJECT_NAME})

set(SRC_CORE
    Source/Main.cpp
    Source/Game.cpp
    Source/Math.cpp
    Source/Random.cpp
)

set(SRC_RENDERER
    Source/Renderer/Shader.cpp
    Source/Renderer/Renderer.cpp
    Source/Renderer/VertexArray.cpp
    Source/Renderer/Texture.cpp
    Source/Renderer/Font.cpp
)

set(SRC_COMPONENTS
    Source/Components/Component.cpp
    Source/Components/Drawing/DrawComponent.cpp
    Source/Components/Physics/RigidBodyComponent.cpp
    Source/Components/Physics/AABBColliderComponent.cpp
    Source/Components/ParticleSystemComponent.cpp
    Source/Components/Drawing/AnimatorComponent.cpp
    Source/Components/Drawing/RectComponent.cpp
    Source/Components/Drawing/StaticSpriteComponent.cpp
    Source/Components/Drawing/UITextComponent.cpp
    Source/Components/Drawing/UIButtonComponent.cpp
    Source/Components/Drawing/UIImageButtonComponent.cpp
    Source/Components/Drawing/SolidShapeComponent.cpp
    Source/Components/Drawing/ShapeComponent.cpp
)

set(SRC_ACTORS
    Source/Actors/Actor.cpp
    Source/Actors/Background.cpp
    Source/Actors/Collectable.cpp
    Source/Actors/Soul.cpp
    Source/Actors/Purifier.cpp
    Source/Actors/CorruptionOverlay.cpp
    Source/Actors/Projectile.cpp
    Source/Actors/HUD.cpp
    Source/Actors/PauseScreen.cpp
    Source/Actors/UIStatWidget.cpp
    Source/Actors/UIHealthBarWidget.cpp
    Source/Actors/UIKeyPrompt.cpp
    Source/Actors/Healer.cpp
    Source/Actors/SlimeProjectile.cpp
    Source/Actors/SlimePuddle.cpp
    Source/Actors/FatMiniboss.cpp
    Source/Actors/Miniboss.cpp
    Source/Actors/GroundSpike.cpp
    Source/Actors/FallingHand.cpp
    Source/Actors/HornMiniboss.cpp
    Source/Actors/EyeProjectile.cpp
    Source/Actors/EyeMiniboss.cpp
    Source/Actors/Enemy.cpp
    Source/Actors/Spawner.cpp
    Source/Actors/Player.cpp
    Source/Actors/FatMiniboss.cpp
    Source/Actors/HornMiniboss.cpp
)

set(SRC_SCENES
    Source/Scenes/Scene.cpp
    Source/Scenes/MainMenuScene.cpp
    Source/Scenes/GameplayScene.cpp
    Source/Scenes/StoreScene.cpp
    Source/Scenes/GameOverScene.cpp
    Source/Scenes/VictoryScene.cpp
    Source/Scenes/InitialDifficultyScene.cpp
    Source/Scenes/LevelSelectionScene.cpp
)

set(SRC_AUDIO
    Source/Audio/AudioSystem.cpp
)

set(ALL_SOURCES
    ${SRC_CORE}
    ${SRC_RENDERER}
    ${SRC_COMPONENTS}
    ${SRC_ACTORS}
    ${SRC_SCENES}
    ${SRC_AUDIO}
)

add_executable(${PROJECT_NAME} ${ALL_SOURCES})

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

set(RESOURCE_SHADERS "${CMAKE_SOURCE_DIR}/Shaders")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCE_SHADERS}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Shaders"
)