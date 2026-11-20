#include "rsf3d.h"

int main(void)
{
    InitWindow(800, 600, "basic light");
    SetTargetFPS(60);
    DisableCursor();

    Camera3D camera = {0};
    camera.position = (Vector3){5.0f, 5.0f, 5.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};

    Shader shader = RF_InitShader();

    RF_Light light = RF_CreateLightPoint((Vector3){-3.0f, 3.0f, -3.0f}, 
        (Color){15,15,15,255}, (Color){100,100,100,255}, WHITE, 3.0f);
    RF_UpdateLightIntoBuffer(&light, 0); // Be in the loop if the light's data is changed a lot

    while (!WindowShouldClose())
    {
        UpdateCamera(&camera, CAMERA_FREE);

        RF_UpdateShader(shader, camera);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);

                BeginShaderMode(shader);

                    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){10.0f, 10.0f}, GRAY);
                    DrawCube((Vector3){0.0f, 0.5f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);

                EndShaderMode();

                DrawSphere(light.position, 0.2f, light.specular);

            EndMode3D();

            DrawText("This is a cube with Blinn-Phong light", 10, 10, 20, WHITE);

        EndDrawing();
    }

    UnloadShader(shader);

    CloseWindow();

    return 0;
}