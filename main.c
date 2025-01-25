#include <math.h>
#include <raylib.h>
#include <stdio.h>

#define ENEMY_COUNT 8
#define BUBBLE_COUNT 8

enum ENEMY { FISH, FISH_BIG, FISH_DART, MINE, MINE_BIG, MINE_SMALL };

typedef struct {
  enum ENEMY type;
  float x, y;
  float orientation;
  float radius;
  bool onPlayer;
} enemy_t;

typedef struct {
  float x, y;
  float orientation;
  float radius;
} bubble_t;

typedef struct {
  float x, y;
  float orientation;
  float speed_x, speed_y;
  float radius;
  int lives;
  int hits;
} player_t;

int main() {

  const int screenWidth = 800;
  const int screenHeight = 450;

  bool pause = false;

  InitWindow(screenWidth, screenHeight,
             "raylib [models] example - geometric shapes");
  InitAudioDevice();
  Texture2D background = LoadTexture("assets/environment/background.png");
  SetTextureWrap(background, TEXTURE_WRAP_REPEAT);
  Texture2D midground = LoadTexture("assets/environment/midground.png");
  SetTextureWrap(midground, TEXTURE_WRAP_REPEAT);
  Texture2D puffy = LoadTexture("assets/puffy.png");
  Texture2D bubble = LoadTexture("assets/FX/explosion.png");
  Texture2D fish_dart = LoadTexture("assets/enemies/fish-dart.png");

  player_t player = {.x = 350.0,
                     .y = 200.0,
                     .orientation = 0,
                     .speed_x = 0,
                     .speed_y = 0,
                     .radius = 50,
                     .lives = 5,
                     .hits = 0};

  bubble_t bubbles[BUBBLE_COUNT] = {0};
  size_t bubble_index = 0;

  enemy_t enemies[ENEMY_COUNT] = {0};
  size_t enemy_index = 0;
  for (size_t i = 0; i < ENEMY_COUNT; ++i) {
    enemies[i] = (enemy_t){.x = GetRandomValue(0, screenWidth),
                           .y = GetRandomValue(0, screenHeight),
                           .orientation = GetRandomValue(0, 2 * PI),
                           .radius = 10,
                           .onPlayer = false};
  }

  int x = 0;
  bool is_puffy_blow = false;

  Camera2D camera = {0};
  camera.target = (Vector2){player.x + 20.0f, player.y + 20.0f};
  camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;

  while (!IsAudioDeviceReady())
    ;
  Sound sound_ambient = LoadSound("assets/sounds/magical_theme.ogg");
  Sound sound_bubble = LoadSound("assets/sounds/bubble_01.ogg");
  Sound sound_hurt = LoadSound("assets/sounds/FEMALE_Gruntwork_1.ogg");
  Sound sound_puffy_death = LoadSound("assets/sounds/puffy_death.ogg");
  Sound sound_fish_dying = LoadSound("assets/sounds/slime_04.ogg");
  // Sound sound_ambient = LoadSound("assets/enchanted tiki 86_0.ogg");
  PlaySound(sound_ambient);

  while (!WindowShouldClose()) {
    if (!pause) {
      is_puffy_blow = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(0);
      int mouse_x = GetMouseX();
      int mouse_y = GetMouseY();
      player.orientation =
          atan2f(mouse_y - camera.offset.y, mouse_x - camera.offset.x);
      if (IsMouseButtonPressed(0)) {
        PlaySound(sound_bubble);
        float speed = 500000;
        player.speed_x +=
            cosf(player.orientation + PI) * speed * GetFrameTime();
        player.speed_y +=
            sinf(player.orientation + PI) * speed * GetFrameTime();

        bubble_index = (bubble_index + 1) % 8;
        bubbles[bubble_index] = (bubble_t){.x = player.x - 25,
                                           .y = player.y - 50,
                                           .orientation = player.orientation,
                                           .radius = 30};
      } else {
        player.speed_x *= 0.998;
        player.speed_y *= 0.998;
      }

      player.x += player.speed_x * GetFrameTime();
      player.y += player.speed_y * GetFrameTime();

      Vector2 player_vec = {player.x + player.radius, player.y + player.radius};

      for (size_t i = 0; i < BUBBLE_COUNT; ++i) {
        bubbles[i].x += cosf(bubbles[i].orientation);
        bubbles[i].y += sinf(bubbles[i].orientation);

        Vector2 bubble_vec = {bubbles[i].x + bubbles[i].radius,
                              bubbles[i].y + bubbles[i].radius};
        for (size_t j = 0; j < ENEMY_COUNT; ++j) {
          Vector2 enemy_vec = {enemies[j].x + enemies[j].radius,
                               enemies[j].y + enemies[j].radius};
          bool collision = CheckCollisionCircles(enemy_vec, enemies[j].radius,
                                                 bubble_vec, bubbles[i].radius);
          if (collision) {
            enemies[j] = (enemy_t){.x = GetRandomValue(0, screenWidth),
                                   .y = GetRandomValue(0, screenHeight),
                                   .orientation = GetRandomValue(0, 2 * PI),
                                   .radius = 10,
                                   .onPlayer = false};
            player.hits += 1;
            PlaySound(sound_fish_dying);
          }
        }
      }

      for (size_t i = 0; i < ENEMY_COUNT; ++i) {
        Vector2 enemy_vec = {enemies[i].x + enemies[i].radius,
                             enemies[i].y + enemies[i].radius};
        bool collision = CheckCollisionCircles(enemy_vec, enemies[i].radius,
                                               player_vec, player.radius);
        if (collision & (collision != enemies[i].onPlayer)) {
          if (player.lives == 0) {
            pause = true;

            PlaySound(sound_puffy_death);
          } else {
            player.lives -= 1;
            enemies[i].onPlayer = true;

            PlaySound(sound_hurt);
          }
        } else {
          enemies[i].onPlayer = collision;
        }

        float speed = 50;

        enemies[i].orientation += GetRandomValue(-1, 1) / 180.0 * PI;
        float player_direction =
            atan2f(player.y - enemies[i].y, player.x - enemies[i].x);

        bool is_player_in_view =
            fabs(fmod(player_direction - enemies[i].orientation, 2 * PI)) / PI *
                180.0 <=
            30.0;

        if (is_player_in_view) {
          enemies[i].orientation = player_direction;
          speed = 250;
        }
        enemies[i].x += cosf(enemies[i].orientation) * GetFrameTime() * speed;
        enemies[i].y += sinf(enemies[i].orientation) * GetFrameTime() * speed;
      }
    }

    camera.target = (Vector2){player.x + 20, player.y + 20};

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

    DrawText(TextFormat("Leben: %i", player.lives), GetScreenWidth() / 25,
             GetScreenHeight() / 25, GetScreenHeight() / 20, WHITE);
    DrawText(TextFormat("Treffer: %i", player.hits), GetScreenWidth() / 25,
             GetScreenHeight() / 10, GetScreenHeight() / 20, WHITE);

    if (pause) {
      DrawText("GAME OVER!",
               GetScreenWidth() / 2 -
                   MeasureText("GAME OVER!", GetScreenHeight() / 5) / 2,
               GetScreenHeight() / 1.5, GetScreenHeight() / 5, RED);
    }

    BeginMode2D(camera);
    DrawTexturePro(
        puffy,
        (Rectangle){
            .x = is_puffy_blow ? 200 : 0, .y = 0, .width = 200, .height = 200},
        (Rectangle){.x = player.x, .y = player.y, .width = 100, .height = 100},
        (Vector2){50, 50}, player.orientation / PI * 180.0, GRAY);

    for (size_t i = 0; i < BUBBLE_COUNT; ++i) {
      DrawTexturePro(
          bubble, (Rectangle){.x = 0, .y = 0, .width = 60, .height = 82},
          (Rectangle){
              .x = bubbles[i].x, .y = bubbles[i].y, .width = 60, .height = 82},
          (Vector2){0}, 0, GRAY);
    }

    for (size_t i = 0; i < ENEMY_COUNT; ++i) {
      enemy_t *enemy = &enemies[i];
      DrawTexturePro(
          fish_dart, (Rectangle){.x = 0, .y = 0, .width = 39, .height = 20},
          (Rectangle){.x = enemy->x, .y = enemy->y, .width = 39, .height = 20},
          (Vector2){20, 10}, enemy->orientation / PI * 180.0, GRAY);
    }

    EndMode2D();
    EndDrawing();
    x++;
  }

  UnloadTexture(background);
  CloseWindow();
}
