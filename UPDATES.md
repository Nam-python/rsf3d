# All updates of RSF3D library

## 0.4.1 - November 21st, 2026:
- `fogColor` in fragment shader: changed from `vec3` to `vec4`.
- Changed in state and function `RF_SetFog`.

## 0.4.0 - November 21st, 2026:
- Added function `RF_UnloadShader`.
- Changed in RSF3D_State.
- Added fog (linear, exponential and exponential squared fog).

## 0.3.3 - November 18th, 2026:
- Fixed wrong directional light calculation in shader.

## 0.3.2 - November 15th, 2026:
- Fixed a small typo (from enum `RF_LIghtType` to `RF_LightType`)
- Changed 2 arguments' types:
  - Argument `shadingModel` of function `RF_SetShadingModel` changed type from `int` to `enum RF_ShadingModel`.
  - Argument `mode` of function `RF_CreateLightPro` changed type from `int` to `enum RF_LightType`.

## 0.3.1 - November 15th, 2026:
- Fixed the color problem in vertex shader and fragment shader.

## 0.3.0 - November 15th, 2026:
- Added Gouraud shading model.

## 0.2.0 - November 14th, 2026:
- Added skybox feature.
- Replaced the Phong light model with the Blinn-Phong light model to get more efficient.
- Added more 2 attributes for spot light (`cutOff` and `outerCutOff` (both must be degrees) in struct `RF_Light`).

## 0.1.0 - November 13th, 2026: 
- Added the Phong light model.
- Supported point light, directional light and basic spot light.