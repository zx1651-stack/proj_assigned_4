/**
* Author: Zirui Xu
* Assignment: Rise of the AI
* Date due: 2025-11-8, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "MenuScene.h"
#include "raylib.h"

MenuScene::MenuScene() : Scene({ 0.0f }, "#000000") {}
MenuScene::MenuScene(Vector2 origin, const char* bgHex) : Scene(origin, bgHex) {}
MenuScene::~MenuScene() { shutdown(); }

void MenuScene::initialise() {
    mGameState.nextSceneID = -1;

    mGameState.bgm = LoadMusicStream("assets/audios/menu.mp3");
    SetMusicVolume(mGameState.bgm, 1.0f);
    PlayMusicStream(mGameState.bgm);
}

void MenuScene::update(float deltaTime) {
    UpdateMusicStream(mGameState.bgm);

    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneID = 1;
    }

    (void)deltaTime;
}

void MenuScene::render() {
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const char* title = "SURVIVE AND ESCAPE";
    const char* prompt = "Press ENTER to start";

    int titleSize = 50;
    int promptSize = 25;

    int titleW = MeasureText(title, titleSize);
    int promptW = MeasureText(prompt, promptSize);

    DrawText(title, (int)(mOrigin.x - titleW / 2.0f), (int)(mOrigin.y - 50), titleSize, WHITE);
    DrawText(prompt, (int)(mOrigin.x - promptW / 2.0f), (int)(mOrigin.y + 20), promptSize, WHITE);
}

void MenuScene::shutdown() {
    StopMusicStream(mGameState.bgm);
    UnloadMusicStream(mGameState.bgm);
}
