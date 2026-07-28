#include "MapGen.h"
#include "BresenhamAlgorithm.h"
#include "IsSolid.h"

bool CanReach(int x0, int y0, int x1, int y1) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	if (dx != 0) {
		int y = y0;
		int D = 2 * dy - dx; // D--> Decision parameter in Bresenham's line algorithm

		for (int i = 1; i < dx; i++) { // Do not check the starting and ending blocks as it is either player or a solid block.
			if (IsSolid(x0 + i, y)) return false; // If the path is obstructed return immediately;
			if (D >= 0) {
				y += 1;
				D = D - 2 * dx;
			}
			D = D + 2 * dy;
		}
	}
	return true;
}