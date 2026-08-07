#include "GetTexture.h"
#include "Constant.h"
#include <cmath>

Texture2D Background;
Texture2D Mountain;
Texture2D BgCloud;
Texture2D CloudLayerFront;
Texture2D CloudLayerMid;
Texture2D CloudLayerBack;
Texture2D LoneCloud;


void InitializeTexture() {
    Background = LoadTexture("assets/background/Layers/sky.png");
	Mountain = LoadTexture("assets/background/Layers/glacial_mountains.png");
	BgCloud = LoadTexture("assets/background/Layers/clouds_bg.png");
	CloudLayerFront = LoadTexture("assets/background/Layers/clouds_mg_3.png");
	CloudLayerMid = LoadTexture("assets/background/Layers/clouds_mg_2.png");
	CloudLayerBack = LoadTexture("assets/background/Layers/clouds_mg_1.png");
	LoneCloud = LoadTexture("assets/background/Layers/cloud_lonely.png");
}

void DrawBackground() {
	Rectangle source = { 0.0f, 0.0f, static_cast<float>(Background.width), static_cast<float>(Background.height) };
	Rectangle destination = { 0.0f, 0.0f, static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight) };

	DrawTexturePro(Background, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE);
}


void Parallax(Camera2D camera, Texture2D texture, float PosY, float ParallaxSpeed, float TextureScale) {

	// Calculate how far the background should be offset based on camera position.
	float scaledWidth = texture.width * TextureScale;
	// fmodf ensures the offset wraps around when it reaches the scaled width of the texture,
	// creating an infinite looping effect.
	float offset = fmodf(camera.target.x * ParallaxSpeed, scaledWidth);

	// Ensure offset is always positive if the camera moves to the left of 0
	if (offset < 0) offset += scaledWidth;

	for (int i = 0; i < 3; i++) {
		DrawTextureEx(
			texture,
			{ -offset + (i * scaledWidth) -  camera.zoom * 20, PosY },
			0.0f,
			TextureScale,
			WHITE
		);
	}
}

void DrawParallax(Camera2D camera) {
	Parallax(camera, Mountain, -30.0f, 0.01f, 1.5f);
	Parallax(camera, BgCloud, static_cast<float>(Mountain.height) / 3.0f, 0.05f);
	Parallax(camera, CloudLayerBack, static_cast<float>(Mountain.height) / 2, 0.08f);
	Parallax(camera, CloudLayerMid, static_cast<float>(Mountain.height) / 2);
	Parallax(camera, CloudLayerFront, static_cast<float>(Mountain.height) / 2,  0.2f);
	Parallax(camera, LoneCloud, -60, 0.02f);
}

void DeInitializeTexture() {
	UnloadTexture(Background);
	UnloadTexture(Mountain);
	UnloadTexture(BgCloud);
	UnloadTexture(CloudLayerBack);
	UnloadTexture(CloudLayerMid);
	UnloadTexture(CloudLayerFront);
	UnloadTexture(LoneCloud);
}