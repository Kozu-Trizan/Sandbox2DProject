#pragma once
#include <raylib.h>
extern Texture2D BlockTexture;

void InitializeTexture();
void DrawBackground();
void DeInitializeTexture();
void Parallax(Camera2D camera, Texture2D texture, float PosY=0, float ParallaxSpeed = 0.1f, float TextureScale = 2.0f);
void DrawParallax(Camera2D camera);
