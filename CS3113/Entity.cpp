/**
* Author: Zirui Xu
* Assignment: Rise of the AI
* Date due: 2025-11-8, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Entity.h"
#include <algorithm>
#include <cmath>
#include "Map.h"

Entity::Entity(Vector2 position,
               Vector2 scale,
               const char* texturePath,
               EntityType type)
    : mPosition(position)
    , mScale(scale)
    , mType(type)
{
    mTexture = LoadTexture(texturePath);
    mMovement = {0.0f, 0.0f};

    mLastRenderX = mPosition.x;
    mFacingLeft = false;
}

Entity::~Entity() {
    if (mTexture.id > 0) {
        UnloadTexture(mTexture);
    }
}

void Entity::resetMovement() { mMovement = {0.0f, 0.0f}; }
void Entity::moveLeft() {
    mMovement.x = -1.0f;
    mFacingLeft = true;
}


void Entity::moveRight() {
    mMovement.x = +1.0f;
    mFacingLeft = false;
}

bool Entity::isColliding(const Entity* other) const {
    if (!other || !other->isActive()) return false;

    Vector2 aSize = getColliderDimensions();
    Vector2 bSize = other->getColliderDimensions();

    float xDist = std::fabs(mPosition.x - other->mPosition.x) - (aSize.x + bSize.x) * 0.5f;
    float yDist = std::fabs(mPosition.y - other->mPosition.y) - (aSize.y + bSize.y) * 0.5f;

    return (xDist < 0.0f && yDist < 0.0f);
}

void Entity::checkCollisionY(void* mapPtr)
{
    Map* map = reinterpret_cast<Map*>(mapPtr);
    if (!map) return;

    mIsCollidingBottom = false;

    Vector2 box = getColliderDimensions();
    float halfW = box.x * 0.5f;
    float halfH = box.y * 0.5f;

    
    Vector2 topCentre= { mPosition.x,  mPosition.y - halfH };
    Vector2 topLeft = { mPosition.x - halfW, mPosition.y - halfH };
    Vector2 topRight = { mPosition.x + halfW, mPosition.y - halfH };

    Vector2 bottomCentre = { mPosition.x, mPosition.y + halfH };
    Vector2 bottomLeft = { mPosition.x - halfW, mPosition.y + halfH };
    Vector2 bottomRight = { mPosition.x + halfW, mPosition.y + halfH };

    float xo = 0.0f, yo = 0.0f;

    if (mVelocity.y < 0.0f) {
        bool hit =
            map->isSolidTileAt(topCentre,  &xo, &yo) ||
            map->isSolidTileAt(topLeft,    &xo, &yo) ||
            map->isSolidTileAt(topRight,   &xo, &yo);
        if (hit) {
            mPosition.y += yo;
            mVelocity.y  = 0.0f;
        }
    }

    if (mVelocity.y > 0.0f) {
        bool hit =
            map->isSolidTileAt(bottomCentre, &xo, &yo) ||
            map->isSolidTileAt(bottomLeft,   &xo, &yo) ||
            map->isSolidTileAt(bottomRight,  &xo, &yo);
        if (hit) {
            mPosition.y -= yo;
            mVelocity.y  = 0.0f;
            mIsCollidingBottom = true;
        }
    }
}

void Entity::checkCollisionX(void* mapPtr)
{
    Map* map = reinterpret_cast<Map*>(mapPtr);
    if (!map) return;

    Vector2 box = getColliderDimensions();
    const float halfW = box.x * 0.5f;
    const float halfH = box.y * 0.5f;

    const float yTop = mPosition.y - (halfH - PROBE_INSET_Y);
    const float yMid = mPosition.y;
    const float yBottom = mPosition.y + (halfH - PROBE_INSET_Y);

    const float xRightProbe = mPosition.x + halfW;
    const float xLeftProbe = mPosition.x - halfW;

    auto probeAndResolve = [&](float probeX, float probeY, bool rightSide) -> bool {
        float xo = 0.0f, yo = 0.0f;
        if (map->isSolidTileAt({probeX, probeY}, &xo, &yo)) {

            if (yo < Y_COLLISION_THRESHOLD) return false;

            if (rightSide && mVelocity.x > 0.0f) {
                mPosition.x -= xo;
                mVelocity.x = 0.0f;
                return true;
            }
            if (!rightSide && mVelocity.x < 0.0f) {
                mPosition.x += xo;
                mVelocity.x = 0.0f;
                return true;
            }
        }
        return false;
    };

    if (probeAndResolve(xRightProbe, yTop, true))  return;
    if (probeAndResolve(xRightProbe, yMid, true))  return;
    if (probeAndResolve(xRightProbe, yBottom, true))  return;

    if (probeAndResolve(xLeftProbe, yTop, false)) return;
    if (probeAndResolve(xLeftProbe, yMid, false)) return;
    if (probeAndResolve(xLeftProbe, yBottom, false)) return;
}





void Entity::update(float dt,
                    Entity* /*player*/,
                    void*   mapPtr,
                    Entity**others,
                    int     count)
{
    if (!isActive()) return;

    mVelocity.x  = mMovement.x * mSpeed;
    mVelocity.x += mAcceleration.x * dt;
    mVelocity.y += mAcceleration.y * dt;

    if (mIsJumping) {
        mIsJumping = false;
        mVelocity.y -= mJumpingPower;
    }

    mPosition.y += mVelocity.y * dt;
    checkCollisionY(mapPtr);

    mPosition.x += mVelocity.x * dt;
    checkCollisionX(mapPtr);
}


void Entity::render() {
    if (!isActive()) return;

    float dx = mPosition.x - mLastRenderX;
    if (dx < 0.0f)      mFacingLeft = true;
    else if (dx > 0.0f) mFacingLeft = false;
    mLastRenderX = mPosition.x;

    Vector2 size = getScale();

    Rectangle src;
    if (mFacingLeft) {
        src = { (float)mTexture.width, 0.0f,
               -(float)mTexture.width, (float)mTexture.height };
    } else {
        src = { 0.0f, 0.0f,
                (float)mTexture.width, (float)mTexture.height };
    }

    Rectangle dst = { mPosition.x, mPosition.y, size.x, size.y };
    Vector2   origin = { dst.width * 0.5f, dst.height * 0.5f };

    DrawTexturePro(mTexture, src, dst, origin, 0.0f, WHITE);
}
