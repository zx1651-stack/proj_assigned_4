#ifndef ENTITY_H
#define ENTITY_H

#include "raylib.h"
#include "cs3113.h"
#include <string>

enum EntityType {
    PLAYER,
    OTHER,
};

enum EntityStatus { ACTIVE, INACTIVE };

class Entity {
public:
    Entity(Vector2 position,
           Vector2 scale,
           const char* texturePath,
           EntityType type = OTHER);

    ~Entity();

    void resetMovement();
    void moveLeft();
    void moveRight();

    void setFacingLeft(bool left) { mFacingLeft = left; }

    void jump() { mIsJumping = true; }
    void setJumpingPower(float p) { mJumpingPower = p; }
    float getJumpingPower() const { return mJumpingPower; }

    void update(float dt,
                Entity* /*player*/ = nullptr,
                void*   /*map*/ = nullptr,
                Entity** others = nullptr,
                int count = 0);

    void render();

    void setPosition(Vector2 p) { mPosition = p; }
    Vector2 getPosition() const { return mPosition; }

    void setScale(Vector2 s) { mScale = s; }
    Vector2 getScale() const {
        return (Vector2){
            mScale.x > 0 ? mScale.x : (float)mTexture.width,
            mScale.y > 0 ? mScale.y : (float)mTexture.height
        };
    }

    void setSpeed(int s) { mSpeed = (float)s; }
    int getSpeed() const { return (int)mSpeed; }

    void setColliderDimensions(Vector2 dims) { mColliderDims = dims; }
    Vector2 getColliderDimensions() const {
        return (mColliderDims.x > 0 && mColliderDims.y > 0) ? mColliderDims : getScale();
    }

    void setAcceleration(Vector2 a) { mAcceleration = a; }
    Vector2 getAcceleration() const { return mAcceleration; }

    bool isCollidingBottom() const { return mIsCollidingBottom; }
    EntityType getType() const { return mType; }

    bool isActive() const { return mStatus == ACTIVE; }
    void deactivate() { mStatus = INACTIVE; }

private:
    void checkCollisionY(void* mapPtr);
    void checkCollisionX(void* mapPtr);
    bool isColliding(const Entity* other) const;

private:
    Texture2D mTexture{};
    Vector2 mPosition{};
    Vector2 mScale{};
    Vector2 mMovement{};
    Vector2 mColliderDims{};

    Vector2  mVelocity{ 0.0f, 0.0f };
    Vector2 mAcceleration{ 0.0f, 0.0f };

    float mSpeed{ 220.0f };
    EntityType mType{ OTHER };
    EntityStatus mStatus{ ACTIVE };

    bool mIsJumping{ false };
    float mJumpingPower{ 650.0f };

    bool mIsCollidingBottom{ false };

    bool mFacingLeft{ false };
    float mLastRenderX{ 0.0f };

    static constexpr float PROBE_INSET_Y = 4.0f;
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;
};

#endif


