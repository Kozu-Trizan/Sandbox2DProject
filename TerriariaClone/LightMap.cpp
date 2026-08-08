#include "LightMap.h"
#include "Constant.h"
#include "Blocks.h"
#include <algorithm>
#include <cstring>
#include <rlgl.h>

// Maximum visible blocks at minimum zoom — generous overestimate.
// At MinZoom = 0.1, visible blocks = ScreenWidth / (BLOCK_SIZE * 0.1) ≈ 640
// We add a small margin for edge blocks.
static const int MAX_LIGHTMAP_W = 700;
static const int MAX_LIGHTMAP_H = 400;

// CPU-side pixel buffer (RGBA, one pixel per visible block)
static Color* LightPixels = nullptr;

// GPU texture used as the lightmap overlay
static Texture2D LightTexture = { 0 };

void InitLightMap() {
    // Allocate the CPU pixel buffer once
    LightPixels = new Color[MAX_LIGHTMAP_W * MAX_LIGHTMAP_H];

    // Create a GPU texture we'll upload to each frame.
    // Using an Image + LoadTextureFromImage is the cleanest Raylib approach.
    Image img = GenImageColor(MAX_LIGHTMAP_W, MAX_LIGHTMAP_H, WHITE);
    LightTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    // Bilinear filtering is the key — the GPU will smoothly interpolate
    // between adjacent block light values when the texture is stretched.
    SetTextureFilter(LightTexture, TEXTURE_FILTER_BILINEAR);
}

void DeInitLightMap() {
    if (LightTexture.id != 0) UnloadTexture(LightTexture);
    delete[] LightPixels;
    LightPixels = nullptr;
}

void DrawLightMap(Block** Univ, Camera2D camera) {
    // Calculate the visible block range — same logic as DrawVisibleWorld
    Vector2 TopLeft = GetScreenToWorld2D({ 0, 0 }, camera);
    Vector2 BotRight = GetScreenToWorld2D({ (float)ScreenWidth, (float)ScreenHeight }, camera);

    int StartX = std::max(0, static_cast<int>(TopLeft.x / BLOCK_SIZE) - 1);
    int StartY = std::max(0, static_cast<int>(TopLeft.y / BLOCK_SIZE) - 1);
    int EndX = std::min(UniverseWidth, static_cast<int>(BotRight.x / BLOCK_SIZE) + 2);
    int EndY = std::min(UniverseHeight, static_cast<int>(BotRight.y / BLOCK_SIZE) + 2);

    int mapW = EndX - StartX;
    int mapH = EndY - StartY;

    // Safety clamp — should never trigger in practice
    if (mapW <= 0 || mapH <= 0) return;
    if (mapW > MAX_LIGHTMAP_W) mapW = MAX_LIGHTMAP_W;
    if (mapH > MAX_LIGHTMAP_H) mapH = MAX_LIGHTMAP_H;

    // Fill the pixel buffer with light values — one pixel per block.
    // White (255,255,255) = fully lit, Black (0,0,0) = fully dark.
    // With BLEND_MULTIPLIED, white passes color through; black kills it.
    for (int y = 0; y < mapH; y++) {
        int worldY = StartY + y;
        for (int x = 0; x < mapW; x++) {
            int worldX = StartX + x;
            std::uint8_t lv = Univ[worldY][worldX].GetLightValue();
            LightPixels[y * mapW + x] = { lv, lv, lv, 255 };
        }
    }

    // Upload only the used sub-rectangle to the GPU.
    // UpdateTextureRec lets us write a sub-region without reallocating.
    Rectangle updateRect = { 0, 0, (float)mapW, (float)mapH };
    UpdateTextureRec(LightTexture, updateRect, LightPixels);

    // Source rect: the portion of our (oversized) texture that has valid data.
    // We offset by 0.5 texels inward so bilinear sampling at block centers
    // reads exact values, and the interpolation happens between blocks.
    Rectangle srcRect = { 0.5f, 0.5f, (float)mapW - 1.0f, (float)mapH - 1.0f };

    // Destination rect: the corresponding world-space area, also inset by
    // half a block so the texel centers align with block centers.
    float destX = (StartX + 0.5f) * BLOCK_SIZE;
    float destY = (StartY + 0.5f) * BLOCK_SIZE;
    float destW = (mapW - 1.0f) * BLOCK_SIZE;
    float destH = (mapH - 1.0f) * BLOCK_SIZE;
    Rectangle dstRect = { destX, destY, destW, destH };

    // Draw with multiply blend: world_color * lightmap_color.
    // Where lightmap is white (255), world color passes through unchanged.
    // Where lightmap is black (0), world color becomes black (shadow).
    BeginBlendMode(BLEND_MULTIPLIED);
    DrawTexturePro(LightTexture, srcRect, dstRect, { 0, 0 }, 0.0f, WHITE);
    EndBlendMode();
}
