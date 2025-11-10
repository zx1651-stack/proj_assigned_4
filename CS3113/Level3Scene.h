#ifndef LEVEL3_SCENE_H
#define LEVEL3_SCENE_H

#include "Scene.h"
#include "Entity.h"
#include "Map.h"
#include "raylib.h"

class Level3Scene : public Scene {
public:
    Level3Scene(Vector2 origin, const char* bgHex)
    : Scene(origin, bgHex) {}

    ~Level3Scene() override { shutdown(); }

    void initialise() override;
    void update(float dt) override;
    void render() override;
    void shutdown() override;

private:
    Entity* mGhost { nullptr };
    float mSpeed = 280.0f;
    Entity* mWitch { nullptr };
    Map* mMap { nullptr };

    Entity** mKeys { nullptr };
    int mKeyCount { 3 };
    int mKeysHit { 0 };
    Entity* mDoor { nullptr };

    Sound mSfxGet {};
    Sound mSfxGameOver {};
    Sound mSfxWin {};

    bool mIsGameOver { false };
    bool mGameOverPlayed { false };
    bool mIsWin { false };
    bool mWinPlayed { false };
    float mWinCountdown { 0.0f };

    bool mPendingReset { false };
    float mResetCountdown { 0.0f };
    bool mResetToGameOver { false };

    float mWitchSpeed {};
    int mWitchDir { -1 };
    float mWitchBounceXLeft { 0.0f };
    float mWitchBounceXRight { 0.0f };

    float mBulletSpeed {};
};

#endif
