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