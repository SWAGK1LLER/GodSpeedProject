#pragma once

#include "CoreMinimal.h"

struct Position
{
	int X = -1;
	int Y = -1;
};

class PROJECTRANSACK_API Inventory
{
public:
	TArray<class AItem*> items;
	class AItem* space[2][4];

	Inventory();
	~Inventory();

	bool ValidateSpace(class AItem& pItem);
	bool ValidateSpace(class AItem& pItem, Position& pOut);
	void AddItem(class AItem& pItem, Position& pPos);
	void AddItem(class AItem& pItem);
	void RemoveItem(class AItem& pItem);
	void ClearInventory();
};
