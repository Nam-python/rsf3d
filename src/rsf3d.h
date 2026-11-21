/*
    RFS3D (RF) - Raylib Shader Forward 3D
    Programming languages: C/C++
    Required C standard: C99
    Required OpenGL version: OpenGL 3.3+

    License:

        MIT License

        Copyright (c) 2026 Phan Trịnh Bảo Nam

        Permission is hereby granted, free of charge, to any person obtaining a copy
        of this software and associated documentation files (the "Software"), to deal
        in the Software without restriction, including without limitation the rights
        to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        copies of the Software, and to permit persons to whom the Software is
        furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be included in all
        copies or substantial portions of the Software.

        THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        SOFTWARE.
*/

#ifndef RSF3D_H
#define RSF3D_H

#include "external/raylib.h"
#include "external/rlgl.h"

#if !defined(__cplusplus)
    #include <stdbool.h>
#endif

#include <stdlib.h> // for NULL

#define MAX_LIGHTS 16
#define RF_MAX_LIGHTS MAX_LIGHTS

#if defined(__cplusplus)
extern "C"
{
#endif

enum RF_LightType
{
    RF_LIGHT_POINT = 0,
    RF_LIGHT_DIRECTIONAL,
    RF_LIGHT_SPOT
};

enum RF_ShadingModel
{
    RF_SHADING_MODEL_BLINN_PHONG = 0,
    RF_SHADING_MODEL_GOURAUD
};

enum RF_FogMode
{
    RF_FOG_LINEAR = 0,
    RF_FOG_EXP, // Exponential fog
    RF_FOG_EXP2 // Exponential squared fog
};

typedef struct RF_Light
{
    Vector3 position;
    Vector3 target;

    Color ambient, diffuse, specular;

    float intensity, constant, linear, quadratic;

    float cutOff; // must be degrees
    float outerCutOff; // must be degrees

    enum RF_LightType mode;
    bool enabled;

} RF_Light;

typedef struct RF_Skybox
{
    Shader shader;
    Model model;
    Vector3 axis;
    float angle;
    float energy;

    int energyLoc;

} RF_Skybox;

Shader RF_InitShader(void); // Initialize RSF3D shader. NOTE: Must be called after InitWindow() and do not call twice
void RF_UpdateShader(Shader shader, Camera3D camera); // Update RSF3D shader, must be called every frame before drawing 3D objects
void RF_UnloadShader(Shader shader); // Unload RSF3D shader, must be called after CloseWindow() and after drawing 3D objects. NOTE: shader must be the same as the one returned by RF_InitShader()

// SHADING MODEL
void RF_SetShadingModel(Shader shader, enum RF_ShadingModel shadingMode); // Set shading model, default is Blinn-Phong

// LIGHT
RF_Light RF_CreateLightPro(Vector3 position, Vector3 target, 
    Color ambient, Color diffuse, Color specular, // Colors
    float intensity, float constant, float linear, float quadratic, // Attenuation
    float cutOff, float outerCutOff, // Spot
    enum RF_LightType mode, bool enabled); // Create light pro

RF_Light RF_CreateLightPoint(Vector3 position, Color ambient, Color diffuse, Color specular, float intensity); // Create point light
RF_Light RF_CreateLightDirectional(Vector3 position, Vector3 target, Color ambient, Color diffuse, Color specular, float intensity); // Create directional light
RF_Light RF_CreateLightSpot(Vector3 position, Vector3 target, Color ambient, Color diffuse, Color specular, float cutOff, float outerCutOff, float intensity); // Create spot light

void RF_UpdateLightIntoBuffer(const RF_Light* light, int index); // Update light into RSF3D state's buffer (should be in the loop if the light's data is changed a lot)

// SKYBOX
RF_Skybox RF_CreateSkybox(void); // Create skybox, must be called after RF_InitShader() and before drawing 3D objects
void RF_UnloadSkybox(RF_Skybox skybox); // Unload skybox
void RF_DrawSkybox(RF_Skybox skybox, Camera3D camera); // Draw skybox, must be called after RF_UpdateShader() and before drawing 3D objects

// FOG
void RF_SetFog(Shader shader, float start, float end, float density, Color color, enum RF_FogMode mode); // Set fog parameters

static int rfLightCount = 0;
static struct
{
    struct
    {
        // POSITION
        Vector3* positions;
        Vector3* targets;

        // COLOR
        Vector3* ambientColors;
        Vector3* diffuseColors;
        Vector3* specularColors;

        // LIGHT'S DETAILS
        float* intensity;
        float* constant; // for attenuation
        float* linear; // for attenuation
        float* quadratic; // for attenuation

        float* cutOff;
        float* outerCutOff;

        int* mode;
        int* enabled;

        int posLoc, tarLoc, ambientLoc, difLoc, specularLoc, 
            intensityLoc, constantLoc, linearLoc, quadrLoc,
            modeLoc, enabledLoc, cutOffLoc, outerCutOffLoc;

        int viewLoc;

        int shadingModelLoc;

    } lights;
    
    struct
    {
        float start;
        float end;
        float density;
        Color color;
        enum RF_FogMode mode;

        int startLoc, endLoc, densityLoc, colorLoc, modeLoc;

    } fog;

} RSF3D_State;

Shader RF_InitShader(void)
{
    const char *vertSrc =
        "#version 330\n"
        "#define MAX_LIGHTS 16\n"

        "in vec3 vertexPosition;\n"
        "in vec4 vertexColor;\n"
        "in vec2 vertexTexCoord;\n"
        "in vec3 vertexNormal;\n"

        "uniform mat4 mvp;\n"
        "uniform mat4 matModel;\n"

        "uniform vec3 viewPos;\n"

        "uniform vec3 lightsPosition[MAX_LIGHTS];\n"
        "uniform vec3 lightsTargets[MAX_LIGHTS];\n"

        "uniform vec3 lightsAmbient[MAX_LIGHTS];\n"
        "uniform vec3 lightsDiffuse[MAX_LIGHTS];\n"
        "uniform vec3 lightsSpecular[MAX_LIGHTS];\n"

        "uniform float lightsIntensity[MAX_LIGHTS];\n"

        "uniform float lightsConstant[MAX_LIGHTS];\n"
        "uniform float lightsLinear[MAX_LIGHTS];\n"
        "uniform float lightsQuadratic[MAX_LIGHTS];\n"

        "uniform float lightsCutOff[MAX_LIGHTS];\n"
        "uniform float lightsOuterCutOff[MAX_LIGHTS];\n"

        "uniform int lightsMode[MAX_LIGHTS];\n"
        "uniform int lightsEnabled[MAX_LIGHTS];\n"

        "uniform int shadingModel;\n"

        "out vec3 fragPosition;\n"
        "out vec3 fragNormal;\n"
        "out vec2 fragTexCoord;\n"
        "out vec4 fragColor;\n"

        "vec3 CalculateLighting(vec3 position, vec3 normal)\n"
        "{\n"
        "    vec3 viewDir = normalize(viewPos - position);\n"
        "    vec3 result = vec3(0.0);\n"

        "    for (int i = 0; i < MAX_LIGHTS; i++)\n"
        "    {\n"
        "        if (lightsEnabled[i] == 0) continue;\n"

        "        vec3 lightDir;\n"
        "        float attenuation = 1.0;\n"

        "        if (lightsMode[i] == 1)\n"
        "        {\n"
        "            lightDir = normalize(lightsPosition[i] - lightsTargets[i]);\n"
        "        }\n"
        "        else\n"
        "        {\n"
        "            vec3 delta = lightsPosition[i] - position;\n"
        "            float distance = length(delta);\n"
        "            lightDir = normalize(delta);\n"

        "            attenuation = 1.0 / max(\n"
        "                lightsConstant[i] +\n"
        "                lightsLinear[i] * distance +\n"
        "                lightsQuadratic[i] * distance * distance,\n"
        "                0.0001);\n"

        "            if (lightsMode[i] == 2)\n"
        "            {\n"
        "                vec3 spotDir = normalize(lightsTargets[i] - lightsPosition[i]);\n"
        "                vec3 fragDir = normalize(position - lightsPosition[i]);\n"

        "                float theta = dot(spotDir, fragDir);\n"
        "                float innerCutoff = cos(radians(lightsCutOff[i]));\n"
        "                float outerCutoff = cos(radians(lightsOuterCutOff[i]));\n"

        "                float epsilon = innerCutoff - outerCutoff;\n"
        "                float spotIntensity = clamp(\n"
        "                    (theta - outerCutoff) / max(epsilon, 0.0001),\n"
        "                    0.0, 1.0);\n"

        "                attenuation *= spotIntensity;\n"
        "            }\n"
        "        }\n"

        "        float diff = max(dot(normal, lightDir), 0.0);\n"

        "        vec3 halfwayDir = normalize(lightDir + viewDir);\n"
        "        float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);\n"

        "        vec3 ambient = lightsAmbient[i];\n"
        "        vec3 diffuse = lightsDiffuse[i] * diff;\n"
        "        vec3 specular = lightsSpecular[i] * spec;\n"

        "        result += (ambient + diffuse + specular) * attenuation * lightsIntensity[i];\n"
        "    }\n"

        "    return result;\n"
        "}\n"

        "void main()\n"
        "{\n"
        "    vec4 worldPos = matModel * vec4(vertexPosition, 1.0);\n"

        "    fragPosition = worldPos.xyz;\n"

        "    mat3 normalMatrix = mat3(transpose(inverse(matModel)));\n"
        "    fragNormal = normalize(normalMatrix * vertexNormal);\n"

        "    fragTexCoord = vertexTexCoord;\n"

        "    vec3 finalColor = vec3(1.0);\n"

        "    if (shadingModel == 1)\n"
        "    {\n"
        "        // Gouraud shading: calculate lighting per vertex\n"
        "        finalColor = CalculateLighting(\n"
        "            fragPosition,\n"
        "            normalize(fragNormal)\n"
        "        );\n"
        "    }\n"

        "    // Pass lighting to fragment shader\n"
        "    fragColor = vec4(finalColor * vertexColor.rgb, vertexColor.a);\n"

        "    gl_Position = mvp * vec4(vertexPosition, 1.0);\n"
        "}\n";

    const char *fragSrc =
        "#version 330\n"
        "#define MAX_LIGHTS 16\n"

        "in vec3 fragPosition;\n"
        "in vec3 fragNormal;\n"
        "in vec2 fragTexCoord;\n"
        "in vec4 fragColor;\n"

        "out vec4 finalColor;\n"

        "uniform vec3 viewPos;\n"
        "uniform vec4 colDiffuse;\n"
        "uniform sampler2D texture0;\n"

        "uniform vec3 lightsPosition[MAX_LIGHTS];\n"
        "uniform vec3 lightsTargets[MAX_LIGHTS];\n"

        "uniform vec3 lightsAmbient[MAX_LIGHTS];\n"
        "uniform vec3 lightsDiffuse[MAX_LIGHTS];\n"
        "uniform vec3 lightsSpecular[MAX_LIGHTS];\n"

        "uniform float lightsIntensity[MAX_LIGHTS];\n"

        "uniform float lightsConstant[MAX_LIGHTS];\n"
        "uniform float lightsLinear[MAX_LIGHTS];\n"
        "uniform float lightsQuadratic[MAX_LIGHTS];\n"

        "uniform float lightsCutOff[MAX_LIGHTS];\n"
        "uniform float lightsOuterCutOff[MAX_LIGHTS];\n"

        "uniform int lightsMode[MAX_LIGHTS];\n"
        "uniform int lightsEnabled[MAX_LIGHTS];\n"

        "uniform int shadingModel;\n"

        "uniform float fogStart;\n"
        "uniform float fogEnd;\n"
        "uniform vec4 fogColor;\n"
        "uniform int fogMode;\n"
        "uniform float fogDensity;\n"

        "void main()\n"
        "{\n"
        "    vec4 texColor = texture(texture0, fragTexCoord) * colDiffuse;\n"
        "    if (texColor.a < 0.5) discard;"
        "    vec3 lighting = vec3(0.0);\n"

        "    if (shadingModel == 1)\n"
        "    {\n"
        "        // Gouraud shading\n"
        "        lighting = fragColor.rgb;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        // Blinn-Phong shading\n"
        "        vec3 normal = normalize(fragNormal);\n"
        "        vec3 viewDir = normalize(viewPos - fragPosition);\n"

        "        for (int i = 0; i < MAX_LIGHTS; i++)\n"
        "        {\n"
        "            if (lightsEnabled[i] == 0) continue;\n"

        "            vec3 lightDir;\n"
        "            float attenuation = 1.0;\n"

        "            if (lightsMode[i] == 1)\n"
        "            {\n"
        "                lightDir = normalize(lightsPosition[i] - lightsTargets[i]);\n"
        "            }\n"
        "            else\n"
        "            {\n"
        "                vec3 delta = lightsPosition[i] - fragPosition;\n"
        "                float distance = length(delta);\n"
        "                lightDir = normalize(delta);\n"

        "                attenuation = 1.0 / max(\n"
        "                    lightsConstant[i] +\n"
        "                    lightsLinear[i] * distance +\n"
        "                    lightsQuadratic[i] * distance * distance,\n"
        "                    0.0001);\n"

        "                if (lightsMode[i] == 2)\n"
        "                {\n"
        "                    vec3 spotDir = normalize(lightsTargets[i] - lightsPosition[i]);\n"
        "                    vec3 fragDir = normalize(fragPosition - lightsPosition[i]);\n"

        "                    float theta = dot(spotDir, fragDir);\n"
        "                    float innerCutoff = cos(radians(lightsCutOff[i]));\n"
        "                    float outerCutoff = cos(radians(lightsOuterCutOff[i]));\n"

        "                    float epsilon = innerCutoff - outerCutoff;\n"
        "                    float spotIntensity = clamp(\n"
        "                        (theta - outerCutoff) / max(epsilon, 0.0001),\n"
        "                        0.0, 1.0);\n"

        "                    attenuation *= spotIntensity;\n"
        "                }\n"
        "            }\n"

        "            float diff = max(dot(normal, lightDir), 0.0);\n"

        "            vec3 halfwayDir = normalize(lightDir + viewDir);\n"
        "            float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);\n"

        "            vec3 ambient = lightsAmbient[i];\n"
        "            vec3 diffuse = lightsDiffuse[i] * diff;\n"
        "            vec3 specular = lightsSpecular[i] * spec;\n"

        "            lighting += (ambient + diffuse + specular) * attenuation * lightsIntensity[i];\n"
        "        }\n"
        "    }\n"

        "    vec3 surfaceColor;\n"
        "\n"
        "    if (shadingModel == 1)\n"
        "    {\n"
        "        // Gouraud shading\n"
        "        surfaceColor = texColor.rgb * fragColor.rgb;\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        // Blinn-Phong shading\n"
        "        surfaceColor = texColor.rgb * lighting * fragColor.rgb;\n"
        "    }\n"
        "\n"
        "    float distanceToCamera = length(fragPosition - viewPos);\n"
        "    float fogFactor = 1.0;\n"
        "\n"
        "    if (fogMode == 0) // Linear\n"
        "    {\n"
        "        fogFactor = (fogEnd - distanceToCamera) /\n"
        "                    max(fogEnd - fogStart, 0.0001);\n"
        "    }\n"
        "    else if (fogMode == 1) // EXP\n"
        "    {\n"
        "        fogFactor = exp(-fogDensity * distanceToCamera);\n"
        "    }\n"
        "    else if (fogMode == 2) // EXP2\n"
        "    {\n"
        "        float d = fogDensity * distanceToCamera;\n"
        "        fogFactor = exp(-(d * d));\n"
        "    }\n"
        "\n"
        "    fogFactor = clamp(fogFactor, 0.0, 1.0);\n"
        "\n"
        "    vec4 result = mix(fogColor, vec4(surfaceColor, texColor.a), fogFactor);\n"
        "    finalColor = result;\n"
        "}\n";

    // TraceLog(LOG_INFO, "Frag code: %s", fragSrc);

    Shader shader = LoadShaderFromMemory(vertSrc, fragSrc);

    // POSITION
    RSF3D_State.lights.posLoc = GetShaderLocation(shader, "lightsPosition");

    // TARGET
    RSF3D_State.lights.tarLoc = GetShaderLocation(shader, "lightsTargets");

    // COLORS
    RSF3D_State.lights.ambientLoc = GetShaderLocation(shader, "lightsAmbient");
    RSF3D_State.lights.difLoc = GetShaderLocation(shader, "lightsDiffuse");
    RSF3D_State.lights.specularLoc = GetShaderLocation(shader, "lightsSpecular");

    // INTENSITY
    RSF3D_State.lights.intensityLoc = GetShaderLocation(shader, "lightsIntensity");

    // ATTENUATION
    RSF3D_State.lights.constantLoc = GetShaderLocation(shader, "lightsConstant");
    RSF3D_State.lights.linearLoc = GetShaderLocation(shader, "lightsLinear");
    RSF3D_State.lights.quadrLoc = GetShaderLocation(shader, "lightsQuadratic");

    // SPOTLIGHT
    RSF3D_State.lights.cutOffLoc = GetShaderLocation(shader, "lightsCutOff");
    RSF3D_State.lights.outerCutOffLoc = GetShaderLocation(shader, "lightsOuterCutOff");

    // LIGHT DETAILS
    RSF3D_State.lights.modeLoc = GetShaderLocation(shader, "lightsMode");
    RSF3D_State.lights.enabledLoc = GetShaderLocation(shader, "lightsEnabled");
    RSF3D_State.lights.viewLoc = GetShaderLocation(shader, "viewPos");

    // SHADING MODEL
    RSF3D_State.lights.shadingModelLoc = GetShaderLocation(shader, "shadingModel");
    RF_SetShadingModel(shader, RF_SHADING_MODEL_BLINN_PHONG); // Blinn-Phong shading model is default

    RSF3D_State.lights.positions = (Vector3*)MemAlloc(sizeof(Vector3) * RF_MAX_LIGHTS);
    RSF3D_State.lights.targets = (Vector3*)MemAlloc(sizeof(Vector3) * RF_MAX_LIGHTS);
    RSF3D_State.lights.ambientColors = (Vector3*)MemAlloc(sizeof(Vector3) * RF_MAX_LIGHTS);
    RSF3D_State.lights.diffuseColors = (Vector3*)MemAlloc(sizeof(Vector3) * RF_MAX_LIGHTS);
    RSF3D_State.lights.specularColors = (Vector3*)MemAlloc(sizeof(Vector3) * RF_MAX_LIGHTS);
    RSF3D_State.lights.intensity = (float*)MemAlloc(sizeof(float) * RF_MAX_LIGHTS);
    RSF3D_State.lights.constant = (float*)MemAlloc(sizeof(float) * RF_MAX_LIGHTS);
    RSF3D_State.lights.linear = (float*)MemAlloc(sizeof(float) * RF_MAX_LIGHTS);
    RSF3D_State.lights.quadratic = (float*)MemAlloc(sizeof(float) * RF_MAX_LIGHTS);
    RSF3D_State.lights.cutOff = (float*)MemAlloc(sizeof(float) * RF_MAX_LIGHTS);
    RSF3D_State.lights.outerCutOff = (float*)MemAlloc(sizeof(float) * RF_MAX_LIGHTS);
    RSF3D_State.lights.mode = (int*)MemAlloc(sizeof(int) * RF_MAX_LIGHTS);
    RSF3D_State.lights.enabled = (int*)MemAlloc(sizeof(int) * RF_MAX_LIGHTS);

    RSF3D_State.fog.startLoc = GetShaderLocation(shader, "fogStart");
    RSF3D_State.fog.endLoc = GetShaderLocation(shader, "fogEnd");
    RSF3D_State.fog.densityLoc = GetShaderLocation(shader, "fogDensity");
    RSF3D_State.fog.colorLoc = GetShaderLocation(shader, "fogColor");
    RSF3D_State.fog.modeLoc = GetShaderLocation(shader, "fogMode");

    RF_SetFog(shader, 0.0f, 100.0f, 0.5f, BLANK, RF_FOG_LINEAR); // Default fog is linear fog, with start = 0.0f, end = 100.0f, density = 0.5f, and color = BLANK (transparent)

    return shader;
}

void RF_UpdateShader(Shader shader, Camera3D camera)
{
    // POSITION
    SetShaderValueV(shader, RSF3D_State.lights.posLoc, RSF3D_State.lights.positions, SHADER_UNIFORM_VEC3, RF_MAX_LIGHTS);

    // TARGET
    SetShaderValueV(shader, RSF3D_State.lights.tarLoc, RSF3D_State.lights.targets, SHADER_UNIFORM_VEC3, RF_MAX_LIGHTS);

    // COLORS
    SetShaderValueV(shader, RSF3D_State.lights.ambientLoc, RSF3D_State.lights.ambientColors, SHADER_UNIFORM_VEC3, RF_MAX_LIGHTS);
    SetShaderValueV(shader, RSF3D_State.lights.difLoc, RSF3D_State.lights.diffuseColors, SHADER_UNIFORM_VEC3, RF_MAX_LIGHTS);
    SetShaderValueV(shader, RSF3D_State.lights.specularLoc, RSF3D_State.lights.specularColors, SHADER_UNIFORM_VEC3, RF_MAX_LIGHTS);

    // INTENSITY
    SetShaderValueV(shader, RSF3D_State.lights.intensityLoc, RSF3D_State.lights.intensity, SHADER_UNIFORM_FLOAT, RF_MAX_LIGHTS);

    // ATTENUATION
    SetShaderValueV(shader, RSF3D_State.lights.constantLoc, RSF3D_State.lights.constant, SHADER_UNIFORM_FLOAT, RF_MAX_LIGHTS);
    SetShaderValueV(shader, RSF3D_State.lights.linearLoc, RSF3D_State.lights.linear, SHADER_UNIFORM_FLOAT, RF_MAX_LIGHTS);
    SetShaderValueV(shader, RSF3D_State.lights.quadrLoc, RSF3D_State.lights.quadratic, SHADER_UNIFORM_FLOAT, RF_MAX_LIGHTS);

    // SPOTLIGHT ATTRIBUTES
    SetShaderValueV(shader, RSF3D_State.lights.cutOffLoc, RSF3D_State.lights.cutOff, SHADER_UNIFORM_FLOAT, RF_MAX_LIGHTS);

    SetShaderValueV(shader, RSF3D_State.lights.outerCutOffLoc, RSF3D_State.lights.outerCutOff, SHADER_UNIFORM_FLOAT, RF_MAX_LIGHTS);

    // LIGHT DETAILS
    SetShaderValueV(shader, RSF3D_State.lights.modeLoc, RSF3D_State.lights.mode, SHADER_UNIFORM_INT, RF_MAX_LIGHTS);
    SetShaderValueV(shader, RSF3D_State.lights.enabledLoc, RSF3D_State.lights.enabled, SHADER_UNIFORM_INT, RF_MAX_LIGHTS);

    // VIEW POS
    SetShaderValue(shader, RSF3D_State.lights.viewLoc, &camera.position, SHADER_UNIFORM_VEC3);
}

void RF_UnloadShader(Shader shader)
{
    if (RSF3D_State.lights.positions != NULL) MemFree(RSF3D_State.lights.positions);
    if (RSF3D_State.lights.targets != NULL) MemFree(RSF3D_State.lights.targets);
    if (RSF3D_State.lights.ambientColors != NULL) MemFree(RSF3D_State.lights.ambientColors);
    if (RSF3D_State.lights.diffuseColors != NULL) MemFree(RSF3D_State.lights.diffuseColors);
    if (RSF3D_State.lights.specularColors != NULL) MemFree(RSF3D_State.lights.specularColors);
    if (RSF3D_State.lights.intensity != NULL) MemFree(RSF3D_State.lights.intensity);
    if (RSF3D_State.lights.constant != NULL) MemFree(RSF3D_State.lights.constant);
    if (RSF3D_State.lights.linear != NULL) MemFree(RSF3D_State.lights.linear);
    if (RSF3D_State.lights.quadratic != NULL) MemFree(RSF3D_State.lights.quadratic);
    if (RSF3D_State.lights.cutOff != NULL) MemFree(RSF3D_State.lights.cutOff);
    if (RSF3D_State.lights.outerCutOff != NULL) MemFree(RSF3D_State.lights.outerCutOff);
    if (RSF3D_State.lights.mode != NULL) MemFree(RSF3D_State.lights.mode);
    if (RSF3D_State.lights.enabled != NULL) MemFree(RSF3D_State.lights.enabled);

    UnloadShader(shader);
}

void RF_SetShadingModel(Shader shader, enum RF_ShadingModel shadingMode)
{
    SetShaderValue(shader, RSF3D_State.lights.shadingModelLoc, &shadingMode, SHADER_UNIFORM_INT);
}

RF_Light RF_CreateLightPro(Vector3 position, Vector3 target, 
    Color ambient, Color diffuse, Color specular, // Colors
    float intensity, float constant, float linear, float quadratic, // Attenuation
    float cutOff, float outerCutOff, // Spot
    enum RF_LightType mode, bool enabled)
{
    RF_Light light = {0};

    if (rfLightCount >= RF_MAX_LIGHTS)
    {
        TraceLog(LOG_WARNING, "The amount of the lights must be less than %d", RF_MAX_LIGHTS);
        return light;
    }

    light.position = position;
    light.target = target;
    light.ambient = ambient;
    light.diffuse = diffuse;
    light.specular = specular;
    light.mode = mode;
    light.intensity = intensity;
    light.enabled = enabled;
    light.constant = constant;
    light.linear = linear;
    light.quadratic = quadratic;

    light.cutOff = cutOff;
    light.outerCutOff = outerCutOff;

    rfLightCount++;

    return light;
}

RF_Light RF_CreateLightPoint(Vector3 position, Color ambient, Color diffuse, Color specular, float intensity)
{
    return RF_CreateLightPro(position, CLITERAL(Vector3){0.0f, 0.0f, 0.0f}, ambient, diffuse, specular, intensity, 1.0f, 0.09f, 0.032f, 0.0f, 0.0f, RF_LIGHT_POINT, true);
}

RF_Light RF_CreateLightDirectional(Vector3 position, Vector3 target, Color ambient, Color diffuse, Color specular, float intensity)
{
    return RF_CreateLightPro(position, target, ambient, diffuse, specular, intensity, 1.0f, 0.09f, 0.032f, 0.0f, 0.0f, RF_LIGHT_DIRECTIONAL, true);
}

RF_Light RF_CreateLightSpot(Vector3 position, Vector3 target, Color ambient, Color diffuse, Color specular, float cutOff, float outerCutOff, float intensity)
{
    return RF_CreateLightPro(position, target, ambient, diffuse, specular, intensity, 1.0f, 0.09f, 0.032f, cutOff, outerCutOff, RF_LIGHT_SPOT, true);
}

void RF_UpdateLightIntoBuffer(const RF_Light* light, int index)
{
    if (light == NULL || index < 0 || index >= RF_MAX_LIGHTS)
    {
        TraceLog(LOG_WARNING, "Light RFS3D must not be null or index must be greater than 0 or less than %d", RF_MAX_LIGHTS);
        return;
    }

    RSF3D_State.lights.positions[index] = light->position;
    RSF3D_State.lights.targets[index] = light->target;

    Vector4 ambient = ColorNormalize(light->ambient);
    Vector4 diffuse = ColorNormalize(light->diffuse);
    Vector4 specular = ColorNormalize(light->specular);

    RSF3D_State.lights.ambientColors[index] = CLITERAL(Vector3){ambient.x, ambient.y, ambient.z};
    RSF3D_State.lights.diffuseColors[index] = CLITERAL(Vector3){diffuse.x, diffuse.y, diffuse.z};
    RSF3D_State.lights.specularColors[index] = CLITERAL(Vector3){specular.x, specular.y, specular.z};

    RSF3D_State.lights.intensity[index] = light->intensity;
    RSF3D_State.lights.constant[index] = light->constant;
    RSF3D_State.lights.linear[index] = light->linear;
    RSF3D_State.lights.quadratic[index] = light->quadratic;

    RSF3D_State.lights.cutOff[index] = light->cutOff;
    RSF3D_State.lights.outerCutOff[index] = light->outerCutOff;

    RSF3D_State.lights.mode[index] = light->mode;
    RSF3D_State.lights.enabled[index] = light->enabled ? 1 : 0;
}

RF_Skybox RF_CreateSkybox(void)
{
    // SKYBOX
	const char *vsCodeSB =
		"#version 330\n"
		"in vec3 vertexPosition;\n"
		"uniform mat4 mvp;\n"
		"out vec3 fragPosition;\n"

		"void main() {\n"
		"    fragPosition = vertexPosition;\n"

		"    vec4 pos = mvp * vec4(vertexPosition, 1.0);\n"
		"    gl_Position = pos.xyzw;\n"
		"}\n";
	
	const char *fsCodeSB =
		"#version 330\n"
		"in vec3 fragPosition;\n"
		"out vec4 finalColor;\n"
		"uniform sampler2D texture0;\n"
		"uniform vec4 colDiffuse;\n"
		"uniform float energy;\n"

		"const float PI = 3.14159265359;\n"

		"void main() {\n"
		"    vec3 dir = normalize(fragPosition);\n"

		"    float u = fract(0.5 + atan(dir.z, dir.x) / (2.0 * PI));\n"
		"    float v = 0.5 - asin(clamp(dir.y, -1.0, 1.0)) / PI;\n"

		"    vec2 uv = vec2(u, v);\n"

		"    vec3 color = texture(texture0, uv).rgb;\n"
		"    color *= colDiffuse.rgb * energy;\n"

		"    finalColor = vec4(color, 1.0);\n"
		"}\n";

    RF_Skybox skybox = {0};
    skybox.shader = LoadShaderFromMemory(vsCodeSB, fsCodeSB);
    skybox.model = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));
    skybox.axis = CLITERAL(Vector3){0.0f, 0.0f, 0.0f};
    skybox.angle = 0.0f;
    skybox.energy = 1.0f;

    skybox.energyLoc = GetShaderLocation(skybox.shader, "energy");
    skybox.model.materials[0].shader = skybox.shader;

    return skybox;
}

