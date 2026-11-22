#include "rsf3d.h"

int main(void)
{
    InitWindow(800, 600, "Window");
    SetTargetFPS(60);
    DisableCursor();

    Shader shader = RF_InitShader();
    Color fogColor = (Color){100, 100, 100, 255};
    RF_SetFog(shader, 0.0f, 10.0f, 0.5f, fogColor, RF_FOG_LINEAR, true); // Set fog parameters

    Camera3D camera = {0};
    camera.position = (Vector3){5.0f, 5.0f, 5.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    
    RF_Light sun = RF_CreateLightDirectional((Vector3){-3.0f, 3.0f, -3.0f}, (Vector3){0.0f, 0.0f, 0.0f}, 
        (Color){15,15,15,255}, (Color){100,100,100,255}, WHITE, 3.0f);
    RF_UpdateLightIntoBuffer(&sun, 0);

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);

        RF_UpdateShader(shader, camera);

        BeginDrawing();
            ClearBackground(fogColor);

            BeginMode3D(camera);

                BeginShaderMode(shader);

                    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){10.0f, 10.0f}, GRAY);
                    DrawCube((Vector3){0.0f, 0.5f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);

                EndShaderMode();

            EndMode3D();

            DrawText("This is a cube with sun in a fog environment (RF_FOG_LINEAR)", 10, 10, 20, WHITE);

        EndDrawing();
    }

    RF_UnloadShader(shader);

    CloseWindow();

    return 0;
}