cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "xeno-scape")
project(${PROJECT_NAME})

find_package(SDL2 2.32.2 EXACT REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)
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
        Source/Components/Particles/ParticleSystemComponent.tpp
        Source/Components/Particles/ParticleSystemComponent.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Blocks/Block.cpp
        Source/Actors/Blocks/Block.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/UI/Font.cpp
        Source/UI/Font.h
        Source/UI/Screens/UIScreen.cpp
        Source/UI/Screens/UIScreen.h
        Source/UI/HUD.cpp
        Source/UI/HUD.h
        Source/UI/Screens/PauseMenu.cpp
        Source/UI/Screens/PauseMenu.h
        Source/UI/UIElement.cpp
        Source/UI/UIElement.h
        Source/UI/UIText.cpp
        Source/UI/UIText.h
        Source/UI/UIButton.h
        Source/UI/UIButton.cpp
        Source/UI/Screens/MainMenu.cpp
        Source/UI/Screens/MainMenu.h
        Source/UI/Timer.cpp
        Source/UI/Timer.h
        Source/Actors/MeleeRobot.cpp
        Source/Actors/MeleeRobot.h
        Source/Actors/Xeno.cpp
        Source/Actors/Xeno.h
        Source/UI/UIRect.cpp
        Source/Actors/XenoArm.cpp
        Source/Actors/XenoArm.h
        Source/Components/Particles/Bullet.cpp
        Source/Components/Particles/Bullet.h
        Source/Components/Particles/Particle.cpp
        Source/Components/Particles/Particle.h
        Source/Components/Particles/PortalBullet.cpp
        Source/Components/Particles/PortalBullet.h
        Source/Components/Particles/ShootBullet.cpp
        Source/Components/Particles/ShootBullet.h
        Source/Actors/XenoGun.cpp
        Source/Actors/XenoGun.h
        Source/Actors/Portal.cpp
        Source/Actors/Portal.h
        Source/UI/Screens/GameOver.cpp
        Source/UI/Screens/GameOver.h
        Source/Actors/Blocks/EndLevelBlock.cpp
        Source/Actors/Blocks/EndLevelBlock.h
        Source/UI/Screens/GameEnd.cpp
        Source/UI/Screens/GameEnd.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/Actors/Blocks/PortalDisabledBlock.cpp
        Source/Actors/Blocks/PortalDisabledBlock.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer OpenGL::OpenGL SDL2_ttf::SDL2_ttf)