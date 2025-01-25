#include <math.h>
#include <raylib.h>
#include <stdio.h>

typedef struct {
  float x, y;
  float orientation;
} bubble_t;

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

  bubble_t bubbles[8] = {0};
  size_t bubble_index = 0;

  int x = 0;
  bool is_puffy_blow = false;

  while (!WindowShouldClose()) {
    is_puffy_blow = IsKeyDown(KEY_SPACE);
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    float angle = atan2f(mouse_y - 200.0, mouse_x - 350.0);
    if (IsMouseButtonPressed(0)) {
      bubble_index = (bubble_index + 1) % 8;
      bubbles[bubble_index] =
          (bubble_t){.x = 350 - 25, .y = 200 - 50, .orientation = angle};
    }

    for (size_t i = 0; i < 8; ++i) {
      bubbles[i].x += cosf(bubbles[i].orientation);
      bubbles[i].y += sinf(bubbles[i].orientation);
    }

    BeginDrawing();
    DrawTexturePro(
        background,
        (Rectangle){.x = x / 60, .y = 0, .width = 288, .height = 256},
        (Rectangle){.x = 0, .y = 0, .width = 800, .height = 450}, (Vector2){0},
        0.f, GRAY);
    DrawTexturePro(
        midground,
        (Rectangle){.x = x / 10, .y = 0, .width = 960, .height = 512},
        (Rectangle){.x = 0, .y = 0, .width = 800, .height = 450}, (Vector2){0},
        0.f, GRAY);
    DrawTexturePro(
        puffy,
        (Rectangle){
            .x = is_puffy_blow ? 200 : 0, .y = 0, .width = 200, .height = 200},
        (Rectangle){.x = 350, .y = 200, .width = 100, .height = 100},
        (Vector2){50, 50}, angle / PI * 180.0, GRAY);
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
