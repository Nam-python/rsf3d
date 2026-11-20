#include "rsf3d.h"

#define MAX_LIGHT_ARRAY 3

int main(void)
{
    SetTraceLogLevel(LOG_NONE);
    InitWindow(800, 600, "sponza");
    SetTargetFPS(60);
    DisableCursor();

    Camera3D camera = {0};
    camera.position = (Vector3){0.01f, 5.0f, 0.01f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};

    Shader shader = RF_InitShader();
    Color ambient = DARKGRAY;

    RF_Light lights[MAX_LIGHT_ARRAY]; float j = 1.2f; float h = 0.75f, intensity = 0.75f;
    lights[0] = RF_CreateLightPoint((Vector3){6,h,0}, ambient, RED, (Color){255, 0, 0, 255}, intensity);
    lights[1] = RF_CreateLightPoint((Vector3){0,h,0}, ambient, GREEN, (Color){0, 255, 0, 255}, intensity);
    lights[2] = RF_CreateLightPoint((Vector3){-6,h,0}, ambient, BLUE, (Color){0, 0, 255, 255}, intensity);

    for (int i = 0; i < MAX_LIGHT_ARRAY; i++)
    {
        lights[i].constant = 0.8f;
        RF_UpdateLightIntoBuffer(&lights[i], i);
    }

    Model model = LoadModel("resources/model/Sponza.glb");

    for (int i = 0; i < model.materialCount; i++)
    {
        model.materials[i].shader = shader;
        GenTextureMipmaps(&model.materials[i].maps[MATERIAL_MAP_ALBEDO].texture);
        SetTextureFilter(model.materials[i].maps[MATERIAL_MAP_ALBEDO].texture, TEXTURE_FILTER_TRILINEAR);
    }

    RF_Skybox skybox = RF_CreateSkybox();
    skybox.model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture = LoadTexture("resources/image/skybox.png");
    SetTextureFilter(skybox.model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture, TEXTURE_FILTER_BILINEAR);

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

                    DrawModel(model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);

                EndShaderMode();

                for (int i = 0; i < MAX_LIGHT_ARRAY; i++)
                {
                    DrawSphere(lights[i].position, 0.1f, lights[i].specular);
                }

            EndMode3D();

            DrawText("Sponza", 10, 10, 20, RAYWHITE);
            DrawFPS(10, 30);
            DrawText(TextFormat("Camera at (%.3f, %.3f, %.3f)", camera.position.x, camera.position.y, camera.position.z), 10, 50, 20, RAYWHITE);

        EndDrawing();
    }

    UnloadShader(shader);

    UnloadModel(model);

    CloseWindow();

    return 0;
}