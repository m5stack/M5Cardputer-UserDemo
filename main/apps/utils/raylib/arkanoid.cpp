/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
#include <hal/hal.h>
#include <apps/utils/theme.h>
#include <apps/utils/audio/audio.h>
#include "raylib.h"

// https://github.com/raysan5/raylib-games/blob/master/classics/src/arkanoid.c
/*******************************************************************************************
 *
 *   raylib - classic game: arkanoid
 *
 *   Sample game developed by Marc Palau and Ramon Santamaria
 *
 *   This game has been created using raylib v1.3 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
 *
 *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define PLAYER_MAX_LIFE     5
#define LINES_OF_BRICKS     5
#define BRICKS_PER_LINE     20
#define BRICK_HEIGHT        10
#define BRICK_INIT_Y_OFFSET BRICK_HEIGHT / 2
#define BALL_RADIUS         3
#define BALL_SPEED          2
#define PLAYER_WIDTH        BALL_RADIUS * 8
#define PLAYER_HEIGHT       BALL_RADIUS
#define PLAYER_SPEED        5

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Player {
    Vector2 position;
    Vector2 size;
    int life;
} Player;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
} Ball;

typedef struct Brick {
    Vector2 position;
    bool active;
} Brick;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
#define screenWidth  240
#define screenHeight 135

struct ArkanoidData_t {
    bool gameOver = false;
    bool pause    = false;

    Player player                                 = {};
    Ball ball                                     = {};
    Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE] = {};
    Vector2 brickSize                             = {};

    bool key_left_state  = false;
    bool key_right_state = false;
};
ArkanoidData_t* _data = nullptr;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)
static void CollisionCallback(void);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
void start_arkanoid()
{
    _data = new ArkanoidData_t;

    GetHAL().canvasSystemBar.deleteSprite();
    GetHAL().canvasKeyboardBar.deleteSprite();
    GetHAL().canvas.deleteSprite();
    GetHAL().canvas.createSprite(GetHAL().display.width(), GetHAL().display.height());

    // Initialization (Note windowTitle is unused on Android)
    //---------------------------------------------------------
    GetHAL().canvas.setFont(FONT_REPL);

    InitGame();
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    auto update_count = GetHAL().millis();

    // Main game loop
    while (1) {
        if (GetHAL().millis() - update_count > 20) {
            GetHAL().feedTheDog();
            GetHAL().update();
            UpdateDrawFrame();

            auto key_event = GetHAL().keyboard.getLatestKeyEvent();
            if (_data->gameOver) {
                if (key_event.state && key_event.keyCode != KEY_NONE) {
                    break;
                }
            }

            update_count = GetHAL().millis();
        }
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();   // Unload loaded data (textures, sounds, models...)
    CloseWindow();  // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    delete _data;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
    _data->brickSize = (Vector2){static_cast<float>(GetScreenWidth()) / BRICKS_PER_LINE, BRICK_HEIGHT};

    // Initialize player
    _data->player.position = (Vector2){(float)screenWidth / 2, screenHeight - PLAYER_HEIGHT * 5};
    _data->player.size     = (Vector2){PLAYER_WIDTH, PLAYER_HEIGHT};
    _data->player.life     = PLAYER_MAX_LIFE;

    // Initialize ball
    _data->ball.position = (Vector2){(float)screenWidth / 2, _data->player.position.y};
    _data->ball.speed    = (Vector2){0, 0};
    _data->ball.radius   = BALL_RADIUS;
    _data->ball.active   = false;

    // Initialize bricks
    int initialDownPosition = BRICK_INIT_Y_OFFSET;

    for (int i = 0; i < LINES_OF_BRICKS; i++) {
        for (int j = 0; j < BRICKS_PER_LINE; j++) {
            _data->brick[i][j].position = (Vector2){j * _data->brickSize.x + _data->brickSize.x / 2,
                                                    i * _data->brickSize.y + initialDownPosition};
            _data->brick[i][j].active   = true;
        }
    }
}

// Update game (one frame)
void UpdateGame(void)
{
    if (!_data->gameOver) {
        // if (IsKeyPressed('P')) pause = !pause;

        auto key_event = GetHAL().keyboard.getLatestKeyEvent();

        printf("key_event.keyCode: %d\n", key_event.keyCode);

        if (key_event.keyCode == KEY_COMMA) {
            _data->key_left_state = key_event.state;
        } else if (key_event.keyCode == KEY_SLASH) {
            _data->key_right_state = key_event.state;
        }

        if (!_data->pause) {
            // Player movement logic
            // if (IsKeyDown(KEY_LEFT)) player.position.x -= 5;
            if (_data->key_left_state) {
                _data->player.position.x -= PLAYER_SPEED;
            }

            if ((_data->player.position.x - _data->player.size.x / 2) <= 0) {
                _data->player.position.x = _data->player.size.x / 2;
            }

            // if (IsKeyDown(KEY_RIGHT)) player.position.x += 5;
            if (_data->key_right_state) {
                _data->player.position.x += PLAYER_SPEED;
            }

            if ((_data->player.position.x + _data->player.size.x / 2) >= screenWidth) {
                _data->player.position.x = screenWidth - _data->player.size.x / 2;
            }

            // Ball launching logic
            if (!_data->ball.active) {
                // if (IsKeyPressed(KEY_SPACE))
                if (key_event.state && key_event.keyCode == KEY_ENTER) {
                    _data->ball.active = true;
                    _data->ball.speed  = (Vector2){0, -BALL_SPEED};
                }
            } else {
                if (key_event.state && key_event.keyCode == KEY_GRAVE) {
                    _data->pause = !_data->pause;
                }
            }

            // Ball movement logic
            if (_data->ball.active) {
                _data->ball.position.x += _data->ball.speed.x;
                _data->ball.position.y += _data->ball.speed.y;
            } else {
                _data->ball.position = (Vector2){_data->player.position.x, _data->player.position.y - BALL_RADIUS * 2};
            }

            // Collision logic: ball vs walls
            if (((_data->ball.position.x + _data->ball.radius) >= screenWidth) ||
                ((_data->ball.position.x - _data->ball.radius) <= 0)) {
                _data->ball.speed.x *= -1;
                CollisionCallback();
            }
            if ((_data->ball.position.y - _data->ball.radius) <= 0) {
                _data->ball.speed.y *= -1;
                CollisionCallback();
            }
            if ((_data->ball.position.y + _data->ball.radius) >= screenHeight) {
                _data->ball.speed  = (Vector2){0, 0};
                _data->ball.active = false;

                _data->player.life--;
                CollisionCallback();
            }

            // Collision logic: ball vs player
            if (CheckCollisionCircleRec(_data->ball.position, _data->ball.radius,
                                        (Rectangle){_data->player.position.x - _data->player.size.x / 2,
                                                    _data->player.position.y - _data->player.size.y / 2,
                                                    _data->player.size.x, _data->player.size.y})) {
                if (_data->ball.speed.y > 0) {
                    _data->ball.speed.y *= -1;
                    _data->ball.speed.x =
                        (_data->ball.position.x - _data->player.position.x) / (_data->player.size.x / 2) * 5;
                }

                CollisionCallback();
            }

            // Collision logic: ball vs bricks
            for (int i = 0; i < LINES_OF_BRICKS; i++) {
                for (int j = 0; j < BRICKS_PER_LINE; j++) {
                    if (_data->brick[i][j].active) {
                        // Hit below
                        if (((_data->ball.position.y - _data->ball.radius) <=
                             (_data->brick[i][j].position.y + _data->brickSize.y / 2)) &&
                            ((_data->ball.position.y - _data->ball.radius) >
                             (_data->brick[i][j].position.y + _data->brickSize.y / 2 + _data->ball.speed.y)) &&
                            ((fabs(_data->ball.position.x - _data->brick[i][j].position.x)) <
                             (_data->brickSize.x / 2 + _data->ball.radius * 2 / 3)) &&
                            (_data->ball.speed.y < 0)) {
                            _data->brick[i][j].active = false;
                            _data->ball.speed.y *= -1;

                            CollisionCallback();
                        }
                        // Hit above
                        else if (((_data->ball.position.y + _data->ball.radius) >=
                                  (_data->brick[i][j].position.y - _data->brickSize.y / 2)) &&
                                 ((_data->ball.position.y + _data->ball.radius) <
                                  (_data->brick[i][j].position.y - _data->brickSize.y / 2 + _data->ball.speed.y)) &&
                                 ((fabs(_data->ball.position.x - _data->brick[i][j].position.x)) <
                                  (_data->brickSize.x / 2 + _data->ball.radius * 2 / 3)) &&
                                 (_data->ball.speed.y > 0)) {
                            _data->brick[i][j].active = false;
                            _data->ball.speed.y *= -1;

                            CollisionCallback();
                        }
                        // Hit left
                        else if (((_data->ball.position.x + _data->ball.radius) >=
                                  (_data->brick[i][j].position.x - _data->brickSize.x / 2)) &&
                                 ((_data->ball.position.x + _data->ball.radius) <
                                  (_data->brick[i][j].position.x - _data->brickSize.x / 2 + _data->ball.speed.x)) &&
                                 ((fabs(_data->ball.position.y - _data->brick[i][j].position.y)) <
                                  (_data->brickSize.y / 2 + _data->ball.radius * 2 / 3)) &&
                                 (_data->ball.speed.x > 0)) {
                            _data->brick[i][j].active = false;
                            _data->ball.speed.x *= -1;

                            CollisionCallback();
                        }
                        // Hit right
                        else if (((_data->ball.position.x - _data->ball.radius) <=
                                  (_data->brick[i][j].position.x + _data->brickSize.x / 2)) &&
                                 ((_data->ball.position.x - _data->ball.radius) >
                                  (_data->brick[i][j].position.x + _data->brickSize.x / 2 + _data->ball.speed.x)) &&
                                 ((fabs(_data->ball.position.y - _data->brick[i][j].position.y)) <
                                  (_data->brickSize.y / 2 + _data->ball.radius * 2 / 3)) &&
                                 (_data->ball.speed.x < 0)) {
                            _data->brick[i][j].active = false;
                            _data->ball.speed.x *= -1;

                            CollisionCallback();
                        }
                    }
                }
            }

            // Game over logic
            if (_data->player.life <= 0) {
                _data->gameOver = true;
            } else {
                _data->gameOver = true;

                for (int i = 0; i < LINES_OF_BRICKS; i++) {
                    for (int j = 0; j < BRICKS_PER_LINE; j++) {
                        if (_data->brick[i][j].active) {
                            _data->gameOver = false;
                        }
                    }
                }
            }
        } else {
            if (key_event.state && key_event.keyCode == KEY_GRAVE) {
                _data->pause = !_data->pause;
            }
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (!_data->gameOver) {
        // Draw player bar
        DrawRectangle(_data->player.position.x - _data->player.size.x / 2,
                      _data->player.position.y - _data->player.size.y / 2, _data->player.size.x, _data->player.size.y,
                      BLACK);

        // Draw player lives
        for (int i = 0; i < _data->player.life; i++) {
            DrawRectangle(10 + 20 * i, screenHeight - 10, 10, 5, LIGHTGRAY);
        }

        // Draw ball
        DrawCircleV(_data->ball.position, _data->ball.radius, MAROON);

        // Draw bricks
        for (int i = 0; i < LINES_OF_BRICKS; i++) {
            for (int j = 0; j < BRICKS_PER_LINE; j++) {
                if (_data->brick[i][j].active) {
                    if ((i + j) % 2 == 0) {
                        DrawRectangle(_data->brick[i][j].position.x - _data->brickSize.x / 2,
                                      _data->brick[i][j].position.y - _data->brickSize.y / 2, _data->brickSize.x,
                                      _data->brickSize.y, GRAY);
                    } else {
                        DrawRectangle(_data->brick[i][j].position.x - _data->brickSize.x / 2,
                                      _data->brick[i][j].position.y - _data->brickSize.y / 2, _data->brickSize.x,
                                      _data->brickSize.y, DARKGRAY);
                    }
                }
            }
        }

        if (_data->pause) {
            GetHAL().canvas.setTextColor(TFT_DARKGRAY);
            GetHAL().canvas.drawCenterString("GAME PAUSED", 120, 56);
        }
    } else {
        GetHAL().canvas.setTextColor(TFT_DARKGRAY);
        GetHAL().canvas.drawCenterString("GAME OVER :)", 126, screenHeight / 2 - 24);
    }

    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Unload all dynamic loaded data (textures, sounds, models...)
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}

void CollisionCallback(void)
{
    audio::play_random_tone();
}
