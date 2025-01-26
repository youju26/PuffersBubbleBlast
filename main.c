#include <math.h>
#include <raylib.h>
#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define ENEMY_COUNT 8
#define BUBBLE_COUNT 8
#define AIR_BUBBLE_COUNT 16

enum ENEMY {
  ENEMY_FISH,
  ENEMY_FISH_BIG,
  ENEMY_FISH_DART,
  ENEMY_TYPE_LEN,
  ENEMY_MINE,
  ENEMY_MINE_BIG,
  ENEMY_MINE_SMALL,
};

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

typedef struct {
  Texture2D texture;
  int width, height, frames, index;
  float duration;
} animation_t;

static const int screenWidth = 800;
static const int screenHeight = 450;

static bool pause = false;
static bool is_puffy_blow = false;

static bubble_t bubbles[BUBBLE_COUNT] = {0};
static size_t bubble_index = 0;

static enemy_t enemies[ENEMY_COUNT] = {0};

static Vector2 air_bubbles[AIR_BUBBLE_COUNT] = {0};
static size_t air_bubble_index = 0;

static player_t entity_player = {.x = 350.0,
                                 .y = 200.0,
                                 .orientation = 0,
                                 .speed_x = 0,
                                 .speed_y = 0,
                                 .radius = 50,
                                 .lives = 5,
                                 .hits = 0};

static Texture2D texture_background, texture_midground, texture_puffy,
    texture_bubble, texture_fish, texture_fish_big, texture_fish_dart,
    texture_air_bubble, texture_particle;

static Sound sound_ambient, sound_bubble, sound_hurt, sound_puffy_death,
    sound_fish_dying;

static Camera2D camera = {0};

void InitTextures() {
  texture_background = LoadTexture("assets/environment/background.png");
  texture_midground = LoadTexture("assets/environment/midground.png");
  texture_puffy = LoadTexture("assets/puffy.png");
  texture_bubble = LoadTexture("assets/FX/explosion.png");
  texture_fish = LoadTexture("assets/enemies/fish.png");
  texture_fish_big = LoadTexture("assets/enemies/fish-big.png");
  texture_fish_dart = LoadTexture("assets/enemies/fish-dart.png");
  texture_air_bubble = LoadTexture("assets/FX/Power_template_1.png");
  texture_particle = LoadTexture("assets/FX/particles.png");

  SetTextureWrap(texture_background, TEXTURE_WRAP_REPEAT);
  SetTextureWrap(texture_midground, TEXTURE_WRAP_REPEAT);
  SetTextureWrap(texture_particle, TEXTURE_WRAP_REPEAT);
}

void InitSounds() {
  while (!IsAudioDeviceReady())
    ;
  sound_ambient = LoadSound("assets/sounds/magical_theme.ogg");
  sound_bubble = LoadSound("assets/sounds/bubble_01.ogg");
  sound_hurt = LoadSound("assets/sounds/FEMALE_Gruntwork_1.ogg");
  sound_puffy_death = LoadSound("assets/sounds/puffy_death.ogg");
  sound_fish_dying = LoadSound("assets/sounds/slime_04.ogg");
}

void InitEnemy(enemy_t *enemy) {
  enemy->type = GetRandomValue(ENEMY_FISH, ENEMY_TYPE_LEN - 1);
  enemy->x = entity_player.x - GetScreenWidth() / 2 +
             GetRandomValue(0, 1) * GetScreenWidth();
  enemy->y = entity_player.y - GetScreenHeight() / 2 +
             GetRandomValue(0, 1) * GetScreenHeight();
  enemy->orientation = GetRandomValue(0, 2 * PI), enemy->onPlayer = false;
  switch (enemy->type) {
  case ENEMY_FISH:
    enemy->radius = 10;
    break;
  case ENEMY_FISH_BIG:
    enemy->radius = 20;
    break;
  case ENEMY_FISH_DART:
    enemy->radius = 10;
    break;
  }
}

void InitCamera() {
  camera.target = (Vector2){entity_player.x + 20.0f, entity_player.y + 20.0f};
  camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
}

void Init() {
  InitWindow(screenWidth, screenHeight,
             "raylib [models] example - geometric shapes");
  InitTextures();
  InitAudioDevice();
  for (size_t i = 0; i < ENEMY_COUNT; ++i) {
    InitEnemy(&enemies[i]);
  }
  InitCamera();

  InitSounds();
  PlaySound(sound_ambient);
}

