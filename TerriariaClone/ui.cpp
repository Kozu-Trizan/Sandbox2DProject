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
    DrawRectangle(static_cast<int>(screenX - 2), static_cast<int>(screenY - 2), static_cast<int>(barWidth + 4), static_cast<int>(barHeight + 4), DARKGRAY);
    DrawRectangle(static_cast<int>(screenX), static_cast<int>(screenY), static_cast<int>(barWidth), static_cast<int>(barHeight), BLACK);

    // Active health fill
    if (currentFillWidth > 0) {
        DrawRectangle(static_cast<int>(screenX), static_cast<int>(screenY), static_cast<int>(currentFillWidth), static_cast<int>(barHeight), barColor);
    }

    // Health text (e.g. "100 / 100")
    DrawText(TextFormat("%.0f / %.0f", player.GetHP(), player.GetMaxHP()), static_cast<int>(screenX + 60), static_cast<int>(screenY + 3), 15, WHITE);
}