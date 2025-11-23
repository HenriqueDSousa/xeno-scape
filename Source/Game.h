// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>

#include <unordered_map>
#include <vector>

#include "Actors/Xeno.h"
#include "Renderer/Renderer.h"
#include "UI/HUD.h"
#include "UI/Screens/PauseMenu.h"
#include "UI/UIRect.h"

enum class SceneTransitionState
{
  None,
  FadingOut,
  LoadingNext,
  FadingIn
};

enum class GameScene
{
  MainMenu,
  Level1,
  TestLevel
};

enum class GameState {
  Gameplay,
  Paused,
  MainMenu,
  GameOver,
};

class Game
{
public:
    static const int WINDOW_WIDTH   = 1280;
    static const int WINDOW_HEIGHT  = 720;
    static const int LEVEL_WIDTH    = 20;
    static const int LEVEL_HEIGHT   = 20;
    // Size of source sprites (in pixels). Your sprites are 16x16.
    static const int SPRITE_SIZE    = 16;
    // Scaled tile size at runtime: use GetTileSize() to retrieve
    static const int SPAWN_DISTANCE = 700;
    static const int FPS = 60;
    static constexpr std::string_view BLOCK_ASSETS_PATH = "../Assets/Sprites/Blocks/";
    static constexpr std::string_view ENTITY_LAYER = "Entity";
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    void SetState(GameState state) { mGameState = state; }
    GameState GetState() { return mGameState; }

    // Scene Handling
    void SetScene(GameScene scene);
    void UnloadScene();
    GameScene GetCurrentScene() const { return mCurrentScene; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);
    const std::vector<class Actor*>& GetActors() const { return mActors; }
    class Xeno* GetPlayer() const { return mPlayer; }

    // Renderer
    class Renderer* GetRenderer() { return mRenderer; }

    // Scale factor for sprites / tiles. Computed at runtime in Initialize().
    int GetGameScale() const { return mGameScale; }
    int GetTileSize() const { return SPRITE_SIZE * mGameScale; }
    bool IsPositionOutOfBounds(const Vector2& position) const;

    // Draw functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);
    std::vector<class DrawComponent*>& GetDrawables() { return mDrawables; }

    // Collider functions
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(class AABBColliderComponent* collider);
    std::vector<class AABBColliderComponent*>& GetColliders() { return mColliders; }

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // UI functions
    const std::vector<class UIScreen*>& GetUIStack() const { return mUIStack; }
    void PushUI(class UIScreen* screen);
    void UpdateUI(float deltaTime);
    void RemoveUI(class UIScreen* screen);
    Font* LoadFont(const std::string& fileName);
    class HUD* GetHud() const { return mHud; }

    // Pause
    void OnPause();
    void OnResume();
    // Game specific

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void UpdateCamera();
    void GenerateOutput();
    void LoadData();

    // Level loading
    int **LoadLevelBlocks(const std::string& fileName);
    void LoadLevelEntities(const std::string& fileName);
    bool LoadTileMap(const std::string& fileName);
    void BuildLevel(int** levelData);
    void LoadBackgroundTexture(const std::string& fileName);

    void UpdateSceneManager(float deltaTime);
    void ApplySceneChange(GameScene scene);

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // Camera
    Vector2 mCameraPos;

    // All the draw components
    std::vector<class DrawComponent*> mDrawables;

    // All the collision components
    std::vector<class AABBColliderComponent*> mColliders;

    // Window
    SDL_Window* mWindow;

    // Renderer
    class Renderer* mRenderer;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;

    GameState mGameState;
    GameScene mCurrentScene;
    int mCurrentLevelWidth;
    int mCurrentLevelHeight;

    // Scene loading
    int mNextBlock;
    int mNextObstacle;
    int mScore;
    int mNextObstacleToScore;

    //Scene transition
    GameScene mNextSceneToLoad;
    SceneTransitionState mSceneTransitionState = SceneTransitionState::None;
    float mFadeAlpha = 0.0f;
    float mFadeSpeed = 1.0f;
    UIRect* mFadeRect;

    //UI elements
    std::vector<class UIScreen*> mUIStack;
    std::unordered_map<std::string, class Font*> mFonts;

    //Pause menu
    PauseMenu* mPauseMenu{};

    //HUD
    class HUD* mHud{};
    // Game-specific
    int **mLevelData;
    Texture* mBackgroundTexture{};
    // Runtime scale for sprites (multiplier for SPRITE_SIZE)
    int mGameScale{1};
    // Mapping from tile ID -> sprite filename (populated per-level)
    std::unordered_map<int, std::string> mTileSpriteMap;

    // Game specific
    Xeno* mPlayer;
};
