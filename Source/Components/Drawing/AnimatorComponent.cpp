#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>
#include <algorithm>
#include <cctype>

AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
    mSpriteTexture = mOwner->GetGame()
        ->GetRenderer()
        ->GetTexture(texPath);

    if (!dataPath.empty())
        LoadSpriteSheetData(dataPath);
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    // Sort frames by their numeric index to ensure correct order
    std::vector<std::pair<int, nlohmann::json>> sortedFrames;
    
    for(const auto& [key, frame] : spriteSheetData["frames"].items()) {
        // Extract numeric index from frame name (e.g., "MeleeRobotIdle0" -> 0)
        std::string keyStr = key;
        int index = -1;
        
        // Find the last sequence of digits in the key
        for (int i = keyStr.length() - 1; i >= 0; i--) {
            if (std::isdigit(keyStr[i])) {
                size_t endPos = i + 1;
                while (i > 0 && std::isdigit(keyStr[i - 1])) {
                    i--;
                }
                index = std::stoi(keyStr.substr(i, endPos - i));
                break;
            }
        }
        
        if (index >= 0) {
            sortedFrames.emplace_back(index, frame);
        }
    }
    
    // Sort by index
    std::sort(sortedFrames.begin(), sortedFrames.end(), 
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    // Resize the sprite sheet data to accommodate all frames
    if (!sortedFrames.empty()) {
        int maxIndex = sortedFrames.back().first;
        mSpriteSheetData.resize(maxIndex + 1);
    }
    
    // Store frames at their correct indices
    for(const auto& [index, frame] : sortedFrames) {
        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData[index] = Vector4(
            static_cast<float>(x)/textureWidth, 
            static_cast<float>(y)/textureHeight,
            static_cast<float>(w)/textureWidth, 
            static_cast<float>(h)/textureHeight
        );
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer)
{
    if (!mIsVisible || !mSpriteTexture) return;
    // Convert the position relative to the screen position
    Vector4 textureRect;
    if (mAnimations.find(mAnimName) != mAnimations.end()) {
        const auto& animFrames = mAnimations[mAnimName];
        int currentFrameIndex = static_cast<int>(mAnimTimer);
        int spriteIndex = animFrames[currentFrameIndex];
        textureRect = mSpriteSheetData[spriteIndex];
    } else {
      renderer->DrawTexture(mOwner->GetPosition(),
      Vector2(mWidth, mHeight),
          mOwner->GetRotation(),
          mColor,
          mSpriteTexture,
          Vector4(0.0f, 0.0f, 1.0f, 1.0f),
          mOwner->GetGame()->GetCameraPos(), false);
      return;
    }
    bool flip = (mOwner->GetScale().x < 0.0f);
    renderer->DrawTexture(mOwner->GetPosition(),
        Vector2(mWidth, mHeight),
        mOwner->GetRotation(),
        mColor,
        mSpriteTexture,textureRect,
        mOwner->GetGame()->GetCameraPos(),
        flip);
}

void AnimatorComponent::Update(float deltaTime) {
    if (mIsPaused || mAnimations.empty()) {
        return;
    }
    
    if (mAnimations.find(mAnimName) != mAnimations.end()) {
        mAnimTimer += deltaTime * mAnimFPS;
        const auto& currentAnim = mAnimations[mAnimName];
        if (mAnimTimer >= currentAnim.size()) {
            mAnimTimer = 0.0f; // Loop
        }
    }
}

void AnimatorComponent::SetAnimation(const std::string& name)
{
    mAnimName = name.c_str();
    Update(0.0f);
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    if (mAnimations.find(name) == mAnimations.end()) {
        mAnimations.emplace(name, spriteNums);
    }
}