void RF_UnloadSkybox(RF_Skybox skybox)
{
    UnloadShader(skybox.shader);

	// if (skybox.model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture.id != 0) skybox.model.materials[0].maps[MATERIAL_MAP_ALBEDO].texture.id = 0;
	skybox.model.materials[0].shader.id = rlGetShaderIdDefault();

    UnloadModel(skybox.model);

    TraceLog(LOG_INFO, "Unloaded skybox");
}

void RF_DrawSkybox(RF_Skybox skybox, Camera3D camera)
{
    SetShaderValue(skybox.shader, skybox.energyLoc, &skybox.energy, SHADER_UNIFORM_FLOAT);

    rlDisableBackfaceCulling();
    rlDisableDepthMask();

    BeginShaderMode(skybox.shader);
    DrawModelEx(skybox.model, camera.position, skybox.axis, skybox.angle, CLITERAL(Vector3){1.0f, 1.0f, 1.0f}, WHITE);
    EndShaderMode();

    rlEnableDepthMask();
    rlEnableBackfaceCulling();
}

void RF_SetFog(Shader shader, float start, float end, float density, Color color, enum RF_FogMode mode)
{
    RSF3D_State.fog.start = start;
    RSF3D_State.fog.end = end;
    RSF3D_State.fog.density = density;
    RSF3D_State.fog.color = color;
    RSF3D_State.fog.mode = mode;

    Vector4 fog = ColorNormalize(color);

    SetShaderValue(shader, RSF3D_State.fog.startLoc, &RSF3D_State.fog.start, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, RSF3D_State.fog.endLoc, &RSF3D_State.fog.end, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, RSF3D_State.fog.densityLoc, &RSF3D_State.fog.density, SHADER_UNIFORM_FLOAT);
    SetShaderValue(shader, RSF3D_State.fog.colorLoc, &fog, SHADER_UNIFORM_VEC4);
    SetShaderValue(shader, RSF3D_State.fog.modeLoc, &RSF3D_State.fog.mode, SHADER_UNIFORM_INT);
}

#if defined(__cplusplus)
}
#endif

#endif