void UpdatePlayer(player_t *player) {
  is_puffy_blow = IsKeyDown(KEY_SPACE) || IsMouseButtonDown(0);
  int mouse_x = GetMouseX();
  int mouse_y = GetMouseY();
  float speed = 60000;

  entity_player.orientation =
      atan2f(mouse_y - camera.offset.y, mouse_x - camera.offset.x);
  if (IsMouseButtonPressed(0)) {
    PlaySound(sound_bubble);

    // PLAYER IMPULSE IN OPPOSITE DIRECTION
    entity_player.speed_x +=
        cosf(entity_player.orientation + PI) * speed * GetFrameTime();
    entity_player.speed_y +=
        sinf(entity_player.orientation + PI) * speed * GetFrameTime();

    // SPAWN BUBBLE
    bubble_index = (bubble_index + 1) % 8;
    bubbles[bubble_index] = (bubble_t){
        .x = entity_player.x - 25 + cosf(entity_player.orientation) * 40,
        .y = entity_player.y - 50 + sinf(entity_player.orientation) * 40,
        .orientation = entity_player.orientation,
        .radius = 30};
  }

  // DEACCELERATION
  entity_player.speed_x *= powf(0.7, GetFrameTime());
  entity_player.speed_y *= powf(0.7, GetFrameTime());

  // UPDATE PLAYER POSITION
  entity_player.x += entity_player.speed_x * GetFrameTime();
  entity_player.y += entity_player.speed_y * GetFrameTime();
}

void UpdateBubble(bubble_t *bubble) {
  // UPDATE BUBBLE POSITION IN DIRECTION
  bubble->x += cosf(bubble->orientation) * GetFrameTime() * 200;
  bubble->y += sinf(bubble->orientation) * GetFrameTime() * 200;

  Vector2 bubble_vec = {bubble->x + bubble->radius, bubble->y + bubble->radius};

  // CHECK FOR COLLISIONS WITH FISH
  for (size_t i = 0; i < ENEMY_COUNT; ++i) {
    Vector2 enemy_vec = {enemies[i].x + enemies[i].radius,
                         enemies[i].y + enemies[i].radius};
    bool collision = CheckCollisionCircles(enemy_vec, enemies[i].radius,
                                           bubble_vec, bubble->radius);
    if (collision) {
      for (size_t ii = 0; ii < 4; ii++) {
        air_bubble_index = (air_bubble_index + 1) % AIR_BUBBLE_COUNT;
        air_bubbles[air_bubble_index] = (Vector2){
            .x = enemies[i].x + GetRandomValue(-20, 20),
            .y = enemies[i].y + GetRandomValue(-20, 20),
        };
      }
      InitEnemy(&enemies[i]);
      entity_player.hits += 1;
      PlaySound(sound_fish_dying);
    }
  }
}

void UpdateAirBubble(Vector2 *bubble) { bubble->y -= GetFrameTime() * 50; }

void UpdateEnemy(enemy_t *enemy) {
  Vector2 player_vec = {.x = entity_player.x + entity_player.radius,
                        .y = entity_player.y + entity_player.radius};

  Vector2 enemy_vec = {.x = enemy->x + enemy->radius,
                       .y = enemy->y + enemy->radius};

  bool collision = CheckCollisionCircles(enemy_vec, enemy->radius, player_vec,
                                         entity_player.radius);
  if (!pause && collision && (collision != enemy->onPlayer)) {
    if (entity_player.lives == 0) {
      pause = true;

      PlaySound(sound_puffy_death);

      for (size_t ii = 0; ii < 4; ii++) {
        air_bubble_index = (air_bubble_index + 1) % AIR_BUBBLE_COUNT;
        air_bubbles[air_bubble_index] = (Vector2){
            .x = entity_player.x + GetRandomValue(-20, 20),
            .y = entity_player.y + GetRandomValue(-20, 20),
        };
      }
    } else {
      entity_player.lives -= 1;
      enemy->onPlayer = true;

      PlaySound(sound_hurt);
    }
  } else {
    enemy->onPlayer = collision;
  }

  float speed = 50;

  enemy->orientation += GetRandomValue(-1, 1) / 180.0 * PI;
  float player_direction =
      atan2f(entity_player.y - enemy->y, entity_player.x - enemy->x);

  bool is_player_in_view =
      fabs(fmod(player_direction - enemy->orientation, 2 * PI)) / PI * 180.0 <=
      30.0;

  // CHARGE TO PLAYER IF IN VIEW
  if (is_player_in_view) {
    enemy->orientation = player_direction;
    speed = 250;
  }

  // UPDATE POSITION IN DIRECTION
  enemy->x += cosf(enemy->orientation) * GetFrameTime() * speed;
  enemy->y += sinf(enemy->orientation) * GetFrameTime() * speed;
}

