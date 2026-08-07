#include "ui.h"

void DrawPlayerHealthBar(const Player& player, float screenX, float screenY) {
    const float barWidth = 200.0f;
    const float barHeight = 20.0f;

    // Calculate health ratio using getters
    float healthRatio = player.GetHP() / player.GetMaxHP();
    float currentFillWidth = barWidth * healthRatio;

    // Dynamic bar color based on remaining health
    Color barColor = GREEN;
    if (healthRatio <= 0.25f) barColor = RED;
    else if (healthRatio <= 0.50f) barColor = ORANGE;

    // Outer border & dark background
    DrawRectangle(screenX - 2, screenY - 2, barWidth + 4, barHeight + 4, DARKGRAY);
    DrawRectangle(screenX, screenY, barWidth, barHeight, BLACK);

    // Active health fill
    if (currentFillWidth > 0) {
        DrawRectangle(screenX, screenY, currentFillWidth, barHeight, barColor);
    }

    // Health text (e.g. "100 / 100")
    DrawText(TextFormat("%.0f / %.0f", player.GetHP(), player.GetMaxHP()), screenX + 60, screenY + 3, 15, WHITE);
}