#include "Blocks.h"
#include "MapGen.h"

void Block::RestoreHealth() {
	this->HP = this->BaseHP;
}

bool Block::operator!=(Block& another) {
	if (
		this->B_ID == another.B_ID &&
		this->BaseHP == another.BaseHP &&
		this->WallID == another.WallID
		) {
		return false;
	}
	return true;
}

bool Block::operator==(Block& another) {
	if (
		this->B_ID == another.B_ID &&
		this->BaseHP == another.BaseHP &&
		this->WallID == another.WallID
		) {
		return true;
	}
	return false;
}

bool Block::SurroundedByAir(BlockPos& pos) {
	for (int i = (pos.x - 1); i <= (pos.x + 1); i++){
		for (int j = (pos.y - 1); j <= (pos.y + 1); j++) {
			if (pos.x == i && pos.y == j) continue;
			if (Universe[j][i] != const_cast<Block&>(Air)) return false;
		}
	}
	return true;
}

std::uint8_t Block::GetLightValue() {
	return this->LightValue;
}

std::uint8_t Block::GetLightDecay() {
	return this->LightDecay;
}

void Block::SetLightValue(std::uint8_t NewLightValue) {
	this->LightValue = NewLightValue;
}

// Returns the source rectangle for the block texture.
// If no texture is available, returns a rectangle with a negative width/height.
Rectangle GetBlockSourceRectangle(std::uint8_t blockID)
{
	switch (blockID)
	{
	case DIRT_BID:
		return { 0, 29 * 16, 16, 16 };
	case GRASS_BID:
		return { 1 * 16, 29 * 16, 16, 16 };
	case STONE_BID:
		return { 16, 0, 16, 16 };
	default:
		return { -1.0f, -1.0f, -1.0f, -1.0f };
	}
}

// Returns a placeholder color for each block type.
// Replace with texture-based rendering later.
Color GetBlockColor(std::uint8_t blockID)
{
	switch (blockID)
	{
	case DIRT_BID:
		return { 139, 90, 43, 255 };    // Brown
	case GRASS_BID:
		return { 34, 139, 34, 255 };     // Forest green
	case STONE_BID:
		return { 136, 140, 141, 255 };   // Gray
	case WOOD_BID:
		return { 139, 69, 19, 255 };     // Saddle brown
	default:
		return MAGENTA;                  // Unmapped block — easy to spot
	}
}