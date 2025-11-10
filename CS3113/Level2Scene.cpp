/**
* Author: Zirui Xu
* Assignment: Rise of the AI
* Date due: 2025-11-8, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#include "Level2Scene.h"
#include "raylib.h"
#include <cmath>

extern int gLives;

static Entity* sBullet = nullptr;
static Entity* sBall = nullptr;


namespace {
    constexpr int LEVEL_WIDTH = 70;
    constexpr int LEVEL_HEIGHT = 8;
    constexpr float TILE_SIZE = 75.0f;


    const unsigned int LEVEL_DATA[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        // row 0
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,

        // row 1
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,

        // row 2
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,

        // row 3
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,

        // row 4
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,

        // row 5
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,

        // row 6
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,

        // row 7
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1
    };

    inline float fclamp(float v, float lo, float hi) {
        return v < lo ? lo : (v > hi ? hi : v);
    }

    bool aabbOverlap(Vector2 pA, Vector2 sA, Vector2 pB, Vector2 sB)
    {
        float dx = fabsf(pA.x - pB.x);
        float dy = fabsf(pA.y - pB.y);
        return (dx < (sA.x*0.5f + sB.x*0.5f)) && (dy < (sA.y*0.5f + sB.y*0.5f));
    }
}


void Level2Scene::initialise() {
    mGameState.nextSceneID = -1;

    mPendingReset = false;
    mResetCountdown = 0.0f;
    mResetToGameOver = false;

    extern int gLives;

    mGameState.bgm = LoadMusicStream("assets/audios/gaming.mp3");
    PlayMusicStream(mGameState.bgm);

    mSfxGameOver = LoadSound("assets/audios/game_over.mp3");
    mSfxWin = LoadSound("assets/audios/win.mp3");
    mIsGameOver = false;
    mGameOverPlayed = false;
    mIsWin = false;
    mWinPlayed = false;
    mWinCountdown = 0.0f;

    mMap = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,
        LEVEL_DATA,
        "assets/wood.png",
        TILE_SIZE,
        1, 1,
        mOrigin
    );

    mGhost = new Entity({ 0.0f, 0.0f }, { 0.0f, 0.0f }, "assets/ghost.png", PLAYER);
    mGhost->setSpeed((int)mSpeed);

    mGhost->setSpeed((int)mSpeed);

    const float GHOST_SCALE = 0.07f;
    Vector2 native = mGhost->getScale();
    Vector2 ghostSize = { native.x * GHOST_SCALE, native.y * GHOST_SCALE };
    mGhost->setScale(ghostSize);

    mGhost->setColliderDimensions(ghostSize);
    mGhost->setAcceleration({ 0.0f, 981.0f });
    mGhost->setJumpingPower(700.0f);

    float left = mOrigin.x - (LEVEL_WIDTH * TILE_SIZE) / 2.0f;
    float top = mOrigin.y - (LEVEL_HEIGHT * TILE_SIZE) / 2.0f;
    int spawnCol = 9;
    int groundRow = LEVEL_HEIGHT - 1;

    float tileCenterX = left + (spawnCol + 0.5f) * TILE_SIZE;
    float tileTopY = top + groundRow * TILE_SIZE;
    float halfH = mGhost->getColliderDimensions().y * 0.5f;

    mGhost->setPosition({ tileCenterX, tileTopY - 100.0f });

    mKeys = new Entity*[mKeyCount];
    mKeysHit = 0;
    const int keyCols[3] = { 17, 36, 55 };
    const int keyRow = 4;

    const float platformTopY = top + keyRow * TILE_SIZE;
    const Vector2 keySize = { TILE_SIZE, TILE_SIZE };
    const float keyHalfH = keySize.y * 0.5f;

    for (int i = 0; i < mKeyCount; ++i) {
        float cx = left + (keyCols[i] + 0.5f) * TILE_SIZE;
        float cy = platformTopY - keyHalfH;
        mKeys[i] = new Entity({ cx, cy }, keySize, "assets/key.png", OTHER);
        mKeys[i]->setColliderDimensions(keySize);
    }

    {
        const int witchCol = 39;
        const int groundRowW = LEVEL_HEIGHT - 1;
        const float WITCH_SCALE = 0.15f;

        mWitch = new Entity({ 0.0f, 0.0f }, { 0.0f, 0.0f }, "assets/witch.png", OTHER);

        Vector2 nat = mWitch->getScale();
        Vector2 wsize = { nat.x * WITCH_SCALE, nat.y * WITCH_SCALE };
        mWitch->setScale(wsize);
        Vector2 collider = { wsize.x * 0.85f, wsize.y * 0.85f };
        mWitch->setColliderDimensions(collider);

        float wcx = left + (witchCol + 0.5f) * TILE_SIZE;
        float groundTopY_W = top + groundRowW * TILE_SIZE;
        float wcy = groundTopY_W - wsize.y * 0.5f;
        mWitch->setPosition({ wcx, wcy });

        int leftBounceCol = 17;
        int rightBounceCol = LEVEL_WIDTH - 1;
        mWitchBounceXLeft = left + (leftBounceCol + 0.5f) * TILE_SIZE;
        mWitchBounceXRight = left + (rightBounceCol + 0.5f) * TILE_SIZE;

        mWitchDir = -1;
        mWitchSpeed = 700.0f;
    }

    mGameState.camera = { 0 };
    mGameState.camera.target = mGhost->getPosition();
    mGameState.camera.offset = mOrigin;
    mGameState.camera.rotation = 0.0f;
    mGameState.camera.zoom = 1.0f;

    mDoor = nullptr;

    mSfxGet = LoadSound("assets/audios/get.mp3");

    mBulletSpeed = 1500.0f;

    const int ballCol = 1;
    const float groundTopY = top + (LEVEL_HEIGHT - 1) * TILE_SIZE;
    const float ballCx = left + (ballCol + 0.5f) * TILE_SIZE;

    sBall = new Entity({ 0.0f, 0.0f }, { 0.0f, 0.0f }, "assets/ball.png", OTHER);

    const float BALL_SCALE = 0.04f;
    Vector2 nat = sBall->getScale();
    Vector2 ballSize = { nat.x * BALL_SCALE, nat.y * BALL_SCALE };
    sBall->setScale(ballSize);
    sBall->setColliderDimensions(ballSize);

    const float ballCy = groundTopY - ballSize.y * 0.5f;
    sBall->setPosition({ ballCx, ballCy });
}



void Level2Scene::update(float dt) {
    extern int gLives;

    if (mPendingReset) {
        if (!mGameOverPlayed) {
            if (mGameState.bgm.stream.buffer) StopMusicStream(mGameState.bgm);
            PlaySound(mSfxGameOver);
            mGameOverPlayed = true;
        }

        mResetCountdown -= dt;
        if (mResetCountdown <= 0.0f) {
            if (mResetToGameOver) {
                mGameState.nextSceneID = 0;
            } else {
                shutdown();
                initialise();
            }
        }
        return;
    }

    if (mIsWin) {
        if (!mWinPlayed) {
            if (mGameState.bgm.stream.buffer) StopMusicStream(mGameState.bgm);
            PlaySound(mSfxWin);
            mWinPlayed = true;
            mWinCountdown = 3.0f;
        } else {
            mWinCountdown -= dt;
            if (mWinCountdown <= 0.0f) {
                mGameState.nextSceneID = 3;
            }
        }
        return;
    }

    UpdateMusicStream(mGameState.bgm);

    mGhost->resetMovement();
    if (IsKeyDown(KEY_A)) mGhost->moveLeft();
    if (IsKeyDown(KEY_D)) mGhost->moveRight();
    if (IsKeyPressed(KEY_W) && mGhost->isCollidingBottom()) mGhost->jump();

    mGhost->update(dt, nullptr, mMap, nullptr, 0);

    if (mWitch) {
        Vector2 p = mWitch->getPosition();
        p.x += mWitchDir * mWitchSpeed * dt;
        if (p.x <= mWitchBounceXLeft) { p.x = mWitchBounceXLeft; mWitchDir = +1; }
        if (p.x >= mWitchBounceXRight) { p.x = mWitchBounceXRight; mWitchDir = -1; }
        mWitch->setPosition(p);
    }

    if (mKeys && mKeysHit < mKeyCount) {
        Vector2 gp = mGhost->getPosition();
        Vector2 gs = mGhost->getColliderDimensions();
        for (int i = 0; i < mKeyCount; ++i) {
            if (!mKeys[i]) continue;
            Vector2 kp = mKeys[i]->getPosition();
            Vector2 ks = mKeys[i]->getColliderDimensions();
            if (aabbOverlap(gp, gs, kp, ks)) {
                PlaySound(mSfxGet);
                delete mKeys[i]; mKeys[i] = nullptr;
                ++mKeysHit;
            }
        }
    }

    if (!sBullet && mKeysHit >= 1) {
        float left = mOrigin.x - (LEVEL_WIDTH * TILE_SIZE) / 2.0f;
        float top = mOrigin.y - (LEVEL_HEIGHT * TILE_SIZE) / 2.0f;

        int bulletCol = 0;
        float bulletCx = left + (bulletCol + 0.5f) * TILE_SIZE;
        const float platformTopY = top + 4 * TILE_SIZE;

        sBullet = new Entity({0.0f, 0.0f}, {0.0f, 0.0f}, "assets/bullet.png", OTHER);

        Vector2 nat = sBullet->getScale();
        float scale = 0.1f;
        Vector2 bulletSize = { nat.x * scale, nat.y * scale };
        sBullet->setScale(bulletSize);
        sBullet->setColliderDimensions(bulletSize);

        float bulletCy = platformTopY - bulletSize.y * 0.5f;
        sBullet->setPosition({ bulletCx, bulletCy });
    }

    if (sBullet) {
        Vector2 bp = sBullet->getPosition();
        bp.x += mBulletSpeed * dt;
        sBullet->setPosition(bp);
    }

    if (sBall && mGhost) {
        const float BALL_FOLLOW_SPEED = 200.0f;
        const float DEADZONE_X = 50.0f;

        Vector2 bp = sBall->getPosition();
        Vector2 gp = mGhost->getPosition();

        float dx = gp.x - bp.x;
        if (fabsf(dx) > DEADZONE_X) {
            float dir = (dx > 0.0f) ? 1.0f : -1.0f;
            bp.x += dir * BALL_FOLLOW_SPEED * dt;
            sBall->setPosition(bp);
        }
    }

    if (!mDoor && mKeysHit >= mKeyCount) {
        int doorCol = LEVEL_WIDTH - 3;
        int groundRow = LEVEL_HEIGHT - 1;

        float left = mOrigin.x - (LEVEL_WIDTH * TILE_SIZE) / 2.0f;
        float top = mOrigin.y - (LEVEL_HEIGHT * TILE_SIZE) / 2.0f;

        float tileCx = left + (doorCol + 0.5f) * TILE_SIZE;
        float groundTopY = top + groundRow * TILE_SIZE;

        mDoor = new Entity({0.0f, 0.0f}, {0.0f, 0.0f}, "assets/door.png", OTHER);

        Vector2 tex = mDoor->getScale();
        const float boxW = 2.0f * TILE_SIZE, boxH = 2.0f * TILE_SIZE;
        float s = fminf(boxW / tex.x, boxH / tex.y) * 0.90f;

        Vector2 doorSize = { tex.x * s, tex.y * s };
        mDoor->setScale(doorSize);
        mDoor->setColliderDimensions(doorSize);

        float doorCx = tileCx;
        float doorCy = groundTopY - doorSize.y * 0.5f;
        mDoor->setPosition({ doorCx, doorCy });
    }

    if (mDoor) {
        Vector2 gp = mGhost->getPosition();
        Vector2 gs = mGhost->getColliderDimensions();
        Vector2 dp = mDoor->getPosition();
        Vector2 ds = mDoor->getColliderDimensions();
        if (aabbOverlap(gp, gs, dp, ds)) {
            mIsWin = true;
        }
    }

    if (mGhost && mWitch) {
        Vector2 gp = mGhost->getPosition();
        Vector2 gs = mGhost->getColliderDimensions();
        Vector2 wp = mWitch->getPosition();
        Vector2 ws = mWitch->getColliderDimensions();

        if (aabbOverlap(gp, gs, wp, ws)) {
            gLives -= 1;
            mPendingReset = true;
            mResetCountdown = 3.0f;
            mResetToGameOver = (gLives <= 0);
        }
    }

    if (mGhost && sBullet) {
        Vector2 gp = mGhost->getPosition();
        Vector2 gs = mGhost->getColliderDimensions();
        Vector2 bp = sBullet->getPosition();
        Vector2 bs = sBullet->getColliderDimensions();

        if (aabbOverlap(gp, gs, bp, bs)) {
            gLives -= 1;
            mPendingReset = true;
            mResetCountdown = 3.0f;
            mResetToGameOver = (gLives <= 0);
        }
    }

    if (mGhost && sBall) {
        Vector2 gp = mGhost->getPosition();
        Vector2 gs = mGhost->getColliderDimensions();
        Vector2 bp = sBall->getPosition();
        Vector2 bs = sBall->getColliderDimensions();

        if (aabbOverlap(gp, gs, bp, bs)) {
            gLives -= 1;
            mPendingReset = true;
            mResetCountdown = 3.0f;
            mResetToGameOver = (gLives <= 0);
        }
    }

    Vector2 target = mGhost->getPosition();
    Vector2 diff = { target.x - mGameState.camera.target.x, target.y - mGameState.camera.target.y };
    mGameState.camera.target.x += diff.x * 0.10f;
    mGameState.camera.target.y += diff.y * 0.10f;

    float left = mOrigin.x - (LEVEL_WIDTH * TILE_SIZE) / 2.0f;
    float right = mOrigin.x + (LEVEL_WIDTH * TILE_SIZE) / 2.0f;
    float top = mOrigin.y - (LEVEL_HEIGHT * TILE_SIZE) / 2.0f;
    float bottom = mOrigin.y + (LEVEL_HEIGHT * TILE_SIZE) / 2.0f;

    float halfW = (1400 / mGameState.camera.zoom) * 0.5f;
    float halfH = (900 / mGameState.camera.zoom) * 0.5f;

    float minX = left + halfW;
    float maxX = right - halfW;
    float minY = top + halfH;
    float maxY = bottom - halfH;

    if (right - left >= 2.0f * halfW) {
        mGameState.camera.target.x = fclamp(mGameState.camera.target.x, minX, maxX);
    } else {
        mGameState.camera.target.x = (left + right) * 0.5f;
    }
    if (bottom - top >= 2.0f * halfH) {
        mGameState.camera.target.y = fclamp(mGameState.camera.target.y, minY, maxY);
    } else {
        mGameState.camera.target.y = (top + bottom) * 0.5f;
    }
}

void Level2Scene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));

    BeginMode2D(mGameState.camera);
        if (mMap) mMap->render();
        if (mKeys) {
            for (int i = 0; i < mKeyCount; ++i) {
                if (mKeys[i]) mKeys[i]->render();
            }
        }
        if (mDoor) mDoor->render();
        if (sBullet) sBullet->render();
        if (sBall) sBall->render();
        if (mWitch) mWitch->render();
        if (mGhost) mGhost->render();
    EndMode2D();

    if (mPendingReset) {
        const char* msg = mResetToGameOver ? "You Lose" : "";
        const int fontSize = 64;
        const int textW = MeasureText(msg, fontSize);
        const int x = (1400 - textW) / 2;
        const int y = (900 - fontSize) / 2;
        DrawText(msg, x, y, fontSize, WHITE);
    }
}

void Level2Scene::shutdown() {
    if (mGhost) { delete mGhost; mGhost = nullptr; }
    if (mMap) { delete mMap; mMap = nullptr; }

    if (mKeys) {
        for (int i = 0; i < mKeyCount; ++i) {
            delete mKeys[i];
            mKeys[i] = nullptr;
        }
        delete[] mKeys;
        mKeys = nullptr;
    }

    if (mDoor) { delete mDoor; mDoor = nullptr; }
    if (sBullet) { delete sBullet; sBullet = nullptr; }
    if (mWitch) { delete mWitch; mWitch = nullptr; }
    if (sBall) { delete sBall; sBall = nullptr; }

    UnloadSound(mSfxGet);
    UnloadSound(mSfxGameOver);
    UnloadSound(mSfxWin);

    if (mGameState.bgm.stream.buffer) {
        StopMusicStream(mGameState.bgm);
        UnloadMusicStream(mGameState.bgm);
    }
}
