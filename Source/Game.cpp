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
#include "Actors/Blocks/Block.h"
#include "Actors/Blocks/EndLevelBlock.h"
#include "Actors/MeleeRobot.h"
#include "CSV.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Json.h"
#include "Random.h"
#include "UI/Font.h"
#include "UI/Screens/MainMenu.h"
#include "UI/Screens/PauseMenu.h"
#include "UI/Screens/UIScreen.h"
#include "UI/UIRect.h"

std::map<GameScene, GameScene> ScenesTransitionMap = {
  // { GameScene::TestLevel, GameScene::MainMenu },
  { GameScene::MainMenu,  GameScene::Level1 },
  {GameScene::Level1, GameScene::Level2},
    {GameScene::Level2, GameScene::MainMenu}

};

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(true)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mLevelData(nullptr)
        ,mGameState(GameState::MainMenu)
        ,mCurrentScene(GameScene::MainMenu)
        ,mCurrentLevelWidth(0)
        ,mCurrentLevelHeight(0)
        ,mFadeRect(nullptr)
        ,mPlayer(nullptr)
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

    SetScene(GameScene::MainMenu);

    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::OnPause() {
  if (mGameState != GameState::Gameplay)
    return;

  mPauseMenu = new PauseMenu(this, "../Assets/Fonts/SuperPixel-m2L8j.ttf");
  SetState(GameState::Paused);

  if (mHud) {
    mHud->SetPaused(true);
  }
}

void Game::OnResume() {
    if (!mPauseMenu)
      return;

    mPauseMenu->Close();
    SetState(GameState::Gameplay);
    mPauseMenu = nullptr;

  if (mHud) {
    mHud->SetPaused(false);
  }
}

void Game::SetNextScene() {
  GameScene nextScene = ScenesTransitionMap[GetCurrentScene()];
  SetScene(nextScene);
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

int **Game::LoadLevelBlocks(const std::string& jsonFileName)
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
      for (const auto &obj : data["layers"] ) {
        if (obj["name"] == "Blocks") {
          map = obj["data"].get<std::vector<int>>();
          break;
        }
      }
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
          level[row][col] = value - 1;
          idx++;
      }
    }
    mCurrentLevelWidth = width;
    mCurrentLevelHeight = height;
    return level;
}

void Game::SetLevelScale() {
  // Compute a reasonable integer scale (mGameScale) based on actual window size

  int winW = 0, winH = 0;
  SDL_GetWindowSize(mWindow, &winW, &winH);
  int scaleW = winW / (LEVEL_WIDTH * Game::SPRITE_SIZE);
  int scaleH = winH / (LEVEL_HEIGHT * Game::SPRITE_SIZE);
  int chosen = std::max(1, std::min(scaleW, scaleH));
  if (chosen <= 0) chosen = 1;
  mGameScale = chosen;

}

void Game::LoadLevelEntities(const std::string& jsonFileName) {
  // Open the JSON file
  std::ifstream ifs(jsonFileName);
  if (!ifs.is_open()) {
    SDL_Log("Failed to open level file: %s", jsonFileName.c_str());
    return;
  }

  nlohmann::json data;
  try {
    ifs >> data;
  } catch (const nlohmann::json::parse_error& e) {
    SDL_Log("Failed to parse level file %s: %s", jsonFileName.c_str(), e.what());
    return;
  }

  if (data.is_null()) {
    SDL_Log("Level file parsed to null: %s", jsonFileName.c_str());
    return;
  }

  // Validate expected fields
  if (!data.contains("width") || !data.contains("height") || !data.contains("layers")) {
    SDL_Log("Level file missing required fields: %s", jsonFileName.c_str());
    return;
  }

  nlohmann::json entities_data;
  for (auto layer : data["layers"]) {
    if (layer.contains("name") && layer["name"] == ENTITY_LAYER) {
      entities_data = layer;
      break;
    }
  }

  //Load enemy
  for (const auto &obj : entities_data["objects"]) {
    std::string type = obj["type"];
    int id = obj["id"];
    float x = static_cast<float>(obj["x"]) * GetGameScale();
    float y = static_cast<float>(obj["y"]) * GetGameScale();
    float width = static_cast<float>(obj["width"]) * GetGameScale();
    float height = static_cast<float>(obj["height"]) * GetGameScale();
    float MinPosX = 0;
    float MaxPosX = 0;
    float MinPosY = 0;
    float MaxPosY = 0;

    if (type == "Xeno") {
      SDL_Log("Loading Player");
      mPlayer = new Xeno(this, width, height);
      // Tiled uses bottom-left for entity position, adjust to center
      mPlayer->SetPosition(Vector2(x + width * 0.5f, y - height * 0.5f));
    }

    if (type == "MeleeRobot") {
      auto enemy = new MeleeRobot(this, width, height);
      // Tiled uses bottom-left for entity position, adjust to center
      enemy->SetPosition(Vector2(x + width * 0.5f, y - height * 0.5f));
    }
  }
}

