#include "Inventory.h"
#include "Blocks.h"

Inventory::Inventory() {
	this->cell = nullptr;
	this->size = 0;
	this->InvHeight = 0;
	this->InvWidth = 0;
	this->InvPosX = 0;
	this->InvPosY = 0;
}

Inventory::Inventory(int size) {
	this->size = size;
	this->cell = new Cell[size];
	this->InvHeight = 16;
	this->InvWidth = this->InvHeight * this->size;
	this->InvPosX = ScreenWidth / 2 - InvWidth;
	this->InvPosY = ScreenHeight;
}

void Inventory::AddItem(Item& item) {
	Block* block = dynamic_cast<Block*>(&item);
	if (!block) return;

	for (int i = 0; i < this->size; i++) {
		if (this->cell[i].ItemCount > 0) {
			Block* existingBlock = dynamic_cast<Block*>(this->cell[i].item);
			if (existingBlock && existingBlock->B_ID == block->B_ID) {
				this->cell[i].ItemCount += 1;
				return;
			}
		}else{
			cell[i].item = new Block(*block);
			cell[i].ItemCount = 1;
			return;
		}
	}
}

int Inventory::GetItemCount(int CellID) {
	return this->cell[CellID + 1].ItemCount;
}