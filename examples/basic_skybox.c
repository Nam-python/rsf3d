#include "rsf3d.h"

int main(void)
{
    InitWindow(800, 600, "basic skybox");
    SetTargetFPS(60);
    DisableCursor();

    Camera3D camera = {0};
    camera.position = (Vector3){5.0f, 5.0f, 5.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};

    Shader shader = RF_InitShader();

    RF_Skybox skybox = RF_CreateSkybox();
    skybox.axis = (Vector3){0,1,0};
    skybox.model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("resources/image/skybox.png");
    SetTextureFilter(skybox.model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture, TEXTURE_FILTER_BILINEAR);
    skybox.energy = 0.9f;

    RF_Light light = RF_CreateLightPoint((Vector3){20.0f, 20.0f, -5.0f}, 
        (Color){15,15,15,255}, (Color){100,100,100,255}, WHITE, 100.0f);

    light.quadratic = 0.09f;

    RF_UpdateLightIntoBuffer(&light, 0); // Be in the loop if the light's data is changed a lot

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        UpdateCamera(&camera, CAMERA_FREE);

        RF_UpdateShader(shader, camera);

        BeginDrawing();
            ClearBackground(BLACK);

            BeginMode3D(camera);

                RF_DrawSkybox(skybox, camera);

                BeginShaderMode(shader);

                    DrawPlane((Vector3){0.0f, 0.0f, 0.0f}, (Vector2){10.0f, 10.0f}, GRAY);
                    DrawCube((Vector3){0.0f, 0.5f, 0.0f}, 1.0f, 1.0f, 1.0f, RED);

                EndShaderMode();

            EndMode3D();

            DrawText("This is a cube in the skybox", 10, 10, 20, WHITE);

        EndDrawing();
    }

    RF_UnloadSkybox(skybox);
    UnloadShader(shader);

    CloseWindow();

    return 0;
}