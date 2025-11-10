// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <vector>

#include "Actors/Actor.h"
#include "CSV.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Json.h"
#include "Random.h"
#include "UI/Font.h"
#include "UI/Menus/PauseMenu.h"
#include "UI/UIScreen.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(true)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mLevelData(nullptr)
        ,mGameState(Menu)
        ,mCurrentScene(MainMenu)
        ,mCurrentWidth(0)
        ,mCurrentHeight(0)
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

    // Initialize SDL_ttf
    if (TTF_Init() != 0)
    {
      SDL_Log("Failed to initialize SDL_ttf");
      return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mCurrentScene = TestLevel;
    LoadData();
    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::LoadData() {
  if (mCurrentScene != MainMenu) {
    mHud = new HUD(this, "../Assets/Fonts/SMB.ttf");
    mHud->SetPaused(false);
  }
  if (mCurrentScene == TestLevel) {
    int** levelData = LoadLevel("../Assets/Levels/TestLevel/testlevel.json");
    BuildLevel(levelData);
  }
}

void Game::OnPause() {
  SDL_Log("Paused");
  mPauseMenu = new PauseMenu(this, "../Assets/Fonts/SuperPixel-m2L8j.ttf");
  SetState(Paused);

  if (mHud) {
    mHud->SetPaused(true);
  }
}

void Game::OnResume() {
    if (!mPauseMenu)
      return;

    mPauseMenu->Close();
    SetState(Gameplay);
    SDL_Log("Resume");
    mPauseMenu = nullptr;

  if (mHud) {
    mHud->SetPaused(false);
  }
}

void Game::InitializeActors()
{
    //
    // LoadBackgroundTexture("../Assets/Sprites/Background.png");
    //
    // for (int i = 0; i < LEVEL_HEIGHT; ++i)
    //     delete[] levelData[i];
    // delete[] levelData;
}

int **Game::LoadLevel(const std::string& jsonFileName)
{
    // Open the JSON file
    std::ifstream ifs(jsonFileName);
    if (!ifs.is_open()) {
        SDL_Log("Failed to open level file: %s", jsonFileName.c_str());
        return nullptr;
    }

    nlohmann::json data;
    try {
        ifs >> data;
    } catch (const nlohmann::json::parse_error& e) {
        SDL_Log("Failed to parse level file %s: %s", jsonFileName.c_str(), e.what());
        return nullptr;
    }

    if (data.is_null()) {
      SDL_Log("Level file parsed to null: %s", jsonFileName.c_str());
      return nullptr;
    }

    // Validate expected fields
    if (!data.contains("width") || !data.contains("height") || !data.contains("layers")) {
      SDL_Log("Level file missing required fields: %s", jsonFileName.c_str());
      return nullptr;
    }

    int width = data["width"].get<int>();
    int height = data["height"].get<int>();

    // Allocate level array
    int **level = new int*[height];
    for (int r = 0; r < height; r++) {
        level[r] = new int[width];
    }

    // Read map data (first layer)
    std::vector<int> map;
    try {
        map = data["layers"][0]["data"].get<std::vector<int>>();
    } catch (const std::exception& e) {
        SDL_Log("Failed to read map data from %s: %s", jsonFileName.c_str(), e.what());
        // clean up
        for (int r = 0; r < height; r++) delete[] level[r];
        delete[] level;
        return nullptr;
    }

    if ((int)map.size() < width * height) {
        SDL_Log("Map data size (%zu) is smaller than width*height (%d)", map.size(), width * height);
        // fill missing with zeros
    }

    int idx = 0;
    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
          int value = 0;
          if (idx < (int)map.size()) value = map[idx];
          level[row][col] = value + 1;
          idx++;
      }
    }

    mCurrentWidth = width;
    mCurrentHeight = height;

    return level;
}

void Game::BuildLevel(int** levelData)
{
    for (int y=0; y < mCurrentHeight; y++) {
        for (int x=0; x < mCurrentWidth; x++) {
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

void Game::PushUI(class UIScreen *screen) {
    if (!screen) {
        SDL_Log("Error loading UI screen!");
        return;
    }
    mUIStack.emplace_back(screen);
}

void Game::UpdateUI(float deltaTime) {

    std::vector<UIScreen*> closingUIs;
    for (auto ui : mUIStack)
    {
        if (ui->GetState() == UIScreen::EActive)
        {
            ui->Update(deltaTime);
        } else if (ui->GetState() == UIScreen::EClosing) {
          closingUIs.emplace_back(ui);
        }
    }

    for (auto ui : closingUIs) {
      delete ui;
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
        if (event.type == SDL_QUIT) {
            Quit();
            continue;
        }
        // Handle pause/resume
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            if (mGameState == Paused) {
                if (!mUIStack.empty() && mUIStack.back() == mPauseMenu) {
                    OnResume();
                }
                } else {
                    OnPause();
                }
                continue;
            }
        // Handle UI input
        if (!mUIStack.empty()) {
            UIScreen* top = mUIStack.back();
            if (!top) {
                RemoveUI(top);
                continue;
            }
            if (event.type == SDL_KEYDOWN) {
                    top->HandleKeyPress(event.key.keysym.sym);
            } else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEMOTION) {
                    top->HandleMouse(event);
            }
            continue;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (mGameState == Gameplay && mUIStack.empty()) {
        for (auto actor : mActors) {
                actor->ProcessInput(state);
        }
    }
}

void Game::UpdateGame(float deltaTime)
{
    // Update all actors and pending actors
    UpdateActors(deltaTime);
    UpdateUI(deltaTime);
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
    // else if (desiredCamX > levelWidthPx - WINDOW_WIDTH)
    //     desiredCamX = levelWidthPx - WINDOW_WIDTH;

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

void Game::RemoveActor(Actor* actor) {
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

void Game::RemoveUI(class UIScreen* screen) {
  if (!screen) {
    SDL_Log("RemoveUI: null screen");
    return;
  }

  auto iter = std::find(mUIStack.begin(), mUIStack.end(), screen);
  if (iter == mUIStack.end()) {
    SDL_Log("RemoveUI: screen not found");
    return;
  }
  mUIStack.erase(iter);
}

Font* Game::LoadFont(const std::string& fileName) {
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end()) {
      return iter->second;
    }
    // Saves on buffer
    Font* font = new Font(mRenderer);
    if (font->Load(fileName)) {
      mFonts.emplace(fileName, font);
      return font;
    }
    font->Unload();
    delete font;
    return nullptr;
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
    // TODO: Fix the background rendering
    // mRenderer->DrawTexture(Vector2(LEVEL_WIDTH * TILE_SIZE * 0.5f, WINDOW_HEIGHT * 0.5f),
    //                        Vector2(LEVEL_WIDTH * TILE_SIZE, LEVEL_HEIGHT * TILE_SIZE),
    //                        0.0f,
    //                        Color::White,
    //                        mBackgroundTexture,
    //                        Vector4(0,0,1,1),
    //                        mCameraPos,
    //                        false);

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
    //Draw UI
    for (auto ui : mUIStack)
    {
        if (ui->GetState() == UIScreen::EActive)
        {
            ui->Draw();
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