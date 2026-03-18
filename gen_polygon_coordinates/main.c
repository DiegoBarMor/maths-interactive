#include "math.h"
#include "raylib.h"
#include "raymath.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define VLEN(v) Vector2Length(v)
#define VADD(a, b) Vector2Add(a, b)
#define VSUB(a, b) Vector2Subtract(a, b)
#define VMUL(v, s) Vector2Scale(v, s)

const char *APP_TITLE = "Polygon";
#define VIEWPORT_W 1600.0f
#define VIEWPORT_H 1200.0f
const int WINDOW_WMIN = 400;
const int WINDOW_HMIN = 400;
const int WINDOW_WINIT = 800;
const int WINDOW_HINIT = 600;

const int TARGET_FPS = 60;

#define NVERTICES_MIN 3
#define NVERTICES_START 6
#define NVERTICES_MAX 200

const float DIAMETER_SCREEN_RATIO = 0.5f;
const float RADIUS_POLYGON = DIAMETER_SCREEN_RATIO * VIEWPORT_H * 0.5f;
const float RING_WIDTH = 3.0f;

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// -----------------------------------------------------------------------------
static Vector2 VERTICES[NVERTICES_MAX] = { (Vector2) {0.0f, 0.0f} };
static Vector2 center, vradius, viewport;
static int nvertices;

static RenderTexture2D target;

#if defined(PLATFORM_WEB)
    static char message[50];
    EM_JS(bool, consume_dirty_textinput_flag, (), {
        if (!window.dirty_textinput) return false;
        window.dirty_textinput = false; // consume the flag
        return true;
    });
    EM_JS(char*, get_textinput_str, (), {
        var input = document.getElementById('msg-input');
        return stringToNewUTF8(input.value);
    });
#endif

void update_draw_frame();

// -----------------------------------------------------------------------------
void draw_vertex(Vector2* vertex) {
    DrawCircle((int)vertex->x, (int)vertex->y, 20, RED);
}


// -----------------------------------------------------------------------------
int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    ////////////////////////////// INITIALIZATION //////////////////////////////
    InitWindow(WINDOW_WINIT, WINDOW_HINIT, APP_TITLE);
    // ToggleFullscreen();
    SetWindowMinSize(WINDOW_WMIN, WINDOW_HMIN);

    target = LoadRenderTexture(VIEWPORT_W, VIEWPORT_H);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    SetTargetFPS(TARGET_FPS);

    nvertices = NVERTICES_START;
    // float rotation = 0.0f;

    vradius = (Vector2) {RADIUS_POLYGON, RADIUS_POLYGON};
    viewport = (Vector2) {VIEWPORT_W, VIEWPORT_H};


    ///////////////////////////////// MAIN LOOP ////////////////////////////////
    #if defined(PLATFORM_WEB)
        emscripten_set_main_loop(update_draw_frame, 0, 1);
    #else
        while (!WindowShouldClose()) update_draw_frame();
    #endif

    ///////////////////////////// DE-INITIALIZATION ////////////////////////////
    CloseWindow();
    return 0;
}

// -----------------------------------------------------------------------------
void update_draw_frame() {
    ////////////////////////////////// UPDATE //////////////////////////////////
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        nvertices = MAX(NVERTICES_MIN, nvertices - 1);
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        nvertices = MIN(nvertices + 1, NVERTICES_MAX);

    float framebuffer_scale = MIN(
        (float)GetScreenWidth()  / VIEWPORT_W,
        (float)GetScreenHeight() / VIEWPORT_H
    );
    Vector2 mouse = GetMousePosition();
    Vector2 virtual_mouse = Vector2Clamp(
        (Vector2){
            (mouse.x - (GetScreenWidth()  - (VIEWPORT_W*framebuffer_scale)) * 0.5f) / framebuffer_scale,
            (mouse.y - (GetScreenHeight() - (VIEWPORT_H*framebuffer_scale)) * 0.5f) / framebuffer_scale
        },
        (Vector2){ 0.0f, 0.0f },
        (Vector2){ (float)VIEWPORT_W, (float)VIEWPORT_H }
    );

    center = Vector2Clamp(
        virtual_mouse, vradius, VSUB(viewport, vradius)
    );
    Vector2 xaxis0 = (Vector2) {0.0f, center.y};
    Vector2 xaxis1 = (Vector2) {VIEWPORT_W, center.y};
    Vector2 yaxis0 = (Vector2) {center.x, 0.0f};
    Vector2 yaxis1 = (Vector2) {center.x, VIEWPORT_H};

    for (int i = 0; i <= nvertices; i++) {
        Vector2* vertex = VERTICES + i;
        float angle = 2*3.14 * i / (nvertices);
        vertex->x = center.x + cosf(angle) * RADIUS_POLYGON;
        vertex->y = center.y + sinf(angle) * RADIUS_POLYGON;
    }


    /////////////////////////////////// DRAW ///////////////////////////////////
    BeginTextureMode(target);
        ClearBackground(BLACK);

        DrawLineEx(xaxis0, xaxis1, 5, WHITE);
        DrawLineEx(yaxis0, yaxis1, 5, WHITE);
        DrawRing(center, RADIUS_POLYGON - RING_WIDTH, RADIUS_POLYGON + RING_WIDTH, 0.0f, 360.0f, 1, WHITE);

        Vector2* vertex = VERTICES;
        for (int i = 0; i <= nvertices; ++i, ++vertex) {
            draw_vertex(vertex);
            Vector2* next = i < nvertices ? vertex + 1 : VERTICES;
            DrawLineEx(*vertex, *next, 5, RED);
        }
        draw_vertex(&center);
    EndTextureMode();

    BeginDrawing();
        ClearBackground(GRAY);
        DrawTexturePro(
            target.texture,
            (Rectangle){ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            (Rectangle){
                (GetScreenWidth()  - (VIEWPORT_W * framebuffer_scale))*0.5f,
                (GetScreenHeight() - (VIEWPORT_H * framebuffer_scale))*0.5f,
                VIEWPORT_W * framebuffer_scale,
                VIEWPORT_H * framebuffer_scale
            },
            (Vector2){ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    EndDrawing();
}


// -----------------------------------------------------------------------------
