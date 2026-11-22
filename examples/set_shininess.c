#include "rsf3d.h"

int main(void)
{
    InitWindow(800, 600, "Window");
    SetTargetFPS(60);
    DisableCursor();

    Camera camera = { 0 };
    camera.position = (Vector3){ 5.0f, 5.0f, 5.0f };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Shader shader = RF_InitShader();

    float shininess = 32.0f; // Set the desired shininess value

    RF_Light sun = RF_CreateLightPoint((Vector3){ 0.0f, 10.0f, 0.0f }, (Color){ 255, 255, 255, 255 }, 
        (Color){ 255, 255, 255, 255 }, (Color){ 255, 255, 255, 255 }, 2.0f);
    RF_UpdateLightIntoBuffer(&sun, 0);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdateCamera(&camera, CAMERA_FREE);

        if (IsKeyDown(KEY_UP)) shininess += 30.0f * dt; // Increase shininess
        if (IsKeyDown(KEY_DOWN)) shininess -= 30.0f * dt; // Decrease shininess

        shininess = Clamp(shininess, 1.0f, 256.0f); // Clamp shininess value between 1 and 256

        RF_SetShininess(shader, shininess); // Update the shader with the new shininess value

        RF_UpdateShader(shader, camera); // Update the shader with the current camera parameters

        BeginDrawing();
            ClearBackground(DARKGRAY);

            BeginMode3D(camera);
                
                BeginShaderMode(shader);
                
                    DrawPlane((Vector3){ 0.0f, 0.0f, 0.0f }, (Vector2){ 10.0f, 10.0f }, LIGHTGRAY);
                    DrawSphere((Vector3){ 0.0f, 1.0f, 0.0f }, 1.0f, RED);
                    DrawCube((Vector3){ 3.0f, 1.0f, 0.0f }, 2.0f, 2.0f, 2.0f, BLUE);

                EndShaderMode();

                DrawSphere(sun.position, 0.2f, sun.diffuse); // Draw the light source as a small sphere

            EndMode3D();

            DrawText(TextFormat("Shininess: %.2f", shininess), 10, 10, 20, WHITE);
            DrawText("Use UP/DOWN keys to adjust shininess", 10, 40, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}