#pragma once
#include "Constant.h"
#include "Blocks.h"

void BreadthFirstSearch(int StartX, int StartY, int TopX, int TopY, int BotX, int BotY);

bool IsWithinLimits(BlockPos CurrBlock, int TopX, int TopY, int BotX, int BotY);

void QueueLightSources(int StartX, int StartY, int EndX, int EndY);

void UpdateLight(Camera2D camera);

void UpdateLocalLight(BlockPos Pos);