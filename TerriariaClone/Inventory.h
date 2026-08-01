#pragma once
#include "Constant.h"

class Inventory;
class Player;

class Item {
public:
	virtual ~Item() = default;
};

class Cell {
private:
	Item* item;
	int ItemCount = 0;

	// Display Properties
	float CellHeight = 16;
	float CellWidth = 16;

public:
	Cell(Item* i=nullptr) : item(i){}
	friend Inventory;
	friend Player;
};

class Inventory {
private:
	Cell* cell = nullptr;
	int size;

	// Display Properties
	float InvHeight;
	float InvWidth;
	float InvPosX;
	float InvPosY;

public:
	Inventory();

	Inventory(int size);

	void AddItem(Item& item);

	int GetItemCount(int CellID);

	friend Player;

};