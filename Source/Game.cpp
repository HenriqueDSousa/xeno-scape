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
#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"


Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(true)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
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

    mWindow = SDL_CreateWindow("Xeno Scape", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
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
    // int** levelData = LoadLevel("../Assets/Levels/Level1/level1-1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);
    // BuildLevel(levelData, LEVEL_WIDTH, LEVEL_HEIGHT);
    // LoadBackgroundTexture("../Assets/Sprites/Background.png");
    //
    // for (int i = 0; i < LEVEL_HEIGHT; ++i)
    //     delete[] levelData[i];
    // delete[] levelData;
}

int **Game::LoadLevel(const std::string& fileName, int width, int height)
{
    int **level = new int*[height];
    for (int i=0; i < height; i++) {
        level[i] = new int[width];
    }
    std::ifstream file(fileName);
    if (!file.is_open()) {
        SDL_Log("Error opening level file!");
        return level;
    }
    int row = 0;
    std::string line;
    while (std::getline(file,line)) {
        auto newRow = CSVHelper::Split(line);
        for (int col = 0; col < width && col < newRow.size(); col++) {
            level[row][col] = newRow[col];
        }
        row++;
    }
    return level;
}

void Game::BuildLevel(int** levelData, int width, int height)
{
    for (int y=0; y < height; y++) {
        for (int x=0; x < width; x++) {
            int tileID = levelData[y][x];

            float worldX = x * TILE_SIZE + TILE_SIZE / 2.0f;
            float worldY =  y * TILE_SIZE + TILE_SIZE / 2.0f;

            switch (tileID) {
                default:
                    break;
            }
        }
    }
}

void Game::LoadBackgroundTexture(const std::string &fileName) {
    if (!mRenderer) {
        return;
    }
    mBackgroundTexture = mRenderer->GetTexture(fileName);
    if (!mBackgroundTexture) {
        SDL_Log("Error loading background texture!");
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
    // TODO: Update to player's x position
    float desiredCamX = 0.0f;

    // Limites do nível em pixels
    float levelWidthPx = LEVEL_WIDTH * TILE_SIZE;

    if (desiredCamX < 0.0f)
        desiredCamX = 0.0f;
    else if (desiredCamX > levelWidthPx - WINDOW_WIDTH)
        desiredCamX = levelWidthPx - WINDOW_WIDTH;

    // Atualiza a posição da câmera (movendo apenas no eixo X)
    mCameraPos.x = desiredCamX;
    mCameraPos.y = 0.0f;

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
    // Draw background
    mRenderer->DrawTexture(Vector2(LEVEL_WIDTH * TILE_SIZE * 0.5f, WINDOW_HEIGHT * 0.5f),
                           Vector2(LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE),
                           0.0f,
                           Color::White,
                           mBackgroundTexture,
                           Vector4(0,0,1,1),
                           mCameraPos,
                           false);

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