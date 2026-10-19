#include "raylib.h"
#include <cmath>

const float windowWidth = 800, windowHeight = 600, ballAcceleration = 0.0625f,
            ballSpeedBase = 256.0f, ballSpeedMax = 4096.0f, ballRadius = 8.0f,
            botOffset = 32.0f, playerSpeed = 512.0f, playerOffset = 32.0f;

const int fontSize = 24, playerWidth = 16, playerHeight = 256;

Vector2 ball, ballDir;
float ballSpeed, leftPlayer, rightPlayer;

int state; // 0 -> MainMenu, 1 -> PongGameSingle, 2 -> PongGameMultiplayer, 3 -> LWon, 4 -> RWon

const char *mainMenuText = "Press S for single player and M for multiplayer",
           *leftWinningText = "LEFT WON! Press R to restart",
           *rightWinningText = "RIGHT WON! Press R to restart";

// Returns true if a <= x <= b.
template <typename T>
bool Between(T a, T b, T x)
{
    return a <= x and x <= b;
}

// Changes the ball direction according to alpha [0, 1].
void ChangeBallDirection(float alpha)
{
    float dir = ballDir.x > 0 ? -1.0f : 1.0f;

    float theta = 30.0f * DEG2RAD + 120.0f * DEG2RAD * alpha;

    ballDir = {std::sinf(theta) * dir, -std::cosf(theta)};
}

// Updates related to the ball.
void UpdateBall()
{
    ball = {ball.x + ballDir.x * ballSpeed * GetFrameTime(),
            ball.y + ballDir.y * ballSpeed * GetFrameTime()};

    if (ballSpeed < ballSpeedMax)
        ballSpeed += ballSpeed * ballAcceleration * GetFrameTime();

    if (ball.x + ballRadius > windowWidth)
        state = 3;
    else if (ball.x - ballRadius < 0)
        state = 4;

    if (ball.x - ballRadius < playerOffset + playerWidth and Between(leftPlayer, leftPlayer + playerHeight, ball.y) and ballDir.x < 0)
        ChangeBallDirection((ball.y - leftPlayer) / playerHeight);
    else if (ball.x + ballRadius > windowWidth - playerOffset - playerWidth and Between(rightPlayer, rightPlayer + playerHeight, ball.y) and ballDir.x > 0)
        ChangeBallDirection((ball.y - rightPlayer) / playerHeight);

    if (ball.y - ballRadius < 0 or ball.y + ballRadius > windowHeight)
        ballDir.y = -ballDir.y;
}

// Updates related to the left player.
void UpdateLeftPlayer()
{
    if (IsKeyDown(KEY_W) or (state == 1 and IsKeyDown(KEY_UP)))
        leftPlayer -= playerSpeed * GetFrameTime();
    if (IsKeyDown(KEY_S) or (state == 1 and IsKeyDown(KEY_DOWN)))
        leftPlayer += playerSpeed * GetFrameTime();

    if (leftPlayer < 0)
        leftPlayer = 0;
    else if (leftPlayer + playerHeight > windowHeight)
        leftPlayer = windowHeight - playerHeight;
}

// Updates related to the right player.
void UpdateRightPlayer()
{
    if (state == 1)
    {
        if (ball.y - botOffset < rightPlayer)
            rightPlayer -= playerSpeed * GetFrameTime();
        else if (ball.y + botOffset > rightPlayer + playerHeight)
            rightPlayer += playerSpeed * GetFrameTime();
    }
    else
    {
        if (IsKeyDown(KEY_UP))
            rightPlayer -= playerSpeed * GetFrameTime();
        if (IsKeyDown(KEY_DOWN))
            rightPlayer += playerSpeed * GetFrameTime();
    }

    if (rightPlayer < 0)
        rightPlayer = 0;
    else if (rightPlayer + playerHeight > windowHeight)
        rightPlayer = windowHeight - playerHeight;
}

// Set the game values to default.
void InitGame()
{
    state = 0;

    ball = {windowWidth / 2, windowHeight / 2};
    ballDir = {0.7f, 0.2f};
    ballSpeed = ballSpeedBase;

    leftPlayer = rightPlayer = (windowHeight - playerHeight) / 2;
}

// Updates related to the game.
void UpdateGame()
{
    if (state != 0)
    {
        UpdateLeftPlayer();
        UpdateRightPlayer();

        if (state <= 2)
            UpdateBall();
        else if (IsKeyPressed(KEY_R))
            InitGame();
    }
    else
    {
        if (IsKeyPressed(KEY_S))
            state = 1;
        else if (IsKeyPressed(KEY_M))
            state = 2;
    }
}

// Draw the pong game.
void DrawGame()
{
    switch (state)
    {
    case 0:
        DrawText(mainMenuText, (windowWidth - MeasureText(mainMenuText, fontSize)) / 2, (windowHeight - fontSize) / 2, fontSize, LIME);
        break;
    case 1:
    case 2:
        DrawCircle(ball.x, ball.y, ballRadius, WHITE);
        DrawRectangle(playerOffset, leftPlayer, playerWidth, playerHeight, WHITE);
        DrawRectangle(windowWidth - playerWidth - playerOffset, rightPlayer, playerWidth, playerHeight, WHITE);
        break;
    case 3:
        DrawText(leftWinningText, (windowWidth - MeasureText(leftWinningText, fontSize)) / 2, (windowHeight - fontSize) / 2, fontSize, LIME);
        break;
    case 4:
        DrawText(rightWinningText, (windowWidth - MeasureText(rightWinningText, fontSize)) / 2, (windowHeight - fontSize) / 2, fontSize, LIME);
        break;
    default:
        break;
    }
}

int main()
{
    InitWindow(windowWidth, windowHeight, "pong");
    SetTargetFPS(60);

    InitGame();

    while (!WindowShouldClose())
    {
        UpdateGame();

        BeginDrawing();
        ClearBackground(BLACK);

        DrawGame();

        EndDrawing();
    }

    CloseWindow();
}