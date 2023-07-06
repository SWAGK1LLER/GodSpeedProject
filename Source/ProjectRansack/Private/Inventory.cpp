#include "Inventory.h"
#include "Item.h"


Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

bool Inventory::ValidateSpace(AItem& pItem)
{
	FSize size = pItem.SizeNeeded;

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			bool haveSpaceY = true;
			for (int y = i; y < size.SizeY && y < 2; y++)
				if (space[y][j] != nullptr)
				{
					haveSpaceY = false;
					break;
				}

			if (!haveSpaceY)
				continue;


			bool haveSpaceX = true;
			for (int x = j; x < size.SizeX && x < 4; x++)
				if (space[i][x] != nullptr)
				{
					haveSpaceX = false;
					break;
				}

			if (!haveSpaceX)
				continue;

			return true;
		}
	}

	return false;
}

bool Inventory::ValidateSpace(AItem& pItem, Position& pOut)
{
	FSize size = pItem.SizeNeeded;

	for ( int i = 0; i < 2; i++)
	{
		for ( int j = 0; j < 4; j++)
		{
			bool haveSpaceY = true;
			for ( int y = i; y < size.SizeY && y < 2; y++)
				if (space[y][j] != nullptr)
				{
					haveSpaceY = false;
					break;
				}

			if (!haveSpaceY)
				continue;


			bool haveSpaceX = true;
			for ( int x = j; x < size.SizeX && x < 4; x++)
				if (space[i][x] != nullptr)
				{
					haveSpaceX = false;
					break;
				}

			if (!haveSpaceX)
				continue;

			pOut.X = i;
			pOut.Y = j;
			return true;
		}
	}

	return false;
}

void Inventory::AddItem(AItem& pItem, Position& pPos)
{
	if (pPos.X == -1 || pPos.Y == -1)
		return;

	items.Add(&pItem);

	for ( int i = pPos.X; i < pPos.X + pItem.SizeNeeded.SizeX; i++)
		for ( int j = pPos.Y; j < pPos.Y + pItem.SizeNeeded.SizeY; j++)
			space[i][j] = &pItem;
}

void Inventory::AddItem(AItem& pItem)
{
	Position pos;
	if (!ValidateSpace(pItem, pos))
		return;

	AddItem(pItem, pos);
}

void Inventory::RemoveItem(AItem& pItem)
{
	if (items.Remove(&pItem) == 0)
		return;

	for ( int i = 0; i < 2; i++)
		for ( int j = 0; j < 4; j++)
			if (space[i][j] == &pItem)
				space[i][j] = nullptr;
}

void Inventory::ClearInventory()
{
	items.Empty();

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 4; j++)
			space[i][j] = nullptr;
}