bool Game::LoadTileMap(const std::string& fileName) {
    std::ifstream ifs(fileName);
    if (!ifs.is_open()) {
        SDL_Log("Failed to open tileset mapping: %s", fileName.c_str());
        return false;
    }

    nlohmann::json j;
    try {
        ifs >> j;
    } catch (const nlohmann::json::parse_error& e) {
        SDL_Log("Failed to parse tileset mapping %s: %s", fileName.c_str(), e.what());
        return false;
    }

    if (!j.is_object() || !j.contains("tiles")) {
        SDL_Log("Tileset mapping missing 'tiles' object: %s", fileName.c_str());
        return false;
    }

    mTileSpriteMap.clear();

    for (const auto& tile : j["tiles"]) {
        if (!tile.is_object()) {
            SDL_Log("Invalid tile entry in %s", fileName.c_str());
            continue;
        }
        if (!tile.contains("id") || !tile.contains("image")) {
            SDL_Log("Tile entry missing 'id' or 'image' in %s", fileName.c_str());
            continue;
        }
        if (!tile["id"].is_number() || !tile["image"].is_string()) {
            SDL_Log("Tile entry has unexpected types (id must be number, image must be string) in %s", fileName.c_str());
            continue;
        }

        int id = tile["id"].get<int>();
        std::string img = tile["image"].get<std::string>();

        // Extract filename (works for both '/' and '\')
        size_t pos = img.find_last_of("/\\");
        std::string filename = (pos == std::string::npos) ? img : img.substr(pos + 1);
        if (filename.empty()) {
            SDL_Log("Tile image filename empty for id %d in %s", id, fileName.c_str());
            continue;
        }

        std::string imagePath = std::string(BLOCK_ASSETS_PATH) + filename;
        mTileSpriteMap.emplace(id, std::move(imagePath));
    }

    return !mTileSpriteMap.empty();
}

