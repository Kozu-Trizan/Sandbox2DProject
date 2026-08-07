#include "WavefrontPropagation.h"
#include <vector>
#include <cstring>

extern Block** Universe;

static bool Visited[UniverseHeight][UniverseWidth];

static std::vector<BlockPos> BFSQueue;
static int BFSHead = 0;

static void InitBFSState() {
	std::memset(Visited, 0, sizeof(Visited));
	BFSQueue.clear();
	BFSHead = 0;
	// Reserve max capacity once; subsequent calls after clear() keep the buffer.
	if (BFSQueue.capacity() == 0) {
		BFSQueue.reserve(static_cast<size_t>(UniverseWidth) * UniverseHeight);
	}
}

// -------------------------------------------------------------------------

bool IsWithinLimits(BlockPos CurrBlock, int TopX, int TopY, int BotX, int BotY) {
	return (
		(CurrBlock.x >= TopX && CurrBlock.x < BotX) &&  
		(CurrBlock.y >= TopY && CurrBlock.y < BotY)
	);
}

void BreadthFirstSearch(int StartX, int StartY, int TopX, int TopY, int BotX, int BotY) {
	// Only enqueue the seed if QueueLightSources hasn't already added it
	if (!Visited[StartY][StartX]) {
		BFSQueue.push_back({ StartX, StartY });
		Visited[StartY][StartX] = true;
	}

	// Cardinal-direction offsets — no diagonals.
	static constexpr int dx[4] = {  0, 0, -1, 1 };
	static constexpr int dy[4] = { -1, 1,  0, 0 };

	while (BFSHead < static_cast<int>(BFSQueue.size())) {
		const BlockPos CurrBlock = BFSQueue[BFSHead++];  // O(1) dequeue, no deallocation

		const int CurrLight = Universe[CurrBlock.y][CurrBlock.x].GetLightValue();
		const int CurrDecay = Universe[CurrBlock.y][CurrBlock.x].GetLightDecay();
		const int PropagatedLight = CurrLight - CurrDecay;

		// Without this, (0 - decay) wraps to ~244 and falsely propagates max light.
		if (PropagatedLight <= 0) continue;

		for (int i = 0; i < 4; i++) {
			const BlockPos Neighbour = { CurrBlock.x + dx[i], CurrBlock.y + dy[i] };
			if (!IsWithinLimits(Neighbour, TopX, TopY, BotX, BotY)) continue;

			if (static_cast<std::uint8_t>(PropagatedLight) > Universe[Neighbour.y][Neighbour.x].GetLightValue()) {
				Universe[Neighbour.y][Neighbour.x].SetLightValue(static_cast<std::uint8_t>(PropagatedLight));
				if (!Visited[Neighbour.y][Neighbour.x]) {
					BFSQueue.push_back(Neighbour);
					Visited[Neighbour.y][Neighbour.x] = true;
				}
			}
		}
	}
}

void QueueLightSources(int StartX, int StartY, int EndX, int EndY) {
	InitBFSState();   // Reset Visited + queue before each full BFS pass
	for (int y = StartY; y < EndY; y++) {
		for (int x = StartX; x < EndX; x++) {
			if (Universe[y][x].B_ID == Air.B_ID && Universe[y][x].WallID == Air.WallID) {
				BFSQueue.push_back({ x, y });
				Visited[y][x] = true;
			}
		}
	}
}

void UpdateLocalLight(BlockPos Pos) {
	int margin = 30; // 30 blocks is enough to cover the max light decay distance
	int StartX = std::max(0, Pos.x - margin);
	int StartY = std::max(0, Pos.y - margin);
	int EndX = std::min(UniverseWidth, Pos.x + margin);
	int EndY = std::min(UniverseHeight, Pos.y + margin);

	// Clear light in this region
	for (int y = StartY; y < EndY; y++) {
		for (int x = StartX; x < EndX; x++) {
			Universe[y][x].SetLightValue(0);
		}
	}

	InitBFSState();

	// Enqueue all light sources in the region
	for (int y = StartY; y < EndY; y++) {
		for (int x = StartX; x < EndX; x++) {
			if (Universe[y][x].B_ID == Air.B_ID && Universe[y][x].WallID == Air.WallID) {
				Universe[y][x].SetLightValue(MAX_LIGHT_VAL);
				BFSQueue.push_back({ x, y });
				Visited[y][x] = true;
			}
		}
	}

	// Enqueue borders of the region if they have light > 0, to let outside light seep back in
	auto TryQueueBorder = [&](int x, int y) {
		if (x >= 0 && x < UniverseWidth && y >= 0 && y < UniverseHeight) {
			if (Universe[y][x].GetLightValue() > 0 && !Visited[y][x]) {
				BFSQueue.push_back({ x, y });
				Visited[y][x] = true;
			}
		}
	};

	for (int x = StartX - 1; x <= EndX; x++) {
		TryQueueBorder(x, StartY - 1);
		TryQueueBorder(x, EndY);
	}
	for (int y = StartY; y < EndY; y++) {
		TryQueueBorder(StartX - 1, y);
		TryQueueBorder(EndX, y);
	}

	BreadthFirstSearch(StartX, StartY, StartX, StartY, EndX, EndY);
}

void UpdateLight(Camera2D camera) {
	Vector2 ViewportDimTop = GetScreenToWorld2D({ 0 ,0 }, camera);
	Vector2 ViewportDimBot = GetScreenToWorld2D({ (float)ScreenWidth ,(float)ScreenHeight }, camera);

	int StartX = std::max(0, static_cast<int>(ViewportDimTop.x / BLOCK_SIZE));
	int StartY = std::max(0, static_cast<int>(ViewportDimTop.y / BLOCK_SIZE));
	int EndX = std::min(UniverseWidth, static_cast<int>(ViewportDimBot.x / BLOCK_SIZE) + 1);
	int EndY = std::min(UniverseHeight, static_cast<int>(ViewportDimBot.y / BLOCK_SIZE) + 1);

	QueueLightSources(StartX, StartY, EndX, EndY);
	BreadthFirstSearch(StartX, StartY, StartX, StartY, EndX, EndY);
}