void Update() {

  for (size_t i = 0; i < BUBBLE_COUNT; ++i) {
    UpdateBubble(&bubbles[i]);
  }

  for (size_t i = 0; i < ENEMY_COUNT; ++i) {
    UpdateEnemy(&enemies[i]);
  }

  for (size_t i = 0; i < AIR_BUBBLE_COUNT; ++i) {
    UpdateAirBubble(&air_bubbles[i]);
  }

  if (pause)
    return;

  UpdatePlayer(&entity_player);

  camera.target = (Vector2){entity_player.x + 20, entity_player.y + 20};
  camera.offset = (Vector2){GetScreenWidth() / 2, GetScreenHeight() / 2};
}

void DrawEnemy(enemy_t *enemy) {
  Texture2D texture;
  int width, height;
  switch (enemy->type) {
  case ENEMY_FISH:
    texture = texture_fish;
    width = 32;
    height = 32;
    break;
  case ENEMY_FISH_BIG:
    texture = texture_fish_big;
    width = 54;
    height = 49;
    break;
  case ENEMY_FISH_DART:
    texture = texture_fish_dart;
    width = 39;
    height = 20;
    break;
  }
  DrawTexturePro(texture,
                 (Rectangle){.x = 0, .y = 0, .width = width, .height = height},
                 (Rectangle){.x = enemy->x,
                             .y = enemy->y,
                             .width = width * 2,
                             .height = height * 2},
                 (Vector2){20, 10}, enemy->orientation / PI * 180.0, GRAY);
}

void Draw() {
  // BACKGROUND
  DrawTexturePro(
      texture_background,
      (Rectangle){
          .x = entity_player.x / 20, .y = 0, .width = 288, .height = 256},
      (Rectangle){.x = 0,
                  .y = 0,
                  .width = GetScreenWidth(),
                  .height = GetScreenHeight()},
      (Vector2){0}, 0.f, GRAY);
  // MIDGROUND
  DrawTexturePro(
      texture_midground,
      (Rectangle){
          .x = entity_player.x / 3, .y = 0, .width = 960, .height = 512},
      (Rectangle){.x = 0,
                  .y = 0,
                  .width = GetScreenWidth(),
                  .height = GetScreenHeight()},
      (Vector2){0}, 0.f, GRAY);
  // PARTICLES
  DrawTexturePro(texture_particle,
                 (Rectangle){.x = entity_player.x / 5,
                             .y = entity_player.y / 5,
                             .width = GetScreenWidth() / 5,
                             .height = GetScreenHeight() / 5},
                 (Rectangle){.x = 0,
                             .y = 0,
                             .width = GetScreenWidth(),
                             .height = GetScreenHeight()},
                 (Vector2){0}, 0.f, GRAY);

  // STATS
  DrawText(TextFormat("Leben: %i", entity_player.lives), GetScreenWidth() / 25,
           GetScreenHeight() / 25, GetScreenHeight() / 20, WHITE);
  DrawText(TextFormat("Treffer: %i", entity_player.hits), GetScreenWidth() / 25,
           GetScreenHeight() / 10, GetScreenHeight() / 20, WHITE);

  // GAMEOVER
  if (pause) {
    DrawText("GAME OVER!",
             GetScreenWidth() / 2 -
                 MeasureText("GAME OVER!", GetScreenHeight() / 5) / 2,
             GetScreenHeight() / 1.5, GetScreenHeight() / 5, RED);
  }

  BeginMode2D(camera);
  // PLAYER PUFFY
  DrawTexturePro(
      texture_puffy,
      (Rectangle){
          .x = is_puffy_blow ? 200 : 0, .y = 0, .width = 200, .height = 200},
      (Rectangle){.x = entity_player.x,
                  .y = entity_player.y,
                  .width = 100,
                  .height = 100},
      (Vector2){50, 50}, entity_player.orientation / PI * 180.0, GRAY);

  // BUBBLES
  for (size_t i = 0; i < BUBBLE_COUNT; ++i) {
    DrawTexturePro(
        texture_bubble, (Rectangle){.x = 0, .y = 0, .width = 60, .height = 82},
        (Rectangle){
            .x = bubbles[i].x, .y = bubbles[i].y, .width = 60, .height = 82},
        (Vector2){0}, 0, GRAY);
  }

  // ENEMIES
  for (size_t i = 0; i < ENEMY_COUNT; ++i) {
    DrawEnemy(&enemies[i]);
  }
  // ENEMIES
  for (size_t i = 0; i < AIR_BUBBLE_COUNT; ++i) {
    Vector2 *bubble = &air_bubbles[i];
    DrawTextureEx(texture_air_bubble, *bubble, 0, .5, GRAY);
  }

  EndMode2D();
}

void UpdateDrawFrame(void) {
  Update();

  BeginDrawing();
  Draw();
  EndDrawing();
}

int main() {
  Init();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  UnloadTexture(texture_background);
  CloseWindow();

  return 0;
}
