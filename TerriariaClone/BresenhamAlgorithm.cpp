#include "MapGen.h"
#include "BresenhamAlgorithm.h"
#include "IsSolid.h"
#include <cmath>

bool CanReach(int x0, int y0, int x1, int y1) {
    int dx = std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;
    
    int startX = x0;
    int startY = y0;

    while (true) {
        if (x0 == x1 && y0 == y1) break;
        
        if ((x0 != startX || y0 != startY) && IsSolid(x0, y0)) {
            return false;
        }
        
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
    return true;
}