void Game::BuildLevel(int** levelData)
{
    for (int y=0; y < mCurrentLevelHeight; y++) {
        for (int x=0; x < mCurrentLevelWidth; x++) {
            int tileID = levelData[y][x];

            int tile = GetTileSize();
            float worldX = x * tile + tile / 2.0f;
            float worldY = y * tile + tile / 2.0f;

            // Skip empty tiles or invalid IDs (only negative IDs are empty)
            if (tileID < 0) {
                continue;
            }

            auto it = mTileSpriteMap.find(tileID);
            if (it != mTileSpriteMap.end()) {
              //End level tile
              if (tileID == 72) {
                auto block = new EndLevelBlock(this, it->second);
                block->SetPosition(Vector2(worldX, worldY));
              } else {
                auto block = new Block(this, it->second);
                block->SetPosition(Vector2(worldX, worldY));
              }
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

void Game::SetScene(GameScene nextScene)
{
  mNextSceneToLoad = nextScene;
  mSceneTransitionState = SceneTransitionState::FadingOut;
}

void Game::UnloadScene() {
  // Use state so we can call this from withing an a actor update
  for(auto *actor : mActors) {
    actor->SetState(ActorState::Destroy);
  }
  // Delete UI screens
  for (auto ui : mUIStack) {
    delete ui;
  }
  mUIStack.clear();
}

void Game::ApplySceneChange(GameScene gameScene) {
  UnloadScene();
  mCurrentScene = gameScene;
  switch (gameScene) {
    case GameScene::MainMenu: {
      // Push main menu UI
      SetState(GameState::MainMenu);
      // if (mAudio->GetSoundState(mBackgroundMusic) == SoundState::Paused) {
        // mAudio->StopSound(mBackgroundMusic);
        // mBackgroundMusic = mAudio->PlaySound("Music.ogg", true);
      // }
      new MainMenu(this, "../Assets/Fonts/SMB.ttf");
      break;
    }
    case GameScene::TestLevel: {
      SetState(GameState::Gameplay);
      LoadTileMap("../Assets/Sprites/Blocks/block_tileset.json");
      int** levelData = LoadLevelBlocks("../Assets/Levels/TestLevel/testlevel.json");
      SetLevelScale();
      BuildLevel(levelData);
      LoadLevelEntities("../Assets/Levels/TestLevel/testlevel.json");

      mHud = new HUD(this, "../Assets/Fonts/SMB.ttf");
      mHud->SetPaused(false);

      break;
    }
    case GameScene::Level1: {
      SetState(GameState::Gameplay);
      LoadTileMap("../Assets/Sprites/Blocks/block_tileset.json");

      int** levelData = LoadLevelBlocks("../Assets/Levels/Level1/level1.json");
      SetLevelScale();
      BuildLevel(levelData);
      LoadLevelEntities("../Assets/Levels/Level1/level1.json");

      mHud = new HUD(this, "../Assets/Fonts/SMB.ttf");
      mHud->SetPaused(false);

      break;
    }
    case GameScene::Level2: {
      SetState(GameState::Gameplay);
      LoadTileMap("../Assets/Sprites/Blocks/block_tileset.json");

      int** levelData = LoadLevelBlocks("../Assets/Levels/Level2/level2.json");
      SetLevelScale();
      BuildLevel(levelData);
      LoadLevelEntities("../Assets/Levels/Level2/level2.json");
      mHud = new HUD(this, "../Assets/Fonts/SMB.ttf");
      mHud->SetPaused(false);

      break;
    }
    default:
      break;
    }
}

void Game::UpdateSceneManager(float deltaTime)
{
  if (mSceneTransitionState != SceneTransitionState::None) {
    switch (mSceneTransitionState) {
      case SceneTransitionState::FadingOut:
        // Increment alpha and ensure we have a single fade rect
        mFadeAlpha += deltaTime / mFadeSpeed;
        if (!mFadeRect) {
          // Create a full-screen black rect used for fading (use screen-space)
          mFadeRect = new UIRect(Vector2::Zero, Vector2(WINDOW_WIDTH * 10, WINDOW_HEIGHT * 10), Vector4(0.0f, 0.0f, 0.0f, mFadeAlpha));
        }
        if (mFadeAlpha >= 1.0f) {
          mFadeAlpha = 1.0f;
          // Ensure rect reflects final alpha (black)
          mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mFadeAlpha));
          // Load the next scene
          mCurrentScene = mNextSceneToLoad;
          ApplySceneChange(mNextSceneToLoad);
          mSceneTransitionState = SceneTransitionState::FadingIn;
        }
        else {
          mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mFadeAlpha));
        }
        break;

      case SceneTransitionState::FadingIn:
        // Decrease alpha and update/remove the fade rect when done
        mFadeAlpha -= deltaTime / mFadeSpeed;
          if (mFadeRect) {
            // Keep using a black rect while fading in
            mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mFadeAlpha));
          }
        if (mFadeAlpha <= 0.0f) {
          mFadeAlpha = 0.0f;
          // Remove and delete the rect
          if (mFadeRect) {
            delete mFadeRect;
            mFadeRect = nullptr;
          }
          mSceneTransitionState = SceneTransitionState::None;
        }
        break;

      default:
        break;
    }
  }
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

    UpdateSceneManager(deltaTime);

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

    while (SDL_PollEvent(&event)) {
      switch (event.type) {

        case SDL_QUIT: {
          Quit();
          break;
        }
        case SDL_KEYDOWN: {
          // Handle pause/resume
          if (event.key.keysym.sym == SDLK_ESCAPE) {
            if (mGameState == GameState::Paused && !mUIStack.empty() && mUIStack.back() == mPauseMenu) {
                OnResume();
            } else if (mGameState == GameState::Gameplay) {
              OnPause();
            }
          }
          // Handle UI input
          if (!mUIStack.empty()) {
            UIScreen* top = mUIStack.back();
            if (!top) {
              RemoveUI(top);
              continue;
            }
            top->HandleKeyPress(event.key.keysym.sym);
          }
          break;
        }
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION: {
          if (!mUIStack.empty()) {
            UIScreen* top = mUIStack.back();
            if (!top) {
              RemoveUI(top);
              continue;
            }
            top->HandleMouse(event);
          }
          break;
        }
        default:
          break;
      }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    if (mGameState == GameState::Gameplay) {
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
    // Compute level size in pixels using current level dimensions
    float levelWidthPx = (mCurrentLevelWidth > 0) ? (mCurrentLevelWidth * GetTileSize()) : (LEVEL_WIDTH * GetTileSize());
    float levelHeightPx = (mCurrentLevelHeight > 0) ? (mCurrentLevelHeight * GetTileSize()) : (LEVEL_HEIGHT * GetTileSize());

    // Default target: center of level
    float targetX = levelWidthPx * 0.5f;
    float targetY = levelHeightPx * 0.5f;

    // If player exists, use player position as camera target
    if (mPlayer && !mPlayer->IsDead()) {
        targetX = mPlayer->GetPosition().x;
        targetY = mPlayer->GetPosition().y;
    }

    // Convert target (center) to camera top-left coordinates
    float camX = targetX - (WINDOW_WIDTH * 0.5f);
    float camY = targetY - (WINDOW_HEIGHT * 0.5f);

    // Clamp horizontally. If level is smaller than window, center the level
    if (levelWidthPx <= WINDOW_WIDTH) {
        camX = (levelWidthPx - WINDOW_WIDTH) * 0.5f;
    } else {
        if (camX < 0.0f) camX = 0.0f;
        if (camX > levelWidthPx - WINDOW_WIDTH) camX = levelWidthPx - WINDOW_WIDTH;
    }

    // Clamp vertically. If level is shorter than the window, center the level
    if (levelHeightPx <= WINDOW_HEIGHT) {
        camY = (levelHeightPx - WINDOW_HEIGHT) * 0.5f;
    } else {
        if (camY < 0.0f) camY = 0.0f;
        if (camY > levelHeightPx - WINDOW_HEIGHT) camY = levelHeightPx - WINDOW_HEIGHT;
    }

    mCameraPos.x = camX;
    mCameraPos.y = camY;
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

bool Game::IsPositionOutOfBounds(const Vector2& position) const {
  float levelWidthPx = (mCurrentLevelWidth > 0) ? (mCurrentLevelWidth * GetTileSize()) : (LEVEL_WIDTH * GetTileSize());
  float levelHeightPx = (mCurrentLevelHeight > 0) ? (mCurrentLevelHeight * GetTileSize()) : (LEVEL_HEIGHT * GetTileSize());

  float offset = 30.0f;
  return position.x < -offset || position.x > levelWidthPx + offset ||
         position.y < -offset || position.y > levelHeightPx + offset;
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
    // Example background draw (use runtime tile size):
    // mRenderer->DrawTexture(Vector2(LEVEL_WIDTH * GetTileSize() * 0.5f, WINDOW_HEIGHT * 0.5f),
    //                        Vector2(LEVEL_WIDTH * GetTileSize(), LEVEL_HEIGHT * GetTileSize()),
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
    if (mFadeRect != nullptr) {
      // Fade rect is a screen-space UI element; draw it without camera offset
      mFadeRect->Draw(mRenderer, Vector2::Zero);
    }

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
        for (int i = 0; i < mCurrentLevelHeight; ++i) {
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