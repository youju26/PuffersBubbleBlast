#include <math.h>
#include <raylib.h>
#include <stdio.h>

typedef struct {
  float x, y;
  float orientation;
} bubble_t;

typedef struct {
  float x, y;
  float orientation;
  float speed_x, speed_y;
} player_t;

int main() {

  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight,
             "raylib [models] example - geometric shapes");
  Texture2D background = LoadTexture("assets/environment/background.png");
  SetTextureWrap(background, TEXTURE_WRAP_REPEAT);
  Texture2D midground = LoadTexture("assets/environment/midground.png");
  SetTextureWrap(midground, TEXTURE_WRAP_REPEAT);
  Texture2D puffy = LoadTexture("assets/puffy.png");
  Texture2D bubble = LoadTexture("assets/FX/explosion.png");

  player_t player = {350.0, 200.0, 0, 0, 0};
  bubble_t bubbles[8] = {0};
  size_t bubble_index = 0;

  int x = 0;
  bool is_puffy_blow = false;

  while (!WindowShouldClose()) {
    is_puffy_blow = IsKeyDown(KEY_SPACE);
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    player.orientation = atan2f(mouse_y - player.y, mouse_x - player.x);
    if (IsMouseButtonPressed(0)) {
      float speed = 500000;
      player.speed_x += cosf(player.orientation + PI) * speed * GetFrameTime();
      player.speed_y += sinf(player.orientation + PI) * speed * GetFrameTime();

      bubble_index = (bubble_index + 1) % 8;
      bubbles[bubble_index] =
          (bubble_t){.x = player.x - 25, .y = player.y - 50, .orientation = player.orientation};
    }
    else {
      player.speed_x *= 0.998;
      player.speed_y *= 0.998;
    }

    player.x += player.speed_x * GetFrameTime();
    player.y += player.speed_y * GetFrameTime();

    for (size_t i = 0; i < 8; ++i) {
      bubbles[i].x += cosf(bubbles[i].orientation);
      bubbles[i].y += sinf(bubbles[i].orientation);
    }

    BeginDrawing();
    DrawTexturePro(
        background,
        (Rectangle){.x = player.x / 60, .y = 0, .width = 288, .height = 256},
        (Rectangle){.x = 0, .y = 0, .width = 800, .height = 450}, (Vector2){0},
        0.f, GRAY);
    DrawTexturePro(
        midground,
        (Rectangle){.x = player.x / 10, .y = 0, .width = 960, .height = 512},
        (Rectangle){.x = 0, .y = 0, .width = 800, .height = 450}, (Vector2){0},
        0.f, GRAY);
    DrawTexturePro(
        puffy,
        (Rectangle){
            .x = is_puffy_blow ? 200 : 0, .y = 0, .width = 200, .height = 200},
        (Rectangle){.x = player.x, .y = player.y, .width = 100, .height = 100},
        (Vector2){50, 50}, player.orientation / PI * 180.0, GRAY);
    for (size_t i = 0; i < 8; ++i) {
      DrawTexturePro(
          bubble,
          (Rectangle){
          .x = 0, .y = 0, .width = 60, .height = 82},
          (Rectangle){.x = bubbles[i].x, .y = bubbles[i].y, .width = 60, .height = 82},
          (Vector2){0}, 0, GRAY);
    }
    EndDrawing();
    x++;
  }

  UnloadTexture(background);
  CloseWindow();
}
